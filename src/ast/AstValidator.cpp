#include "ast/AstValidator.h"

namespace lua::ast {

class Validator {
public:
    AstValidationResult run(const Node& root)
    {
        if (root.type() != NodeType::Block) {
            error(root, "root node must be Block");
        }
        validateNode(root);
        return std::move(result_);
    }

private:
    void error(const Node& node, const std::string& message)
    {
        result_.diagnostics_.push_back({node.id(), message});
    }

    const Node* required(const Node& node, const std::string& role)
    {
        const Node* value = node.child(role);
        if (value == nullptr) {
            error(node, "missing child: " + role);
        }
        return value;
    }

    void expectList(const Node& node, const std::string& role, bool requiredRole = true)
    {
        const Node* value = requiredRole ? required(node, role) : node.child(role);
        if (value != nullptr && value->type() != NodeType::List) {
            error(node, "child must be List: " + role);
        }
    }

    void expectExpression(const Node& node, const std::string& role)
    {
        const Node* value = required(node, role);
        if (value != nullptr && !isExpression(value->type())) {
            error(node, "child must be expression: " + role);
        }
    }

    void expectBlock(const Node& node, const std::string& role)
    {
        const Node* value = required(node, role);
        if (value != nullptr && value->type() != NodeType::Block) {
            error(node, "child must be Block: " + role);
        }
    }

    void validateNode(const Node& node)
    {
        for (const Child& child : node.children()) {
            if (child.node == nullptr) {
                error(node, "child is null: " + child.role);
            } else {
                validateNode(*child.node);
            }
        }

        switch (node.type()) {
        case NodeType::Block:
            expectList(node, "statements");
            break;
        case NodeType::LocalDeclaration:
        case NodeType::GlobalDeclaration:
            expectList(node, "names");
            expectList(node, "values", false);
            break;
        case NodeType::AssignmentStatement:
            expectList(node, "targets");
            expectList(node, "values");
            break;
        case NodeType::FunctionDeclaration:
            required(node, "name");
            expectList(node, "parameters");
            expectBlock(node, "body");
            break;
        case NodeType::IfStatement:
            expectExpression(node, "condition");
            expectBlock(node, "then");
            expectList(node, "elseif", false);
            break;
        case NodeType::ElseIfBranch:
            expectExpression(node, "condition");
            expectBlock(node, "body");
            break;
        case NodeType::NumericForStatement:
            required(node, "variable");
            expectExpression(node, "start");
            expectExpression(node, "finish");
            expectExpression(node, "step");
            expectBlock(node, "body");
            break;
        case NodeType::GenericForStatement:
            expectList(node, "names");
            expectList(node, "iterators");
            expectBlock(node, "body");
            break;
        case NodeType::WhileStatement:
            expectExpression(node, "condition");
            expectBlock(node, "body");
            break;
        case NodeType::RepeatStatement:
            expectBlock(node, "body");
            expectExpression(node, "condition");
            break;
        case NodeType::ReturnStatement:
            expectList(node, "values", false);
            break;
        case NodeType::UnaryExpression:
            expectExpression(node, "operand");
            break;
        case NodeType::BinaryExpression:
            expectExpression(node, "left");
            expectExpression(node, "right");
            break;
        case NodeType::IndexExpression:
            expectExpression(node, "object");
            expectExpression(node, "index");
            break;
        case NodeType::FieldExpression:
            expectExpression(node, "object");
            required(node, "field");
            break;
        case NodeType::CallExpression:
            expectExpression(node, "function");
            expectList(node, "arguments");
            break;
        case NodeType::MethodCallExpression:
            expectExpression(node, "object");
            expectList(node, "arguments");
            break;
        case NodeType::TableConstructor:
            expectList(node, "fields");
            break;
        case NodeType::TableField:
            expectExpression(node, "value");
            break;
        case NodeType::FunctionExpression:
            expectList(node, "parameters");
            expectBlock(node, "body");
            break;
        default:
            break;
        }
    }

    AstValidationResult result_;
};

bool AstValidationResult::isValid() const
{
    return diagnostics_.empty();
}

const std::vector<AstDiagnostic>& AstValidationResult::diagnostics() const
{
    return diagnostics_;
}

AstValidationResult AstValidator::validate(const Node& root) const
{
    Validator validator;
    return validator.run(root);
}

} // namespace lua::ast
