#pragma once

#include "ast/Node.h"

namespace lua::ast {

class StatementNode : public Node {
public:
    StatementNode(NodeType type, std::string value = {});
};

} // namespace lua::ast
