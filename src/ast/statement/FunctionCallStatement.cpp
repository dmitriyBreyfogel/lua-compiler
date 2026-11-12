#include "ast/statement/FunctionCallStatement.h"

#include "ast/expression/ExpressionNode.h"

namespace lua::ast {

FunctionCallStatement::FunctionCallStatement()
    : StatementNode(NodeType::CallStatement)
{
}

void FunctionCallStatement::onChildAdded(const std::string& role, Node& child)
{
    if (role == "call") {
        call_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

} // namespace lua::ast
