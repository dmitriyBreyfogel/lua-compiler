#pragma once

#include "ast/statement/LoopStatement.h"

namespace lua::ast {

class ExpressionNode;

class NumericForStatement : public LoopStatement {
public:
    NumericForStatement();

    const ExpressionNode* variable() const { return variable_; }
    const ExpressionNode* start() const { return start_; }
    const ExpressionNode* finish() const { return finish_; }
    const ExpressionNode* step() const { return step_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* variable_ = nullptr;
    ExpressionNode* start_ = nullptr;
    ExpressionNode* finish_ = nullptr;
    ExpressionNode* step_ = nullptr;
};

} // namespace lua::ast
