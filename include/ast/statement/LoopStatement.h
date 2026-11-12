#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class BlockStatement;

class LoopStatement : public StatementNode {
public:
    const BlockStatement* body() const { return body_; }

protected:
    explicit LoopStatement(NodeType type);
    void onChildAdded(const std::string& role, Node& child) override;

private:
    BlockStatement* body_ = nullptr;
};

} // namespace lua::ast
