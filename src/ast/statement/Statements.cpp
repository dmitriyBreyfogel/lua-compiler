#include "ast/statement/Statements.h"

#include <utility>

namespace lua::ast {

BlockStatement::BlockStatement()
    : StatementNode(NodeType::Block)
{
}

EmptyStatement::EmptyStatement()
    : StatementNode(NodeType::EmptyStatement)
{
}

BreakStatement::BreakStatement()
    : StatementNode(NodeType::BreakStatement)
{
}

DeclarationStatement::DeclarationStatement(NodeType type)
    : StatementNode(type)
{
}

LocalDeclarationStatement::LocalDeclarationStatement()
    : DeclarationStatement(NodeType::LocalDeclaration)
{
}

GlobalDeclarationStatement::GlobalDeclarationStatement()
    : DeclarationStatement(NodeType::GlobalDeclaration)
{
}

GlobalAllDeclarationStatement::GlobalAllDeclarationStatement()
    : DeclarationStatement(NodeType::GlobalAllDeclaration)
{
}

AssignmentStatement::AssignmentStatement()
    : StatementNode(NodeType::AssignmentStatement)
{
}

FunctionDeclarationStatement::FunctionDeclarationStatement(std::string scope)
    : StatementNode(NodeType::FunctionDeclaration, std::move(scope))
{
}

FunctionCallStatement::FunctionCallStatement()
    : StatementNode(NodeType::CallStatement)
{
}

LabelStatement::LabelStatement(std::string name)
    : StatementNode(NodeType::LabelStatement, std::move(name))
{
}

GotoStatement::GotoStatement(std::string name)
    : StatementNode(NodeType::GotoStatement, std::move(name))
{
}

ConditionalStatement::ConditionalStatement(NodeType type)
    : StatementNode(type)
{
}

IfStatement::IfStatement()
    : ConditionalStatement(NodeType::IfStatement)
{
}

ElseIfStatement::ElseIfStatement()
    : ConditionalStatement(NodeType::ElseIfBranch)
{
}

LoopStatement::LoopStatement(NodeType type)
    : StatementNode(type)
{
}

NumericForStatement::NumericForStatement()
    : LoopStatement(NodeType::NumericForStatement)
{
}

GenericForStatement::GenericForStatement()
    : LoopStatement(NodeType::GenericForStatement)
{
}

WhileStatement::WhileStatement()
    : LoopStatement(NodeType::WhileStatement)
{
}

RepeatStatement::RepeatStatement()
    : LoopStatement(NodeType::RepeatStatement)
{
}

ReturnStatement::ReturnStatement()
    : StatementNode(NodeType::ReturnStatement)
{
}

} // namespace lua::ast
