#ifndef DGAINS_H
#define DGAINS_H

#include <QMainWindow>
#include <QMessageBox>
#include "widgets/pidgainwidget.h"
#include "client.h"

namespace Ui {
class DGains;
}

class DGains : public QMainWindow
{
    Q_OBJECT

public:
    explicit DGains(QWidget *parent = nullptr);
    ~DGains();

private slots:
    void on_btnGlobalSet_clicked();

private:
    Ui::DGains *ui;

    double* global_data;
    const int bpm_count;
};

#endif // DGAINS_H
