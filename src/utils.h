#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <memory>
#include <string>
#include <vector>

namespace clidoc {

// Defines terminal types.
enum class TerminalType {
  OPTION_ARGUEMENT,
  GNU_OPTION,
  OPERAND,
  COMMENT,
  ARGUMENT,
  K_OPTIONS,
  K_DOUBLE_HYPHEN,
  OPTION_DEFAULT_VALUE,
  POSIX_OPTION,
  GROUPED_OPTIONS,
  // for terminals that would not be instaniation.
  OTHER,
};

// Defines non-terminal types.
enum class NonTerminalType {
  // logical.
  LOGIX_AND,
  LOGIC_XOR,
  LOGIC_OPTIONAL,
  LOGIC_ONEORMORE,
  LOGIC_EMPTY,
  // general container.
  GENERAL_CONTAINER,
  // start node.
  DOC,
};

class Token {
 public:
  // constructors.
  Token(TerminalType type)
      : has_value_(false), type_(type) { /* empty */ }
  Token(TerminalType type, const std::string &value)
      : has_value_(true), type_(type), value_(value) { /* empty */ }

  bool has_value() const { return has_value_; }
  std::string value() const { return value_; }
  TerminalType type() const { return type_; }

 private:
  const bool has_value_;
  const TerminalType type_;
  const std::string value_;
};

// Contains tokenized arguments during semantic analysis.
class TokenInProcessCollection {
 public:
};

// Interface for symbols in parsing tree.
class NodeInterface {
 public:
  virtual bool ProcessToken(TokenInProcessCollection *token_collection) {
    throw "NotImplementedError.";
  }
  virtual ~NodeInterface() { /* empty */ }
};

using SharedPtrNode = std::shared_ptr<NodeInterface>;
using VecSharedPtrNode = std::vector<SharedPtrNode>;

template <typename Derived>
class SharedPtrNodeInterface : public NodeInterface {
 public:
  // type alias for shared_ptr.
  using SharedPtr = std::shared_ptr<Derived>;
  // function for init shared_ptr.
  template <typename... Args>
  static SharedPtr Init(Args&&... args);
};

template <typename Derived>
template <typename... Args>
typename SharedPtrNodeInterface<Derived>::SharedPtr
SharedPtrNodeInterface<Derived>::Init(Args&&... args) {
  return std::make_shared<Derived>(args...);
}

// Template for terminal types.
template <TerminalType T>
class Terminal : public SharedPtrNodeInterface<Terminal<T>> {
 public:
  Terminal(const Token &token) : token_(token) { /* empty */ }
  bool ProcessToken(TokenInProcessCollection *token_collection) override;

  const Token token_;
};

// Template for non-terminal types.
template <NonTerminalType T>
class NonTerminal : public SharedPtrNodeInterface<NonTerminal<T>> {
 public:
  bool ProcessToken(TokenInProcessCollection *token_collection) override;

  // Container of symbols.
  VecSharedPtrNode children_;
};

// Terminal classes.
using OptionArguement    = Terminal<TerminalType::OPTION_ARGUEMENT>;
using GnuOption          = Terminal<TerminalType::GNU_OPTION>;
using Operand            = Terminal<TerminalType::OPERAND>;
using Comment            = Terminal<TerminalType::COMMENT>;
using OptionDefaultValue = Terminal<TerminalType::OPTION_DEFAULT_VALUE>;
using PosixOption        = Terminal<TerminalType::POSIX_OPTION>;
using GroupedOptions     = Terminal<TerminalType::GROUPED_OPTIONS>;
using KDoubleHyphen      = Terminal<TerminalType::K_DOUBLE_HYPHEN>;
using KOptions           = Terminal<TerminalType::K_OPTIONS>;

// Non-terminal classes.
using Doc                = NonTerminal<NonTerminalType::DOC>;
using LogicAnd           = NonTerminal<NonTerminalType::LOGIX_AND>;
using LogicXor           = NonTerminal<NonTerminalType::LOGIC_XOR>;
using LogicOptional      = NonTerminal<NonTerminalType::LOGIC_OPTIONAL>;
using LogicOneOrMore     = NonTerminal<NonTerminalType::LOGIC_ONEORMORE>;

// For capturing option bindings.
class OptionBinding : public SharedPtrNodeInterface<OptionBinding> {
 public:
  // binding without argument, for synonym options, i.e. "-h, --help".
  OptionBinding(const Token &token_option)
      : token_option_(token_option) { /* empty */ }
  // binding option and option_argument.
  OptionBinding(const Token &token_option,
                const Token &token_optin_argument)
      : token_option_(token_option),
        token_option_argument_(token_optin_argument) { /* empty */ }
  const Token token_option_;
  const Token token_option_argument_ = Token(TerminalType::OTHER);
};

// For capturing default value of option(s) argument.
class DefaultValue : public SharedPtrNodeInterface<DefaultValue> {
 public:
  DefaultValue() = default;
  DefaultValue(const Token &default_value)
      : default_value_(default_value) { /* empty */ }
  const Token default_value_ = Token(TerminalType::OTHER);
};

using GeneralContainer = NonTerminal<NonTerminalType::GENERAL_CONTAINER>;

}  // namespace clidoc 

#include "utils-inl.h"

#endif
