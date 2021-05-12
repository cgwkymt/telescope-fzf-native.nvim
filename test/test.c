#include "fzf.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>

#define test_fun_type void __attribute__((unused))

#define call_alg(alg, case, txt, pat, assert_block)                            \
  {                                                                            \
    string_t text = {.data = txt, .size = strlen(txt)};                        \
    string_t pattern = {.data = pat, .size = strlen(pat)};                     \
    result_t res = alg(case, false, true, &text, &pattern, true, NULL);        \
    assert_block;                                                              \
    if (res.pos) {                                                             \
      free(res.pos->data);                                                     \
      free(res.pos);                                                           \
    }                                                                          \
  }                                                                            \
  {                                                                            \
    slab_t *slab = make_default_slab();                                        \
    string_t text = {.data = txt, .size = strlen(txt)};                        \
    string_t pattern = {.data = pat, .size = strlen(pat)};                     \
    result_t res = alg(case, false, true, &text, &pattern, true, slab);        \
    assert_block;                                                              \
    if (res.pos) {                                                             \
      free(res.pos->data);                                                     \
      free(res.pos);                                                           \
    }                                                                          \
    free_slab(slab);                                                           \
  }

// TODO(conni2461): Implement normalize and test it here

static test_fun_type test_fuzzy_match_v2(void **state) {
  call_alg(fuzzy_match_v2, true, "So Danco Samba", "So", {
    assert_int_equal(0, res.start);
    assert_int_equal(2, res.end);
    assert_int_equal(56, res.score);

    assert_int_equal(2, res.pos->size);
    assert_int_equal(1, res.pos->data[0]);
    assert_int_equal(0, res.pos->data[1]);
  });

  call_alg(fuzzy_match_v2, false, "So Danco Samba", "sodc", {
    assert_int_equal(0, res.start);
    assert_int_equal(7, res.end);
    assert_int_equal(89, res.score);

    assert_int_equal(4, res.pos->size);
    assert_int_equal(6, res.pos->data[0]);
    assert_int_equal(3, res.pos->data[1]);
    assert_int_equal(1, res.pos->data[2]);
    assert_int_equal(0, res.pos->data[3]);
  });

  call_alg(fuzzy_match_v2, false, "Danco", "danco", {
    assert_int_equal(0, res.start);
    assert_int_equal(5, res.end);
    assert_int_equal(128, res.score);

    assert_int_equal(5, res.pos->size);
    assert_int_equal(4, res.pos->data[0]);
    assert_int_equal(3, res.pos->data[1]);
    assert_int_equal(2, res.pos->data[2]);
    assert_int_equal(1, res.pos->data[3]);
    assert_int_equal(0, res.pos->data[4]);
  });
}

static test_fun_type test_fuzzy_match_v1(void **state) {
  call_alg(fuzzy_match_v1, true, "So Danco Samba", "So", {
    assert_int_equal(0, res.start);
    assert_int_equal(2, res.end);
    assert_int_equal(56, res.score);

    assert_int_equal(2, res.pos->size);
    assert_int_equal(0, res.pos->data[0]);
    assert_int_equal(1, res.pos->data[1]);
  });

  call_alg(fuzzy_match_v1, false, "So Danco Samba", "sodc", {
    assert_int_equal(0, res.start);
    assert_int_equal(7, res.end);
    assert_int_equal(89, res.score);

    assert_int_equal(4, res.pos->size);
    assert_int_equal(0, res.pos->data[0]);
    assert_int_equal(1, res.pos->data[1]);
    assert_int_equal(3, res.pos->data[2]);
    assert_int_equal(6, res.pos->data[3]);
  });

  call_alg(fuzzy_match_v1, false, "Danco", "danco", {
    assert_int_equal(0, res.start);
    assert_int_equal(5, res.end);
    assert_int_equal(128, res.score);

    assert_int_equal(5, res.pos->size);
    assert_int_equal(0, res.pos->data[0]);
    assert_int_equal(1, res.pos->data[1]);
    assert_int_equal(2, res.pos->data[2]);
    assert_int_equal(3, res.pos->data[3]);
    assert_int_equal(4, res.pos->data[4]);
  });
}

static test_fun_type test_prefix_match(void **state) {
  call_alg(prefix_match, true, "So Danco Samba", "So", {
    assert_int_equal(0, res.start);
    assert_int_equal(2, res.end);
    assert_int_equal(56, res.score);
  });

  call_alg(prefix_match, false, "So Danco Samba", "sodc", {
    assert_int_equal(-1, res.start);
    assert_int_equal(-1, res.end);
    assert_int_equal(0, res.score);
  });

  call_alg(prefix_match, false, "Danco", "danco", {
    assert_int_equal(0, res.start);
    assert_int_equal(5, res.end);
    assert_int_equal(128, res.score);
  });
}

static test_fun_type test_exact_match(void **state) {
  call_alg(exact_match_naive, true, "So Danco Samba", "So", {
    assert_int_equal(0, res.start);
    assert_int_equal(2, res.end);
    assert_int_equal(56, res.score);
  });

  call_alg(exact_match_naive, false, "So Danco Samba", "sodc", {
    assert_int_equal(-1, res.start);
    assert_int_equal(-1, res.end);
    assert_int_equal(0, res.score);
  });

  call_alg(exact_match_naive, false, "Danco", "danco", {
    assert_int_equal(0, res.start);
    assert_int_equal(5, res.end);
    assert_int_equal(128, res.score);
  });
}

static test_fun_type test_suffix_match(void **state) {
  call_alg(suffix_match, true, "So Danco Samba", "So", {
    assert_int_equal(-1, res.start);
    assert_int_equal(-1, res.end);
    assert_int_equal(0, res.score);
  });

  call_alg(suffix_match, false, "So Danco Samba", "sodc", {
    assert_int_equal(-1, res.start);
    assert_int_equal(-1, res.end);
    assert_int_equal(0, res.score);
  });

  call_alg(suffix_match, false, "Danco", "danco", {
    assert_int_equal(0, res.start);
    assert_int_equal(5, res.end);
    assert_int_equal(128, res.score);
  });
}

static test_fun_type test_equal_match(void **state) {
  call_alg(equal_match, true, "So Danco Samba", "So", {
    assert_int_equal(-1, res.start);
    assert_int_equal(-1, res.end);
    assert_int_equal(0, res.score);
  });

  call_alg(equal_match, false, "So Danco Samba", "sodc", {
    assert_int_equal(-1, res.start);
    assert_int_equal(-1, res.end);
    assert_int_equal(0, res.score);
  });

  call_alg(equal_match, false, "Danco", "danco", {
    assert_int_equal(0, res.start);
    assert_int_equal(5, res.end);
    assert_int_equal(128, res.score);
  });
}

#define test_pat(case, str, test_block)                                        \
  {                                                                            \
    pattern_t *pat = parse_pattern(case, false, str);                          \
    test_block;                                                                \
    free_pattern(pat);                                                         \
  }

static test_fun_type test_parse_pattern(void **state) {
  test_pat(case_smart, "lua", {
    assert_int_equal(1, pat->size);
    assert_int_equal(1, pat->cap);
    assert_false(pat->only_inv);

    assert_int_equal(1, pat->ptr[0]->size);
    assert_int_equal(1, pat->ptr[0]->cap);

    assert_int_equal(term_fuzzy, pat->ptr[0]->ptr[0].typ);
    assert_string_equal("lua", pat->ptr[0]->ptr[0].text.data);
    assert_false(pat->ptr[0]->ptr[0].case_sensitive);
  });

  test_pat(case_smart, "file\\ ", {
    assert_int_equal(1, pat->size);
    assert_int_equal(1, pat->cap);
    assert_false(pat->only_inv);

    assert_int_equal(1, pat->ptr[0]->size);
    assert_int_equal(1, pat->ptr[0]->cap);

    assert_int_equal(term_fuzzy, pat->ptr[0]->ptr[0].typ);
    assert_string_equal("file ", pat->ptr[0]->ptr[0].text.data);
    assert_false(pat->ptr[0]->ptr[0].case_sensitive);
  });

  test_pat(case_smart, "file\\ with\\ space", {
    assert_int_equal(1, pat->size);
    assert_int_equal(1, pat->cap);
    assert_false(pat->only_inv);

    assert_int_equal(1, pat->ptr[0]->size);
    assert_int_equal(1, pat->ptr[0]->cap);

    assert_int_equal(term_fuzzy, pat->ptr[0]->ptr[0].typ);
    assert_string_equal("file with space", pat->ptr[0]->ptr[0].text.data);
    assert_false(pat->ptr[0]->ptr[0].case_sensitive);
  });

  test_pat(case_smart, "file\\  new", {
    assert_int_equal(2, pat->size);
    assert_int_equal(2, pat->cap);
    assert_false(pat->only_inv);

    assert_int_equal(1, pat->ptr[0]->size);
    assert_int_equal(1, pat->ptr[0]->cap);
    assert_int_equal(1, pat->ptr[1]->size);
    assert_int_equal(1, pat->ptr[1]->cap);

    assert_int_equal(term_fuzzy, pat->ptr[0]->ptr[0].typ);
    assert_string_equal("file ", pat->ptr[0]->ptr[0].text.data);
    assert_false(pat->ptr[0]->ptr[0].case_sensitive);

    assert_int_equal(term_fuzzy, pat->ptr[1]->ptr[0].typ);
    assert_string_equal("new", pat->ptr[1]->ptr[0].text.data);
    assert_false(pat->ptr[1]->ptr[0].case_sensitive);
  });

  test_pat(case_smart, "!Lua", {
    assert_int_equal(1, pat->size);
    assert_int_equal(1, pat->cap);
    assert_true(pat->only_inv);

    assert_int_equal(1, pat->ptr[0]->size);
    assert_int_equal(1, pat->ptr[0]->cap);

    assert_int_equal(term_exact, pat->ptr[0]->ptr[0].typ);
    assert_string_equal("Lua", pat->ptr[0]->ptr[0].text.data);
    assert_true(pat->ptr[0]->ptr[0].case_sensitive);
    assert_true(pat->ptr[0]->ptr[0].inv);
  });

  test_pat(case_smart, "!fzf !test", {
    assert_int_equal(2, pat->size);
    assert_int_equal(2, pat->cap);
    assert_true(pat->only_inv);

    assert_int_equal(1, pat->ptr[0]->size);
    assert_int_equal(1, pat->ptr[0]->cap);
    assert_int_equal(1, pat->ptr[1]->size);
    assert_int_equal(1, pat->ptr[1]->cap);

    assert_int_equal(term_exact, pat->ptr[0]->ptr[0].typ);
    assert_string_equal("fzf", pat->ptr[0]->ptr[0].text.data);
    assert_false(pat->ptr[0]->ptr[0].case_sensitive);
    assert_true(pat->ptr[0]->ptr[0].inv);

    assert_int_equal(term_exact, pat->ptr[1]->ptr[0].typ);
    assert_string_equal("test", pat->ptr[1]->ptr[0].text.data);
    assert_false(pat->ptr[1]->ptr[0].case_sensitive);
    assert_true(pat->ptr[1]->ptr[0].inv);
  });

  test_pat(case_smart, "Lua", {
    assert_int_equal(1, pat->size);
    assert_int_equal(1, pat->cap);
    assert_false(pat->only_inv);

    assert_int_equal(1, pat->ptr[0]->size);
    assert_int_equal(1, pat->ptr[0]->cap);

    assert_int_equal(term_fuzzy, pat->ptr[0]->ptr[0].typ);
    assert_string_equal("Lua", pat->ptr[0]->ptr[0].text.data);
    assert_true(pat->ptr[0]->ptr[0].case_sensitive);
  });

  test_pat(case_smart, "'src | ^Lua", {
    assert_int_equal(1, pat->size);
    assert_int_equal(1, pat->cap);
    assert_false(pat->only_inv);

    assert_int_equal(2, pat->ptr[0]->size);
    assert_int_equal(2, pat->ptr[0]->cap);

    assert_int_equal(term_exact, pat->ptr[0]->ptr[0].typ);
    assert_string_equal("src", pat->ptr[0]->ptr[0].text.data);
    assert_false(pat->ptr[0]->ptr[0].case_sensitive);

    assert_int_equal(term_prefix, pat->ptr[0]->ptr[1].typ);
    assert_string_equal("Lua", pat->ptr[0]->ptr[1].text.data);
    assert_true(pat->ptr[0]->ptr[1].case_sensitive);
  });

  test_pat(case_smart, ".lua$ 'previewer !'term !asdf", {
    assert_int_equal(4, pat->size);
    assert_int_equal(4, pat->cap);
    assert_false(pat->only_inv);

    assert_int_equal(1, pat->ptr[0]->size);
    assert_int_equal(1, pat->ptr[0]->cap);
    assert_int_equal(1, pat->ptr[1]->size);
    assert_int_equal(1, pat->ptr[1]->cap);
    assert_int_equal(1, pat->ptr[2]->size);
    assert_int_equal(1, pat->ptr[2]->cap);
    assert_int_equal(1, pat->ptr[3]->size);
    assert_int_equal(1, pat->ptr[3]->cap);

    assert_int_equal(term_suffix, pat->ptr[0]->ptr[0].typ);
    assert_string_equal(".lua", pat->ptr[0]->ptr[0].text.data);
    assert_false(pat->ptr[0]->ptr[0].case_sensitive);

    assert_int_equal(term_exact, pat->ptr[1]->ptr[0].typ);
    assert_string_equal("previewer", pat->ptr[1]->ptr[0].text.data);
    assert_int_equal(0, pat->ptr[1]->ptr[0].case_sensitive);

    assert_int_equal(term_fuzzy, pat->ptr[2]->ptr[0].typ);
    assert_string_equal("term", pat->ptr[2]->ptr[0].text.data);
    assert_false(pat->ptr[2]->ptr[0].case_sensitive);
    assert_true(pat->ptr[2]->ptr[0].inv);

    assert_int_equal(term_exact, pat->ptr[3]->ptr[0].typ);
    assert_string_equal("asdf", pat->ptr[3]->ptr[0].text.data);
    assert_false(pat->ptr[3]->ptr[0].case_sensitive);
    assert_true(pat->ptr[3]->ptr[0].inv);
  });
}

static void score_wrapper(char *pattern, char **input, int *expected) {
  slab_t *slab = make_default_slab();
  pattern_t *pat = parse_pattern(case_smart, false, pattern);
  int i = 0;
  char *one = input[i];
  while (one != NULL) {
    int score = get_score(one, pat, slab);
    assert_int_equal(expected[i], score);
    i++;
    one = input[i];
  }

  free_pattern(pat);
  free_slab(slab);
}

static test_fun_type score_integration(void **state) {
  {
    char *input[] = {"fzf", "main.c", "src/fzf", "fz/noooo", NULL};
    int expected[] = {0, 1, 0, 1};
    score_wrapper("!fzf", input, expected);
  }
  {
    char *input[] = {"src/fzf.c", "README.md", "lua/asdf", "test/test.c", NULL};
    int expected[] = {0, 1, 1, 0};
    score_wrapper("!fzf !test", input, expected);
  }
  {
    char *input[] = {"file ", "file lua", "lua", NULL};
    int expected[] = {0, 200, 0};
    score_wrapper("file\\ lua", input, expected);
  }
  {
    char *input[] = {"file with space", "file lua", "lua", "src", "test", NULL};
    int expected[] = {32, 32, 0, 0, 0};
    score_wrapper("\\ ", input, expected);
  }
  {
    char *input[] = {"src/fzf.h",       "README.md",       "build/fzf",
                     "lua/fzf_lib.lua", "Lua/fzf_lib.lua", NULL};
    int expected[] = {80, 0, 0, 0, 80};
    score_wrapper("'src | ^Lua", input, expected);
  }
  {
    char *input[] = {"lua/random_previewer", "README.md",
                     "previewers/utils.lua", "previewers/buffer.lua",
                     "previewers/term.lua",  NULL};
    int expected[] = {0, 0, 328, 328, 0};
    score_wrapper(".lua$ 'previewer !'term", input, expected);
  }
}

static void pos_wrapper(char *pattern, char **input, int **expected) {
  slab_t *slab = make_default_slab();
  pattern_t *pat = parse_pattern(case_smart, false, pattern);
  int i = 0;
  char *one = input[i];
  while (one != NULL) {
    position_t *pos = get_positions(one, pat, slab);
    for (size_t j = 0; j < pos->size; j++) {
      assert_int_equal(expected[i][j], pos->data[j]);
    }
    free_positions(pos);
    i++;
    one = input[i];
  }

  free_pattern(pat);
  free_slab(slab);
}

static test_fun_type pos_integration(void **state) {
  {
    char *input[] = {"src/fzf.c",       "src/fzf.h",
                     "lua/fzf_lib.lua", "lua/telescope/_extensions/fzf.lua",
                     "README.md",       NULL};
    int match1[] = {6, 5, 4};
    int match2[] = {6, 5, 4};
    int match3[] = {6, 5, 4};
    int match4[] = {28, 27, 26};
    int *expected[] = {match1, match2, match3, match4, NULL};
    pos_wrapper("fzf", input, expected);
  }
  {
    char *input[] = {"fzf", "main.c", "src/fzf", "fz/noooo", NULL};
    int *expected[] = {};
    pos_wrapper("!fzf", input, expected);
  }
  {
    char *input[] = {"src/fzf.c", "lua/fzf_lib.lua", "build/libfzf", NULL};
    int match1[] = {6, 5, 4};
    int *expected[] = {match1, NULL, NULL};
    pos_wrapper("fzf !lib", input, expected);
  }
  {
    char *input[] = {"src/fzf.c", "README.md", "lua/asdf", "test/test.c", NULL};
    int *expected[] = {};
    pos_wrapper("!fzf !test", input, expected);
  }
  {
    char *input[] = {"file ", "file lua", "lua", NULL};
    int match1[] = {7, 6, 5, 4, 3, 2, 1, 0};
    int *expected[] = {NULL, match1, NULL};
    pos_wrapper("file\\ lua", input, expected);
  }
  {
    char *input[] = {"file with space", "lul lua", "lua", "src", "test", NULL};
    int match1[] = {4};
    int match2[] = {3};
    int *expected[] = {match1, match2, NULL, NULL, NULL};
    pos_wrapper("\\ ", input, expected);
  }
  {
    char *input[] = {"src/fzf.h",       "README.md",       "build/fzf",
                     "lua/fzf_lib.lua", "Lua/fzf_lib.lua", NULL};
    int match1[] = {0, 1, 2};
    int match2[] = {0, 1, 2};
    int *expected[] = {match1, NULL, NULL, NULL, match2};
    pos_wrapper("'src | ^Lua", input, expected);
  }
  {
    char *input[] = {"lua/random_previewer", "README.md",
                     "previewers/utils.lua", "previewers/buffer.lua",
                     "previewers/term.lua",  NULL};
    int match1[] = {16, 17, 18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8};
    int match2[] = {17, 18, 19, 20, 0, 1, 2, 3, 4, 5, 6, 7, 8};
    int *expected[] = {NULL, NULL, match1, match2, NULL};
    pos_wrapper(".lua$ 'previewer !'term", input, expected);
  }
}

int main(void) {
  const struct CMUnitTest tests[] = {
      // Algorithms
      cmocka_unit_test(test_fuzzy_match_v2),
      cmocka_unit_test(test_fuzzy_match_v1),
      cmocka_unit_test(test_prefix_match),
      cmocka_unit_test(test_exact_match),
      cmocka_unit_test(test_suffix_match),
      cmocka_unit_test(test_equal_match),

      // Pattern
      cmocka_unit_test(test_parse_pattern),

      // Integration
      cmocka_unit_test(score_integration),
      cmocka_unit_test(pos_integration),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}