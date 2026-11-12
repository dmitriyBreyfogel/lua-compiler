#pragma once

#include "ast/statement/ConditionalStatement.h"

namespace lua::ast {

class ElseIfStatement : public ConditionalStatement {
public:
    ElseIfStatement();

    const Node* body() const { return child("body"); }
};

} // namespace lua::ast
