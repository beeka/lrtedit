#ifndef PAGEEDITOR_H
#define PAGEEDITOR_H

#include <QDialog>
#include <QIcon>

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
    void setPreviewImage(const QIcon &icon);

   private:
    Ui::PageEditor *ui;
};

#endif // PAGEEDITOR_H
