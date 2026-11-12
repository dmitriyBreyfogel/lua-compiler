#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class BlockStatement;
class ExpressionNode;
class ListNode;

class FunctionDeclarationStatement : public StatementNode {
public:
    explicit FunctionDeclarationStatement(std::string scope);

    const ExpressionNode* name() const { return name_; }
    const ListNode* parameters() const { return parameters_; }
    const BlockStatement* body() const { return body_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* name_ = nullptr;
    ListNode* parameters_ = nullptr;
    BlockStatement* body_ = nullptr;
};

} // namespace lua::ast
