#ifndef EXPERT_H
#define EXPERT_H

#include <QMainWindow>
#include <qepicspv.h>
#include "pgains.h"
#include "igains.h"
#include "dgains.h"
#include "pidterms.h"
#include "client.h"
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
    void onIntegralUntiWindupInit(const QVariant &);

    void onRfOnlyInit(const QVariant &);

    void onDebugModeInit(const QVariant &);

    void onNormalizeInputsInit(const QVariant &);

    void onNormalizeInputsValueChanged(const QVariant &);

    void onScaleOutputsInit(const QVariant &);

    void onCorrectionStatusInit(const QVariant &);

    void onCorrectionStatusChanged(const QVariant &);

    void onControlAlgorithmChanged(const QVariant &);

    void enableInputs();

    void disableInputs();

    void on_btnPGains_clicked();

    void on_btnIGains_clicked();

    void on_btnDGains_clicked();

    void on_btnPidTerms_clicked();

private:
    Ui::Expert *ui;

    QEpicsPV* control_algorithm;
    QEpicsPV* integral_untiwindup;
    QEpicsPV* rf_only;
    QEpicsPV* debug_mode;
    QEpicsPV* normalizeInputs;
    QEpicsPV* scaleOutputs;
    QEpicsPV* window_size;
    QEpicsPV* smoothing_factor;
    QEpicsPV* avg_algo;
    QEpicsPV* correction_status;

    PGains*   pGains;
    IGains*   iGains;
    DGains*   dGains;
    PIDTerms* pidTerms;
};

#endif // EXPERT_H
