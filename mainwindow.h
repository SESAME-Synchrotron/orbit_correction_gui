#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

#include <qepicspv.h>
#include "expert.h"
#include "client.h"
#include "macros.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_chkBoxInfIterations_stateChanged(int);

    void on_btnStartCorrection_clicked();

    void print_stdout();

    void on_btnStopCorrection_clicked();

    void on_correctionEnd(int);

    void disableInputs();

    void enableInputs();

    void onNumIterationsInit(const QVariant &);

    void onIncludeRfInit(const QVariant &);

    void onApplyRegularizationInit(const QVariant &);

    void onCorrectionStatusInit(const QVariant &);

    void onCorrectionStatusChanged(const QVariant &);

    void on_btnExpert_clicked();

    void startLogging();

    void logData();

private:
    Ui::MainWindow *ui;

    bool inf_iterations;

    QString base_path;
    QString data_path;
    QString logs_path;
    QProcess* correction_process;
    QFile* logFile;
    QTimer* timer;

    QEpicsPV* energy;
    QEpicsPV* num_iterations;
    QEpicsPV* num_singular_values;
    QEpicsPV* max_frequency_change;
    QEpicsPV* max_current_change;
    QEpicsPV* max_read_fail;
    QEpicsPV* correctionStatus;
    QEpicsPV* include_rf;
    QEpicsPV* apply_regularization;
    QEpicsPV* regularization_Param;

    /*Expert References*/
    QEpicsPV* rf_only;
    QEpicsPV* debug_mode;
    /*Averaging Expert*/
    QEpicsPV* window_size;
    QEpicsPV* smoothing_factor;
    QEpicsPV* avg_algo;
    /*End Averaging Expert*/

    Expert* expert;
};
#endif // MAINWINDOW_H
