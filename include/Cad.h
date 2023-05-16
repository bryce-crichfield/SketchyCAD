#pragma once

#include <Engine.h>
#include <iomanip>
#include <sstream>
#include <string>

namespace Cad {
struct World;
struct Controller;
struct IMode;
struct ICommand;
} // namespace Cad

namespace Cad {
struct IObject {
    unsigned id;

    IObject(unsigned id) : id(id) {}
    virtual ~IObject() = default;
    
    unsigned GetId() { return id; }
    void SetId(unsigned id) { this->id = id; }
    virtual void Update(Controller& controller) = 0;
    virtual bool IsHovered(Controller& controller) = 0;
    virtual void Apply(Transform transform) = 0;
    virtual std::unique_ptr<IObject> Clone(unsigned id) = 0;

    virtual std::string ToString() = 0;
};

namespace Object {
class Line : public IObject {
    Vector2 start;
    Vector2 end;

  public:
    Line(unsigned id, Vector2 start, Vector2 end);
    void Update(Controller& controller) override;
    bool IsHovered(Controller& controller) override;

    void Apply(Transform transform) {
        start = transform.Apply(start);
        end = transform.Apply(end);
    }

    std::unique_ptr<IObject> Clone(unsigned id) override {
        return std::make_unique<Line>(id, start, end);
    }

    std::string ToString() override {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Line: " << start.x << ", " << start.y << " -> " << end.x << ", " << end.y;
        return ss.str();
    }
};

class Circle : public IObject {
    Vector2 center;
    float radius;

  public:
    Circle(unsigned id, Vector2 center, float radius);
    void Update(Controller& controller) override;
    bool IsHovered(Controller& controller) override;

    void Apply(Transform transform) {
        std::cout << "Applying transform to circle" << std::endl;
        center = transform.Apply(center);
        radius *= transform.scale;
        std::cout << "Center: " << center.x << ", " << center.y << std::endl;
        std::cout << "Radius: " << radius << std::endl;
    }

    std::unique_ptr<IObject> Clone(unsigned new_id) override {
        return std::make_unique<Circle>(new_id, center, radius);
    }

    std::string ToString() override {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Circle: " << center.x << ", " << center.y << " -> " << radius;
        return ss.str();
    }
};
} // namespace Object

struct World {
    std::unordered_map<unsigned, std::unique_ptr<IObject>> objects;
    std::queue<unsigned> unique_id_queue;

    void draw_grid(Controller& controller);
    void draw_objects(Controller& controller);
    void draw_origin(Controller& controller);
    void draw_cursor(Controller& controller);

  public:
    const unsigned MAX_OBJECTS = 1000;

    World();

    void Update(Controller& controller);
    void Register(unsigned id, std::unique_ptr<IObject> object);

    std::unique_ptr<IObject> Remove(unsigned id);

    std::optional<unsigned> GetNextId() {
        if (unique_id_queue.empty()) {
            return std::nullopt;
        }
        unsigned id = unique_id_queue.front();
        unique_id_queue.pop();
        return id;
    } 

    std::optional<IObject*> Get(unsigned id)
    {
        if (objects.find(id) == objects.end()) {
            return std::nullopt;
        }
        return objects[id].get();
    }

    void Foreach(std::function<bool(unsigned, IObject&)> callback)
    {
        for (auto& pair : objects) {
            if (callback(pair.first, *pair.second)) {
                return;
            }
        }
    }

    unsigned GetObjectCount() {
        return objects.size();
    }
};

struct Cursor {
    float x;
    float y;
    float snap_size = 0.5;
    bool snap_mode = false;

  public:
    float GetX();
    float GetY();
    Vector2 GetPosition();
    void SetPosition(float x, float y) {
        this->x = x;
        this->y = y;
    }

    void OnInput(Controller& controller);
    void OnUpdate(Controller& controller);
    void OnRender(Controller& controller);
    
    bool GetSnapMode() { return snap_mode; }
    void SetSnapMode(bool snap_mode) { this->snap_mode = snap_mode; }
};
struct Clipboard;
struct Clipboard {
    std::set<unsigned> objects;
    public:

    Clipboard() = default;

    void Select(unsigned id) {
        objects.insert(id);
    }

    void Deselect(unsigned id) {
        objects.erase(id);
    }

    bool IsSelected(unsigned id) {
        return objects.find(id) != objects.end();
    }

    // Iterator over selected objects provided controller
    void Foreach(Controller& controller, std::function<void(unsigned, IObject&)> callback);
};

class CameraController {
    Camera camera;
    bool bypass = false;

    DebouncedKey up_key = DebouncedKey(Key::Up, 0.325);
    DebouncedKey down_key = DebouncedKey(Key::Down, 0.325);
    DebouncedKey left_key = DebouncedKey(Key::Left, 0.325);
    DebouncedKey right_key = DebouncedKey(Key::Right, 0.325);

  public:
    CameraController() = default;
    void Pan(int dx, int dy, float grid_size);
    void ZoomIn(Controller& controller, float grid_size);
    void ZoomOut(Controller& controller, float grid_size);
    void Zero(Graphics& graphics, float grid_size);
    void Update(Controller& controller);
    void SetBypass(bool bypass);
     Camera& GetCamera();
};

class Controller {
    std::stack<std::unique_ptr<ICommand>> undo_stack;
    CameraController camera_controller;
    std::unique_ptr<IMode> current_mode = nullptr;
    World world;
    Cursor cursor;
    Clipboard clipboard;

    State* state = nullptr;

    Interactor static_interactor;

    float grid_size = 1.0f;

  public:
    std::vector<unsigned> selected = {};

    Controller(InteractionLock& lock) : camera_controller(), static_interactor(lock) {}

    void OnStart(State& state);
    void OnInput(State& state);
    void OnUpdate(State& state);
    void OnRender(State& state);

    void Update(State& state);
    void Command(std::unique_ptr<ICommand> command);
    void Undo();

    Graphics& GetGraphics();
    Input& GetInput();
    Chronometer& GetChrono();
    World& GetWorld();
    Camera& GetCamera();
    StaticContext& GetStaticContext()
    {
        if (state == nullptr) {
            throw std::runtime_error("Controller::GetStaticContext() called before OnStart()");
        }
        return state->static_context;
    }
    Cursor& GetCursor() { return cursor; }
    Clipboard& GetClipboard() { return clipboard; }
    CameraController& GetCameraController() { return camera_controller; }
    float GetGridSize() { return grid_size; }
    void SetGridSize(float size) { grid_size = size; }
};

} // namespace Cad

namespace Cad {
struct ICommand {
    virtual ~ICommand() = default;
    virtual void Forward(Controller& controller) = 0;
    virtual void Backward(Controller& controller) = 0;
};

namespace Command {
struct Argument {
    enum class Type {
        STRING,
        INT,
        FLOAT,
    };

    std::string string_value;
    int int_value;
    float float_value;

    Type type;

    Argument(std::string string_value);

    Argument(int int_value);

    Argument(float float_value);

    float GetFloat();
    int GetInt();
    std::string GetString();

    static std::optional<Argument> TryParseFloat(std::string arg);
    static std::optional<Argument> TryParseInt(std::string arg);
    static std::optional<Argument> TryParseString(std::string arg);
};
class Signature {
    std::string keyword;
    std::vector<Argument::Type> arg_types;

  public:
    Signature(std::string keyword, std::vector<Argument::Type> arg_types);

    virtual std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) = 0;

    std::string GetKeyword() const;
    const std::vector<Argument::Type>& GetArgTypes() const;
};
struct Dispatcher {
    std::queue<std::unique_ptr<Cad::ICommand>> execution_queue;
    std::vector<std::unique_ptr<Signature>> signatures;

    struct ParserResult {
        bool success;
        std::vector<Argument> arguments;
        std::string error;

        static ParserResult Success(std::vector<Argument> arguments);
        static ParserResult Error(std::string error);

        bool IsSuccess();
        bool IsError();
    };

    static ParserResult Parse(Signature& signature, std::string arg_string);

  public:
    Dispatcher() {}

    void Register(std::unique_ptr<Signature> signature);

    void Dispatch(std::string command_string);

    void Execute(Controller& controller);
};

class Create : public ICommand {
    unsigned obj_id;
    bool is_forward;

  public:
    Create() {}

    virtual std::optional<unsigned>  BuildAndRegister(Controller& controller) = 0;

    void Forward(Controller& controller) override;

    void Backward(Controller& controller) override;
};

class CreateLine : public Create {
    Vector2 start;
    Vector2 end;

  public:
    CreateLine(Vector2 start, Vector2 end) : start(start), end(end) {}

    std::optional<unsigned>  BuildAndRegister(Controller& controller) override;

    struct By2Point : Cad::Command::Signature {
        By2Point()
            : Cad::Command::Signature("line_by_2point",
                  {Argument::Type::FLOAT, Argument::Type::FLOAT, Argument::Type::FLOAT, Argument::Type::FLOAT})
        {
        }

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            Vector2 start(args[0].GetFloat(), args[1].GetFloat());
            Vector2 end(args[2].GetFloat(), args[3].GetFloat());
            return std::make_unique<CreateLine>(start, end);
        }
    };

    struct ByStartAngleLength : Cad::Command::Signature {
        ByStartAngleLength()
            : Cad::Command::Signature("line_by_point_angle",
                  {Argument::Type::FLOAT, Argument::Type::FLOAT, Argument::Type::FLOAT, Argument::Type::FLOAT})
        {
        }

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            Vector2 start(args[0].GetFloat(), args[1].GetFloat());
            float angle = args[2].GetFloat();
            float length = args[3].GetFloat();

            Vector2 end = start + Vector2::FromAngle(angle) * length;

            return std::make_unique<CreateLine>(start, end);
        }
    };
};

class CreateCircle : public Create {
    Vector2 center;
    Vector2 radius;

  public:
    CreateCircle(Vector2 center, Vector2 radius) : center(center), radius(radius) {}

    std::optional<unsigned>  BuildAndRegister(Controller& controller) override;

    struct ByRadius : Cad::Command::Signature {
        ByRadius()
            : Cad::Command::Signature(
                  "circle_by_radius", {Argument::Type::FLOAT, Argument::Type::FLOAT, Argument::Type::FLOAT})
        {
        }

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            Vector2 center(args[0].GetFloat(), args[1].GetFloat());
            Vector2 radius(args[2].GetFloat(), args[1].GetFloat());
            return std::make_unique<CreateCircle>(center, radius);
        }
    };

    struct By2Point : Cad::Command::Signature {
        By2Point()
            : Cad::Command::Signature("circle_by_2point",
                  {Argument::Type::FLOAT, Argument::Type::FLOAT, Argument::Type::FLOAT, Argument::Type::FLOAT})
        {
        }

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            Vector2 start(args[0].GetFloat(), args[1].GetFloat());
            Vector2 end(args[2].GetFloat(), args[3].GetFloat());

            Vector2 radius = end - start;
            Vector2 center = start + radius * 0.5f;

            return std::make_unique<CreateCircle>(center, radius);
        }
    };
};

class SnapCommand : public ICommand {
    float size;
    float old_size;

  public:
    SnapCommand(float size) : size(size) {}

    void Forward(Controller& controller) override
    {
        old_size = controller.GetGridSize();
        controller.SetGridSize(size);
        controller.GetCameraController().Zero(controller.GetGraphics(), size);
    }

    void Backward(Controller& controller) override { controller.GetCursor().snap_size = old_size; }

    struct Signature : public Cad::Command::Signature {
        Signature() : Cad::Command::Signature("snap", {Argument::Type::FLOAT}) {}

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            float size = args[0].GetFloat();
            size = std::clamp(size, 1.0f, 50.0f);
            return std::make_unique<SnapCommand>(size);
        }
    };
};

struct ToggleSnapCommand : public ICommand {
    ToggleSnapCommand() = default;

    void Forward(Controller& controller) override
    {
        bool mode = controller.GetCursor().GetSnapMode();
        controller.GetCursor().SetSnapMode(!mode);
    }

    void Backward(Controller& controller) override { 

    }

    struct Signature : public Cad::Command::Signature {
        Signature() : Cad::Command::Signature("snap_toggle", {}) {}

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            return std::make_unique<ToggleSnapCommand>();
        }
    };
};

class ClearCommand : public ICommand {
  public:
    ClearCommand() {}

    void Forward(Controller& controller) override { controller.GetStaticContext().output.lines.clear(); }

    void Backward(Controller& controller) override {}

    struct Signature : public Cad::Command::Signature {
        Signature() : Cad::Command::Signature("clear", {}) {}

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            return std::make_unique<ClearCommand>();
        }
    };
};

struct PanCommand : public ICommand {
    Vector2 old_pos;
    Vector2 new_pos;

    PanCommand(Vector2 new_pos) : old_pos(0, 0), new_pos(new_pos) {}

    void Forward(Controller& controller) override
    {
        auto& camera = controller.GetCamera();
        auto& graphics = controller.GetGraphics();
        old_pos = Vector2(camera.pan_x, camera.pan_y);
        camera.pan_x = new_pos.x + graphics.GetWidth() * 0.5f;
        new_pos.x = camera.pan_x;
        camera.pan_y = new_pos.y + graphics.GetHeight() * 0.5f;
        new_pos.y = camera.pan_y;
    }

    void Backward(Controller& controller) override {  
        auto& camera = controller.GetCamera();
        camera.pan_x = old_pos.x;
        camera.pan_y = old_pos.y;
    }

    struct Signature : Cad::Command::Signature {
        Signature() : Cad::Command::Signature("pan", {Argument::Type::FLOAT, Argument::Type::FLOAT}) {}

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            Vector2 new_pos(args[0].GetFloat(), args[1].GetFloat());
            return std::make_unique<PanCommand>(new_pos);
        }
    };
};

struct ZoomCommand : public ICommand {
    float old_zoom;
    float new_zoom;

    ZoomCommand(float new_zoom) : old_zoom(0), new_zoom(new_zoom) {}

    void Forward(Controller& controller) override
    {
        auto& camera = controller.GetCamera();
        old_zoom = camera.scale;
        camera.scale = new_zoom;
    }

    void Backward(Controller& controller) override { controller.GetCamera().scale = old_zoom; }

    struct Signature : Cad::Command::Signature {
        Signature() : Cad::Command::Signature("zoom", {Argument::Type::FLOAT}) {}

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            float new_zoom = args[0].GetFloat();
            return std::make_unique<ZoomCommand>(new_zoom);
        }
    };
};


struct TranslateCommand : public ICommand {
    Vector2 delta;

    TranslateCommand(Vector2 delta) : delta(delta) {}

    void Forward(Controller& controller) override
    {
        Transform transform;
        transform.Translate(delta.x, delta.y);

        controller.GetWorld().Foreach([&](unsigned int id, IObject& object) {
            bool is_selected = controller.GetClipboard().IsSelected(id);
            if (is_selected) {
                object.Apply(transform);
            }
            return false;
        });
    }

    void Backward(Controller& controller) override 
    {
        Transform transform;
        transform.Translate(-delta.x, -delta.y);

        controller.GetWorld().Foreach([&](unsigned int id, IObject& object) {
            bool is_selected = controller.GetClipboard().IsSelected(id);
            if (is_selected) {
                object.Apply(transform);
            }
            return true;
        });
    }

    struct Signature : Cad::Command::Signature {
        Signature() : Cad::Command::Signature("translate", {Argument::Type::FLOAT, Argument::Type::FLOAT}) {}

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            Vector2 delta(args[0].GetFloat(), args[1].GetFloat());
            return std::make_unique<TranslateCommand>(delta);
        }
    };
};

struct PasteCommand : public ICommand {
    std::set<unsigned> create_ids;
    Vector2 delta;

    PasteCommand(float dx, float dy) : delta(Vector2(dx, dy)) {}

    void Forward(Controller& controller) override
    {
        Clipboard& clipboard = controller.GetClipboard();
        World& world = controller.GetWorld();

        clipboard.Foreach(controller, [&](unsigned int id, IObject& object) {
            auto new_id = world.GetNextId();
            if (!new_id.has_value()) return true;
            auto copy = object.Clone(new_id.value());
            Transform transform;
            transform.Translate(delta.x, delta.y);
            copy->Apply(transform);
            world.Register(new_id.value(), std::move(copy));
            // create_ids.insert(new_id.value());
            return false;
        });
    }

    void Backward(Controller& controller) override
    {
        // World& world = controller.GetWorld();
        // for (auto id : create_ids) {
        //     world.Remove(id);
        // }
    }

    struct Signature : Cad::Command::Signature {
        Signature() : Cad::Command::Signature("paste", {Argument::Type::FLOAT, Argument::Type::FLOAT}) {}

        std::unique_ptr<Cad::ICommand> Apply(std::vector<Argument>& args) override
        {
            float dx = args[0].GetFloat();
            float dy = args[1].GetFloat();
            return std::make_unique<PasteCommand>(dx, dy);
        }
    };
};

} // namespace Command
} // namespace Cad

namespace Cad {
struct IMode {
    IMode() {}

    virtual ~IMode() {}

    virtual void OnEnter(Controller& controller) {}

    virtual void OnExit(Controller& controller) {}

    virtual void Update(Controller& controller) = 0;

    std::string GetName() { return typeid(*this).name(); }
};

namespace Mode {
class Line : public IMode {
    std::stack<Vector2> points;

  public:
    Line() : points() {}

    void Update(Controller& controller) override;
};

class Circle : public IMode {
    std::stack<Vector2> points;

  public:
    Circle() : points() {}

    void Update(Controller& controller) override;
};

class Select : public IMode {

  public:
    Select() {}

    void Update(Controller& controller) override
    {
        Clipboard& clipboard = controller.GetClipboard();
        Input &input = controller.GetInput();

        std::cout << controller.GetWorld().GetObjectCount() << std::endl;

        // Establish any new selections in the clipboard
        controller.GetWorld().Foreach([&](unsigned int id, IObject& object) {
            bool is_hover = object.IsHovered(controller);
            bool ctrl_click = input.IsHeld(Key::LControl) && input.IsPressed(MouseButton::LEFT);
            bool is_interacted = is_hover && ctrl_click;
            bool is_selected = clipboard.IsSelected(id);

            if (!is_selected && is_interacted) {
                clipboard.Select(id);
            }
            
            if (is_selected && is_interacted) {
                clipboard.Deselect(id);
            }

            return false;
        });

    }
};
} // namespace Mode

} // namespace Cad
