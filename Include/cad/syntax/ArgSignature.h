#pragma once

#include <cad/syntax/Arg.h>

#include <vector>
#include <memory>
namespace Cad {
    struct Command;
}

namespace Cad::Syntax {

struct ArgSignature {
    std::string keyword;
    std::vector<Arg::Type> signature;

    ArgSignature(std::string keyword, std::vector<Arg::Type> signature) : keyword(keyword), signature(signature) {}
    virtual ~ArgSignature() {}

    virtual std::unique_ptr<Command> Create(std::vector<Arg> args) = 0;

    std::string GetKeyword() { return keyword; }
    std::vector<Arg::Type> GetSignature() { return signature; }
    unsigned GetArgCount() { return signature.size(); }
    std::string GetName() {
        std::string name = keyword + "(";
        for (auto arg_type : signature) {
            name += Arg::TypeToString(arg_type) + ", ";
        }
        name.pop_back();
        name.pop_back();
        name += ")";
        return name;
    }

    bool Matches(std::vector<Arg> args)
    {
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
} // namespace Cad