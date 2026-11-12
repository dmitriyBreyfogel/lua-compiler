#include "semantic/SemanticTransformer.h"

namespace lua::semantic {

namespace {

void normalizeNode(ast::Node& node)
{
    if (node.type() == ast::NodeType::NumericForStatement) {
        if (node.child("step") == nullptr) {
            node.addChild("step", ast::createNode(ast::NodeType::IntegerLiteral, "1"));
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
