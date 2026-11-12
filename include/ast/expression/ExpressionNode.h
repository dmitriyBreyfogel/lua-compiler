#pragma once

#include "ast/Node.h"

namespace lua::ast {

class ExpressionNode : public Node {
public:
    ExpressionNode(NodeType type, std::string value = {});
};

} // namespace lua::ast
