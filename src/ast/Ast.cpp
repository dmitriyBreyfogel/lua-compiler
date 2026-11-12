#include "ast/Ast.h"

#include <utility>

namespace lua::ast {

namespace {

std::uint64_t nextNodeId = 1;

}

Node::Node(NodeKind kind, std::string value)
    : id_(nextNodeId++), kind_(kind), value_(std::move(value))
{
}

std::uint64_t Node::id() const noexcept
{
    return id_;
}

NodeKind Node::kind() const noexcept
{
    return kind_;
}

const std::string& Node::value() const noexcept
{
    return value_;
}

const std::vector<Child>& Node::children() const noexcept
{
    return children_;
}

Node& Node::addChild(std::string role, NodePtr child)
{
    children_.push_back({std::move(role), std::move(child)});
    return *this;
}

Node& Node::addChild(std::string role, Node* child)
{
    return addChild(std::move(role), NodePtr(child));
}

NodePtr makeNode(NodeKind kind, std::string value)
{
    return std::make_unique<Node>(kind, std::move(value));
}

const char* nodeKindName(NodeKind kind) noexcept
{
    switch (kind) {
    case NodeKind::IntegerLiteral: return "IntegerLiteral";
    case NodeKind::FloatLiteral: return "FloatLiteral";
    case NodeKind::StringLiteral: return "StringLiteral";
    case NodeKind::BooleanLiteral: return "BooleanLiteral";
    case NodeKind::NilLiteral: return "NilLiteral";
    case NodeKind::VarargExpression: return "VarargExpression";
    case NodeKind::Identifier: return "Identifier";
    case NodeKind::UnaryExpression: return "UnaryExpression";
    case NodeKind::BinaryExpression: return "BinaryExpression";
    case NodeKind::IndexExpression: return "IndexExpression";
    case NodeKind::FieldExpression: return "FieldExpression";
    case NodeKind::CallExpression: return "CallExpression";
    case NodeKind::MethodCallExpression: return "MethodCallExpression";
    case NodeKind::TableConstructor: return "TableConstructor";
    case NodeKind::TableField: return "TableField";
    case NodeKind::FunctionExpression: return "FunctionExpression";
    }

    return "Unknown";
}

} // namespace lua::ast
