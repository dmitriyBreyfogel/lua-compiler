#pragma once

#include "ast/statement/ConditionalStatement.h"

namespace lua::ast {

class BlockStatement;
class ListNode;

class IfStatement : public ConditionalStatement {
public:
    IfStatement();

    const BlockStatement* thenBlock() const { return thenBlock_; }
    const ListNode* elseIfBranches() const { return elseIfBranches_; }
    const BlockStatement* elseBlock() const { return elseBlock_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    BlockStatement* thenBlock_ = nullptr;
    ListNode* elseIfBranches_ = nullptr;
    BlockStatement* elseBlock_ = nullptr;
};

} // namespace lua::ast
