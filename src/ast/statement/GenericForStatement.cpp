#include "ast/statement/GenericForStatement.h"

#include "ast/SupportNodes.h"

namespace lua::ast {

GenericForStatement::GenericForStatement()
    : LoopStatement(NodeType::GenericForStatement)
{
}

void GenericForStatement::onChildAdded(const std::string& role, Node& child)
{
    LoopStatement::onChildAdded(role, child);
    if (role == "names") {
        names_ = dynamic_cast<ListNode*>(&child);
    } else if (role == "iterators") {
        iterators_ = dynamic_cast<ListNode*>(&child);
    }
}

} // namespace lua::ast
