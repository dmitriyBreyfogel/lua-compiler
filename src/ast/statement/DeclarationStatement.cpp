#include "ast/statement/DeclarationStatement.h"

#include "ast/SupportNodes.h"

namespace lua::ast {

DeclarationStatement::DeclarationStatement(NodeType type)
    : StatementNode(type)
{
}

void DeclarationStatement::onChildAdded(const std::string& role, Node& child)
{
    if (role == "names") {
        names_ = dynamic_cast<ListNode*>(&child);
    } else if (role == "values") {
        values_ = dynamic_cast<ListNode*>(&child);
    }
}

} // namespace lua::ast
