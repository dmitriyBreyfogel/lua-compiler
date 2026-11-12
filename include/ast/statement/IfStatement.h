#pragma once

#include "ast/statement/ConditionalStatement.h"

namespace lua::ast {

class IfStatement : public ConditionalStatement {
public:
    IfStatement();

    const Node* thenBlock() const { return child("then"); }
    const Node* elseIfBranches() const { return child("elseif"); }
    const Node* elseBlock() const { return child("else"); }
};

} // namespace lua::ast
