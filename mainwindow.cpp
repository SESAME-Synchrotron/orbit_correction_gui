#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->energy->addItem("800");
    ui->energy->addItem("2500");
    ui->energy->setCurrentText("2500");

    this->inf_iterations = true;
    this->ui->numIterations->setEnabled(false);

    this->include_rf = true;
    this->apply_regularization = false;
    this->energy = 2500;
    this->num_iterations = 1;
    this->num_singular_values = 40;
    this->max_read_fail = 5;
    this->max_frequency_change = 1000.0;
    this->max_current_change = 0.1;

    this->rf_only = false;
    this->debug_mode = false;
    /*Averaging*/
    this->avg_algo = "ema";
    this->window_size = 5;
    this->smoothing_factor = 0.33;

    this->correction_process = new QProcess();
    this->data_path = "/home/control/Documents/sofb/orbit_correction/data";

    QObject::connect(correction_process, SIGNAL(readyReadStandardOutput()), this, SLOT(on_stdOut()));
    QObject::connect(correction_process, SIGNAL(readyReadStandardError()), this, SLOT(on_stdOut()));
    QObject::connect(correction_process, SIGNAL(finished(int)), this, SLOT(on_correctionEnd(int)));

    this->expert = NULL;

    CONNECT_CLOSE_BUTTON;
}

MainWindow::~MainWindow()
{
    if (this->correction_process->state() == QProcess::Running)
        this->correction_process->terminate();
    delete ui;
}

void MainWindow::on_energy_currentTextChanged(const QString &energyText)
{
    this->energy = energyText.toInt();
}

void MainWindow::on_numIterations_valueChanged(int value)
{
    this->num_iterations = value;
}

void MainWindow::on_chkBoxInfIterations_stateChanged(int state)
{
    this->inf_iterations = state;

    if (state)
        this->ui->numIterations->setEnabled(false);
    else
        this->ui->numIterations->setEnabled(true);
}

void MainWindow::on_numSingularValues_valueChanged(int value)
{
    this->num_singular_values = value;
}

void MainWindow::on_maxFreqChange_valueChanged(double value)
{
    this->max_frequency_change = value;
}

void MainWindow::on_maxCurrChange_valueChanged(double value)
{
    this->max_current_change = value;
}

void MainWindow::on_maxReadFail_valueChanged(int value)
{
    this->max_read_fail = value;
}

void MainWindow::on_chkBoxIncludeRf_stateChanged(int state)
{
    this->include_rf = state;
}

void MainWindow::on_chkBoxApplyReg_stateChanged(int state)
{
    this->apply_regularization = state;
}

void MainWindow::on_btnStartCorrection_clicked()
{
    int correction_iterations = num_iterations;
    QString correction_iterations_lbl = QString::number(num_iterations);
    if (this->inf_iterations)
    {
        correction_iterations = -1;
        correction_iterations_lbl = "Inf";
    }

    QStringList params;
    params << "-energy" << QString::number(this->energy);
    params << "-num_iter" << QString::number(correction_iterations);
    params << "-num_singular" << QString::number(num_singular_values);
    params << "-max_freq_change" << QString::number(max_frequency_change);
    params << "-max_curr_change" << QString::number(max_current_change);
    params << "-max_read_fail" << QString::number(max_read_fail);
    params << "-data_path" << data_path;
    params << "-avg_algo" << avg_algo;
    params << "-window_size" << QString::number(window_size);
    params << "-avg_smooth" << QString::number(smoothing_factor);
    if (this->include_rf)
        params << "-include_rf";
    if (this->apply_regularization)
        params << "-apply_reg";
    if (this->rf_only)
        params << "-rf_only";
    if (this->debug_mode)
        params << "-no_set";

    disableInputs();
    ui->btnStartCorrection->setEnabled(false);
    ui->btnStopCorrection->setEnabled(true);
    ui->lblLogs->setText("");
    ui->lblCurrentIterationLimit->setText("out of " + correction_iterations_lbl);
    correction_process->start("sofb", params);
}

void MainWindow::on_stdOut()
{
    QString p_stdout = correction_process->readAllStandardOutput();
    QString p_stderr = correction_process->readAllStandardError();
    if(!p_stderr.isEmpty())
        ui->lblLogs->setText(ui->lblLogs->text() + p_stderr);
    else
        ui->lblLogs->setText(ui->lblLogs->text() + p_stdout);
}

void MainWindow::on_btnStopCorrection_clicked()
{
    this->correction_process->terminate();
}

void MainWindow::on_correctionEnd(int status)
{
  QString state = "successfully";
  if (status)
      state = "unsuccessfully";

  ui->lblLogs->setText(ui->lblLogs->text() + "\ncorrection ended " + state);
  enableInputs();
  ui->btnStartCorrection->setEnabled(true);
  ui->btnStopCorrection->setEnabled(false);
}

void MainWindow::disableInputs()
{
    this->ui->energy->setEnabled(false);
    this->ui->chkBoxInfIterations->setEnabled(false);
    this->ui->numIterations->setEnabled(false);
    this->ui->numSingularValues->setEnabled(false);
    this->ui->maxFreqChange->setEnabled(false);
    this->ui->maxCurrChange->setEnabled(false);
    this->ui->maxReadFail->setEnabled(false);
    this->ui->chkBoxIncludeRf->setEnabled(false);
    this->ui->chkBoxApplyReg->setEnabled(false);
}

void MainWindow::enableInputs()
{
    this->ui->energy->setEnabled(true);
    this->ui->chkBoxInfIterations->setEnabled(true);
    this->ui->numSingularValues->setEnabled(true);
    this->ui->maxFreqChange->setEnabled(true);
    this->ui->maxCurrChange->setEnabled(true);
    this->ui->maxReadFail->setEnabled(true);
    this->ui->chkBoxIncludeRf->setEnabled(true);
    this->ui->chkBoxApplyReg->setEnabled(true);

    if (this->inf_iterations)
        this->ui->numIterations->setEnabled(false);
    else
        this->ui->numIterations->setEnabled(true);
}

void MainWindow::on_btnExpert_clicked()
{
    OPEN_UI(expert, Expert, &avg_algo, &window_size, &smoothing_factor, &rf_only, &debug_mode, this);
}
