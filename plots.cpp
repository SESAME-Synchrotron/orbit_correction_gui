#include "plots.h"
#include "ui_plots.h"

Plots::Plots(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Plots),
    xOrbitRms(CircularBuffer(100)),
    yOrbitRms(CircularBuffer(100))
{
    ui->setupUi(this);

    for(int i = 1; i <= 12; i++)
    {
        for(int j = 1; j <= 4; j++)
        {
            this->xDataPVs.push_back(new QEpicsPV(QString::asprintf("SR-DI-LBR%d-bpm%d:getSlowAcquisitionXScale", i, j), this));
            this->yDataPVs.push_back(new QEpicsPV(QString::asprintf("SR-DI-LBR%d-bpm%d:getSlowAcquisitionYScale", i, j), this));
            QObject::connect(this->xDataPVs[j + 4*(i-1) - 1], SIGNAL(valueChanged(QVariant)), this, SLOT(horizontalDataChanged()));
            QObject::connect(this->yDataPVs[j + 4*(i-1) - 1], SIGNAL(valueChanged(QVariant)), this, SLOT(verticalDataChanged()));
        }
    }

    for (int i = 0; i < 48; ++i)
    {
        xBpmIndexArr[i] = i + 1.0;
        yBpmIndexArr[i] = i + 1.0;
    }

    estimatedOrbit = new QEpicsPV("SOFB:Estimation:Orbit", this);
    QObject::connect(estimatedOrbit, SIGNAL(valueChanged(const QVariant&)), this, SLOT(estimatedOrbitChanged()));

    currentXOrbitRms = new QEpicsPV("SR-DI:getXOrbitRMS", this);
    currentYOrbitRms = new QEpicsPV("SR-DI:getYOrbitRMS", this);
    QObject::connect(currentXOrbitRms, SIGNAL(valueChanged(const QVariant&)), this, SLOT(xRmsUpdated(const QVariant&)));
    QObject::connect(currentYOrbitRms, SIGNAL(valueChanged(const QVariant&)), this, SLOT(yRmsUpdated(const QVariant&)));

    this->ui->plotX->setAutoReplot(true);
    this->ui->plotY->setAutoReplot(true);
    this->ui->plotXRms->setAutoReplot(true);
    this->ui->plotYRms->setAutoReplot(true);
    this->ui->plotX->setAxisScale(QwtPlot::xBottom, 1, 48, 4);
    this->ui->plotY->setAxisScale(QwtPlot::xBottom, 1, 48, 4);
    this->ui->plotXRms->setAxisScale(QwtPlot::xBottom, 1, this->xOrbitRms.size, 4);
    this->ui->plotYRms->setAxisScale(QwtPlot::xBottom, 1, this->yOrbitRms.size, 4);

    this->xCurve = new QwtPlotCurve("Horizontal Oribt");
    this->xEstimatedCurve = new QwtPlotCurve("Estimtaed Horizontal Orbit");
    this->yCurve = new QwtPlotCurve("Vertical Oribt");
    this->yEstimatedCurve = new QwtPlotCurve("Estimtaed Vertical Orbit");
    this->xRmsCurve= new QwtPlotCurve("Orbit X Rms");
    this->yRmsCurve= new QwtPlotCurve("Orbit Y Rms");
    this->xCurve->setPen(Qt::black, 1);
    this->xEstimatedCurve->setPen(Qt::red, 1);
    this->yCurve->setPen(Qt::black, 1);
    this->yEstimatedCurve->setPen(Qt::red, 1);
    this->xRmsCurve->setPen(Qt::black, 1);
    this->yRmsCurve->setPen(Qt::black, 1);

    QwtLegend *xLegend = new QwtLegend();
    this->ui->plotX->insertLegend(xLegend, QwtPlot::BottomLegend);
    QwtLegend *yLegend = new QwtLegend();
    this->ui->plotY->insertLegend(yLegend, QwtPlot::BottomLegend);

    QwtPlotGrid* gridX = new QwtPlotGrid;
    gridX->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    gridX->setPen(Qt::black, 0.25);
    gridX->attach(this->ui->plotX);

    QwtPlotGrid* gridY = new QwtPlotGrid;
    gridY->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    gridY->setPen(Qt::black, 0.25);
    gridY->attach(this->ui->plotY);

    QwtPlotGrid* gridXRms = new QwtPlotGrid;
    gridXRms->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    gridXRms->setPen(Qt::black, 0.25);
    gridXRms->attach(this->ui->plotXRms);

    QwtPlotGrid* gridYRms = new QwtPlotGrid;
    gridYRms->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    gridYRms->setPen(Qt::black, 0.25);
    gridYRms->attach(this->ui->plotYRms);

    this->verticalCorrectors = NULL;
    this->horizontalCorrectors = NULL;
    this->orbit = NULL;

    CONNECT_CLOSE_BUTTON;

    setAttribute(Qt::WA_DeleteOnClose);
}

Plots::~Plots()
{
    delete ui;
}

void Plots::on_btnVerticalCorrectors_clicked()
{
    OPEN_UI(verticalCorrectors, VerticalCorrectors, this);
}

void Plots::on_btnHorizontalCorrectors_clicked()
{
    OPEN_UI(horizontalCorrectors, HorizontalCorrectors, this);
}

void Plots::on_btnOrbit_clicked()
{
    orbit = new Orbit(this);
    showUI(orbit);
}

void Plots::horizontalDataChanged()
{
    for(int i = 0; i < 48; i++)
        xData[i] = this->xDataPVs[i]->get().toDouble();

    this->xPlotData = new QwtPointArrayData(xBpmIndexArr, xData, 48);
    this->xCurve->setSamples(this->xPlotData);
    this->xCurve->attach(this->ui->plotX);
    this->ui->plotX->replot();
    this->ui->plotX->update();
}

void Plots::verticalDataChanged()
{
    for(int i = 0; i < 48; i++)
        yData[i] = this->yDataPVs[i]->get().toDouble();

    this->yPlotData = new QwtPointArrayData(yBpmIndexArr, yData, 48);
    this->yCurve->setSamples(this->yPlotData);
    this->yCurve->attach(this->ui->plotY);
    this->ui->plotY->replot();
    this->ui->plotY->update();
}

void Plots::estimatedOrbitChanged()
{
    QStringList estimatedOrbitValues = estimatedOrbit->get().toStringList();
    if (estimatedOrbitValues.size())
    {
        for (int i = 0; i < 48; ++i)
        {
            xEstimatedData[i] = estimatedOrbitValues[i].toDouble();
            yEstimatedData[i] = estimatedOrbitValues[i+48].toDouble();
        }
    }
    this->xEstimatedPlotData = new QwtPointArrayData(xBpmIndexArr, xEstimatedData, 48);
    this->xEstimatedCurve->setSamples(this->xEstimatedPlotData);
    this->xEstimatedCurve->attach(this->ui->plotX);
    this->ui->plotX->replot();
    this->ui->plotX->update();

    this->yEstimatedPlotData = new QwtPointArrayData(yBpmIndexArr, yEstimatedData, 48);
    this->yEstimatedCurve->setSamples(this->yEstimatedPlotData);
    this->yEstimatedCurve->attach(this->ui->plotY);
    this->ui->plotY->replot();
    this->ui->plotY->update();
}

void Plots::xRmsUpdated(const QVariant& val)
{
    this->xOrbitRms.enqueue(val.toDouble());
    this->ui->plotXRms->setAxisScale(QwtPlot::xBottom, 1, this->xOrbitRms.numItems, 4);

    double timeIndexArr[this->xOrbitRms.size];
    for(int i = 0; i < this->xOrbitRms.size; i++) {
        timeIndexArr[i] = i + 1.0;
    }

    this->xRmsPlotData = new QwtPointArrayData(timeIndexArr, this->xOrbitRms.dataArr, this->xOrbitRms.numItems);
    this->xRmsCurve->setSamples(this->xRmsPlotData);
    this->xRmsCurve->attach(this->ui->plotXRms);
    this->ui->plotXRms->replot();
    this->ui->plotXRms->update();
}

void Plots::yRmsUpdated(const QVariant& val)
{
    this->yOrbitRms.enqueue(val.toDouble());
    this->ui->plotYRms->setAxisScale(QwtPlot::xBottom, 1, this->yOrbitRms.numItems, 4);

    double timeIndexArr[this->yOrbitRms.size];
    for(int i = 0; i < this->yOrbitRms.size; i++) {
        timeIndexArr[i] = i + 1.0;
    }

    this->yRmsPlotData = new QwtPointArrayData(timeIndexArr, this->yOrbitRms.dataArr, this->yOrbitRms.numItems);
    this->yRmsCurve->setSamples(this->yRmsPlotData);
    this->yRmsCurve->attach(this->ui->plotYRms);
    this->ui->plotYRms->replot();
    this->ui->plotYRms->update();
}
