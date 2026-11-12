#pragma once

#include "ast/statement/ConditionalStatement.h"

namespace lua::ast {

class BlockStatement;

class ElseIfStatement : public ConditionalStatement {
public:
    ElseIfStatement();

    const BlockStatement* body() const { return body_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    BlockStatement* body_ = nullptr;
};

} // namespace lua::ast
