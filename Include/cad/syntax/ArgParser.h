#pragma once

#include <cad/syntax/Arg.h>
#include <optional>
#include <vector>

namespace Cad::Syntax {

struct ArgParser {
    std::optional<Arg> TryParseFloat(std::string arg);
    std::optional<Arg> TryParseInteger(std::string arg);
    std::optional<Arg> TryParseBoolean(std::string arg);
    std::optional<Arg> TryParseVector2(std::string arg);
    std::optional<Arg> TryParseString(std::string arg);

    std::vector<Arg> Parse(std::vector<std::string> args);
};
}