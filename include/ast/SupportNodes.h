#pragma once

#include "ast/Node.h"

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

} // namespace lua::ast
