#pragma once

#include "ast/statement/LoopStatement.h"

namespace lua::ast {

class GenericForStatement : public LoopStatement {
public:
    GenericForStatement();

    const Node* names() const { return child("names"); }
    const Node* iterators() const { return child("iterators"); }
};

} // namespace lua::ast
