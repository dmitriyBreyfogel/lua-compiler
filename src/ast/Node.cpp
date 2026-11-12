#include "ast/Node.h"

#include <utility>

namespace lua::ast {

namespace {

std::uint64_t nextNodeId = 1;

}

Node::Node(NodeType type, std::string value)
    : id_(nextNodeId++), type_(type), value_(std::move(value))
{
}

std::uint64_t Node::id() const
{
    return id_;
}

NodeType Node::type() const
{
    return type_;
}

const std::string& Node::value() const
{
    return value_;
}

const std::vector<Child>& Node::children() const
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

const char* nodeTypeName(NodeType type)
{
    switch (type) {
    case NodeType::Block: return "Block";
    case NodeType::List: return "List";
    case NodeType::DeclarationName: return "DeclarationName";
    case NodeType::Attribute: return "Attribute";
    case NodeType::EmptyStatement: return "EmptyStatement";
    case NodeType::BreakStatement: return "BreakStatement";
    case NodeType::FunctionDeclaration: return "FunctionDeclaration";
    case NodeType::LocalDeclaration: return "LocalDeclaration";
    case NodeType::GlobalDeclaration: return "GlobalDeclaration";
    case NodeType::GlobalAllDeclaration: return "GlobalAllDeclaration";
    case NodeType::AssignmentStatement: return "AssignmentStatement";
    case NodeType::CallStatement: return "CallStatement";
    case NodeType::LabelStatement: return "LabelStatement";
    case NodeType::GotoStatement: return "GotoStatement";
    case NodeType::IfStatement: return "IfStatement";
    case NodeType::ElseIfBranch: return "ElseIfBranch";
    case NodeType::NumericForStatement: return "NumericForStatement";
    case NodeType::GenericForStatement: return "GenericForStatement";
    case NodeType::WhileStatement: return "WhileStatement";
    case NodeType::RepeatStatement: return "RepeatStatement";
    case NodeType::ReturnStatement: return "ReturnStatement";
    case NodeType::IntegerLiteral: return "IntegerLiteral";
    case NodeType::FloatLiteral: return "FloatLiteral";
    case NodeType::StringLiteral: return "StringLiteral";
    case NodeType::BooleanLiteral: return "BooleanLiteral";
    case NodeType::NilLiteral: return "NilLiteral";
    case NodeType::VarargExpression: return "VarargExpression";
    case NodeType::Identifier: return "Identifier";
    case NodeType::UnaryExpression: return "UnaryExpression";
    case NodeType::BinaryExpression: return "BinaryExpression";
    case NodeType::IndexExpression: return "IndexExpression";
    case NodeType::FieldExpression: return "FieldExpression";
    case NodeType::CallExpression: return "CallExpression";
    case NodeType::MethodCallExpression: return "MethodCallExpression";
    case NodeType::TableConstructor: return "TableConstructor";
    case NodeType::TableField: return "TableField";
    case NodeType::FunctionExpression: return "FunctionExpression";
    }

    return "Unknown";
}

} // namespace lua::ast
