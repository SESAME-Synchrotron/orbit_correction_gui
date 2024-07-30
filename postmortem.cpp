#include "postmortem.h"
#include "ui_postmortem.h"

Postmortem::Postmortem(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Postmortem)
{
    ui->setupUi(this);

    QObject::connect(this->ui->btnDialog, &QPushButton::clicked, this, &Postmortem::showFileDialog);

    surfaceHC = new Q3DSurface();
    surfaceHC->setFlags(surfaceHC->flags() ^ Qt::FramelessWindowHint);
    surfaceHC->activeTheme()->setLightStrength(0.0);
    surfaceHC->activeTheme()->setAmbientLightStrength(1.0);

    surfaceVC = new Q3DSurface();
    surfaceVC->setFlags(surfaceVC->flags() ^ Qt::FramelessWindowHint);
    surfaceVC->activeTheme()->setLightStrength(0.0);
    surfaceVC->activeTheme()->setAmbientLightStrength(1.0);

    setupCorrectorsPlots("Horizontal");
    setupCorrectorsPlots("Vertical");
    setupRfPlot();

    for (int i = 0; i < NUM_SAMPLES; ++i)
        sample_indices[i] = i;

    CONNECT_CLOSE_BUTTON;
    setAttribute(Qt::WA_DeleteOnClose);
}

Postmortem::~Postmortem()
{
    if (surfaceHC)
        delete surfaceHC;
    if (surfaceVC)
        delete surfaceVC;
    if (rfCurve)
        delete rfCurve;
    delete ui;
}

void Postmortem::setupCorrectorsPlots(QString type)
{
    QString title;
    Q3DSurface* pltSurface;
    QHBoxLayout* plt;
    if (type == "Horizontal")
    {
        title = "Horizontal Corrector Number";
        pltSurface = surfaceHC;
        plt = this->ui->ltPostmortemHCPlot;
    } else
    {
        title = "Vertical Corrector Number";
        pltSurface = surfaceVC;
        plt = this->ui->ltPostmortemVCPlot;
    }

    QWidget* container = QWidget::createWindowContainer(pltSurface);

    QValue3DAxis* axisX = new QValue3DAxis();
    QValue3DAxis* axisY = new QValue3DAxis();
    QValue3DAxis* axisZ = new QValue3DAxis();

    axisX->setTitle(title);
    axisX->setTitleVisible(true);
    axisX->setLabelFormat("%d");
    axisX->setRange(1, NUM_CORRECTORS/2);
    axisX->setSegmentCount(8);

    axisY->setTitle("Delta Current (A)");
    axisY->setTitleVisible(true);
    axisY->setLabelFormat("%.7f");

    axisZ->setTitle("Time");
    axisZ->setTitleVisible(true);
    axisZ->setLabelFormat("%d");
    axisZ->setRange(0, NUM_SAMPLES - 1);
    axisZ->setSegmentCount(20);

    pltSurface->setAxisX(axisX);
    pltSurface->setAxisY(axisY);
    pltSurface->setAxisZ(axisZ);

    plt->addWidget(container);
}

void Postmortem::setupRfPlot()
{
    this->ui->pltRf->setAutoReplot(true);
    this->ui->pltRf->setAxisScale(QwtPlot::xBottom, 0, 99, 5);
    this->rfCurve = new QwtPlotCurve("RF Postmortem");
    this->rfCurve->setPen(Qt::black, 1);
    QwtPlotGrid* gridRf = new QwtPlotGrid;
    gridRf->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    gridRf->setPen(Qt::black, 0.25);
    gridRf->attach(this->ui->pltRf);
}

void Postmortem::showFileDialog()
{
    QString postmortem_path = QDir::homePath() + "/nfs/machine/PostMortem/orbit_correction";
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", postmortem_path, "All Files (*);;Text Files (*.txt)", NULL, QFileDialog::DontUseNativeDialog);
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            QMessageBox::warning(NULL, "Error", "Could not read the file");
        handlePlotting(&file);
        this->ui->btnDialog->setText("Change File");
    }
}

void Postmortem::handlePlotting(QFile* file)
{
    QTextStream in(file);
    QStringList lines;
    int corrector_number = 1;
    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList samples = line.trimmed().split(" ");
        for (int sample_idx = 0; sample_idx < samples.length(); ++sample_idx)
        {
            buffer[sample_idx][corrector_number-1] = samples[sample_idx].toFloat();
        }
        corrector_number++;
    }

    QSurfaceDataArray* dataHC = new QSurfaceDataArray;
    QSurfaceDataArray* dataVC = new QSurfaceDataArray;
    for (int sample_idx = 0; sample_idx < NUM_SAMPLES; ++sample_idx)
    {
        QSurfaceDataRow* dataRowHC = new QSurfaceDataRow(NUM_CORRECTORS/2);
        QSurfaceDataRow* dataRowVC = new QSurfaceDataRow(NUM_CORRECTORS/2);
        for (int corrector_number = 1; corrector_number <= NUM_CORRECTORS/2; ++corrector_number)
        {
            QSurfaceDataItem itemHC(QVector3D(corrector_number, buffer[sample_idx][corrector_number-1], sample_idx));
            QSurfaceDataItem itemVC(QVector3D(corrector_number, buffer[sample_idx][corrector_number-1 + NUM_CORRECTORS/2], sample_idx));
            (*dataRowHC)[corrector_number-1] = itemHC;
            (*dataRowVC)[corrector_number-1] = itemVC;
        }
        *dataHC << dataRowHC;
        *dataVC << dataRowVC;
        rf_data[sample_idx] = buffer[sample_idx][64] * 1e6;
    }

    QSurface3DSeries* seriesHC = new QSurface3DSeries;
    seriesHC->dataProxy()->resetArray(dataHC);
    if (surfaceHC->seriesList().length())
        surfaceHC->removeSeries(surfaceHC->seriesList()[0]);
    surfaceHC->addSeries(seriesHC);

    QSurface3DSeries* seriesVC = new QSurface3DSeries;
    seriesVC->dataProxy()->resetArray(dataVC);
    if (surfaceVC->seriesList().length())
        surfaceVC->removeSeries(surfaceVC->seriesList()[0]);
    surfaceVC->addSeries(seriesVC);

    this->ui->pltRf->detachItems();
    QwtPlotCurve* curve = new QwtPlotCurve();
    curve->setPen(Qt::blue, 2);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->attach(this->ui->pltRf);
    QwtPointArrayData* rfPlotData = new QwtPointArrayData(sample_indices, rf_data, 100);
    curve->setSamples(rfPlotData);
}
