#include "ast/Ast.h"

#include <utility>

namespace lua::ast {

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

} // namespace lua::ast
