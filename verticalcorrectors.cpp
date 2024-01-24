#include "verticalcorrectors.h"
#include "ui_verticalcorrectors.h"

VerticalCorrectors::VerticalCorrectors(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VerticalCorrectors)
{
    ui->setupUi(this);
}

VerticalCorrectors::~VerticalCorrectors()
{
    delete ui;
}
