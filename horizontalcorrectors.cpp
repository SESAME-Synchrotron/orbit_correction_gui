#include "horizontalcorrectors.h"
#include "ui_horizontalcorrectors.h"

HorizontalCorrectors::HorizontalCorrectors(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HorizontalCorrectors)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
}

HorizontalCorrectors::~HorizontalCorrectors()
{
    delete ui;
}
