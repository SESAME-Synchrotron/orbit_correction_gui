#include "pidgainwidget.h"
#include "ui_pidgainwidget.h"

PIDGainWidget::PIDGainWidget(int bpmIndex, QString type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PIDGainWidget)
{
    ui->setupUi(this);

    pvName = "SOFB:PID:K" + type;

    this->ui->label->setText(QString::asprintf("BPM #%02d", bpmIndex + 1));
    this->ui->gain->setVariableNameProperty(pvName);
    this->ui->gain->setArrayIndex(bpmIndex);
}

PIDGainWidget::~PIDGainWidget()
{
    delete ui;
}
