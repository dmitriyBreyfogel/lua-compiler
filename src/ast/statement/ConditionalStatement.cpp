#include "ast/statement/ConditionalStatement.h"

#include "ast/expression/ExpressionNode.h"

namespace lua::ast {

ConditionalStatement::ConditionalStatement(NodeType type)
    : StatementNode(type)
{
}

void ConditionalStatement::onChildAdded(const std::string& role, Node& child)
{
    if (role == "condition") {
        condition_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

} // namespace lua::ast
