#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class ExpressionNode;

class FunctionCallStatement : public StatementNode {
public:
    FunctionCallStatement();

    const ExpressionNode* call() const { return call_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* call_ = nullptr;
};

} // namespace lua::ast
