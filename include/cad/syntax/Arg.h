#pragma once

#include <core/math/Vector2.h>
#include <string>
namespace Cad::Syntax {
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
    Core::Vector2 value_vector2;
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

    static Arg FromVector2(Core::Vector2 value)
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

    float AsFloat() {
        if (type != Type::FLOAT) {
            throw std::runtime_error("Arg is not a float");
        }

        return value_float;
    }

    Core::Vector2 AsVector2() {
        if (type != Type::VECTOR2) {
            throw std::runtime_error("Arg is not a vector2");
        }

        return value_vector2;
    }

    Type GetType() { return type; }

    static std::string TypeToString(Type type) {
        switch (type) {
        case Type::FLOAT:
            return "float";
        case Type::INTEGER:
            return "integer";
        case Type::BOOLEAN:
            return "boolean";
        case Type::VECTOR2:
            return "vector2";
        case Type::STRING:
            return "string";
        default:
            return "unknown";
        }
    }
};
}