#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class BreakStatement : public StatementNode {
public:
    BreakStatement();
};

} // namespace lua::ast
