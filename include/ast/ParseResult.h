#pragma once

#include "ast/Ast.h"

namespace lua::ast {

void setParsedChunk(NodePtr chunk);
NodePtr takeParsedChunk();
void clearParsedChunk();

} // namespace lua::ast
