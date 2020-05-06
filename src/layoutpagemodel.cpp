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

#include "layoutpagemodel.h"
#include "layoutpage.h"

LayoutPageModel::LayoutPageModel(LayoutPage *lp) : m_layoutPage(lp)
{
}

int LayoutPageModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return colCount;
}

QVariant LayoutPageModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    int row = index.row();
    const bool isPhoto = (index.row() < m_layoutPage->photos.size());
    if (!isPhoto)
      row -= m_layoutPage->photos.size();

    const LayoutElement *le = nullptr;
    if (isPhoto)
      le = &(m_layoutPage->photos[row]);
    else
      le = &(m_layoutPage->text[row]);

    switch (index.column())
    {
      case colType:
        if (isPhoto)
          return QString("Photo");
        else
          return QString("Text");
      case colIndex:
        return QVariant::fromValue(le->index);
      case colWidth:
        return QVariant::fromValue(le->pos.width());
      case colHeight:
        return QVariant::fromValue(le->pos.height());
      case colX:
        return QVariant::fromValue(le->pos.x());
      case colY:
        return QVariant::fromValue(le->pos.y());
      case colCount:
        return QVariant();  // Should not get here!
    }
  }

  return QVariant();
}

Qt::ItemFlags LayoutPageModel::flags(const QModelIndex &index) const
{
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant LayoutPageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch (section)
    {
      case colType:
        return QString("Type");
      case colIndex:
        return QString("#");
      case colWidth:
        return QString("W");
      case colHeight:
        return QString("H");
      case colX:
        return QString("X");
      case colY:
        return QString("Y");
      case colCount:
        return QVariant();  // Should not get here!
    }
  }

  return QVariant();
}

int LayoutPageModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_layoutPage->photos.size() + m_layoutPage->text.size();
}

bool LayoutPageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (role == Qt::EditRole)
  {
    if (!index.isValid() || index.column() <= colIndex)
      return false;

    int row = index.row();
    const bool isPhoto = (index.row() < m_layoutPage->photos.size());
    if (!isPhoto)
      row -= m_layoutPage->photos.size();

    LayoutElement *le = nullptr;
    if (isPhoto)
      le = &(m_layoutPage->photos[row]);
    else
      le = &(m_layoutPage->text[row]);

    switch (index.column())
    {
      case colType:
        return false;
      case colIndex:
        return false;
      case colWidth:
        le->pos.setWidth(value.toReal());
        break;
      case colHeight:
        le->pos.setHeight(value.toReal());
        break;
      case colX:
        // le->pos.setX(value.toReal());
        le->pos.moveTo(value.toReal(), le->pos.y());
        break;
      case colY:
        // le->pos.setY(value.toReal());
        le->pos.moveTo(le->pos.x(), value.toReal());
        break;
      case colCount:
        return false;  // Should not get here!
    }

    emit dataChanged(index, index);

    return true;
  }

  return false;
}

void LayoutPageModel::invalidate()
{
  emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}
