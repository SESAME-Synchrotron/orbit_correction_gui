#ifndef IGAINS_H
#define IGAINS_H

#include <QMainWindow>
#include <QMessageBox>
#include "widgets/pidgainwidget.h"
#include "client.h"

namespace Ui {
class IGains;
}

class IGains : public QMainWindow
{
    Q_OBJECT

public:
    explicit IGains(QWidget *parent = nullptr);
    ~IGains();

private slots:
    void on_btnGlobalSet_clicked();

private:
    Ui::IGains *ui;

    double* global_data;
    const int bpm_count;
};

#endif // IGAINS_H
