#include "pgains.h"
#include "ui_pgains.h"

PGains::PGains(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PGains),
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
        this->ui->ltGains->addWidget(new PIDGainWidget(index, "P", this));

    CONNECT_CLOSE_BUTTON;
}

PGains::~PGains()
{
    delete ui;
}

void PGains::on_btnGlobalSet_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Confirmation", "Are you sure you want to set a Global Gain?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes){
        double value = this->ui->globalSet->getValue();
        for (int i = 0; i < bpm_count; ++i)
            global_data[i] = value;
         Client::writeDoubleArray("SOFB:PID:KP", global_data, 96);
    }
}
