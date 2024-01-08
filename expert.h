#ifndef EXPERT_H
#define EXPERT_H

#include <QMainWindow>
#include <qepicspv.h>
#include "macros.h"

namespace Ui {
class Expert;
}

class Expert : public QMainWindow
{
    Q_OBJECT

public:
    explicit Expert(QWidget *parent = nullptr);
    ~Expert();

private slots:
    void onRfOnlyInit(const QVariant &);

    void onDebugModeInit(const QVariant &);

    void onCorrectionStatusInit(const QVariant &);

    void onCorrectionStatusChanged(const QVariant &);

    void enableInputs();

    void disableInputs();

private:
    Ui::Expert *ui;

    QEpicsPV* rf_only;
    QEpicsPV* debug_mode;
    QEpicsPV* window_size;
    QEpicsPV* smoothing_factor;
    QEpicsPV* avg_algo;
    QEpicsPV* correctionStatus;
};

#endif // EXPERT_H
