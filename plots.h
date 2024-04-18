#ifndef PLOTS_H
#define PLOTS_H

#include <QMainWindow>
#include <QESimpleShape.h>
#include <qepicspv.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_grid.h>
#include "horizontalcorrectors.h"
#include "verticalcorrectors.h"
#include "orbit.h"
#include "macros.h"
#include "circularbuffer.h"

namespace Ui {
class Plots;
}

class Plots : public QMainWindow
{
    Q_OBJECT

public:
    explicit Plots(QWidget *parent = nullptr);
    ~Plots();

private slots:
    void on_btnVerticalCorrectors_clicked();

    void on_btnHorizontalCorrectors_clicked();

    void horizontalDataChanged();

    void verticalDataChanged();

    void estimatedOrbitChanged();

    void xRmsUpdated(const QVariant&);

    void yRmsUpdated(const QVariant&);

    void on_btnOrbit_clicked();

private:
    Ui::Plots *ui;

    double xBpmIndexArr[48];
    double yBpmIndexArr[48];
    double xData[48];
    double xEstimatedData[48];
    double yData[48];
    double yEstimatedData[48];
    CircularBuffer xOrbitRms;
    CircularBuffer yOrbitRms;

    QwtPointArrayData* xPlotData;
    QwtPointArrayData* xEstimatedPlotData;
    QwtPointArrayData* yPlotData;
    QwtPointArrayData* yEstimatedPlotData;
    QwtPointArrayData* xRmsPlotData;
    QwtPointArrayData* yRmsPlotData;

    QwtPlotCurve* xCurve;
    QwtPlotCurve* xEstimatedCurve;
    QwtPlotCurve* yCurve;
    QwtPlotCurve* yEstimatedCurve;
    QwtPlotCurve* xRmsCurve;
    QwtPlotCurve* yRmsCurve;

    QList<QEpicsPV*> xDataPVs;
    QList<QEpicsPV*> yDataPVs;
    QEpicsPV* currentXOrbitRms;
    QEpicsPV* currentYOrbitRms;
    QEpicsPV* estimatedOrbit;

    HorizontalCorrectors* horizontalCorrectors;
    VerticalCorrectors* verticalCorrectors;
    Orbit* orbit;
};

#endif // PLOTS_H
