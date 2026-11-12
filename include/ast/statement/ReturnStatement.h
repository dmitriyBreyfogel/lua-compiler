#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class ReturnStatement : public StatementNode {
public:
    ReturnStatement();

    const Node* values() const { return child("values"); }
};

} // namespace lua::ast
