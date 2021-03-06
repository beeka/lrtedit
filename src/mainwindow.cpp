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
#include "pageeditor.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

// From kayleeFrye_onDeck at
// https://stackoverflow.com/questions/2536524/copy-directory-using-qt
bool copyPath(QString sourceDir, QString destinationDir, bool overWriteDirectory)
{
  QDir originDirectory(sourceDir);

  if (!originDirectory.exists())
  {
    return false;
  }

  QDir destinationDirectory(destinationDir);

  if (destinationDirectory.exists() && !overWriteDirectory)
  {
    return false;
  }
  else if (destinationDirectory.exists() && overWriteDirectory)
  {
    destinationDirectory.removeRecursively();
  }

  originDirectory.mkpath(destinationDir);

  foreach (QString directoryName, originDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
  {
    QString destinationPath = destinationDir + "/" + directoryName;
    originDirectory.mkpath(destinationPath);
    copyPath(sourceDir + "/" + directoryName, destinationPath, overWriteDirectory);
  }

  foreach (QString fileName, originDirectory.entryList(QDir::Files))
  {
    QFile::copy(sourceDir + "/" + fileName, destinationDir + "/" + fileName);
  }

  /*! Possible race-condition mitigation? */
  QDir finalDestination(destinationDir);
  finalDestination.refresh();

  if (finalDestination.exists())
  {
    return true;
  }

  return false;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuBar->hide();

    ui->pagesPreview->setViewMode(QListWidget::IconMode);
    ui->pagesPreview->setIconSize(QSize(100, 100));
    ui->pagesPreview->setResizeMode(QListWidget::Adjust);

    determineRoots();
    // setRoot("C:\\Program Files\\Adobe\\Adobe Lightroom\\Templates\\Layout Templates");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::determineRoots()
{
  m_backupRoot = QDir::home().filePath("backups");
  qDebug() << "Determined backup root to be" << m_backupRoot;

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
    qInfo() << "Loading template from" << specificTemplatePages;
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

    m_layoutPages.clear();

    // pages/#/title
    // pages/#/pageWidth
    // pages/#/pageHeight
    // pages/#/previewName
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
            le.pos.setX(elements.getDouble(QString::number(e) + "/transform/x"));
            le.pos.setY(elements.getDouble(QString::number(e) + "/transform/y"));
            le.pos.setWidth(elements.getDouble(QString::number(e) + "/transform/width"));
            le.pos.setHeight(elements.getDouble(QString::number(e) + "/transform/height"));
            // placeholderType = "photo",
            if (elements.getString(QString::number(e) + "/placeholderType") == "photo")
            {
                le.index = elements.getInt(QString::number(e) + "/hints/photoIndex");
                lp.photos.resize(le.index);
                lp.photos[le.index - 1] = le;
            }
            else if (elements.getString(QString::number(e) + "/placeholderType") == "text")
            {
                le.index = elements.getInt(QString::number(e) + "/hints/textIndex");
                lp.text.resize(std::max(lp.text.size(), le.index));  // NB: max required as order is not guaranteed
                lp.text[le.index - 1] = le;
            }
        }
        const auto br = lp.boundingBox();
        qDebug() << "Bounding box is" << br;
        qDebug() << "Margins: top=" << br.top() << ", bottom=" << (lp.size.height() - br.bottom())
                 << ", left=" << br.left() << ", right=" << (lp.size.width() - br.right());

        const QImage image = lp.createImage();

        // Save a preview
        const QString previewPath = QDir::temp().filePath(lp.previewName);
        // const QString previewPath = QDir("C:/tmp").filePath(lp.previewName);
        image.scaled(QSize(100, 100), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation).save(previewPath);

        // Save a full png?
        //        const QString pngPath = previewPath.left(previewPath.length()
        //        - 4) + ".png";
        //        // qDebug() << "PNG saved to" <<
        //        image.save(pngPath);

        auto *item = new QListWidgetItem(QIcon(QPixmap::fromImage(image)),
                                         QString::number(i));
        item->setData(Qt::UserRole, QVariant::fromValue(m_layoutPages.size()));
        ui->pagesPreview->addItem(item);

        m_layoutPages.append(lp);

        //        ui->pagesPreview->addItem(
        //            new QListWidgetItem(QIcon(QPixmap::fromImage(image)),
        //            QString::number(i)));  // page.getString("title")));
#if 0
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

        const QString snappedPreviewPath = previewPath.left(previewPath.length() - 4) + "s.jpg";
        snappedImage.scaled(QSize(100, 100), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)
            .save(snappedPreviewPath);

        m_layoutPages.append(lp);

        ui->pagesPreview->addItem(
            new QListWidgetItem(QIcon(QPixmap::fromImage(snappedImage)), QString::number(i) + "s"));
#endif
    }

    // Dump updated positions
    for (auto const &lp : m_layoutPages)
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

        for (auto const &t : lp.text)
        {
            qInfo() << lp.name << "text" << t.index
                    << QString("transform = {height = %1, width = %2, x = %3, y = %4}")
                           .arg(t.pos.height())
                           .arg(t.pos.width())
                           .arg(t.pos.x())
                           .arg(t.pos.y());
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

void MainWindow::on_pagesPreview_itemDoubleClicked(QListWidgetItem *item)
{
  bool hasIndex = false;
  const int index = item->data(Qt::UserRole).toInt(&hasIndex);
  LayoutPage lp = m_layoutPages[index];

  PageEditor editor(this);
  editor.setPreviewImage(item->icon());
  if (hasIndex)
    editor.setLayoutPage(&lp);
  const int result = editor.exec();
  if (result == QDialog::Accepted)
  {
    if (hasIndex)
    {
      m_layoutPages[index] = lp;

      // Update the icon / preview
      const QImage image = lp.createImage();
      item->setIcon(QIcon(QPixmap::fromImage(image)));
    }
  }
}

void MainWindow::on_actionSave_triggered()
{
    // Update / generate the stored file with the current layout
    auto &table = m_currentTemplate;
    int page = 1;
    for (auto const &lp : m_layoutPages)
    {
        int element = 1;
        for (auto const &p : lp.photos)
        {
            const QString transform = QString("pages/%1/1/children/%2/transform").arg(page).arg(element);

            table.setAttr(transform + "/x", p.pos.x());
            table.setAttr(transform + "/y", p.pos.y());
            table.setAttr(transform + "/width", p.pos.width());
            table.setAttr(transform + "/height", p.pos.height());

            element++;
        }

        const QImage previewImage =
            lp.createImage().scaled(QSize(100, 100), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const QString previewPath = QFileInfo(m_currentTemplatePath).dir().filePath(lp.previewName);
        previewImage.save(previewPath);

        page++;
    }

    // Write the layout to a file
    {
        const LuaParser::NamedVariant nv("pages", QVariant::fromValue(m_currentTemplate));

        const QString s = LuaGenerator::Generate(nv);

        const QString path = m_currentTemplatePath;
        QFile f(path);
        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            qDebug() << "Writing" << s.size() << "bytes to" << path;
            f.write(s.toUtf8());
        }
        else
        {
            qCritical() << "Error opening file for writing:" << path;
        }
    }
}

void MainWindow::on_actionBackup_triggered() {
  const QString backupDir = QFileDialog::getExistingDirectory(this, tr("Select a directory to backup these custom pages"), m_backupRoot,
                                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  // const QString backupDir("C:/Users/Steven/backups");

  if (backupDir.isEmpty())
    return;

  // Update backup root?
  m_backupRoot = backupDir;

  // Copy m_currentTemplatePath to backupDir
  QStringList l = m_currentTemplatePath.split('/');  // QDir::separator());
  l.removeLast();  // Remove lua filename (e.g. templatePages.lua)
  l.removeLast();  // Remove template name (e.g. custompages13x11-blurb)
  const QString rootDir = l.join('/');  //(QDir::separator());

  const QString name = QFileInfo(rootDir).fileName();
  const QString destinationDir = QDir(backupDir).filePath(name + ".bak");
  const bool overWriteDirectory(false);
  qDebug() << "Copying" << rootDir << "to" << destinationDir;
  const bool okay = copyPath(rootDir, destinationDir, overWriteDirectory);
  qDebug() << "Copied" << rootDir << "to" << destinationDir << ": result =" << okay;
  if (okay)
    QMessageBox::information(this, "Backup Success", "Templates backed up to:\n" + destinationDir);
  else
    QMessageBox::critical(this, "Backup Failure", "Error backing up to:\n" + destinationDir);
}
