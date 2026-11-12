#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class AssignmentStatement : public StatementNode {
public:
    AssignmentStatement();

    const Node* targets() const { return child("targets"); }
    const Node* values() const { return child("values"); }
};

} // namespace lua::ast
