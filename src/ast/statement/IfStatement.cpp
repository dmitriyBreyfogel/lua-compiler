#include "ast/statement/IfStatement.h"

#include "ast/SupportNodes.h"
#include "ast/statement/BlockStatement.h"

namespace lua::ast {

IfStatement::IfStatement()
    : ConditionalStatement(NodeType::IfStatement)
{
}

void IfStatement::onChildAdded(const std::string& role, Node& child)
{
    ConditionalStatement::onChildAdded(role, child);
    if (role == "then") {
        thenBlock_ = dynamic_cast<BlockStatement*>(&child);
    } else if (role == "elseif") {
        elseIfBranches_ = dynamic_cast<ListNode*>(&child);
    } else if (role == "else") {
        elseBlock_ = dynamic_cast<BlockStatement*>(&child);
    }
}

} // namespace lua::ast
