#include "postmortem.h"
#include "ui_postmortem.h"

Postmortem::Postmortem(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Postmortem)
{
    ui->setupUi(this);
}

Postmortem::~Postmortem()
{
    delete ui;
}
