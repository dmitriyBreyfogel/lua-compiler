#include "semantic/SemanticTransformer.h"

namespace lua::semantic {

namespace {

void normalizeNode(ast::Node& node)
{
    if (node.type() == ast::NodeType::NumericForStatement) {
        auto& loop = static_cast<ast::NumericForStatement&>(node);
        if (loop.step() == nullptr) {
            loop.addChild("step", ast::makeNode(ast::NodeType::IntegerLiteral, "1"));
        }
    }

    for (const ast::Child& child : node.children()) {
        normalizeNode(*child.node);
    }
}

}

void SemanticTransformer::normalize(ast::Node& block)
{
    normalizeNode(block);
}

} // namespace lua::semantic
