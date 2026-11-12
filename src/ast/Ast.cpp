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

std::uint64_t Node::id() const
{
    return id_;
}

NodeKind Node::kind() const
{
    return kind_;
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

NodePtr makeNode(NodeKind kind, std::string value)
{
    return std::make_unique<Node>(kind, std::move(value));
}

const char* nodeKindName(NodeKind kind)
{
    switch (kind) {
    case NodeKind::Chunk: return "Chunk";
    case NodeKind::Block: return "Block";
    case NodeKind::List: return "List";
    case NodeKind::DeclarationName: return "DeclarationName";
    case NodeKind::Attribute: return "Attribute";
    case NodeKind::EmptyStatement: return "EmptyStatement";
    case NodeKind::BreakStatement: return "BreakStatement";
    case NodeKind::FunctionDeclaration: return "FunctionDeclaration";
    case NodeKind::LocalDeclaration: return "LocalDeclaration";
    case NodeKind::GlobalDeclaration: return "GlobalDeclaration";
    case NodeKind::GlobalAllDeclaration: return "GlobalAllDeclaration";
    case NodeKind::AssignmentStatement: return "AssignmentStatement";
    case NodeKind::CallStatement: return "CallStatement";
    case NodeKind::LabelStatement: return "LabelStatement";
    case NodeKind::GotoStatement: return "GotoStatement";
    case NodeKind::IfStatement: return "IfStatement";
    case NodeKind::ElseIfBranch: return "ElseIfBranch";
    case NodeKind::NumericForStatement: return "NumericForStatement";
    case NodeKind::GenericForStatement: return "GenericForStatement";
    case NodeKind::WhileStatement: return "WhileStatement";
    case NodeKind::RepeatStatement: return "RepeatStatement";
    case NodeKind::ReturnStatement: return "ReturnStatement";
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
