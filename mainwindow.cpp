#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->numIterations->setEnabled(false);

    this->energy               = new QEpicsPV("SOFB:EnergyLevel");
    this->num_iterations       = new QEpicsPV("SOFB:NumIterations");
    this->num_singular_values  = new QEpicsPV("SOFB:NumSingularVals");
    this->max_frequency_change = new QEpicsPV("SOFB:MaxFreqChange");
    this->max_current_change   = new QEpicsPV("SOFB:MaxCurrentChange");
    this->max_read_fail        = new QEpicsPV("SOFB:MaxReadFail");
    this->correctionStatus     = new QEpicsPV("SOFB:CorrectionStatus");
    this->include_rf           = new QEpicsPV("SOFB:IncludeRf");
    this->apply_regularization = new QEpicsPV("SOFB:ApplyRegularization");
    this->rf_only              = new QEpicsPV("SOFB:OnlyRf");
    this->debug_mode           = new QEpicsPV("SOFB:NoSetPv");
    this->avg_algo             = new QEpicsPV("SOFB:MovAvg:Algo");
    this->window_size          = new QEpicsPV("SOFB:MovAvg:WindowSize");
    this->smoothing_factor     = new QEpicsPV("SOFB:MovAvg:SmoothingFactor");

    this->correction_process = new QProcess();
    this->base_path = "/home/control/Documents/sofb/orbit_correction/";
    this->data_path = this->base_path + "data";
    this->logs_path = this->base_path + "logs";
    this->logFile = new QFile(this->logs_path + "/last_run.log");
    this->timer = new QTimer(this);

    QObject::connect(correction_process, SIGNAL(finished(int)), this, SLOT(on_correctionEnd(int)));
    QObject::connect(num_iterations, SIGNAL(valueInited(const QVariant &)), this, SLOT(onNumIterationsInit(const QVariant &)));
    QObject::connect(include_rf, SIGNAL(valueInited(const QVariant &)), this, SLOT(onIncludeRfInit(const QVariant &)));
    QObject::connect(apply_regularization, SIGNAL(valueInited(const QVariant &)), this, SLOT(onApplyRegularizationInit(const QVariant &)));
    QObject::connect(correctionStatus, SIGNAL(valueInited(const QVariant &)), this, SLOT(onCorrectionStatusInit(const QVariant &)));
    QObject::connect(correctionStatus, SIGNAL(valueChanged(const QVariant &)), this, SLOT(onCorrectionStatusChanged(const QVariant &)));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(logData()));

    this->expert = NULL;

    CONNECT_CLOSE_BUTTON;
}

MainWindow::~MainWindow()
{
    if (logFile->isOpen())
        logFile->close();
    delete ui;
}

void MainWindow::onNumIterationsInit(const QVariant& numOfIterations)
{
    if (numOfIterations == -1)
    {
        this->inf_iterations = true;
        this->ui->chkBoxInfIterations->setChecked(1);
    }
    else
    {
        this->inf_iterations = false;
        this->ui->chkBoxInfIterations->setChecked(0);
    }
}

void MainWindow::onIncludeRfInit(const QVariant &val)
{
    this->ui->chkBoxIncludeRf->setChecked(val.toBool());
}

void MainWindow::onApplyRegularizationInit(const QVariant &val)
{
    this->ui->chkBoxApplyReg->setChecked(val.toBool());
}

void MainWindow::on_chkBoxInfIterations_stateChanged(int state)
{
    this->inf_iterations = state;

    if (state)
    {
        Client::writePV("SOFB:NumIterations", -1);
        this->ui->numIterations->setEnabled(false);
    }
    else
    {
        Client::writePV("SOFB:NumIterations", 1);
        this->ui->numIterations->setEnabled(true);
    }
}

void MainWindow::on_btnStartCorrection_clicked()
{
    int correction_iterations = num_iterations->get().toInt();
    QString correction_iterations_lbl = QString::number(correction_iterations);
    if (this->inf_iterations)
    {
        correction_iterations = -1;
        correction_iterations_lbl = "Inf";
    }

    QStringList energyLevels = this->energy->getEnum();
    QStringList movAvgAlgos = this->avg_algo->getEnum();
    QStringList params;
    params << "-energy" << energyLevels[this->energy->get().toInt()];
    params << "-num_iter" << QString::number(correction_iterations);
    params << "-num_singular" << num_singular_values->get().toString();
    params << "-max_freq_change" << max_frequency_change->get().toString();
    params << "-max_curr_change" << max_current_change->get().toString();
    params << "-max_read_fail" << max_read_fail->get().toString();
    params << "-data_path" << data_path;
    params << "-logs_path" << logs_path;
    params << "-avg_algo" << movAvgAlgos[this->avg_algo->get().toInt()];
    params << "-window_size" << window_size->get().toString();
    params << "-avg_smooth" << smoothing_factor->get().toString();
    if (this->include_rf->get().toBool())
        params << "-include_rf";
    if (this->apply_regularization->get().toBool())
        params << "-apply_reg";
    if (this->rf_only->get().toBool())
        params << "-rf_only";
    if (this->debug_mode->get().toBool())
        params << "-no_set";

    disableInputs();
    ui->btnStartCorrection->setEnabled(false);
    ui->btnStopCorrection->setEnabled(true);
    ui->lblLogs->setText("");
    ui->lblCurrentIterationLimit->setText("out of " + correction_iterations_lbl);
    correction_process->start("sofb", params);
}

void MainWindow::print_stdout()
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
    QString processName = "sofb$";
    QProcess pkillProcess;
    pkillProcess.start("pkill", QStringList() << processName);
    pkillProcess.waitForFinished();

    if (pkillProcess.exitCode() != 0)
    {
        std::cout << "Error in Terminating Correction Processes: " << pkillProcess.errorString().toStdString() << std::endl;
    }
}

void MainWindow::on_correctionEnd(int status)
{
  QString state = "Success";
  if (status)
    state = "Fail";

  ui->lblLogs->setText(ui->lblLogs->text() + "\n" + state);
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

void MainWindow::onCorrectionStatusInit(const QVariant &status)
{
    const int running = 1;
    const int debug = 3;
    if (status == running || status == debug)
    {
        ui->btnStartCorrection->setEnabled(false);
        ui->btnStopCorrection->setEnabled(false);
        startLogging();
        disableInputs();
    }
}

void MainWindow::onCorrectionStatusChanged(const QVariant &status)
{
    const int success = 0;
    const int running = 1;
    const int fail = 2;
    const int debug = 3;
    if (status == success || status == fail)
    {
        ui->btnStartCorrection->setEnabled(true);
        ui->btnStopCorrection->setEnabled(false);
        enableInputs();
        //this->timer->stop();
        //if (logFile->isOpen())
        //    logFile->close();
    } else if (status == running || status == debug)
    {
        ui->btnStartCorrection->setEnabled(false);
        ui->btnStopCorrection->setEnabled(true);
        startLogging();
        disableInputs();
    }
}

void MainWindow::on_btnExpert_clicked()
{
    OPEN_UI(expert, Expert, this);
}

void MainWindow::startLogging()
{
    if(!logFile->isOpen())
    {
      if(!logFile->open(QIODevice::ReadOnly))
          this->ui->lblLogs->setText("Warning: Could not read log file\n" + logFile->errorString());
      else
          this->timer->start(500);
    }
}

void MainWindow::logData()
{
    QTextStream in(logFile);
    QString content = in.readAll();
    this->ui->lblLogs->setText(content);
    logFile->seek(0);
}
