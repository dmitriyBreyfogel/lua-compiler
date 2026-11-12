#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class BlockStatement : public StatementNode {
public:
    BlockStatement();

    const Node* statements() const { return child("statements"); }
    const Node* returnStatement() const { return child("return"); }
};

} // namespace lua::ast
