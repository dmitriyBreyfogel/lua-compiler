#pragma once

#include "ast/statement/LoopStatement.h"

namespace lua::ast {

class RepeatStatement : public LoopStatement {
public:
    RepeatStatement();

    const Node* condition() const { return child("condition"); }
};

} // namespace lua::ast
