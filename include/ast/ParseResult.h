#pragma once

#include "ast/Ast.h"

namespace lua::ast {

void setParsedBlock(NodePtr block);
NodePtr takeParsedBlock();
void clearParsedBlock();

} // namespace lua::ast
