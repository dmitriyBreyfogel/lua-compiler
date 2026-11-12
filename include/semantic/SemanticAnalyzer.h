#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/Ast.h"

namespace lua::semantic {

enum class SymbolType {
    Local,
    Global,
    Parameter,
    LoopVariable,
    Label,
};

struct Symbol {
    std::uint64_t id;
    std::string name;
    SymbolType type;
    std::uint64_t declarationId;
    bool readOnly;
};

struct Diagnostic {
    std::uint64_t nodeId;
    std::string message;
};

class SemanticResult {
public:
    const std::vector<Symbol>& symbols() const;
    const std::vector<Diagnostic>& diagnostics() const;
    const Symbol* bindingFor(const ast::Node& node) const;
    bool isUpvalue(const ast::Node& node) const;

public:
    std::vector<Symbol> symbols_;
    std::vector<Diagnostic> diagnostics_;
    std::unordered_map<std::uint64_t, std::uint64_t> bindings_;
    std::unordered_set<std::uint64_t> upvalues_;
};

class SemanticAnalyzer {
public:
    SemanticResult analyze(const ast::Node& block);
};

const char* symbolTypeName(SymbolType type);

} // namespace lua::semantic
