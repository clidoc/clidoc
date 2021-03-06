
#include "gtest/gtest.h"
#include "clidoc/ast/parser_proxy.h"
#include "clidoc/ast/process_logic.h"
#include "clidoc/ast/ast_nodes.h"
#include "clidoc/ast/ast_visitor_helper.h"

using namespace clidoc;

TEST(process_logic, StructureOptimizerLogic) {
  auto and_1 = LogicAnd::Init();
  auto and_2 = LogicAnd::Init();
  auto command_ptr = Command::Init("whatever");
  and_1->AddChild(and_2);
  and_2->AddChild(command_ptr);
  
  StructureOptimizerLogic optimizer;
  auto visitor = GenerateVisitor<NonTerminalVisitor>(&optimizer);
  and_1->Accept(&visitor);

  EXPECT_EQ("LogicAnd(Command[whatever])", and_1->ToString());
}

TEST(process_logic, AmbiguityHandlerLogic) {
  auto xor_1 = LogicXor::Init();
  xor_1->AddChild(GroupedOptions::Init("-oFILE"));
  
  OptionBindingRecorder recorder;
  recorder.RecordBinding(
      Token(TerminalType::POSIX_OPTION, "-o"),
      Token(TerminalType::ARGUMENT, "FILE"));
  recorder.ProcessCachedBindings();

  AmbiguityHandlerLogic ambiguity_handler_logic(&recorder);
  auto visitor = GenerateVisitor<TerminalVisitor>(
      &ambiguity_handler_logic);
  xor_1->Accept(&visitor);
  EXPECT_EQ("LogicXor(LogicOr(PosixOption[-o], Argument[FILE]))",
            xor_1->ToString());
}

TEST(process_logic, TerminalTypeModifierLogic) {
  auto and_1 = LogicAnd::Init();
  and_1->AddChild(PosixOption::Init("-c"));
  and_1->AddChild(Command::Init(">whatever<"));
  and_1->AddChild(GnuOption::Init("--long"));

  TerminalTypeModifierLogic<Argument> callable;
  auto type_modifier = GenerateVisitor<TerminalVisitor>(&callable);
  and_1->Accept(&type_modifier);

  EXPECT_EQ("LogicAnd("
            "Argument[-c], Argument[>whatever<], Argument[--long])",
            and_1->ToString());
}

TEST(process_logic, DoubleHyphenHandlerLogic) {
  auto and_1 = LogicAnd::Init();
  and_1->AddChild(PosixOption::Init("-c"));
  and_1->AddChild(KDoubleHyphen::Init("--"));
  and_1->AddChild(PosixOption::Init("-c"));
  and_1->AddChild(Command::Init(">whatever<"));
  and_1->AddChild(GnuOption::Init("--long"));

  DoubleHyphenHandlerLogic double_dash_handler_logic;
  auto visitor = GenerateVisitor<TerminalVisitor>(
      &double_dash_handler_logic);
  and_1->Accept(&visitor);
  EXPECT_EQ("LogicAnd(PosixOption[-c], "
            "Argument[-c], Argument[>whatever<], Argument[--long])",
            and_1->ToString());
}

TEST(process_logic, FocusedElementCollectorLogic) {
  auto and_1 = LogicAnd::Init();
  and_1->AddChild(PosixOption::Init("-c"));
  and_1->AddChild(Argument::Init("ARG1"));
  and_1->AddChild(Argument::Init("ARG2"));
  and_1->AddChild(GnuOption::Init("--output"));
  and_1->AddChild(Argument::Init("FILE"));

  OptionBindingRecorder recorder;
  recorder.RecordBinding(
      Token(TerminalType::POSIX_OPTION, "-c"),
      Token(TerminalType::ARGUMENT, "ARG1"));
  recorder.RecordBinding(
      Token(TerminalType::GNU_OPTION, "--output"),
      Token(TerminalType::ARGUMENT, "FILE"));
  recorder.ProcessCachedBindings();

  FocusedElementCollectorLogic logic(&recorder);
  auto visitor = GenerateVisitor<TerminalVisitor>(&logic);
  and_1->Accept(&visitor);
  auto focused_elements = logic.GetFocusedElements();

  EXPECT_EQ(3u, focused_elements.size());
  auto iter = focused_elements.begin();
  EXPECT_EQ("--output", (iter++)->value());
  EXPECT_EQ("-c", (iter++)->value());
  EXPECT_EQ("ARG2", (iter++)->value()); 
  EXPECT_EQ(focused_elements.end(), iter); 
}

TEST(process_logic, BoundArgumentCleanerLogic) {
  auto and_1 = LogicAnd::Init();
  and_1->AddChild(PosixOption::Init("-c"));
  and_1->AddChild(Argument::Init("ARG1"));
  and_1->AddChild(Argument::Init("ARG2"));

  OptionBindingRecorder recorder;
  recorder.RecordBinding(
      Token(TerminalType::POSIX_OPTION, "-c"),
      Token(TerminalType::ARGUMENT, "ARG1"));
  recorder.ProcessCachedBindings();

  auto bound_arguments = recorder.GetBoundArguments();
  BoundArgumentCleanerLogic bound_argument_cleaner_logic(bound_arguments);
  auto visitor = GenerateVisitor<TerminalVisitor>(
      &bound_argument_cleaner_logic);
  and_1->Accept(&visitor);
  EXPECT_EQ("LogicAnd(PosixOption[-c], Argument[ARG2])",
            and_1->ToString());
}
