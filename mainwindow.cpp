#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "qdebug.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , thresholdFreq(100.0/1000000.0)
    , remoteHost("dev.control@10.2.0.23")
{
    ui->setupUi(this);
    this->ui->numIterations->setEnabled(false);

    this->sampling_frequency   = new QEpicsPV("SOFB:SamplingFrequency");
    this->energy               = new QEpicsPV("SOFB:EnergyLevel");
    this->num_iterations       = new QEpicsPV("SOFB:NumIterations");
    this->num_singular_values  = new QEpicsPV("SOFB:NumSingularVals");
    this->correction_factor    = new QEpicsPV("SOFB:CorrectionFactor");
    this->max_frequency_change = new QEpicsPV("SOFB:MaxFreqChange");
    this->max_current_change   = new QEpicsPV("SOFB:MaxCurrentChange");
    this->max_read_fail        = new QEpicsPV("SOFB:MaxReadFail");
    this->correction_status    = new QEpicsPV("SOFB:CorrectionStatus");
    this->include_rf           = new QEpicsPV("SOFB:IncludeRf");
    this->apply_regularization = new QEpicsPV("SOFB:ApplyRegularization");
    this->rf_only              = new QEpicsPV("SOFB:OnlyRf");
    this->debug_mode           = new QEpicsPV("SOFB:NoSetPv");
    this->avg_algo             = new QEpicsPV("SOFB:MovAvg:Algo");
    this->window_size          = new QEpicsPV("SOFB:MovAvg:WindowSize");
    this->smoothing_factor     = new QEpicsPV("SOFB:MovAvg:SmoothingFactor");
    this->regularization_Param = new QEpicsPV("SOFB:RegularizationParam");
    this->getFrequency         = new QEpicsPV("BO-RF-SGN1:getFrequency");

    this->correction_process = new QProcess();
    this->logProcess = new QProcess();
    this->base_path = "/home/dev.control/orbit-correction/";
    this->data_path = this->base_path + "data";
    this->logs_path = this->base_path + "logs";
    this->logFile = new QFile(this->logs_path + "/last_run.log");
    this->loggingTimer = new QTimer(this);

    QObject::connect(correction_process, SIGNAL(finished(int)), this, SLOT(on_correctionEnd(int)));
    QObject::connect(num_iterations, SIGNAL(valueInited(const QVariant &)), this, SLOT(onNumIterationsInit(const QVariant &)));
    QObject::connect(include_rf, SIGNAL(valueInited(const QVariant &)), this, SLOT(onIncludeRfInit(const QVariant &)));
    QObject::connect(apply_regularization, SIGNAL(valueInited(const QVariant &)), this, SLOT(onApplyRegularizationInit(const QVariant &)));
    QObject::connect(correction_status, SIGNAL(valueInited(const QVariant &)), this, SLOT(onCorrectionStatusInit(const QVariant &)));
    QObject::connect(correction_status, SIGNAL(valueChanged(const QVariant &)), this, SLOT(onCorrectionStatusChanged(const QVariant &)));
    QObject::connect(rf_only, SIGNAL(valueInited(const QVariant &)), this, SLOT(checkRfOnlyRun()));
    QObject::connect(rf_only, SIGNAL(valueChanged(const QVariant &)), this, SLOT(checkRfOnlyRun()));
    QObject::connect(getFrequency, SIGNAL(valueInited(const QVariant &)), this, SLOT(frequencyPvInit(const QVariant &)));
    QObject::connect(getFrequency, SIGNAL(valueChanged(const QVariant &)), this, SLOT(checkRfOnlyRun()));
    QObject::connect(loggingTimer, SIGNAL(timeout()), this, SLOT(logDataSSH()));

    this->expert = NULL;
    this->historyLogs = NULL;
    this->plots = NULL;

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
    params << "-sampling_freq" << sampling_frequency->get().toString();
    params << "-energy" << energyLevels[this->energy->get().toInt()];
    params << "-num_iter" << QString::number(correction_iterations);
    params << "-num_singular" << num_singular_values->get().toString();
    params << "-correction_factor" << correction_factor->get().toString();
    params << "-max_freq_change" << max_frequency_change->get().toString();
    params << "-max_curr_change" << max_current_change->get().toString();
    params << "-max_read_fail" << max_read_fail->get().toString();
    params << "-data_path" << data_path;
    params << "-logs_path" << logs_path;
    params << "-avg_algo" << movAvgAlgos[this->avg_algo->get().toInt()];
    params << "-window_size" << window_size->get().toString();
    params << "-avg_smooth" << smoothing_factor->get().toString();
    params << "-reg_param" << regularization_Param->get().toString();
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
    //for (QString param : params)
    //{
    //    std::cout << param.toStdString() << " ";
    //}

    QStringList sshArgs;
    sshArgs << remoteHost << "sofb" << params;
    correction_process->setProgram("ssh");
    correction_process->setArguments(sshArgs);

    if (!correction_process->startDetached()) {
        std::cout << "Error: Unable to start Correction\n";
    }
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
    //pkillProcess.start("pkill", QStringList() << processName);

    QStringList sshArgs;
    sshArgs << remoteHost << "pkill" << processName;
    pkillProcess.setProgram("ssh");
    pkillProcess.setArguments(sshArgs);

    if (!pkillProcess.startDetached()) {
        std::cout << "Error: Unable to stop Correction\n";
    }
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
        this->loggingTimer->start(500);
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
        this->loggingTimer->stop();
        if (logFile->isOpen())
            logFile->close();
    } else if (status == running || status == debug)
    {
        ui->btnStartCorrection->setEnabled(false);
        ui->btnStopCorrection->setEnabled(true);
        this->loggingTimer->start(500);
        disableInputs();
    }
}

void MainWindow::on_btnExpert_clicked()
{
    OPEN_UI(expert, Expert, this);
}

void MainWindow::startLocalLogging()
{
    if(!logFile->isOpen())
    {
      if(!logFile->open(QIODevice::ReadOnly))
          this->ui->lblLogs->setText("Warning: Could not read log file\n" + logFile->errorString());
      else
          this->loggingTimer->start(500);
    }
}

void MainWindow::logLocalData()
{
    QTextStream in(logFile);
    QString content = in.readAll();
    this->ui->lblLogs->setText(content);
    logFile->seek(0);
}

void MainWindow::logDataSSH()
{
    QStringList sshArgs;
    sshArgs << remoteHost << "cat" << this->logs_path + "/last_run.log";

    logProcess->start("ssh", sshArgs);
    if (logProcess->waitForFinished())
    {
        QString output = logProcess->readAll();
        this->ui->lblLogs->setText(output);
    } else {
        std::cout << "SSH Command: ssh " << sshArgs.join(" ").toStdString() << std::endl;
        std::cout << "SSH Exit Code: " << logProcess->exitCode()  << std::endl;
        std::cout << "SSH Error: " << logProcess->errorString().toStdString() << std::endl;
        std::cout << "Error: Unable to execute SSH command\n";
    }
}

void MainWindow::frequencyPvInit(const QVariant& val)
{
    currentFreq = val.toDouble();
    prevFreq = val.toDouble();
}

void MainWindow::checkRfOnlyRun()
{
    currentFreq = getFrequency->get().toDouble();
    if (fabs(currentFreq - prevFreq) > thresholdFreq || rf_only->get().toBool())
        this->ui->ledRfOnly->setValue(1);
    else
      this->ui->ledRfOnly->setValue(0);
    prevFreq = currentFreq;
}

void MainWindow::on_btnHistoryLogs_clicked()
{
    OPEN_UI(historyLogs, Logs, this);
}

void MainWindow::on_btnPlots_clicked()
{
    OPEN_UI(plots, Plots, this);
}
