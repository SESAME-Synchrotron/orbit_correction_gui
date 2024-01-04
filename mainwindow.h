#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

#include "expert.h"
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
    void on_energy_currentTextChanged(const QString &arg1);

    void on_numIterations_valueChanged(int value);

    void on_chkBoxInfIterations_stateChanged(int arg1);

    void on_numSingularValues_valueChanged(int value);

    void on_maxFreqChange_valueChanged(double value);

    void on_maxCurrChange_valueChanged(double value);

    void on_maxReadFail_valueChanged(int value);

    void on_chkBoxIncludeRf_stateChanged(int arg1);

    void on_chkBoxApplyReg_stateChanged(int arg1);

    void on_btnStartCorrection_clicked();

    void on_stdOut();

    void on_btnStopCorrection_clicked();

    void on_correctionEnd(int);

    void disableInputs();

    void enableInputs();

    void on_btnExpert_clicked();

private:
    Ui::MainWindow *ui;

    bool inf_iterations;
    bool include_rf;
    bool apply_regularization;
    int energy;
    int num_iterations;
    int num_singular_values;
    int max_read_fail;
    double max_frequency_change;
    double max_current_change;

    QString data_path;
    QProcess* correction_process;

    /*Expert References*/
    bool rf_only;
    bool debug_mode;
    /*Averaging Expert*/
    int window_size;
    double smoothing_factor;
    QString avg_algo;
    /*End Averaging Expert*/

    Expert* expert;
};
#endif // MAINWINDOW_H
