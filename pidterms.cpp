#include "pidterms.h"
#include "ui_pidterms.h"

PIDTerms::PIDTerms(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PIDTerms)
{
    ui->setupUi(this);
}

PIDTerms::~PIDTerms()
{
    delete ui;
}
