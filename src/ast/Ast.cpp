#include "ast/Ast.h"

#include <utility>

namespace lua::ast {

NodePtr createNode(NodeType type, std::string value)
{
    return std::make_unique<Node>(type, std::move(value));
}

} // namespace lua::ast
