#pragma once

#include <iosfwd>
#include <string>

#include "ast/Ast.h"

namespace lua::ast {

class DotWriter final {
public:
    static void writeExpression(std::ostream& output, const Node& root,
                                const std::string& graphName = "ast");
};

} // namespace lua::ast
