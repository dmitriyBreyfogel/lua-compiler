#pragma once

#include "ast/statement/LoopStatement.h"

namespace lua::ast {

class ExpressionNode;

class RepeatStatement : public LoopStatement {
public:
    RepeatStatement();

    const ExpressionNode* condition() const { return condition_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* condition_ = nullptr;
};

} // namespace lua::ast
