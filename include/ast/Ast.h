#pragma once

#include "ast/Node.h"
#include "ast/SupportNodes.h"
#include "ast/expression/LiteralExpressions.h"
#include "ast/expression/Expressions.h"
#include "ast/statement/Statements.h"

namespace lua::ast {

NodePtr makeNode(NodeType type, std::string value = {});
} // namespace lua::ast
