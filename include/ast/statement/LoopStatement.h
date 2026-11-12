#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class LoopStatement : public StatementNode {
public:
    const Node* body() const { return child("body"); }

protected:
    explicit LoopStatement(NodeType type);
};

} // namespace lua::ast
