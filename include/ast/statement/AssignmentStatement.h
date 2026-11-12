#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class ListNode;

class AssignmentStatement : public StatementNode {
public:
    AssignmentStatement();

    const ListNode* targets() const { return targets_; }
    const ListNode* values() const { return values_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ListNode* targets_ = nullptr;
    ListNode* values_ = nullptr;
};

} // namespace lua::ast
