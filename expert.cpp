#include "expert.h"
#include "ui_expert.h"

#include <iostream>

Expert::Expert(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Expert)
{   
    ui->setupUi(this);

    this->rf_only           = new QEpicsPV("SOFB:OnlyRf");
    this->debug_mode        = new QEpicsPV("SOFB:NoSetPv");
    this->window_size       = new QEpicsPV("SOFB:MovAvg:WindowSize");
    this->smoothing_factor  = new QEpicsPV("SOFB:MovAvg:SmoothingFactor");
    this->correction_status = new QEpicsPV("SOFB:CorrectionStatus");

    QObject::connect(rf_only, SIGNAL(valueInited(const QVariant &)), this, SLOT(onRfOnlyInit(const QVariant &)));
    QObject::connect(debug_mode, SIGNAL(valueInited(const QVariant &)), this, SLOT(onDebugModeInit(const QVariant &)));
    QObject::connect(correction_status, SIGNAL(valueInited(const QVariant &)), this, SLOT(onCorrectionStatusInit(const QVariant &)));
    QObject::connect(correction_status, SIGNAL(valueChanged(const QVariant &)), this, SLOT(onCorrectionStatusChanged(const QVariant &)));

    CONNECT_CLOSE_BUTTON;
}

Expert::~Expert()
{
    delete ui;
}

void Expert::onRfOnlyInit(const QVariant &val)
{
    this->ui->chkBoxRfOnly->setChecked(val.toBool());
}

void Expert::onDebugModeInit(const QVariant &val)
{
    this->ui->chkBoxDebugMode->setChecked(val.toBool());
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

void Expert::enableInputs()
{
    this->ui->avgAlgo->setEnabled(true);
    this->ui->windowSize->setEnabled(true);
    this->ui->smoothingFactor->setEnabled(true);
    this->ui->chkBoxRfOnly->setEnabled(true);
    this->ui->chkBoxDebugMode->setEnabled(true);
}

void Expert::disableInputs()
{
    this->ui->avgAlgo->setEnabled(false);
    this->ui->windowSize->setEnabled(false);
    this->ui->smoothingFactor->setEnabled(false);
    this->ui->chkBoxRfOnly->setEnabled(false);
    this->ui->chkBoxDebugMode->setEnabled(false);
}
