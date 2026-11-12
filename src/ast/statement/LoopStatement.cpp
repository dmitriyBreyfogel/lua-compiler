#include "ast/statement/LoopStatement.h"

#include "ast/statement/BlockStatement.h"

namespace lua::ast {

LoopStatement::LoopStatement(NodeType type)
    : StatementNode(type)
{
}

void LoopStatement::onChildAdded(const std::string& role, Node& child)
{
    if (role == "body") {
        body_ = dynamic_cast<BlockStatement*>(&child);
    }
}

} // namespace lua::ast
