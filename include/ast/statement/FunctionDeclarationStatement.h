#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class FunctionDeclarationStatement : public StatementNode {
public:
    explicit FunctionDeclarationStatement(std::string scope);

    const Node* name() const { return child("name"); }
    const Node* parameters() const { return child("parameters"); }
    const Node* body() const { return child("body"); }
};

} // namespace lua::ast
