#include "ast/statement/GotoStatement.h"

#include <utility>

namespace lua::ast {

GotoStatement::GotoStatement(std::string name)
    : StatementNode(NodeType::GotoStatement, std::move(name))
{
}

} // namespace lua::ast
