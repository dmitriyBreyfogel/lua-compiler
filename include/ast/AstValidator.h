#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ast/Ast.h"

namespace lua::ast {

class Validator;

struct AstDiagnostic {
    std::uint64_t nodeId;
    std::string message;
};

class AstValidationResult {
public:
    bool isValid() const;
    const std::vector<AstDiagnostic>& diagnostics() const;

private:
    friend class AstValidator;
    friend class Validator;

    std::vector<AstDiagnostic> diagnostics_;
};

class AstValidator {
public:
    AstValidationResult validate(const Node& root) const;
};

} // namespace lua::ast
