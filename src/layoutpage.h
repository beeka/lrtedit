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
#ifndef LAYOUTPAGE_H
#define LAYOUTPAGE_H

#include "layoutelement.h"

#include <QImage>
#include <QMarginsF>
#include <QVector>

class LayoutPage
{
   public:
    QString name;
    QString previewName;

    QSize size;

    QVector<LayoutElement> photos;
    // QVector<LayoutElement> text;

    QRectF boundingBox() const;

    void setHorizontalSpacing(const int spacing, int captureWidth = 42);

    void setVerticalSpacing(const int spacing, int captureWidth = 42);

    void setSpacing(const int spacing, int captureWidth = 42);

    void snapToGrid(const double spacing);

    /// Snap the edges of the boxes to the desired margin
    void alignToMargins(const QMarginsF &margin, int captureWidth = 42);

    QImage createImage() const;
};

#endif // LAYOUTPAGE_H
