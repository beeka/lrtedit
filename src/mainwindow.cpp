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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "layoutpage.h"
#include "luagenerator.h"
#include "luaparser.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
                                          QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->listWidget->setViewMode(QListWidget::IconMode);
    ui->listWidget->setIconSize(QSize(100, 100));
    ui->listWidget->setResizeMode(QListWidget::Adjust);

    determineRoots();
    // setRoot("C:\\Program Files\\Adobe\\Adobe Lightroom\\Templates\\Layout Templates");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::determineRoots()
{
    // const QString programFiles = qgetenv("ProgramFiles");
    const QString programFiles = qgetenv("ProgramW6432");
    m_installRoot = QDir(programFiles).filePath("Adobe/Adobe Lightroom/Templates/Layout Templates");
    if (!QFile::exists(m_installRoot))
    {
        qCritical() << "Path does not exist!" << m_installRoot;
        m_installRoot.clear();
    }
    else
    {
        qDebug() << "Determined install root to be" << m_installRoot;
    }

    const QString appData = qgetenv("APPDATA");
    // const QString programFiles = qgetenv("ProgramFiles");
    m_userRoot = QDir(appData).filePath("Adobe/Lightroom/Layout Templates");
    if (!QFile::exists(m_userRoot))
    {
        qCritical() << "Path does not exist!" << m_userRoot;
        m_userRoot.clear();
    }
    else
    {
        qDebug() << "Determined user root to be" << m_userRoot;
    }

    loadTemplateSizes(m_installRoot);
    loadTemplates(m_userRoot);
}

void MainWindow::setRoot(const QString &path)
{
    qInfo() << "Setting root to" << path;

    loadTemplateSizes(path);
}


void MainWindow::loadTemplateSizes(const QString &path)
{
    const QString templateSizesPath = QDir(path).filePath("layout_template_sizes.lua");
    qInfo() << "Reading template sizes from" << templateSizesPath;

    using namespace LuaParser;
    const NamedVariant nv = readLuaStruct(templateSizesPath);

    // Convert back to text for viewing
    ui->textEdit->setText(LuaGenerator::Generate(nv));

    m_templateSizes = nv.value().value<Table>();

    ui->templateSizesCB->clear();

    for (int i = 1; i <= m_templateSizes.hash(); i++)
    {
        const Table format = m_templateSizes[i].value<Table>();
        // qDebug() << i << "entry is" << format.hash() << format.keys();
        // const QString title = format["title"].toString();
        const QString id = format["id"].toString();
        // const int w = format["width"].toInt();
        // const int h = format["height"].toInt();
        ui->templateSizesCB->addItem(id, QVariant::fromValue(format));
    }
}


void MainWindow::loadTemplates(const QString &path)
{
    // e.g. C:\Users\XXXX\AppData\Roaming\Adobe\Lightroom\Layout Templates
    // will have a directory, e.g. "13x11-blurb", with *.lrtemplate files, e.g. "custompages13x11-blurb.lrtemplate"
    // and a matching directory with the actual template, e.g. "custompages13x11-blurb\templatePages.lua"

    QDir templateRoot(path);
    for (const auto &subd : templateRoot.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        qDebug() << "template root\\" << subd;
        QDir d2(templateRoot.filePath(subd));
        for (const auto &lrtd : d2.entryList(QStringList() << "*.lrtemplate", QDir::Files))
        {
            qDebug() << "Found lrtd" << lrtd;

            // Could read the template file and extract the "value\resources" attr
            // but there is a strong pattern, so just guess the template base dir
            const QString specificTemplateBase = QDir(d2).filePath(QFileInfo(lrtd).baseName());
            const QString specificTemplatePages = QDir(specificTemplateBase).filePath("templatePages.lua");
            if (!QFile::exists(specificTemplatePages))
            {
                qWarning() << "Exepcted pages lua not found:" << specificTemplatePages;
                continue;  // Try the next one
            }

            ui->templatesCB->addItem(specificTemplatePages);
            loadTemplate(specificTemplatePages);
        }
    }
}


void MainWindow::loadTemplate(const QString &specificTemplatePages)
{
    m_currentTemplatePath = specificTemplatePages;

    using namespace LuaParser;
    const NamedVariant nv = readLuaStruct(specificTemplatePages);
    m_currentTemplate = nv.value().value<Table>();

    // Convert back to text for viewing
    ui->textEdit->setText(LuaGenerator::Generate(nv));

    // Read the title from "hints\bookTitle", e.g. "Custom Pages"
    const QString groupTitle = m_currentTemplate.getString("hints/bookTitle");
    qDebug() << "book title is" << groupTitle;

    auto pages = m_currentTemplate.getAttr("pages").value<LuaParser::Table>();
    qDebug() << pages.hash() << "pages";

    QList<LayoutPage> layoutPages;

    // pages/#/title
    // pages/#/pageWidth
    // pages/#/pageHeight
    // pages/#/previewName
    // pages/#/1/children/#/hints/placeholderType = "photo"
    // pages/#/1/children/#/hints/placeholderType = "photo"
    // pages/#/1/children/#/hints/photoIndex
    // pages/#/1/children/#/transform/x
    // pages/#/1/children/#/transform/y
    // pages/#/1/children/#/transform/width
    // pages/#/1/children/#/transform/height
    for (int i = 1; i <= pages.hash(); i++)
    {
        auto page = pages[i].value<LuaParser::Table>();

        LayoutPage lp;
        lp.name = page.getString("name");
        lp.previewName = page.getString("previewName");
        lp.size.setWidth(page.getInt("pageWidth"));
        lp.size.setHeight(page.getInt("pageHeight"));

        // qDebug() << page.getInt("pageWidth") << page.getInt("pageHeight") << page.getString("previewName");
        auto elements = page.getAttr("1/children").value<LuaParser::Table>();
        for (int e = 1; e <= elements.hash(); e++)
        {
            LayoutElement le;
            le.index = elements.getInt(QString::number(e) + "/hints/photoIndex");
            le.pos.setX(elements.getDouble(QString::number(e) + "/transform/x"));
            le.pos.setY(elements.getDouble(QString::number(e) + "/transform/y"));
            le.pos.setWidth(elements.getDouble(QString::number(e) + "/transform/width"));
            le.pos.setHeight(elements.getDouble(QString::number(e) + "/transform/height"));
            lp.photos.resize(le.index);
            lp.photos[le.index - 1] = le;
        }
        const auto br = lp.boundingBox();
        qDebug() << "Bounding box is" << br;
        qDebug() << "Margins: top=" << br.top() << ", bottom=" << (lp.size.height() - br.bottom())
                 << ", left=" << br.left() << ", right=" << (lp.size.width() - br.right());

        const QImage image = lp.createImage();

        // Save a preview
        const QString previewPath = QDir("c:/tmp").filePath(lp.previewName);
        image.scaled(QSize(100, 100), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation).save(previewPath);

        // Save a full png?
        const QString pngPath = previewPath.left(previewPath.length() - 4) + ".png";
        // qDebug() << "PNG saved to" <<
        image.save(pngPath);

        ui->listWidget->addItem(
            new QListWidgetItem(QIcon(QPixmap::fromImage(image)), QString::number(i)));  // page.getString("title")));

        const double grid = 0.5;
        const double border = 52.5;  // All-round margin
        const QMarginsF margin(border, border, border, border);
        // const QMarginsF margin(56.5, 56.5, 56.5, 56.5);
        const int spacing = 10;
        const int captureWidth = 42;

        lp.snapToGrid(grid);
        lp.alignToMargins(margin, captureWidth);
        lp.setSpacing(spacing, captureWidth);

        const QImage snappedImage = lp.createImage();
        const QString snappedPath = previewPath.left(previewPath.length() - 4) + "snapped.png";
        snappedImage.save(snappedPath);
        layoutPages.append(lp);

        ui->listWidget->addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(snappedImage)), QString::number(i) + "s"));
    }

    // Dump updated positions

    for (auto const &lp : layoutPages)
    {
        for (auto const &p : lp.photos)
        {
            qInfo() << lp.name << "photo" << p.index
                    << QString("transform = {height = %1, width = %2, x = %3, y = %4}")
                           .arg(p.pos.height())
                           .arg(p.pos.width())
                           .arg(p.pos.x())
                           .arg(p.pos.y());
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    // QFileDialog::getExistingDirectory(this, "Select Lightroom Template
    // Directory")
    const QString path =
        QFileDialog::getOpenFileName(this, tr("Select Lightroom Template Directory"),
                                     "C:\\Program Files\\Adobe\\Adobe Lightroom\\Templates\\Layout Templates",
                                     tr("Top Level (layout_template_sizes.lua)"));

    // Check we were given a file path
    if (path.isEmpty()) return;

    qDebug() << path << "selected";

    // Determine templates directory
    QString templatesDir = QFileInfo(path).absoluteDir().path();

    qInfo() << "Template directory is " << templatesDir;

    setRoot(templatesDir);
}

void MainWindow::on_templateSizesCB_currentIndexChanged(int index) {}
