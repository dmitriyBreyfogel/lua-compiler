#pragma once

#include "ast/statement/LoopStatement.h"

namespace lua::ast {

class ListNode;

class GenericForStatement : public LoopStatement {
public:
    GenericForStatement();

    const ListNode* names() const { return names_; }
    const ListNode* iterators() const { return iterators_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ListNode* names_ = nullptr;
    ListNode* iterators_ = nullptr;
};

} // namespace lua::ast
