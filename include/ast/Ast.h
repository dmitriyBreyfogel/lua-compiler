#pragma once

#include "ast/Node.h"
#include "ast/expression/Expressions.h"
#include "ast/statement/Statements.h"

namespace lua::ast {

class ListNode : public Node {
public:
    ListNode();
};

class DeclarationNameNode : public Node {
public:
    explicit DeclarationNameNode(std::string value);
};

class AttributeNode : public Node {
public:
    explicit AttributeNode(std::string value);
};

NodePtr makeNode(NodeType type, std::string value = {});
} // namespace lua::ast
