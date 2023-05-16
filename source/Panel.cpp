#include <Engine.h>

Alignment LayoutStrategy::GetAlignment()
{
    return alignment;
}

void LayoutStrategy::SetAlignment(Alignment alignment)
{
    this->alignment = alignment;
}

void VerticalLayout::Layout(Panel &parent, Panel &child)
{
    Vector2 parent_position = parent.GetPosition();
    Vector2 parent_size = parent.GetSize();

    Vector2 child_position = child.GetPosition();
    Vector2 child_size = child.GetSize();

    switch (GetAlignment())
    {
    case Alignment::START:
        child.SetPosition(layout_x, layout_y);
        break;
    case Alignment::MIDDLE:
        child.SetPosition(layout_x + (parent_size.x - child_size.x) / 2, layout_y);
        break;
    case Alignment::END:
        child.SetPosition(layout_x + parent_size.x - child_size.x, layout_y);
        break;
    }

    layout_y += child_size.y;
}

void VerticalLayout::Reset(unsigned x, unsigned y)
{
    layout_x = x;
    layout_y = y;
}

void HorizontalLayout::Layout(Panel &parent, Panel &child)
{
    Vector2 parent_position = parent.GetPosition();
    Vector2 parent_size = parent.GetSize();

    Vector2 child_position = child.GetPosition();
    Vector2 child_size = child.GetSize();

    switch (GetAlignment())
    {
    case Alignment::START:
        child.SetPosition(layout_x, layout_y);
        break;
    case Alignment::MIDDLE:
        child.SetPosition(layout_x, layout_y + (parent_size.y - child_size.y) / 2);
        break;
    case Alignment::END:
        child.SetPosition(layout_x, layout_y + parent_size.y - child_size.y);
        break;
    }

    layout_x += child_size.x;
}

void HorizontalLayout::Reset(unsigned x, unsigned y)
{
    layout_x = x;
    layout_y = y;
}

Panel::Panel(unsigned width, unsigned height) : Component(width, height)
{
    m_layout_strategy = std::make_unique<VerticalLayout>();
}

void Panel::Input(State &state)
{
    OnInput(state);

    for (std::shared_ptr<Panel> &child : m_children)
    {
        child->Input(state);
    }
}

void Panel::Update(State &state)
{
    m_layout_strategy->Reset(GetPosition().x, GetPosition().y);
    for (std::shared_ptr<Panel> &child : m_children)
    {
        if (m_layout_strategy)
            m_layout_strategy->Layout(*this, *child);
    }

    OnUpdate(state);

    for (std::shared_ptr<Panel> &child : m_children)
    {
        child->Update(state);
    }
}

void Panel::Render(State &state)
{
    OnRender(state);

    for (std::shared_ptr<Panel> &child : m_children)
    {
        child->Render(state);
    }
}

void Panel::AddChild(std::shared_ptr<Panel> child)
{
    m_children.push_back(child);
}
