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

AssignmentStatement::AssignmentStatement()
    : StatementNode(NodeType::AssignmentStatement)
{
}

FunctionDeclarationStatement::FunctionDeclarationStatement(std::string scope)
    : StatementNode(NodeType::FunctionDeclaration, std::move(scope))
{
}

CallStatement::CallStatement()
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

BranchStatement::BranchStatement(NodeType type)
    : StatementNode(type)
{
}

LoopStatement::LoopStatement(NodeType type)
    : StatementNode(type)
{
}

ReturnStatement::ReturnStatement()
    : StatementNode(NodeType::ReturnStatement)
{
}

} // namespace lua::ast
