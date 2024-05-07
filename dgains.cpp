#include "dgains.h"
#include "ui_dgains.h"

DGains::DGains(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DGains),
    bpm_count(96)
{
    ui->setupUi(this);

    const int num_rows = 32;
    const int num_cols= 3;
    global_data = new double[bpm_count];

    this->ui->ltGains->setRowStretch(num_rows - 1, 0);
    this->ui->ltGains->setColumnStretch(num_cols - 1, 0);
    this->ui->ltGains->setHorizontalSpacing(5);

    for (int index = 0; index < bpm_count; ++index)
        this->ui->ltGains->addWidget(new PIDGainWidget(index, "D", this));

    CONNECT_CLOSE_BUTTON;
}

DGains::~DGains()
{
    delete ui;
}

void DGains::on_btnGlobalSet_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Confirmation", "Are you sure you want to set a Global Gain?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes){
        double value = this->ui->globalSet->getValue();
        for (int i = 0; i < bpm_count; ++i)
            global_data[i] = value;
         Client::writeDoubleArray("SOFB:PID:KD", global_data, 96);
    }
}
