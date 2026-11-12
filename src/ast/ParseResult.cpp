#include "ast/ParseResult.h"

#include <utility>

namespace lua::ast {

namespace {

NodePtr parsedChunk;

}

void setParsedChunk(NodePtr chunk)
{
    parsedChunk = std::move(chunk);
}

NodePtr takeParsedChunk()
{
    return std::move(parsedChunk);
}

void clearParsedChunk()
{
    parsedChunk.reset();
}

} // namespace lua::ast
