#include "ast/SupportNodes.h"

#include <utility>

namespace lua::ast {

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

} // namespace lua::ast
