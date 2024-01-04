#pragma once

#include <cad/Command.h>
#include <cad/Controller.h>

#include <core/output/Output.h>

#include <queue>

namespace Cad {
struct Dispatcher {
    std::queue<std::unique_ptr<Command>> command_queue;
    using SignatureList = std::vector<std::unique_ptr<Syntax::ArgSignature>>;
    std::unordered_map<std::string, SignatureList> commands_map;

  public:
    Dispatcher() {}

    void Register(std::unique_ptr<Syntax::ArgSignature> signature);
    void Dispatch(std::string command_string, Core::Output& stream);
    void Execute(Controller& controller);

    std::vector<Syntax::ArgSignature*> GetCommands() {
        std::vector<Syntax::ArgSignature*> commands;
        for (auto& command : commands_map) {
            for (auto& signature : command.second) {
                commands.push_back(signature.get());
            }
        }
        return commands;
    }
};
}