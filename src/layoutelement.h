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
#ifndef LAYOUTELEMENT_H
#define LAYOUTELEMENT_H

#include <QRectF>

// A photo or text box on a LayoutPage
class LayoutElement
{
   public:
    int index;
    QRectF pos;

    /// Returns true if this element is completely to the right of the other.
    /// The rectangles must overlap in the vertical axis.
    bool isToTheRightOf(const LayoutElement &other) const;

    /// Returns true if this element is completely to the left of the other.
    /// The rectangles must overlap in the vertical axis.
    bool isToTheLeftOf(const LayoutElement &other) const;

    /// Returns true if this element is completely below the other.
    /// The rectangles must overlap in the horizontal axis.
    bool isBelow(const LayoutElement &other) const;

    /// Returns true if this element is completely below the other.
    /// The rectangles must overlap in the horizontal axis.
    bool isAbove(const LayoutElement &other) const;

    void snapToGrid(const double spacing);
    void snapTopToGrid(const double spacing);
    void snapBottomToGrid(const double spacing);
    void snapLeftToGrid(const double spacing);
    void snapRightToGrid(const double spacing);
};

#endif // LAYOUTELEMENT_H
