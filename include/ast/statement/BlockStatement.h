#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class ListNode;
class ReturnStatement;

class BlockStatement : public StatementNode {
public:
    BlockStatement();

    const ListNode* statements() const { return statements_; }
    const ReturnStatement* returnStatement() const { return returnStatement_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ListNode* statements_ = nullptr;
    ReturnStatement* returnStatement_ = nullptr;
};

} // namespace lua::ast
