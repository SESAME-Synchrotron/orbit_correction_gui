#ifndef ORBIT_H
#define ORBIT_H

#include <QMainWindow>
#include <qepicspv.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_legend_data.h>
#include <qwt_point_data.h>
#include <qwt_plot_grid.h>
#include <QTimer>
#include "circularbuffer.h"
#include "macros.h"

Q_DECLARE_METATYPE(const QwtPlotItem *)

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
    void onLegendStatusChanged(const QVariant& itemInfo, bool status, int index);

private:
    Ui::Orbit *ui;

    int selectedBpm;

    QwtLegend *legend;

    QwtPointArrayData* orbitPlotData;
    QwtPointArrayData* estimatedOrbitPlotData;
    QwtPointArrayData* goldenOrbitPlotData;

    QwtPlotCurve* orbitCurve;
    QwtPlotCurve* estimatedOrbitCurve;

    QList<QEpicsPV*> orbitPVs;
    QEpicsPV* estimatedOrbit;

    QwtPlotCurve* goldenOrbitCurve;
    QList<double> goldenOrbit = { 0.0966270,-0.0639160,-0.1138840,0.0297150,0.1085450,-0.0585260,0.0740740, -0.0345910,-0.0982160,0.105720,
            0.0255970, -0.0461660,-0.0133710,0.0626610, -0.0127320,-0.0365430,0.0114410, 0.0096740,0.0151860, -0.0535010,
            0.0300440, -0.0489440,0.1588690,-0.1088250,-0.0748970,0.1103370,-0.1322090,-0.0166580,-0.0105350,0.1766930,
            -0.0802840,-1.1057050,1.1261570,0.033410,-0.0259340,-0.1140820,0.0995390,0.0457110,0.0302260,-0.0640140,
            0.0254080,0.0158370,-0.0299150,0.024490, 0.0012270,-0.0122770,0.0385210,-0.0271050,-0.0114030,-0.0243460,
            -0.0203920,0.1258180,0.130880,-0.2010310,0.0502450,-0.0017720,-0.014030,-0.0093580,0.0283130,0.0057030,
            -0.2725260,0.2648550,0.0417980,-0.0013640,-0.0043230,-0.1220530,-0.1101510,0.2306030,-0.0655630,-0.1953330,
            0.1750430,0.0679120,-0.0991780,0.0745020,-0.0483440,0.0672650,-0.1691740,0.0655770,0.048350,0.0101620,
            -0.1334280,-0.2018990,0.2135240,0.0794760,0.0917580,0.215160,-0.2744740,-0.0298710,-0.053250,0.0974680,
            -0.1098650,0.0929160,0.0448540,-0.0397430,-0.1589360,0.161590 };

    QTimer* timer;

    const int windowSize;
    CircularBuffer orbitData;
    CircularBuffer estimatedOrbitData;
    double goldenOrbitData[2];

    double* timeIndexArr;
    double goldenTimeIndexArr[2];
};

#endif // ORBIT_H
