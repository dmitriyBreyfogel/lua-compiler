#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class ExpressionNode;

class ConditionalStatement : public StatementNode {
public:
    const ExpressionNode* condition() const { return condition_; }

protected:
    explicit ConditionalStatement(NodeType type);
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* condition_ = nullptr;
};

} // namespace lua::ast
