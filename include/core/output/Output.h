#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace Core { 
struct Output {
    virtual void Write(std::string text) = 0;
    virtual void Writeln(std::string text) = 0;
    virtual std::string Flush() = 0;
    virtual void Clear() = 0;
    virtual void Dump(std::vector<std::string>& lines) = 0;
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

    std::string Flush() override
    {
        std::string result = buffer.str();
        buffer.str("");
        buffer.clear();

        if (result.size() > 0)
            std::cout << result << std::endl;
        return result;
    }

    void Clear() override
    {
        buffer.str("");
        buffer.clear();
    }

    virtual void Dump(std::vector<std::string>& lines) {
        std::string line;
        while (std::getline(buffer, line)) {
            lines.push_back(line);
        }
    }

};

}