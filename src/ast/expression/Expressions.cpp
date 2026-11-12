#include "ast/expression/Expressions.h"

#include <utility>

#include "ast/SupportNodes.h"
#include "ast/statement/BlockStatement.h"

namespace lua::ast {

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

void UnaryExpression::onChildAdded(const std::string& role, Node& child)
{
    if (role == "operand") {
        operand_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

BinaryExpression::BinaryExpression(std::string operation)
    : ExpressionNode(NodeType::BinaryExpression, std::move(operation))
{
}

void BinaryExpression::onChildAdded(const std::string& role, Node& child)
{
    if (role == "left") {
        left_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "right") {
        right_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

IndexExpression::IndexExpression()
    : ExpressionNode(NodeType::IndexExpression)
{
}

void IndexExpression::onChildAdded(const std::string& role, Node& child)
{
    if (role == "object") {
        object_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "index") {
        index_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

FieldExpression::FieldExpression(std::string kind)
    : ExpressionNode(NodeType::FieldExpression, std::move(kind))
{
}

void FieldExpression::onChildAdded(const std::string& role, Node& child)
{
    if (role == "object") {
        object_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "field") {
        field_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

CallExpression::CallExpression()
    : ExpressionNode(NodeType::CallExpression)
{
}

void CallExpression::onChildAdded(const std::string& role, Node& child)
{
    if (role == "function") {
        function_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "arguments") {
        arguments_ = dynamic_cast<ListNode*>(&child);
    }
}

MethodCallExpression::MethodCallExpression(std::string method)
    : ExpressionNode(NodeType::MethodCallExpression, std::move(method))
{
}

void MethodCallExpression::onChildAdded(const std::string& role, Node& child)
{
    if (role == "object") {
        object_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "arguments") {
        arguments_ = dynamic_cast<ListNode*>(&child);
    }
}

TableConstructorExpression::TableConstructorExpression()
    : ExpressionNode(NodeType::TableConstructor)
{
}

void TableConstructorExpression::onChildAdded(const std::string& role, Node& child)
{
    if (role == "fields") {
        fields_ = dynamic_cast<ListNode*>(&child);
    }
}

TableFieldExpression::TableFieldExpression()
    : ExpressionNode(NodeType::TableField)
{
}

void TableFieldExpression::onChildAdded(const std::string& role, Node& child)
{
    if (role == "key") {
        key_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "value") {
        value_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

FunctionExpression::FunctionExpression()
    : ExpressionNode(NodeType::FunctionExpression)
{
}

void FunctionExpression::onChildAdded(const std::string& role, Node& child)
{
    if (role == "parameters") {
        parameters_ = dynamic_cast<ListNode*>(&child);
    } else if (role == "body") {
        body_ = dynamic_cast<BlockStatement*>(&child);
    }
}

} // namespace lua::ast
