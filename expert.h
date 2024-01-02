#ifndef EXPERT_H
#define EXPERT_H

#include <QMainWindow>
#include "macros.h"

namespace Ui {
class Expert;
}

class Expert : public QMainWindow
{
    Q_OBJECT

public:
    explicit Expert(QString* avg_algo, int* window_size, double* smoothing_factor,
                    bool* rf_only, QWidget *parent = nullptr);
    ~Expert();

private slots:
    void on_windowSize_valueChanged(int value);

    void on_avgAlgoChanged(const QString &avg_algo_text);

    void on_smoothingFactor_valueChanged(double value);

    void on_chkBoxRfOnly_stateChanged(int arg1);

private:
    Ui::Expert *ui;

    QString* avg_algo;
    int* window_size;
    double* smoothing_factor;
    bool* rf_only;
};

#endif // EXPERT_H
