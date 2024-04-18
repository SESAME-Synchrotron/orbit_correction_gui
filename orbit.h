#ifndef ORBIT_H
#define ORBIT_H

#include <QMainWindow>
#include <qepicspv.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_grid.h>
#include <QTimer>
#include "circularbuffer.h"
#include "macros.h"

namespace Ui {
class Orbit;
}

class Orbit : public QMainWindow
{
    Q_OBJECT

public:
    explicit Orbit(QWidget *parent = nullptr);
    ~Orbit();

private slots:
    void orbitChanged();
    void selectedBpmNumberChanged(const int&);

private:
    Ui::Orbit *ui;

    int selectedBpm;

    QwtPointArrayData* orbitPlotData;
    QwtPointArrayData* estimatedOrbitPlotData;

    QwtPlotCurve* orbitCurve;
    QwtPlotCurve* estimatedOrbitCurve;

    QList<QEpicsPV*> orbitPVs;
    QEpicsPV* estimatedOrbit;

    QTimer* timer;

    const int windowSize;
    CircularBuffer orbitData;
    CircularBuffer estimatedOrbitData;

    double* timeIndexArr;
};

#endif // ORBIT_H
