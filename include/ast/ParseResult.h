#pragma once

#include "ast/Ast.h"

namespace lua::ast {

void setParsedBlock(Node* block);
NodePtr takeParsedBlock();
void clearParsedBlock();

} // namespace lua::ast
