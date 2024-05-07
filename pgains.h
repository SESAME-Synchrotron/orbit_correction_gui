#ifndef PGAINS_H
#define PGAINS_H

#include <QMainWindow>
#include <QMessageBox>
#include "widgets/pidgainwidget.h"
#include "client.h"
#include "macros.h"

namespace Ui {
class PGains;
}

class PGains : public QMainWindow
{
    Q_OBJECT

public:
    explicit PGains(QWidget *parent = nullptr);
    ~PGains();

private slots:
    void on_btnGlobalSet_clicked();

private:
    Ui::PGains *ui;

    double* global_data;
    const int bpm_count;
};

#endif // PGAINS_H
