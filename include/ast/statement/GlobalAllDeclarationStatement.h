#pragma once

#include "ast/statement/DeclarationStatement.h"

namespace lua::ast {

class AttributeNode;

class GlobalAllDeclarationStatement : public DeclarationStatement {
public:
    GlobalAllDeclarationStatement();

    const AttributeNode* attribute() const { return attribute_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    AttributeNode* attribute_ = nullptr;
};

} // namespace lua::ast
