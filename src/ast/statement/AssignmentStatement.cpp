#include "ast/statement/AssignmentStatement.h"

namespace lua::ast {

AssignmentStatement::AssignmentStatement()
    : StatementNode(NodeType::AssignmentStatement)
{
}

} // namespace lua::ast
