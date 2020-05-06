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
#ifndef LAYOUTPAGEMODEL_H
#define LAYOUTPAGEMODEL_H

#include <QAbstractTableModel>

class LayoutPage;

class LayoutPageModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  LayoutPageModel(LayoutPage *lp);

  enum
  {
    colType,
    colIndex,
    colWidth,
    colHeight,
    colX,
    colY,
    colCount
  };

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

 public slots:
  /// Mark the model as being changed (e.g. the m_layoutPage object was changed externally)
  void invalidate();

 private:
  LayoutPage *m_layoutPage;
};

#endif  // LAYOUTPAGEMODEL_H
