#include "expert.h"
#include "ui_expert.h"

#include <iostream>

Expert::Expert(QString* avg_algo, int* window_size, double* smoothing_factor,
               bool* rf_only, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Expert)
{   
    ui->setupUi(this);

    this->rf_only = rf_only;
    this->ui->chkBoxRfOnly->setChecked(*rf_only);

    this->window_size = window_size;
    this->ui->windowSize->setValue(*window_size);

    this->smoothing_factor = smoothing_factor;
    this->ui->smoothingFactor->setValue(*smoothing_factor);

    this->avg_algo = avg_algo;
    ui->avgAlgo->addItem("moving");
    ui->avgAlgo->addItem("ema");
    ui->avgAlgo->setCurrentText(*avg_algo);

    QObject::connect(this->ui->avgAlgo, SIGNAL(currentTextChanged(const QString&)), this, SLOT(on_avgAlgoChanged(const QString&)));

    CONNECT_CLOSE_BUTTON;
}

Expert::~Expert()
{
    delete ui;
}

void Expert::on_windowSize_valueChanged(int value)
{
    *this->window_size = value;
}

void Expert::on_avgAlgoChanged(const QString &selected_avg_algo)
{
    *this->avg_algo = selected_avg_algo;
}

void Expert::on_smoothingFactor_valueChanged(double value)
{
    *this->smoothing_factor = value;
}

void Expert::on_chkBoxRfOnly_stateChanged(int state)
{
    *this->rf_only = state;
}
