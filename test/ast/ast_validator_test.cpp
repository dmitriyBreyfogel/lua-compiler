#include <cassert>

#include "ast/AstValidator.h"

int main()
{
    lua::ast::AstValidator validator;

    auto validBlock = lua::ast::makeNode(lua::ast::NodeType::Block);
    validBlock->addChild("statements", lua::ast::makeNode(lua::ast::NodeType::List));
    assert(validator.validate(*validBlock).isValid());

    auto invalidLoop = lua::ast::makeNode(lua::ast::NodeType::NumericForStatement);
    const lua::ast::AstValidationResult result = validator.validate(*invalidLoop);
    assert(!result.isValid());
    assert(result.diagnostics().size() == 6);
}
