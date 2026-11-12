#include "ast/expression/ExpressionNode.h"

#include <utility>

namespace lua::ast {

ExpressionNode::ExpressionNode(NodeType type, std::string value)
    : Node(type, std::move(value))
{
}

} // namespace lua::ast
