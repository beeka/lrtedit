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
#ifndef PAGEEDITOR_H
#define PAGEEDITOR_H

#include <QDialog>
#include <QIcon>

class LayoutPage;
class LayoutPageModel;

namespace Ui {
class PageEditor;
}

class PageEditor : public QDialog
{
    Q_OBJECT

   public:
    explicit PageEditor(QWidget *parent = nullptr);
    ~PageEditor();

   public slots:
    void refreshPreviewImage();

    void setPreviewImage(const QIcon &icon);

    void setLayoutPage(LayoutPage *lp);

   private slots:
    void on_snapMarginsBtn_clicked();

    void on_applySpacingBtn_clicked();

    void on_snapToGridBtn_clicked();

   private:
    Ui::PageEditor *ui;
    LayoutPage *m_layoutPage;
    LayoutPageModel *m_layoutPageModel;
};

#endif // PAGEEDITOR_H
