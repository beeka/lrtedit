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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "luaparser.h"

#include "layoutpage.h"

#include <QList>
#include <QListWidgetItem>
#include <QMainWindow>

// From https://forums.adobe.com/thread/1254145, can use local templates:
// C:\Users\XXXXXX\AppData\Roaming\Adobe\Lightroom\Layout Templates\12x12-blurb
//     mypages10x8-blurb.lrtemplate
//     mypages10x8-blurb
//         mypages1411562662_preview.jpg
//         templatePages.lua

// Right-clicking on a book page and "Save as Custom Page" will copy the page to
// C:\Users\XXXXX\AppData\Roaming\Adobe\Lightroom\Layout Templates

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

   public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   protected slots:
    void determineRoots();
    void setRoot(const QString &path);
    void loadTemplateSizes(const QString &path);
    void loadTemplates(const QString &path);

    /// @param specificTemplatePages The path to the .lua file which contains the template to modify
    void loadTemplate(const QString &specificTemplatePages);
   private slots:
    void on_actionOpen_triggered();

    void on_templateSizesCB_currentIndexChanged(int index);

    void on_pagesPreview_itemDoubleClicked(QListWidgetItem *item);

    void on_actionSave_triggered();

   private:
    Ui::MainWindow *ui;

    /// The top-level installation directory, e.g. %ProgramFiles%\Adobe\Adobe Lightroom\Templates\Layout Templates
    QString m_installRoot;

    /// The top-level user app data directory, e.g. %APPDATA%\Adobe\Lightroom\Layout Templates
    QString m_userRoot;

    LuaParser::Table m_templateSizes;

    QString m_currentTemplatePath;
    LuaParser::Table m_currentTemplate;
    QList<LayoutPage> m_layoutPages;
};

#endif // MAINWINDOW_H
