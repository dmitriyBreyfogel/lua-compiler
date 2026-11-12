#include "ast/statement/ElseIfStatement.h"

#include "ast/statement/BlockStatement.h"

namespace lua::ast {

ElseIfStatement::ElseIfStatement()
    : ConditionalStatement(NodeType::ElseIfBranch)
{
}

void ElseIfStatement::onChildAdded(const std::string& role, Node& child)
{
    ConditionalStatement::onChildAdded(role, child);
    if (role == "body") {
        body_ = dynamic_cast<BlockStatement*>(&child);
    }
}

} // namespace lua::ast
