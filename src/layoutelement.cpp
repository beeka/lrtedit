//  This file is part of LrtEdit.
//
// LrtEdit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LrtEdit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LrtEdit.  If not, see <https://www.gnu.org/licenses/>.
#include "layoutelement.h"

static bool verticallyOverlap(const QRectF &a, const QRectF &b)
{
    return a.top() <= b.bottom() && a.bottom() >= b.top();
}

static bool horizontallyOverlap(const QRectF &a, const QRectF &b)
{
    return a.left() <= b.right() && a.right() >= b.left();
}

bool LayoutElement::isToTheRightOf(const LayoutElement &other) const
{
    return verticallyOverlap(this->pos, other.pos) && this->pos.left() >= other.pos.right();
}

bool LayoutElement::isToTheLeftOf(const LayoutElement &other) const
{
    return verticallyOverlap(this->pos, other.pos) && this->pos.right() >= other.pos.left();
}

bool LayoutElement::isBelow(const LayoutElement &other) const
{
    return horizontallyOverlap(this->pos, other.pos) && this->pos.top() >= other.pos.bottom();
}

bool LayoutElement::isAbove(const LayoutElement &other) const
{
    return horizontallyOverlap(this->pos, other.pos) && this->pos.bottom() >= other.pos.top();
}

void LayoutElement::snapTopToGrid(const double spacing)
{
    const double diff = fmod(pos.top(), spacing);
    if (diff < spacing / 2)
    {
        pos.setTop(pos.top() - diff);
    }
    else
    {
        pos.setTop(pos.top() + (spacing - diff));
    }
}

void LayoutElement::snapBottomToGrid(const double spacing)
{
    const double diff = fmod(pos.bottom(), spacing);
    if (diff < spacing / 2)
    {
        pos.setBottom(pos.bottom() - diff);
    }
    else
    {
        pos.setBottom(pos.bottom() + (spacing - diff));
    }
}

void LayoutElement::snapLeftToGrid(const double spacing)
{
    const double diff = fmod(pos.left(), spacing);
    if (diff < spacing / 2)
    {
        pos.setLeft(pos.left() - diff);
    }
    else
    {
        pos.setLeft(pos.left() + (spacing - diff));
    }
}

void LayoutElement::snapRightToGrid(const double spacing)
{
    const double diff = fmod(pos.right(), spacing);
    if (diff < spacing / 2)
    {
        pos.setRight(pos.right() - diff);
    }
    else
    {
        pos.setRight(pos.right() + (spacing - diff));
    }
}

void LayoutElement::snapToGrid(const double spacing)
{
    snapTopToGrid(spacing);
    snapBottomToGrid(spacing);
    snapLeftToGrid(spacing);
    snapRightToGrid(spacing);
}
