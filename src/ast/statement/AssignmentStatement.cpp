#include "ast/statement/AssignmentStatement.h"

#include "ast/SupportNodes.h"

namespace lua::ast {

AssignmentStatement::AssignmentStatement()
    : StatementNode(NodeType::AssignmentStatement)
{
}

void AssignmentStatement::onChildAdded(const std::string& role, Node& child)
{
    if (role == "targets") {
        targets_ = dynamic_cast<ListNode*>(&child);
    } else if (role == "values") {
        values_ = dynamic_cast<ListNode*>(&child);
    }
}

} // namespace lua::ast
