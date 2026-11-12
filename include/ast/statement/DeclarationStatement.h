#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class DeclarationStatement : public StatementNode {
public:
    const Node* names() const { return child("names"); }
    const Node* values() const { return child("values"); }

protected:
    explicit DeclarationStatement(NodeType type);
};

} // namespace lua::ast
