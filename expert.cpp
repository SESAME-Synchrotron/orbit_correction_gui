#include "expert.h"
#include "ui_expert.h"

#include <iostream>

Expert::Expert(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Expert)
{   
    ui->setupUi(this);

    this->control_algorithm   = new QEpicsPV("SOFB:Control:Algo");
    this->integral_untiwindup = new QEpicsPV("SOFB:PID:IntegralUntiWindup");
    this->rf_only             = new QEpicsPV("SOFB:OnlyRf");
    this->debug_mode          = new QEpicsPV("SOFB:NoSetPv");
    this->normalizeInputs     = new QEpicsPV("SOFB:NormalizeInputs");
    this->scaleOutputs        = new QEpicsPV("SOFB:ScaleOutputs");
    this->window_size         = new QEpicsPV("SOFB:MovAvg:WindowSize");
    this->smoothing_factor    = new QEpicsPV("SOFB:MovAvg:SmoothingFactor");
    this->correction_status   = new QEpicsPV("SOFB:CorrectionStatus");

    QObject::connect(integral_untiwindup, SIGNAL(valueInited(const QVariant &)), this, SLOT(onIntegralUntiWindupInit(const QVariant &)));
    QObject::connect(rf_only, SIGNAL(valueInited(const QVariant &)), this, SLOT(onRfOnlyInit(const QVariant &)));
    QObject::connect(debug_mode, SIGNAL(valueInited(const QVariant &)), this, SLOT(onDebugModeInit(const QVariant &)));
    QObject::connect(normalizeInputs, SIGNAL(valueInited(const QVariant &)), this, SLOT(onNormalizeInputsInit(const QVariant &)));
    QObject::connect(normalizeInputs, SIGNAL(valueChanged(const QVariant &)), this, SLOT(onNormalizeInputsValueChanged(const QVariant &)));
    QObject::connect(scaleOutputs, SIGNAL(valueInited(const QVariant &)), this, SLOT(onScaleOutputsInit(const QVariant &)));
    QObject::connect(correction_status, SIGNAL(valueInited(const QVariant &)), this, SLOT(onCorrectionStatusInit(const QVariant &)));
    QObject::connect(correction_status, SIGNAL(valueChanged(const QVariant &)), this, SLOT(onCorrectionStatusChanged(const QVariant &)));
    QObject::connect(control_algorithm, SIGNAL(valueChanged(const QVariant &)), this, SLOT(onControlAlgorithmChanged(const QVariant &)));

    this->pGains   = NULL;
    this->iGains   = NULL;
    this->dGains   = NULL;
    this->pidTerms = NULL;

    CONNECT_CLOSE_BUTTON;
}

Expert::~Expert()
{
    delete ui;
}

void Expert::onIntegralUntiWindupInit(const QVariant &val)
{
    this->ui->chkBoxIntegralUntiWindup->setChecked(val.toBool());
}

void Expert::onRfOnlyInit(const QVariant &val)
{
    this->ui->chkBoxRfOnly->setChecked(val.toBool());
}

void Expert::onDebugModeInit(const QVariant &val)
{
    this->ui->chkBoxDebugMode->setChecked(val.toBool());
}

void Expert::onNormalizeInputsInit(const QVariant &val)
{
    this->ui->chkBoxNormalizeInputs->setChecked(val.toBool());
}

void Expert::onNormalizeInputsValueChanged(const QVariant &val)
{
    if (val == 0)
    {
        Client::writePV("SOFB:ScaleOutputs", 0);
        this->ui->chkBoxScaleOutputs->setChecked(0);
        this->ui->chkBoxScaleOutputs->setEnabled(false);
        this->ui->lblScaleOutputs->setStyleSheet("color: rgb(128, 128, 128)");
    } else {
        this->ui->chkBoxScaleOutputs->setEnabled(true);
        this->ui->lblScaleOutputs->setStyleSheet("color: rgb(0, 0, 0)");
    }
}

void Expert::onScaleOutputsInit(const QVariant &val)
{
    this->ui->chkBoxScaleOutputs->setChecked(val.toBool());
}

void Expert::onCorrectionStatusInit(const QVariant &status)
{
    const int running = 1;
    const int debug = 3;
    if (status == running || status == debug)
    {
      disableInputs();
    }
}

void Expert::onCorrectionStatusChanged(const QVariant &status)
{
    const int success = 0;
    const int running = 1;
    const int fail = 2;
    const int debug = 3;
    if (status == success || status == fail)
    {
      enableInputs();
    } else if (status == running || status == debug)
    {
      disableInputs();
    }
}

void Expert::onControlAlgorithmChanged(const QVariant & algorithm)
{
    const int None = 0;
    const int P = 1;
    const int PI = 2;
    const int PID = 3;

    if (algorithm == None)
    {
        this->ui->btnPGains->setEnabled(false);
        this->ui->btnIGains->setEnabled(false);
        this->ui->btnDGains->setEnabled(false);
        this->ui->btnPidTerms->setEnabled(false);
        this->ui->chkBoxIntegralUntiWindup->setEnabled(false);
        this->ui->DCutoffFrequency->setEnabled(false);
    } else if (algorithm == P)
    {
        this->ui->btnPGains->setEnabled(true);
        this->ui->btnIGains->setEnabled(false);
        this->ui->btnDGains->setEnabled(false);
        this->ui->btnPidTerms->setEnabled(true);
        this->ui->chkBoxIntegralUntiWindup->setEnabled(false);
        this->ui->DCutoffFrequency->setEnabled(false);
    } else if (algorithm == PI)
    {
        this->ui->btnPGains->setEnabled(true);
        this->ui->btnIGains->setEnabled(true);
        this->ui->btnDGains->setEnabled(false);
        this->ui->btnPidTerms->setEnabled(true);
        this->ui->chkBoxIntegralUntiWindup->setEnabled(true);
        this->ui->DCutoffFrequency->setEnabled(false);
    } else if (algorithm == PID)
    {
        this->ui->btnPGains->setEnabled(true);
        this->ui->btnIGains->setEnabled(true);
        this->ui->btnDGains->setEnabled(true);
        this->ui->btnPidTerms->setEnabled(true);
        this->ui->chkBoxIntegralUntiWindup->setEnabled(true);
        this->ui->DCutoffFrequency->setEnabled(true);
    }
}

void Expert::enableInputs()
{
    const int PI = 2;
    const int PID = 3;

    this->ui->avgAlgo->setEnabled(true);
    this->ui->windowSize->setEnabled(true);
    this->ui->smoothingFactor->setEnabled(true);
    this->ui->chkBoxRfOnly->setEnabled(true);
    this->ui->chkBoxDebugMode->setEnabled(true);
    this->ui->chkBoxNormalizeInputs->setEnabled(true);
    if (this->normalizeInputs->get() == 1)
        this->ui->chkBoxScaleOutputs->setEnabled(true);
    if (this->control_algorithm->get() == PI || this->control_algorithm->get() == PID)
        this->ui->chkBoxIntegralUntiWindup->setEnabled(true);
    if (this->control_algorithm->get() == PID)
        this->ui->DCutoffFrequency->setEnabled(true);
}

void Expert::disableInputs()
{
    this->ui->avgAlgo->setEnabled(false);
    this->ui->windowSize->setEnabled(false);
    this->ui->smoothingFactor->setEnabled(false);
    this->ui->chkBoxRfOnly->setEnabled(false);
    this->ui->chkBoxDebugMode->setEnabled(false);
    this->ui->chkBoxNormalizeInputs->setEnabled(false);
    this->ui->chkBoxScaleOutputs->setEnabled(false);
    this->ui->chkBoxIntegralUntiWindup->setEnabled(false);
    this->ui->DCutoffFrequency->setEnabled(false);
}

void Expert::on_btnPGains_clicked()
{
    OPEN_UI(pGains, PGains, this);
}

void Expert::on_btnIGains_clicked()
{
    OPEN_UI(iGains, IGains, this);
}

void Expert::on_btnDGains_clicked()
{
    OPEN_UI(dGains, DGains, this);
}

void Expert::on_btnPidTerms_clicked()
{
    OPEN_UI(pidTerms, PIDTerms, this);
}
