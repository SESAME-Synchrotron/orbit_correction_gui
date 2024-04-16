#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QList>
#include <QStack>

#include <qepicspv.h>
#include "expert.h"
#include "logs.h"
#include "plots.h"
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

    void startLocalLogging();

    void logLocalData();

    void logDataSSH();

    void on_btnHistoryLogs_clicked();

    void checkRfOnlyRun();

    void frequencyPvInit(const QVariant &);

    void on_btnPlots_clicked();

    void on_btnRemoveCorrection_clicked();

    void saveCorrectorsAndRF();

    void onStackLengthChanged(int);

signals:
    void stackLengthChanged(int);

private:
    Ui::MainWindow *ui;

    const double thresholdFreq;
    const QString remoteHost;

    bool inf_iterations;
    double currentFreq, prevFreq;

    QString base_path;
    QString data_path;
    QString logs_path;
    QFile* logFile;
    QTimer* loggingTimer;

    QEpicsPV* energy;
    QEpicsPV* num_iterations;
    QEpicsPV* num_singular_values;
    QEpicsPV* correction_factor;
    QEpicsPV* max_frequency_change;
    QEpicsPV* max_current_change;
    QEpicsPV* max_read_fail;
    QEpicsPV* correction_status;
    QEpicsPV* include_rf;
    QEpicsPV* apply_regularization;
    QEpicsPV* regularization_Param;
    QEpicsPV* sampling_frequency;
    QEpicsPV* getFrequency;
    QEpicsPV* setFrequency;
    QList<QEpicsPV*> getCorrectors_currents;
    QList<QString> setCorrectors_currents;
    QStack<std::array<double, 65>> correctionStack;

    /*Expert References*/
    QEpicsPV* rf_only;
    QEpicsPV* debug_mode;
    QEpicsPV* normalize_inputs;
    QEpicsPV* scale_outputs;
    /*Averaging Expert*/
    QEpicsPV* window_size;
    QEpicsPV* smoothing_factor;
    QEpicsPV* avg_algo;
    /*End Averaging Expert*/

    QProcess* correction_process;
    QProcess* logProcess;

    Expert* expert;
    Logs* historyLogs;
    Plots* plots;
};
#endif // MAINWINDOW_H
