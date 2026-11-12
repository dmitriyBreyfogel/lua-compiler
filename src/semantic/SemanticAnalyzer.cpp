#include "semantic/SemanticAnalyzer.h"

#include <unordered_map>

namespace lua::semantic {

namespace {

using ast::Node;
using ast::NodeType;

struct Scope {
    std::unordered_map<std::string, std::uint64_t> names;
    struct Label {
        std::uint64_t symbolId;
        int localCount;
    };
    std::unordered_map<std::string, Label> labels;
    int localCount;
    int functionDepth;
};

class Analyzer {
public:
    SemanticResult run(const Node& block)
    {
        pushScope();
        visitBlock(block, false);
        return std::move(result_);
    }

private:
    const Node* child(const Node& node, const std::string& role)
    {
        for (const ast::Child& item : node.children()) {
            if (item.role == role) {
                return item.node.get();
            }
        }
        return nullptr;
    }

    void visitList(const Node* list)
    {
        if (list == nullptr) {
            return;
        }

        for (const ast::Child& item : list->children()) {
            visit(*item.node);
        }
    }

    void visitBlock(const Node& block, bool createScope)
    {
        if (createScope) {
            pushScope();
        }

        const Node* statements = child(block, "statements");
        declareLabels(statements);
        visitList(statements);
        visitNode(child(block, "return"));

        if (createScope) {
            scopes_.pop_back();
        }
    }

    void declareLabels(const Node* statements)
    {
        if (statements == nullptr) {
            return;
        }

        int localCount = 0;
        for (const ast::Child& item : statements->children()) {
            const Node& statement = *item.node;
            if (statement.type() == NodeType::LabelStatement) {
                if (scopes_.back().labels.count(statement.value()) != 0) {
                    error(statement, "duplicate label: " + statement.value());
                } else {
                    const std::uint64_t id = declare(statement.value(), SymbolType::Label, statement);
                    scopes_.back().labels[statement.value()] = {id, localCount};
                }
            }

            localCount += localDeclarations(statement);
        }
    }

    int localDeclarations(const Node& statement)
    {
        if (statement.type() == NodeType::LocalDeclaration) {
            const Node* names = child(statement, "names");
            return names == nullptr ? 0 : static_cast<int>(names->children().size());
        }
        if (statement.type() == NodeType::FunctionDeclaration && statement.value() == "local") {
            return 1;
        }
        return 0;
    }

    void visit(const Node& node)
    {
        switch (node.type()) {
        case NodeType::Block:
            visitBlock(node, true);
            break;
        case NodeType::LocalDeclaration:
            visitList(child(node, "values"));
            declareNames(child(node, "names"), SymbolType::Local);
            break;
        case NodeType::GlobalDeclaration:
            declareNames(child(node, "names"), SymbolType::Global);
            visitList(child(node, "values"));
            break;
        case NodeType::GlobalAllDeclaration:
            break;
        case NodeType::AssignmentStatement:
            visitList(child(node, "values"));
            visitAssignmentTargets(child(node, "targets"));
            break;
        case NodeType::FunctionDeclaration:
            visitFunctionDeclaration(node);
            break;
        case NodeType::FunctionExpression:
            visitFunction(node);
            break;
        case NodeType::NumericForStatement:
            visitNode(child(node, "start"));
            visitNode(child(node, "finish"));
            visitNode(child(node, "step"));
            pushScope();
            declareNode(*child(node, "variable"), SymbolType::LoopVariable);
            ++loopDepth_;
            visitNode(child(node, "body"));
            --loopDepth_;
            scopes_.pop_back();
            break;
        case NodeType::GenericForStatement:
            visitList(child(node, "iterators"));
            pushScope();
            declareNames(child(node, "names"), SymbolType::LoopVariable);
            ++loopDepth_;
            visitNode(child(node, "body"));
            --loopDepth_;
            scopes_.pop_back();
            break;
        case NodeType::WhileStatement:
            visitNode(child(node, "condition"));
            ++loopDepth_;
            visitNode(child(node, "body"));
            --loopDepth_;
            break;
        case NodeType::RepeatStatement:
            ++loopDepth_;
            visitNode(child(node, "body"));
            visitNode(child(node, "condition"));
            --loopDepth_;
            break;
        case NodeType::IfStatement:
            visitNode(child(node, "condition"));
            visitNode(child(node, "then"));
            visitList(child(node, "elseif"));
            visitNode(child(node, "else"));
            break;
        case NodeType::ElseIfBranch:
            visitNode(child(node, "condition"));
            visitNode(child(node, "body"));
            break;
        case NodeType::BreakStatement:
            if (loopDepth_ == 0) {
                error(node, "break outside loop");
            }
            break;
        case NodeType::GotoStatement:
            resolveLabel(node);
            break;
        case NodeType::FieldExpression:
            visitNode(child(node, "object"));
            break;
        case NodeType::Identifier:
            bind(node, resolveName(node));
            break;
        case NodeType::VarargExpression:
            if (varargDepth_ == 0) {
                error(node, "vararg outside function with vararg parameter");
            }
            break;
        default:
            for (const ast::Child& item : node.children()) {
                visit(*item.node);
            }
            break;
        }
    }

    void visitNode(const Node* node)
    {
        if (node != nullptr) {
            visit(*node);
        }
    }

    void visitFunctionDeclaration(const Node& node)
    {
        const Node* name = child(node, "name");
        if (name != nullptr && name->type() == NodeType::Identifier) {
            declareNode(*name, node.value() == "local" ? SymbolType::Local : SymbolType::Global);
        } else {
            visitNode(name);
        }
        visitFunction(node);
    }

    void visitFunction(const Node& node)
    {
        ++functionDepth_;
        pushScope();
        const Node* parameters = child(node, "parameters");
        bool acceptsVararg = false;
        if (parameters != nullptr) {
            for (const ast::Child& item : parameters->children()) {
                if (item.node->type() == NodeType::VarargExpression) {
                    acceptsVararg = true;
                } else {
                    declareNode(*item.node, SymbolType::Parameter);
                }
            }
        }

        if (acceptsVararg) {
            ++varargDepth_;
        }
        visitNode(child(node, "body"));
        if (acceptsVararg) {
            --varargDepth_;
        }
        --functionDepth_;
        scopes_.pop_back();
    }

    void declareNames(const Node* names, SymbolType type)
    {
        if (names == nullptr) {
            return;
        }
        for (const ast::Child& item : names->children()) {
            declareNode(*item.node, type);
        }
    }

    void declareNode(const Node& node, SymbolType type)
    {
        const std::string& name = node.value();
        Scope& scope = type == SymbolType::Global ? scopes_.front() : scopes_.back();
        const std::uint64_t id = declare(name, type, node);
        scope.names[name] = id;
        if (type == SymbolType::Local) {
            ++scope.localCount;
        }
        bind(node, id);
    }

    std::uint64_t declare(const std::string& name, SymbolType type, const Node& node)
    {
        const std::uint64_t id = result_.symbols_.size() + 1;
        result_.symbols_.push_back({id, name, type, node.id(), isReadOnly(node)});
        return id;
    }

    bool isReadOnly(const Node& node)
    {
        for (const ast::Child& child : node.children()) {
            if (child.node->type() != NodeType::Attribute) {
                continue;
            }
            if (child.node->value() == "const" || child.node->value() == "close") {
                return true;
            }
            error(*child.node, "unknown local attribute: " + child.node->value());
        }
        return false;
    }

    void visitAssignmentTargets(const Node* targets)
    {
        if (targets == nullptr) {
            return;
        }
        for (const ast::Child& item : targets->children()) {
            visit(*item.node);
            if (item.node->type() != NodeType::Identifier) {
                continue;
            }
            const Symbol* symbol = result_.bindingFor(*item.node);
            if (symbol != nullptr && symbol->readOnly) {
                error(*item.node, "assignment to read-only variable: " + symbol->name);
            }
        }
    }

    std::uint64_t resolveName(const Node& node)
    {
        const std::string& name = node.value();
        for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
            const auto found = scope->names.find(name);
            if (found != scope->names.end()) {
                const Symbol& symbol = result_.symbols_[found->second - 1];
                if (scope->functionDepth < functionDepth_ && symbol.type != SymbolType::Global) {
                    result_.upvalues_.insert(node.id());
                }
                return found->second;
            }
        }

        Scope& global = scopes_.front();
        const auto found = global.names.find(name);
        if (found != global.names.end()) {
            return found->second;
        }

        const std::uint64_t id = declare(name, SymbolType::Global, node);
        global.names[name] = id;
        return id;
    }

    void resolveLabel(const Node& node)
    {
        for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
            const auto found = scope->labels.find(node.value());
            if (found != scope->labels.end()) {
                if (scope == scopes_.rbegin() && found->second.localCount > scope->localCount) {
                    error(node, "goto jumps into scope of local variable: " + node.value());
                    return;
                }
                bind(node, found->second.symbolId);
                return;
            }
        }
        error(node, "unknown label: " + node.value());
    }

    void bind(const Node& node, std::uint64_t symbolId)
    {
        result_.bindings_[node.id()] = symbolId;
    }

    void error(const Node& node, const std::string& message)
    {
        result_.diagnostics_.push_back({node.id(), message});
    }

    void pushScope()
    {
        scopes_.push_back({{}, {}, 0, functionDepth_});
    }

    SemanticResult result_;
    std::vector<Scope> scopes_;
    int loopDepth_ = 0;
    int functionDepth_ = 0;
    int varargDepth_ = 0;
};

}

const std::vector<Symbol>& SemanticResult::symbols() const
{
    return symbols_;
}

const std::vector<Diagnostic>& SemanticResult::diagnostics() const
{
    return diagnostics_;
}

const Symbol* SemanticResult::bindingFor(const ast::Node& node) const
{
    const auto found = bindings_.find(node.id());
    if (found == bindings_.end()) {
        return nullptr;
    }
    return &symbols_[found->second - 1];
}

bool SemanticResult::isUpvalue(const ast::Node& node) const
{
    return upvalues_.count(node.id()) != 0;
}

SemanticResult SemanticAnalyzer::analyze(const ast::Node& block)
{
    Analyzer analyzer;
    return analyzer.run(block);
}

const char* symbolTypeName(SymbolType type)
{
    switch (type) {
    case SymbolType::Local: return "local";
    case SymbolType::Global: return "global";
    case SymbolType::Parameter: return "parameter";
    case SymbolType::LoopVariable: return "loop variable";
    case SymbolType::Label: return "label";
    }
    return "unknown";
}

} // namespace lua::semantic
