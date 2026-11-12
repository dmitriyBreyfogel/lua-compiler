#pragma once

#include "ast/expression/ExpressionNode.h"

namespace lua::ast {

class BlockStatement;
class ListNode;

class IdentifierExpression : public ExpressionNode {
public:
    explicit IdentifierExpression(std::string name);
};

class VarargExpression : public ExpressionNode {
public:
    explicit VarargExpression(std::string name = {});
};

class UnaryExpression : public ExpressionNode {
public:
    explicit UnaryExpression(std::string operation);

    const ExpressionNode* operand() const { return operand_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* operand_ = nullptr;
};

class BinaryExpression : public ExpressionNode {
public:
    explicit BinaryExpression(std::string operation);

    const ExpressionNode* left() const { return left_; }
    const ExpressionNode* right() const { return right_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* left_ = nullptr;
    ExpressionNode* right_ = nullptr;
};

class IndexExpression : public ExpressionNode {
public:
    IndexExpression();

    const ExpressionNode* object() const { return object_; }
    const ExpressionNode* index() const { return index_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* object_ = nullptr;
    ExpressionNode* index_ = nullptr;
};

class FieldExpression : public ExpressionNode {
public:
    explicit FieldExpression(std::string kind = {});

    const ExpressionNode* object() const { return object_; }
    const ExpressionNode* field() const { return field_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* object_ = nullptr;
    ExpressionNode* field_ = nullptr;
};

class CallExpression : public ExpressionNode {
public:
    CallExpression();

    const ExpressionNode* function() const { return function_; }
    const ListNode* arguments() const { return arguments_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* function_ = nullptr;
    ListNode* arguments_ = nullptr;
};

class MethodCallExpression : public ExpressionNode {
public:
    explicit MethodCallExpression(std::string method);

    const ExpressionNode* object() const { return object_; }
    const ListNode* arguments() const { return arguments_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* object_ = nullptr;
    ListNode* arguments_ = nullptr;
};

class TableConstructorExpression : public ExpressionNode {
public:
    TableConstructorExpression();

    const ListNode* fields() const { return fields_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ListNode* fields_ = nullptr;
};

class TableFieldExpression : public ExpressionNode {
public:
    TableFieldExpression();

    const ExpressionNode* key() const { return key_; }
    const ExpressionNode* fieldValue() const { return value_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ExpressionNode* key_ = nullptr;
    ExpressionNode* value_ = nullptr;
};

class FunctionExpression : public ExpressionNode {
public:
    FunctionExpression();

    const ListNode* parameters() const { return parameters_; }
    const BlockStatement* body() const { return body_; }

protected:
    void onChildAdded(const std::string& role, Node& child) override;

private:
    ListNode* parameters_ = nullptr;
    BlockStatement* body_ = nullptr;
};

} // namespace lua::ast
