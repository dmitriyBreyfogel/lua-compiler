#pragma once

#include <iosfwd>
#include <string>

#include "ast/Ast.h"

namespace lua::ast {

class DotWriter {
public:
    static void writeTree(std::ostream& output, const Node& root,
                          const std::string& graphName = "ast");
};

} // namespace lua::ast
