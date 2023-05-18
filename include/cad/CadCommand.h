#pragma once

#include <Engine.h>
#include <cad/CadRegistry.h>
#include <cad/CadViewfinder.h>
#include <cad/CadController.h>

namespace Cad {

struct Command {
    virtual void Forward(Controller& controller) = 0;
    virtual void Backward(Controller& controller) {}
    bool IsBackward() { return false; }
};

namespace Syntax {

struct Arg {
  public:
    enum class Type {
        FLOAT,
        INTEGER,
        BOOLEAN,
        VECTOR2,
        STRING
    };

  private:
    Type type;

    float value_float;
    int value_integer;
    bool value_boolean;
    Vector2 value_vector2;
    std::string value_string;

  public:
    static Arg FromFloat(float value)
    {
        Arg arg;
        arg.type = Type::FLOAT;
        arg.value_float = value;
        return arg;
    }

    static Arg FromInteger(int value)
    {
        Arg arg;
        arg.type = Type::INTEGER;
        arg.value_integer = value;
        return arg;
    }

    static Arg FromBoolean(bool value)
    {
        Arg arg;
        arg.type = Type::BOOLEAN;
        arg.value_boolean = value;
        return arg;
    }

    static Arg FromVector2(Vector2 value)
    {
        Arg arg;
        arg.type = Type::VECTOR2;
        arg.value_vector2 = value;
        return arg;
    }

    static Arg FromString(std::string value)
    {
        Arg arg;
        arg.type = Type::STRING;
        arg.value_string = value;
        return arg;
    }

    std::string AsString() {
        if (type != Type::STRING) {
            throw std::runtime_error("Arg is not a string");
        }

        return value_string;
    }

    Type GetType() { return type; }
};

struct ArgSignature {
    std::string keyword;
    std::vector<Arg::Type> signature;

    ArgSignature(std::string keyword, std::vector<Arg::Type> signature) : keyword(keyword), signature(signature) {}
    virtual ~ArgSignature() {}

    virtual std::unique_ptr<Command> Create(std::vector<Arg> args) = 0;

    std::string GetKeyword() { return keyword; }
    std::vector<Arg::Type> GetSignature() { return signature; }
    unsigned GetArgCount() { return signature.size(); }

    bool Matches(std::vector<Arg> args) {
        if (args.size() != signature.size()) {
            return false;
        }

        int i = 0;
        for (auto arg_type : signature) {
            if (arg_type != args[i].GetType()) {
                return false;
            }
            i++;
        }

        return true;
    }
};

struct ArgParser {
    std::optional<Arg> TryParseFloat(std::string arg);
    std::optional<Arg> TryParseInteger(std::string arg);
    std::optional<Arg> TryParseBoolean(std::string arg);
    std::optional<Arg> TryParseVector2(std::string arg);
    std::optional<Arg> TryParseString(std::string arg);

    std::vector<Arg> Parse(std::vector<std::string> args);
};

} // namespace Syntax

struct Dispatcher {
    std::queue<std::unique_ptr<Command>> command_queue;
    using SignatureList = std::vector<std::unique_ptr<Syntax::ArgSignature>>;
    std::unordered_map<std::string, SignatureList> commands_map;

  public:
    Dispatcher() {}

    void Register(std::unique_ptr<Syntax::ArgSignature> signature);
    void Dispatch(std::string command_string, OutputStream& stream);
    void Execute(Controller& controller);
};
}; // namespace Cad