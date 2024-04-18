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

    QObject::connect(this->ui->bpmNumber, SIGNAL(valueChanged(const int&)), this, SLOT(selectedBpmNumberChanged(const int&)));
    this->selectedBpm = this->ui->bpmNumber->getValue();

    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(orbitChanged()));

    /* Plot Configuration */
    this->ui->plotOrbit->setAutoReplot(true);
    this->ui->plotOrbit->setAxisScale(QwtPlot::xBottom, 1, windowSize, 10);

    this->orbitCurve= new QwtPlotCurve("Oribt");
    this->estimatedOrbitCurve = new QwtPlotCurve("Estimtaed Orbit");
    this->orbitCurve->setPen(Qt::black, 1);
    this->estimatedOrbitCurve->setPen(Qt::red, 1);

    QwtLegend *legend = new QwtLegend();
    this->ui->plotOrbit->insertLegend(legend, QwtPlot::BottomLegend);

    QwtPlotGrid* gridX = new QwtPlotGrid;
    gridX->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    gridX->setPen(Qt::black, 0.25);
    gridX->attach(this->ui->plotOrbit);
    /* Plot Configuration */

    this->timer->start(100);

    CONNECT_CLOSE_BUTTON;
    setAttribute(Qt::WA_DeleteOnClose);
}

void Orbit::orbitChanged()
{
    this->orbitData.enqueue(orbitPVs[selectedBpm]->get().toDouble());

    QStringList estimatedOrbitValues = estimatedOrbit->get().toStringList();
    if (estimatedOrbitValues.size())
    {
        this->estimatedOrbitData.enqueue(estimatedOrbitValues[selectedBpm].toDouble());
    }

    this->orbitPlotData = new QwtPointArrayData(timeIndexArr, this->orbitData.dataArr, this->orbitData.size);
    this->orbitCurve->setSamples(this->orbitPlotData);
    this->orbitCurve->attach(this->ui->plotOrbit);
    this->ui->plotOrbit->replot();
    this->ui->plotOrbit->update();

    this->estimatedOrbitPlotData = new QwtPointArrayData(timeIndexArr, this->estimatedOrbitData.dataArr, this->estimatedOrbitData.size);
    this->estimatedOrbitCurve->setSamples(this->estimatedOrbitPlotData);
    this->estimatedOrbitCurve->attach(this->ui->plotOrbit);
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

Orbit::~Orbit()
{
    delete ui;
}
