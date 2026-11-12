#include "ast/ParseResult.h"

#include <utility>

namespace lua::ast {

namespace {

NodePtr parsedBlock;

}

void setParsedBlock(NodePtr block)
{
    parsedBlock = std::move(block);
}

NodePtr takeParsedBlock()
{
    return std::move(parsedBlock);
}

void clearParsedBlock()
{
    parsedBlock.reset();
}

} // namespace lua::ast
