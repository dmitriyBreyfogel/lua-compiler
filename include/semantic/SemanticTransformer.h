#pragma once

#include "ast/Ast.h"

namespace lua::semantic {

class SemanticTransformer {
public:
    void normalize(ast::Node& block);
};

} // namespace lua::semantic
