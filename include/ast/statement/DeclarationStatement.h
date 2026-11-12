#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class ListNode;

class DeclarationStatement : public StatementNode {
public:
    const ListNode* names() const { return names_; }
    const ListNode* values() const { return values_; }

protected:
    explicit DeclarationStatement(NodeType type);
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ListNode* names_ = nullptr;
    ListNode* values_ = nullptr;
};

} // namespace lua::ast
