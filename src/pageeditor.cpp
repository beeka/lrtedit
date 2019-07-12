#include "pageeditor.h"
#include "ui_pageeditor.h"

PageEditor::PageEditor(QWidget *parent) :
                                          QDialog(parent),
                                          ui(new Ui::PageEditor)
{
    ui->setupUi(this);
}

PageEditor::~PageEditor()
{
    delete ui;
}

void PageEditor::setPreviewImage(const QIcon &icon)
{
    ui->previewImage->setPixmap(icon.pixmap(icon.availableSizes().first()));
}
