
#include "test_utils-inl.h"

TEST(tokenizer_for_argv, token_type) {
  CheckTokenTypes(
      "--", {
        TerminalType::K_DOUBLE_HYPHEN,
      });
  CheckTokenTypes(
      "--long SOME_VALUE", {
        TerminalType::GNU_OPTION,
        TerminalType::ARGUMENT,
      });
  CheckTokenTypes(
      "-c --long -afjb", {
      TerminalType::POSIX_OPTION,
      TerminalType::GNU_OPTION,
      TerminalType::GROUPED_OPTIONS,
    });
  CheckTokenTypes(
      "-f test_file.py FILE abc 42.19243", {
        TerminalType::POSIX_OPTION,
        TerminalType::ARGUMENT,
        TerminalType::ARGUMENT,
        TerminalType::ARGUMENT,
        TerminalType::ARGUMENT,
      });
}

TEST(tokenizer_for_argv, raw_token_type) {
  CheckRawTokenTypes(
      "--long = SOME_VALUE", {
        TypeID::GNU_OPTION,
        TypeID::K_EQUAL_SIGN,
        TypeID::ARGUMENT,
      });
}

TEST(tokenizer_for_argv, ambiguous) {
  // such kind of ambiguity should be handle after tokenized.
  CheckTokenTypes(
      "--long = SOME_VALUE", {
        TerminalType::GNU_OPTION,
        TerminalType::ARGUMENT,
      });
  CheckTokenTypes(
      "--long= SOME_VALUE", {
        TerminalType::ARGUMENT,
        TerminalType::ARGUMENT,
      });
  CheckTokenTypes(
      "--long =SOME_VALUE", {
        TerminalType::GNU_OPTION,
        TerminalType::ARGUMENT,
      });
  CheckTokenTypes(
      "--long=SOME_VALUE", {
        TerminalType::ARGUMENT,
      });
}

TEST(tokenizer_for_argv, token_value) {
  CheckTokenValues(
      "-f test_file.py FILE abc 42.19243", 
      {"-f", "test_file.py", "FILE", "abc", "42.19243"});
}

TEST(tokenizer_for_argv, raw_token_value) {
  CheckRawTokenValues(
      "-f test_file.py FILE abc 42.19243", 
      {"-f", "test_file.py", "FILE", "abc", "42.19243"});
}