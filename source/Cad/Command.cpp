#include <Cad.h>

namespace Cad::Command {
Argument::Argument(std::string string_value) : string_value(string_value), type(Argument::Type::STRING) {}

Argument::Argument(int int_value) : int_value(int_value), type(Argument::Type::INT) {}

Argument::Argument(float float_value) : float_value(float_value), type(Argument::Type::FLOAT) {}

float Argument::GetFloat() { return float_value; }

int Argument::GetInt() { return int_value; }

std::string Argument::GetString() { return string_value; }

std::optional<Argument> Argument::TryParseFloat(std::string arg)
{
    try {
        float value = std::stof(arg);
        return Argument(value);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<Argument> Argument::TryParseInt(std::string arg)
{
    try {
        int value = std::stoi(arg);
        return Argument(value);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<Argument> Argument::TryParseString(std::string arg) { return Argument(arg); }

Signature::Signature(std::string keyword, std::vector<Argument::Type> arg_types)
    : keyword(keyword), arg_types(arg_types)
{
}
std::string Signature::GetKeyword() const { return keyword; }
const std::vector<Argument::Type>& Signature::GetArgTypes() const { return arg_types; }

} // namespace Cad::Command

namespace Cad::Command {
Dispatcher::ParserResult Dispatcher::ParserResult::Success(std::vector<Argument> arguments)
{
    ParserResult result;
    result.success = true;
    result.arguments = arguments;
    return result;
}

Dispatcher::ParserResult Dispatcher::ParserResult::Error(std::string error)
{
    ParserResult result;
    result.success = false;
    result.error = error;
    return result;
}

bool Dispatcher::ParserResult::IsSuccess() { return success; }

bool Dispatcher::ParserResult::IsError() { return !success; }

Dispatcher::ParserResult Dispatcher::Parse(Signature& signature, std::string arg_string)
{
    std::string keyword = signature.GetKeyword();
    const std::vector<Argument::Type>& arg_types = signature.GetArgTypes();
    std::vector<Argument> arguments;
    std::string error_msg = "";

    // Split the input into space delimited words
    std::vector<std::string> words;
    std::string word;
    for (auto character : arg_string) {
        if (character == ' ') {
            words.push_back(word);
            word = "";
        } else {
            word += character;
        }
    }
    words.push_back(word);

    // Verify that word count equals num args + 1 for keyword
    unsigned n_args = arg_types.size();
    if (words.size() != n_args + 1) {
        error_msg = "Invalid number of arguments, got " + std::to_string(words.size() - 1) + " expected " +
                    std::to_string(n_args);
        return Dispatcher::ParserResult::Error(error_msg);
    }

    // Verify that the first word is the keyword
    if (words[0] != keyword) {
        error_msg = "Invalid keyword, got " + words[0] + " expected " + keyword;
        return Dispatcher::ParserResult::Error(error_msg);
    }

    // If no args require parsing, return success
    if (n_args == 0) {
        return Dispatcher::ParserResult::Success(arguments);
    }

    // Iterate words from [1..n] and try to parse them, if any fail return an error
    for (unsigned i = 0; i < n_args; i++) {
        auto& word = words.at(i + 1);

        std::optional<Argument> try_float = Argument::TryParseFloat(word);
        if (try_float.has_value()) {
            arguments.push_back(try_float.value());
            continue;
        }

        std::optional<Argument> try_int = Argument::TryParseInt(word);
        if (try_int.has_value()) {
            arguments.push_back(try_int.value());
            continue;
        }

        std::optional<Argument> try_string = Argument::TryParseString(word);
        if (try_string.has_value()) {
            arguments.push_back(try_string.value());
            continue;
        }

        // We have an invalid argument
        error_msg = "Invalid argument " + word;
        return Dispatcher::ParserResult::Error(error_msg);
    }

    // Collect results and return
    return Dispatcher::ParserResult::Success(arguments);
}

void Dispatcher::Register(std::unique_ptr<Signature> signature) { signatures.push_back(std::move(signature)); }

void Dispatcher::Dispatch(std::string command_string)
{
    std::cout << "Dispatching " << command_string << std::endl;
    for (std::unique_ptr<Signature>& signature : signatures) {
        auto result = Parse(*signature, command_string);
        if (result.IsSuccess()) {
            auto command = signature->Apply(result.arguments);
            execution_queue.push(std::move(command));
            return;
        }
    }
}

void Dispatcher::Execute(Controller& controller)
{
    while (!execution_queue.empty()) {
        auto command = std::move(execution_queue.front());
        execution_queue.pop();
        controller.Command(std::move(command));
    }
}
} // namespace Cad::Command
