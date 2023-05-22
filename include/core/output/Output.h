#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace Core { 
struct Output {
    virtual void Write(std::string text) = 0;
    virtual void Writeln(std::string text) = 0;
    virtual void Clear() = 0;
     virtual std::string Dump() const = 0;
};

class BufferedOutput : public Output {
    std::stringstream buffer;

  public:
    BufferedOutput() = default;

    void Write(std::string text) override
    {
        buffer << text;
    }

    void Writeln(std::string text) override { 
        buffer << text << std::endl;
    }

    void Clear() override
    {
        buffer.str("");
        buffer.clear();
    }

    virtual std::string Dump() const {
        // Copy lines without destroying them
        std::string lines = buffer.str();
        return lines;
    }

};

}