#include "ast/statement/StatementNode.h"

#include <utility>

namespace lua::ast {

StatementNode::StatementNode(NodeType type, std::string value)
    : Node(type, std::move(value))
{
}

} // namespace lua::ast
