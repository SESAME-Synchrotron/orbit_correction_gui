#include "orbit.h"
#include "ui_orbit.h"

Orbit::Orbit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Orbit),
    windowSize(100),
    orbitData(CircularBuffer(windowSize)),
    estimatedOrbitData(CircularBuffer(windowSize))
{
    ui->setupUi(this);

    for(int i = 1; i <= 12; i++)
        for(int j = 1; j <= 4; j++)
            this->orbitPVs.push_back(new QEpicsPV(QString::asprintf("SR-DI-LBR%d-bpm%d:getSlowAcquisitionXScale", i, j), this));
    for(int i = 1; i <= 12; i++)
        for(int j = 1; j <= 4; j++)
            this->orbitPVs.push_back(new QEpicsPV(QString::asprintf("SR-DI-LBR%d-bpm%d:getSlowAcquisitionYScale", i, j), this));
    estimatedOrbit = new QEpicsPV("SOFB:Estimation:Orbit", this);

    timeIndexArr = new double[windowSize];
    for(int i = 0; i < this->windowSize; i++) {
        timeIndexArr[i] = i + 1.0;
    }
    goldenTimeIndexArr[0] = timeIndexArr[0];
    goldenTimeIndexArr[1] = timeIndexArr[windowSize-1];

    QObject::connect(this->ui->bpmNumber, SIGNAL(valueChanged(const int&)), this, SLOT(selectedBpmNumberChanged(const int&)));
    this->selectedBpm = this->ui->bpmNumber->getValue() - 1;

    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(orbitChanged()));

    /* Plot Configuration */
    this->ui->plotOrbit->setAutoReplot(true);
    this->ui->plotOrbit->setAxisScale(QwtPlot::xBottom, 1, windowSize, 10);

    this->orbitCurve= new QwtPlotCurve("Oribt");
    this->estimatedOrbitCurve = new QwtPlotCurve("Estimtaed Orbit");
    this->goldenOrbitCurve = new QwtPlotCurve("Golden Orbit");
    this->orbitCurve->setPen(Qt::black, 1);
    this->estimatedOrbitCurve->setPen(Qt::red, 1);
    this->goldenOrbitCurve->setPen(Qt::green, 1);

    // Legend
    legend = new QwtLegend();
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    this->ui->plotOrbit->insertLegend(legend, QwtPlot::BottomLegend);
    QObject::connect(legend, &QwtLegend::checked, this, &Orbit::onLegendStatusChanged);
    // End Legend

    QwtPlotGrid* gridX = new QwtPlotGrid;
    gridX->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    gridX->setPen(Qt::black, 0.25);
    gridX->attach(this->ui->plotOrbit);
    /* End Plot Configuration */

    this->timer->start(100);

    CONNECT_CLOSE_BUTTON;
    setAttribute(Qt::WA_DeleteOnClose);
}

void Orbit::orbitChanged()
{
    this->orbitData.enqueue(orbitPVs[selectedBpm]->get().toDouble());
    this->ui->plotOrbit->setAxisScale(QwtPlot::xBottom, 1, this->orbitData.numItems, 10);

    QStringList estimatedOrbitValues = estimatedOrbit->get().toStringList();
    if (estimatedOrbitValues.size())
    {
        this->estimatedOrbitData.enqueue(estimatedOrbitValues[selectedBpm].toDouble());
    }

    this->orbitPlotData = new QwtPointArrayData(timeIndexArr, this->orbitData.dataArr, this->orbitData.numItems);
    this->orbitCurve->setSamples(this->orbitPlotData);
    this->orbitCurve->attach(this->ui->plotOrbit);
    this->ui->plotOrbit->replot();
    this->ui->plotOrbit->update();

    this->estimatedOrbitPlotData = new QwtPointArrayData(timeIndexArr, this->estimatedOrbitData.dataArr, this->estimatedOrbitData.numItems);
    this->estimatedOrbitCurve->setSamples(this->estimatedOrbitPlotData);
    this->estimatedOrbitCurve->attach(this->ui->plotOrbit);
    this->ui->plotOrbit->replot();
    this->ui->plotOrbit->update();


    goldenOrbitData[0] = goldenOrbitData[1] = goldenOrbit[selectedBpm];
    goldenOrbitPlotData = new QwtPointArrayData(goldenTimeIndexArr, goldenOrbitData, 2);
    goldenOrbitCurve->setSamples(goldenOrbitPlotData);
    goldenOrbitCurve->attach(this->ui->plotOrbit);
    this->ui->plotOrbit->replot();
    this->ui->plotOrbit->update();
}

void Orbit::selectedBpmNumberChanged(const int& val)
{
    this->selectedBpm = val - 1;

    while (!orbitData.isEmpty())
        orbitData.dequeue();

    while (!estimatedOrbitData.isEmpty())
        estimatedOrbitData.dequeue();
}

void Orbit::onLegendStatusChanged(const QVariant& itemInfo, bool status, int index)
{
    QwtPlotItem* plotItem = qvariant_cast <QwtPlotItem*>(itemInfo);

    if (plotItem)
    {
        QString curveName = plotItem->title().text();
        if (curveName == "Oribt")
        {
            if (status)
                orbitCurve->setVisible(true);
            else
                orbitCurve->setVisible(false);
        } else if (curveName == "Estimtaed Orbit")
        {
            if (status)
                estimatedOrbitCurve->setVisible(true);
            else
                estimatedOrbitCurve->setVisible(false);
        } else if (curveName == "Golden Orbit")
        {
            if (status)
                goldenOrbitCurve->setVisible(true);
            else
                goldenOrbitCurve->setVisible(false);
        }
    } else {
        std::cout << "ERROR!" << std::endl;
    }
}

Orbit::~Orbit()
{
    delete ui;
}
