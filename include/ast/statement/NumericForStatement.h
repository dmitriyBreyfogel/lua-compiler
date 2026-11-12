#pragma once

#include "ast/statement/LoopStatement.h"

namespace lua::ast {

class NumericForStatement : public LoopStatement {
public:
    NumericForStatement();

    const Node* variable() const { return child("variable"); }
    const Node* start() const { return child("start"); }
    const Node* finish() const { return child("finish"); }
    const Node* step() const { return child("step"); }
};

} // namespace lua::ast
