#include "macros.h"

void showUI(QWidget* widget)
{
    widget->show();
    widget->activateWindow();
    widget->setMaximumSize(widget->width(), widget->height());
    widget->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, widget->size(), qApp->desktop()->availableGeometry()));
}

void loadUI(QString fileName)
{
    QUiLoader loader;
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QWidget* w = loader.load(&file);
    QObject::connect(w->findChild<QPushButton*>("btnClose"), SIGNAL(clicked(bool)), w, SLOT(close()));
    showUI(w);
}
