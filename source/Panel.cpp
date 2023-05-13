#include <Window.h>

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

void GridLayoutStrategy::Layout(Panel &parent, Panel &child)
{
    Vector2 parent_position = parent.GetPosition();
    Vector2 parent_size = parent.GetSize();

    Vector2 child_position = child.GetPosition();
    Vector2 child_size = child.GetSize();

    Vector2 grid_size = parent_size / Vector2(grid_width, grid_height);

    Vector2 position = parent_position + Vector2(index_x, index_y) * grid_size; 
    child.SetPosition(position.x, position.y);

    index_x++;
    if (index_x >= grid_width)
    {
        index_x = 0;
        index_y++;
    }
}

void GridLayoutStrategy::Reset(unsigned x, unsigned y)
{
    layout_x = x;
    layout_y = y;
    index_x = 0;
    index_y = 0;
}

void Panel::Update(State &state)
{
    m_layout_strategy->Reset(GetPosition().x, GetPosition().y);
    for (std::unique_ptr<Panel> &child : m_children)
    {
        if (m_layout_strategy)
            m_layout_strategy->Layout(*this, *child);
    }

    OnUpdate(state);

    m_border->Draw(*this, state.graphics);

    for (std::unique_ptr<Panel> &child : m_children)
    {
        child->Update(state);
    }
}

void Panel::AddChild(std::unique_ptr<Panel> child)
{
    m_children.push_back(std::move(child));
}
