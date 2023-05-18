#include <cad/CadCommand.h>

#include <regex>

namespace Cad {

std::optional<Syntax::Arg> Syntax::ArgParser::TryParseFloat(std::string arg)
{
    try {
        float value = std::stof(arg);
        return Syntax::Arg::FromFloat(value);
    } catch (std::invalid_argument) {
        return std::nullopt;
    }
}

std::optional<Syntax::Arg> Syntax::ArgParser::TryParseInteger(std::string arg)
{
    try {
        int value = std::stoi(arg);
        return Syntax::Arg::FromInteger(value);
    } catch (std::invalid_argument) {
        return std::nullopt;
    }
}

std::optional<Syntax::Arg> Syntax::ArgParser::TryParseBoolean(std::string arg)
{
    if (arg == "true") return Syntax::Arg::FromBoolean(true);
    if (arg == "false") return Syntax::Arg::FromBoolean(false);
    return std::nullopt;
}

std::optional<Syntax::Arg> Syntax::ArgParser::TryParseVector2(std::string arg)
{
    // Match the [x, y] format where x and y are floats.
    std::regex vector2_regex(R"(\[(-?\d+\.?\d*), (-?\d+\.?\d*)\])");
    std::smatch match;
    if (std::regex_match(arg, match, vector2_regex)) {
        float x = std::stof(match[1]);
        float y = std::stof(match[2]);
        return Syntax::Arg::FromVector2(Vector2(x, y));
    }

    return std::nullopt;
}

std::optional<Syntax::Arg> Syntax::ArgParser::TryParseString(std::string arg)
{
    // Match the \"string\" format where string is any string of characters.
    std::regex string_regex(R"(\"(.*)\")");
    std::smatch match;
    if (std::regex_match(arg, match, string_regex)) {
        std::string value = match[1];
        return Syntax::Arg::FromString(value);
    }

    return std::nullopt;
}

std::vector<Syntax::Arg> Syntax::ArgParser::Parse(std::vector<std::string> args)
{
    std::vector<Syntax::Arg> result;

    for (std::string arg : args) {
        if (auto value = TryParseFloat(arg)) {
            result.push_back(value.value());
            continue;
        }
        if (auto value = TryParseInteger(arg)) {
            result.push_back(value.value());
            continue;
        }
        if (auto value = TryParseBoolean(arg)) {
            result.push_back(value.value());
            continue;
        }
        if (auto value = TryParseVector2(arg)) {
            result.push_back(value.value());
            continue;
        }
        if (auto value = TryParseString(arg)) {
            result.push_back(value.value());
            continue;
        }
        // Unrecognized argument
        std::cout << "Unrecognized argument: " << arg << std::endl;
        return std::vector<Syntax::Arg>();
    }
    return result;
}

void Dispatcher::Register(std::unique_ptr<Syntax::ArgSignature> signature) {
    std::string command_keyword = signature->GetKeyword();
    if (commands_map.find(command_keyword) == commands_map.end()) {
        commands_map[command_keyword] = std::vector<std::unique_ptr<Syntax::ArgSignature>>();
    }
    auto& signature_list = commands_map[command_keyword];
    signature_list.push_back(std::move(signature));
}

void Dispatcher::Dispatch(std::string command_string, OutputStream& stream)
{
    // Tokenize the command string by spaces, but if the token is surrounded by quotes or brackets, keep it as one token.
    std::vector<std::string> tokens;
    std::regex token_regex(R"((?:[^\s"[\]]|"[^"]*"|\[[^\]]*\])+)"); // Matches any string of characters that is not whitespace, or is surrounded by quotes or brackets.
    std::smatch match;
    while (std::regex_search(command_string, match, token_regex)) {
        tokens.push_back(match[0]);
        command_string = match.suffix();
    }

    // Error if no command entered
    if (tokens.size() == 0) {
        stream.PushLine("[ERROR]: No command entered.", Color::RED);
        return;
    }

    // Extract the command keyword and error if not registered
    std::string command_keyword = tokens[0];
    if (commands_map.find(command_keyword) == commands_map.end()) {
        std::string error = "[ERROR]: Unrecognized command: " + command_keyword;
        stream.PushLine(error, Color::RED);
        return;
    }

    // Parse the arguments
    Syntax::ArgParser parser;
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    std::vector<Syntax::Arg> parsed_args = parser.Parse(args);

    // Find a matching signature
    std::vector<std::unique_ptr<Syntax::ArgSignature>>& signatures = commands_map[command_keyword];
    std::optional<Syntax::ArgSignature*> matching_signature = std::nullopt;
    for (std::unique_ptr<Syntax::ArgSignature>& signature : signatures) {
        if (signature->Matches(parsed_args)) {
            matching_signature = signature.get();
            break;
        }
    }

    // Error if no matching signature
    if (!matching_signature) {
        std::string error = "[ERROR]: No matching signature for command: " + command_keyword;
        stream.PushLine(error, Color::RED);
        return;
    }

    // Enqueue the command
    Syntax::ArgSignature& signature = *matching_signature.value();
    std::unique_ptr<Command> command = signature.Create(parsed_args);
    command_queue.push(std::move(command));
}

void Dispatcher::Execute(Controller& controller) {
    for (int i = 0; i < command_queue.size(); i++) {
        std::unique_ptr<Command> command = std::move(command_queue.front());
        command_queue.pop();
        command->Forward(controller);
        // TODO: add undo stack to dispatcher
    }

    // Clear the command queue
    while (!command_queue.empty()) {
        command_queue.pop();
    }
}

}; // namespace Cad