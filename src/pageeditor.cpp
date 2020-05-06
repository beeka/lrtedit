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

#include "pageeditor.h"
#include "ui_pageeditor.h"

#include "layoutpage.h"
#include "layoutpagemodel.h"

namespace
{
const double grid = 0.5;
const double border = 52.5;  // All-round margin
const QMarginsF margin(border, border, border, border);
const int spacing = 10;
const int captureWidth = 42;
}  // namespace

PageEditor::PageEditor(QWidget *parent)
    : QDialog(parent), ui(new Ui::PageEditor), m_layoutPage(nullptr), m_layoutPageModel(nullptr)
{
  ui->setupUi(this);
}

PageEditor::~PageEditor()
{
  delete ui;
  delete m_layoutPageModel;
}

void PageEditor::refreshPreviewImage()
{
  if (!m_layoutPage)
    return;

  ui->summaryLbl->setText(
      QString("%1 [%2 x %3]").arg(m_layoutPage->name).arg(m_layoutPage->size.width()).arg(m_layoutPage->size.height()));

  const QImage snappedImage = m_layoutPage->createImage(true);

  const auto icon = QIcon(QPixmap::fromImage(snappedImage));

  ui->previewImage->setPixmap(icon.pixmap(icon.availableSizes().first()));
}

void PageEditor::setPreviewImage(const QIcon &icon)
{
  ui->previewImage->setPixmap(icon.pixmap(icon.availableSizes().first()));
}

void PageEditor::setLayoutPage(LayoutPage *lp)
{
  m_layoutPage = lp;

  if (m_layoutPageModel)
    delete m_layoutPageModel;

  m_layoutPageModel = new LayoutPageModel(lp);
  ui->tableView->setModel(m_layoutPageModel);

  refreshPreviewImage();

  connect(m_layoutPageModel, &LayoutPageModel::dataChanged, this, &PageEditor::refreshPreviewImage);
}

void PageEditor::on_snapMarginsBtn_clicked()
{
  if (!m_layoutPage)
    return;

  m_layoutPage->alignToMargins(margin, captureWidth);
  m_layoutPageModel->invalidate();
}

void PageEditor::on_applySpacingBtn_clicked()
{
  if (!m_layoutPage)
    return;

  m_layoutPage->setSpacing(spacing, captureWidth);
  m_layoutPageModel->invalidate();
}

void PageEditor::on_snapToGridBtn_clicked()
{
  if (!m_layoutPage)
    return;

  m_layoutPage->snapToGrid(grid);
  m_layoutPageModel->invalidate();
}
