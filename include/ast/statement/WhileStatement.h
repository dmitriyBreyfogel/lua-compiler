#pragma once

#include "ast/statement/LoopStatement.h"

namespace lua::ast {

class WhileStatement : public LoopStatement {
public:
    WhileStatement();

    const Node* condition() const { return child("condition"); }
};

} // namespace lua::ast
