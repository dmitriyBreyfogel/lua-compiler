#pragma once

#include "ast/Node.h"

namespace lua::ast {

NodePtr createNode(NodeType type, std::string value = {});
} // namespace lua::ast
