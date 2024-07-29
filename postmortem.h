#ifndef POSTMORTEM_H
#define POSTMORTEM_H

#include <QMainWindow>
#include <Q3DSurface>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <qwt_point_data.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include "macros.h"

#define NUM_CORRECTORS  64
#define NUM_SAMPLES     100

using namespace QtDataVisualization;

namespace Ui {
class Postmortem;
}

class Postmortem : public QMainWindow
{
    Q_OBJECT

public:
    explicit Postmortem(QWidget *parent = nullptr);
    ~Postmortem();

private slots:
    void showFileDialog();
    void handlePlotting(QFile* file);
    void setupCorrectorsPlots(QString);
    void setupRfPlot();

private:
    Ui::Postmortem *ui;

    double buffer[NUM_SAMPLES][NUM_CORRECTORS+1];
    double rf_data[NUM_SAMPLES];
    double sample_indices[NUM_SAMPLES];

    Q3DSurface* surfaceHC;
    Q3DSurface* surfaceVC;
    QwtPlotCurve* rfCurve;
};

#endif // POSTMORTEM_H
