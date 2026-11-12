#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class ConditionalStatement : public StatementNode {
public:
    const Node* condition() const { return child("condition"); }

protected:
    explicit ConditionalStatement(NodeType type);
};

} // namespace lua::ast
