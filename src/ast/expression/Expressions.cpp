#include "ast/expression/Expressions.h"

#include <utility>

namespace lua::ast {

LiteralExpression::LiteralExpression(NodeType type, std::string value)
    : ExpressionNode(type, std::move(value))
{
}

IdentifierExpression::IdentifierExpression(std::string name)
    : ExpressionNode(NodeType::Identifier, std::move(name))
{
}

VarargExpression::VarargExpression(std::string name)
    : ExpressionNode(NodeType::VarargExpression, std::move(name))
{
}

UnaryExpression::UnaryExpression(std::string operation)
    : ExpressionNode(NodeType::UnaryExpression, std::move(operation))
{
}

BinaryExpression::BinaryExpression(std::string operation)
    : ExpressionNode(NodeType::BinaryExpression, std::move(operation))
{
}

IndexExpression::IndexExpression()
    : ExpressionNode(NodeType::IndexExpression)
{
}

FieldExpression::FieldExpression(std::string kind)
    : ExpressionNode(NodeType::FieldExpression, std::move(kind))
{
}

CallExpression::CallExpression()
    : ExpressionNode(NodeType::CallExpression)
{
}

MethodCallExpression::MethodCallExpression(std::string method)
    : ExpressionNode(NodeType::MethodCallExpression, std::move(method))
{
}

TableConstructorExpression::TableConstructorExpression()
    : ExpressionNode(NodeType::TableConstructor)
{
}

TableFieldExpression::TableFieldExpression()
    : ExpressionNode(NodeType::TableField)
{
}

FunctionExpression::FunctionExpression()
    : ExpressionNode(NodeType::FunctionExpression)
{
}

} // namespace lua::ast
