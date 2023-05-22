#pragma once

#include <cad/Controller.h>
#include <cad/syntax/Syntax.h>
#include <cad/object/ObjectBuilder.h>

namespace Cad {

struct Command {
    virtual void Forward(Controller& controller) = 0;
    virtual void Backward(Controller& controller) {}
    bool IsBackward() { return false; }
};

struct ShowCommand : public Cad::Command {

    ShowCommand() { }

    void Forward(Cad::Controller& cad) override { 
        auto& stream = cad.GetOutput();
        auto& input = cad.GetInput();
        auto cursor = cad.GetViewfinder().GetCursor(cad);
        // convert cursor to world space
        auto transform = cad.GetViewfinder().GetViewTransform();
        cursor = transform.Inverse().Apply(cursor);


        std::string message = "Cursor: " + std::to_string(cursor.x) + ", " + std::to_string(cursor.y);
        stream.Writeln(message); }

    void Backward(Cad::Controller &cad) override {
        std::cout << "undo" << std::endl;
    }

    struct Signature : public Cad::Syntax::ArgSignature {
        Signature() : ArgSignature("show", {}) {}

        std::unique_ptr<Cad::Command> Create(std::vector<Cad::Syntax::Arg> args) override {
            return std::make_unique<ShowCommand>();;
        }
    };
};

struct PanCommand : public Cad::Command {
    float x, y;

    PanCommand(float x, float y) : x(x), y(y) { }

    void Forward(Cad::Controller& cad) override {
        cad.GetViewfinder().Pan(x, y);
    }

    void Backward(Cad::Controller &cad) override {
        cad.GetViewfinder().Pan(-x, -y);
    }

    struct Signature : public Cad::Syntax::ArgSignature {
        Signature() : ArgSignature("pan", {Cad::Syntax::Arg::Type::FLOAT, Cad::Syntax::Arg::Type::FLOAT}) {}

        std::unique_ptr<Cad::Command> Create(std::vector<Cad::Syntax::Arg> args) override {
            float x = args[0].AsFloat();
            float y = args[1].AsFloat();
            return std::make_unique<PanCommand>(x, y);;
        }
    };
};

struct ZoomCommand : public Cad::Command {
    float z;

    ZoomCommand(float z) : z(z) { }

    void Forward(Cad::Controller& cad) override {
        std::cout << "zoom" << std::endl;
        cad.GetViewfinder().Zoom(z);
    }

    void Backward(Cad::Controller &cad) override {
        cad.GetViewfinder().Zoom(-z);
    }

    struct Signature : public Cad::Syntax::ArgSignature {
        Signature() : ArgSignature("zoom", {Cad::Syntax::Arg::Type::FLOAT}) {}

        std::unique_ptr<Cad::Command> Create(std::vector<Cad::Syntax::Arg> args) override {
            float z = args[0].AsFloat();
            return std::make_unique<ZoomCommand>(z);;
        }
    };
};

struct ZeroCommand : public Cad::Command {
    ZeroCommand() = default;

    void Forward(Cad::Controller& cad) override { 
        cad.GetViewfinder().Zero(cad); 
    }

    void Backward(Cad::Controller &cad) override {
        std::cout << "undo" << std::endl;
    }

    struct Signature : public Cad::Syntax::ArgSignature {
        Signature() : ArgSignature("zero", {}) {}

        std::unique_ptr<Cad::Command> Create(std::vector<Cad::Syntax::Arg> args) override {
            return std::make_unique<ZeroCommand>();
        }
    };
};

struct SnapCommand : public Cad::Command {
    float size;

    SnapCommand(float size) : size(size) {}

    void Forward(Cad::Controller& cad) override { 
        cad.GetViewfinder().Snap(size); 
    }

    void Backward(Cad::Controller &cad) override {
        std::cout << "undo" << std::endl;
    }

    struct Signature : Cad::Syntax::ArgSignature {
        Signature() : ArgSignature("snap", {Cad::Syntax::Arg::Type::FLOAT}) {}

        std::unique_ptr<Cad::Command> Create(std::vector<Cad::Syntax::Arg> args) override {
            float size = args[0].AsFloat();
            return std::make_unique<SnapCommand>(size);
        }
    };
};

struct ClearCommand : public Cad::Command {
    ClearCommand() = default;

    void Forward(Cad::Controller& cad) override {
        cad.GetOutput().Clear();
    }

    void Backward(Cad::Controller &cad) override {
        std::cout << "undo" << std::endl;
    }

    struct Signature : Cad::Syntax::ArgSignature {
        Signature() : ArgSignature("clear", {}) {}

        std::unique_ptr<Cad::Command> Create(std::vector<Cad::Syntax::Arg> args) override {
            return std::make_unique<ClearCommand>();
        }
    };
};

struct CreateCircle : public Cad::Command {
    Core::Vector2 center;
    float radius;

    CreateCircle(Core::Vector2 center, float radius) : center(center), radius(radius) {}

    void Forward(Cad::Controller& cad) override{
        auto& registry = cad.GetRegistry();
        auto circle_builder = Cad::CircleObjectBuilder(center, radius);
        registry.CreateObject(circle_builder);
    };

    void Backward(Cad::Controller &cad) override {
        std::cout << "undo" << std::endl;
    }

    struct Signature : Cad::Syntax::ArgSignature {
        Signature() : ArgSignature("circle", {Cad::Syntax::Arg::Type::VECTOR2, Cad::Syntax::Arg::Type::FLOAT}) {}

        std::unique_ptr<Cad::Command> Create(std::vector<Cad::Syntax::Arg> args) override {
            Core::Vector2 center = args[0].AsVector2();
            float radius = args[1].AsFloat();
            return std::make_unique<CreateCircle>(center, radius);
        }
    };
};


} // namespace Cad