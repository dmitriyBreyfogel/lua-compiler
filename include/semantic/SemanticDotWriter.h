#pragma once

#include <iosfwd>

#include "ast/Ast.h"
#include "semantic/SemanticAnalyzer.h"

namespace lua::semantic {

class SemanticDotWriter {
public:
    static void writeTree(std::ostream& output, const ast::Node& block, const SemanticResult& result);
};

} // namespace lua::semantic
