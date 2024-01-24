#include "horizontalcorrectors.h"
#include "ui_horizontalcorrectors.h"

HorizontalCorrectors::HorizontalCorrectors(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HorizontalCorrectors)
{
    ui->setupUi(this);
}

HorizontalCorrectors::~HorizontalCorrectors()
{
    delete ui;
}
