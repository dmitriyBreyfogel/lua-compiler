#include "ast/statement/LabelStatement.h"

#include <utility>

namespace lua::ast {

LabelStatement::LabelStatement(std::string name)
    : StatementNode(NodeType::LabelStatement, std::move(name))
{
}

} // namespace lua::ast
