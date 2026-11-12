#include "ast/Ast.h"

#include <utility>

namespace lua::ast {

namespace {

std::uint64_t nextNodeId = 1;

}

Node::Node(NodeType type, std::string value)
    : id_(nextNodeId++), type_(type), value_(std::move(value))
{
}

ExpressionNode::ExpressionNode(NodeType type, std::string value)
    : Node(type, std::move(value))
{
}

StatementNode::StatementNode(NodeType type, std::string value)
    : Node(type, std::move(value))
{
}

BlockNode::BlockNode()
    : StatementNode(NodeType::Block)
{
}

ListNode::ListNode()
    : Node(NodeType::List)
{
}

DeclarationNameNode::DeclarationNameNode(std::string value)
    : Node(NodeType::DeclarationName, std::move(value))
{
}

AttributeNode::AttributeNode(std::string value)
    : Node(NodeType::Attribute, std::move(value))
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

NodePtr makeNode(NodeType type, std::string value)
{
    switch (type) {
    case NodeType::Block:
        return std::make_unique<BlockNode>();
    case NodeType::List:
        return std::make_unique<ListNode>();
    case NodeType::DeclarationName:
        return std::make_unique<DeclarationNameNode>(std::move(value));
    case NodeType::Attribute:
        return std::make_unique<AttributeNode>(std::move(value));
    case NodeType::IntegerLiteral:
    case NodeType::FloatLiteral:
    case NodeType::StringLiteral:
    case NodeType::BooleanLiteral:
    case NodeType::NilLiteral:
    case NodeType::VarargExpression:
    case NodeType::Identifier:
    case NodeType::UnaryExpression:
    case NodeType::BinaryExpression:
    case NodeType::IndexExpression:
    case NodeType::FieldExpression:
    case NodeType::CallExpression:
    case NodeType::MethodCallExpression:
    case NodeType::TableConstructor:
    case NodeType::TableField:
    case NodeType::FunctionExpression:
        return std::make_unique<ExpressionNode>(type, std::move(value));
    default:
        return std::make_unique<StatementNode>(type, std::move(value));
    }
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
