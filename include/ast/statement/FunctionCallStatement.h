#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class FunctionCallStatement : public StatementNode {
public:
    FunctionCallStatement();

    const Node* call() const { return child("call"); }
};

} // namespace lua::ast
