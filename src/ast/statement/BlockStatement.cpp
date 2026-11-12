#include "ast/statement/BlockStatement.h"

#include "ast/SupportNodes.h"
#include "ast/statement/ReturnStatement.h"

namespace lua::ast {

BlockStatement::BlockStatement()
    : StatementNode(NodeType::Block)
{
}

void BlockStatement::onChildAdded(const std::string& role, Node& child)
{
    if (role == "statements") {
        statements_ = dynamic_cast<ListNode*>(&child);
    } else if (role == "return") {
        returnStatement_ = dynamic_cast<ReturnStatement*>(&child);
    }
}

} // namespace lua::ast
