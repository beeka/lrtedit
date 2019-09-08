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
#include "layoutpage.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>

#include <math.h>

// Ignore comparisons with floats for this translation unit, as we only want exact matches anyway
#pragma clang diagnostic ignored "-Wfloat-equal"

QRectF LayoutPage::boundingBox() const
{
    QRectF br = photos.value(0).pos;
    for (auto const &le : photos)
    {
        br = br.united(le.pos);
    }
    return br;
}

void LayoutPage::setHorizontalSpacing(const int spacing, int captureWidth)
{
    for (auto &subject : photos)
    {
        for (auto &other : photos)
        {
            if (other.isToTheRightOf(subject))
            {
                // Calculate the gap
                auto currentGap = fabs(subject.pos.right() - other.pos.left());
                // qDebug() << subject.index << other.index << "horz gap is" << currentGap;
                if (currentGap > 0.1 && currentGap != spacing && currentGap <= captureWidth)
                {
                    auto const desiredEdge = subject.pos.right() + spacing;
                    qDebug() << "Setting horizontal gap of" << subject.index << "to" << other.index << "from"
                             << currentGap << ". Left edge changing from" << other.pos.left() << "to" << desiredEdge;
                    other.pos.setLeft(desiredEdge);
                }
            }
        }
    }
}

void LayoutPage::setVerticalSpacing(const int spacing, int captureWidth)
{
    for (auto &subject : photos)
    {
        for (auto &other : photos)
        {
            if (other.isBelow(subject))
            {
                // Calculate the gap
                auto currentGap = fabs(subject.pos.bottom() - other.pos.top());
                // qDebug() << subject.index << other.index << "vert gap is" << currentGap;
                if (currentGap > 0.1 && currentGap != spacing && currentGap <= captureWidth)
                {
                    auto const desiredEdge = subject.pos.bottom() + spacing;
                    qDebug() << "Setting vertical gap of" << subject.index << "to" << other.index << "from"
                             << currentGap << ". Top edge changing from" << other.pos.top() << "to" << desiredEdge;
                    other.pos.setTop(desiredEdge);
                }
            }
        }
    }
}


void LayoutPage::setSpacing(const int spacing, int captureWidth)
{
    setHorizontalSpacing(spacing, captureWidth);
    setVerticalSpacing(spacing, captureWidth);
}


void LayoutPage::snapToGrid(const double spacing)
{
    for (auto &p : photos)
    {
        p.snapToGrid(spacing);
    }
}


void LayoutPage::alignToMargins(const QMarginsF &margin, int captureWidth)
{
    Q_ASSERT(!size.isNull());

    const auto frame = QRectF(0, 0, size.width(), size.height()).marginsRemoved(margin);
    for (auto &p : photos)
    {
        if (frame.top() != p.pos.top() && fabs(frame.top() - p.pos.top()) < captureWidth)
        {
            qDebug() << "Aligning top margin of" << p.index << "from" << p.pos.top() << "to" << frame.top();
            p.pos.setTop(frame.top());
        }

        if (frame.bottom() != p.pos.bottom() && fabs(frame.bottom() - p.pos.bottom()) < captureWidth)
        {
            qDebug() << "Aligning bottom margin of" << p.index << "from" << p.pos.bottom() << "to" << frame.bottom();
            p.pos.setBottom(frame.bottom());
        }

        if (frame.left() != p.pos.left() && fabs(frame.left() - p.pos.left()) < captureWidth)
        {
            qDebug() << "Aligning left margin of" << p.index << "from" << p.pos.left() << "to" << frame.left();
            p.pos.setLeft(frame.left());
        }

        if (frame.right() != p.pos.right() && fabs(frame.right() - p.pos.right()) < captureWidth)
        {
            qDebug() << "Aligning right margin of" << p.index << "from" << p.pos.right() << "to" << frame.right();
            p.pos.setRight(frame.right());
        }
    }
}

QImage LayoutPage::createImage() const
{
    QGraphicsScene scene;
    // scene.setBackgroundBrush(Qt::lightGray);

    scene.addRect(QRectF(QPointF(0, 0), size), QPen(Qt::lightGray), QBrush(Qt::white));

    for (auto const &p : photos)
    {
        scene.addRect(p.pos, QPen(Qt::black), QBrush(Qt::darkGray));
        // add a cross
        QPen pen(Qt::darkGreen, 5);
        const int w = 20;  // pt in each direction
        const auto c = p.pos.center();
        scene.addLine(c.x() - w, c.y(), c.x() + w, c.y(), pen);
        scene.addLine(c.x(), c.y() - w, c.x(), c.y() + w, pen);
    }

    for (auto const &t : text)
    {
        // Draw our own fill, rather than Qt::HorPattern, to get better control of scaling
        const int s = 20;  // Space between (top of) each line
        const int w = 5;   // Line width
        const QColor textColour(Qt::black);

        // NB: drawing from the bottom, as the axis is inverted (and the drawing is flipped)
        for (qreal r = t.pos.bottom(); r > (t.pos.top() + s); r -= s)
        {
            const QRectF line(t.pos.left(), r - w, t.pos.width(), w);
            scene.addRect(line, QPen(Qt::transparent), QBrush(textColour));
        }
        // draw half a line at the top (i.e. bottom when flipped)
        const QRectF line(t.pos.topLeft(), QSizeF(t.pos.width() / 2, w));
        scene.addRect(line, QPen(Qt::transparent), QBrush(textColour));
    }

    scene.clearSelection();                         // Selections would also render to the file
    scene.setSceneRect(scene.itemsBoundingRect());  // Re-shrink the scene to it's bounding contents
    // Create the image with the exact size of the shrunk scene
    QImage image(scene.sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);  // Start all pixels transparent

    QPainter painter(&image);
    scene.render(&painter);
    return image.mirrored(false, true);  // Mirror vertically as the axis is inverted
}
