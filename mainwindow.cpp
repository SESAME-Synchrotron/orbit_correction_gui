#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "qdebug.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , thresholdFreq(100.0/1000000.0)
    , remoteHost("dev.control@10.2.0.23")
    , bumpFile({{"No Bump", "golden_2500_wo_bump.txt"},
                {"Bump 0.6", "golden_2500_0p6_bump.txt"},
                {"Bump 1.2", "golden_2500_1p2_bump.txt"}})
{
    ui->setupUi(this);
    this->ui->numIterations->setEnabled(false);
    this->ui->bump->addItem("No Bump", QVariant("No Bump"));
    this->ui->bump->addItem("Bump 0.6", QVariant("Bump 0.6"));
    this->ui->bump->addItem("Bump 1.2", QVariant("Bump 1.2"));

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
    this->normalize_inputs     = new QEpicsPV("SOFB:NormalizeInputs");
    this->scale_outputs        = new QEpicsPV("SOFB:ScaleOutputs");
    this->control_algo         = new QEpicsPV("SOFB:Control:Algo");
    this->estimation_algo      = new QEpicsPV("SOFB:Estimation:Algo");
    this->avg_algo             = new QEpicsPV("SOFB:MovAvg:Algo");
    this->window_size          = new QEpicsPV("SOFB:MovAvg:WindowSize");
    this->smoothing_factor     = new QEpicsPV("SOFB:MovAvg:SmoothingFactor");
    this->regularization_Param = new QEpicsPV("SOFB:RegularizationParam");
    this->getFrequency         = new QEpicsPV("BO-RF-SGN1:getFrequency");
    this->setFrequency         = new QEpicsPV("BO-RF-SGN1:setFrequency");

    /* Current PVs for removing correction */
    for (int cellIdx = 1; cellIdx <= 16; ++cellIdx)
    {
        for (int idx = 1; idx <= 2; ++idx)
        {
            this->getCorrectors_currents.push_back(new QEpicsPV(QString::asprintf("SRC%02d-PS-HC%d:getIload", cellIdx, idx)));
            this->getCorrectors_currents.push_back(new QEpicsPV(QString::asprintf("SRC%02d-PS-VC%d:getIload", cellIdx, idx)));

            this->setCorrectors_currents.push_back(QString::asprintf("SRC%02d-PS-HC%d:setReference", cellIdx, idx));
            this->setCorrectors_currents.push_back(QString::asprintf("SRC%02d-PS-VC%d:setReference", cellIdx, idx));
        }
    }
    /**/

    this->correction_process = new QProcess();
    this->logProcess = new QProcess();
    this->base_path = "/home/dev.control/orbit-correction/";
    this->data_path = this->base_path + "data";
    this->pm_path = "/mnt/machine-nfs01/machine/PostMortem/orbit_correction";
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
    QObject::connect(energy, SIGNAL(valueInited(const QVariant &)), this, SLOT(bumpInputEnableDisable(const QVariant &)));
    QObject::connect(energy, SIGNAL(valueChanged(const QVariant &)), this, SLOT(bumpInputEnableDisable(const QVariant &)));
    QObject::connect(loggingTimer, SIGNAL(timeout()), this, SLOT(logDataSSH()));
    QObject::connect(this, SIGNAL(stackLengthChanged(int)), this, SLOT(onStackLengthChanged(int)));

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

void MainWindow::bumpInputEnableDisable(const QVariant &energyIndex)
{
    QStringList energyLevels = this->energy->getEnum();

    if (energyLevels[energyIndex.toUInt()] == "800")
    {
        this->ui->bump->setEnabled(false);
        this->golden_path = this->data_path + "/golden_800.txt";
    } else if (energyLevels[energyIndex.toUInt()] == "2500")
    {
        this->ui->bump->setEnabled(true);
        QString type = this->ui->bump->currentData().toString();
        this->golden_path = this->data_path + "/" + this->bumpFile.at(type);
    }
}

void MainWindow::on_btnStartCorrection_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Confirmation", "Are you sure you want to start correction?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
      return;

    int correction_iterations = num_iterations->get().toInt();
    QString correction_iterations_lbl = QString::number(correction_iterations);
    if (this->inf_iterations)
    {
        correction_iterations = -1;
        correction_iterations_lbl = "Inf";
    }

    QStringList energyLevels = this->energy->getEnum();
    QStringList ControlAlgos = this->control_algo->getEnum();
    QStringList EstimationAlgos = this->estimation_algo->getEnum();
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
    params << "-golden_path" << golden_path;
    params << "-pm_path" << pm_path;
    params << "-control_algo" << ControlAlgos[this->control_algo->get().toInt()];
    params << "-estimation_algo" << EstimationAlgos[this->estimation_algo->get().toInt()];
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
    if (this->normalize_inputs->get().toBool())
        params << "-normalize_inputs";
    if (this->scale_outputs->get().toBool())
        params << "-scale_outputs";

    disableInputs();
    ui->btnStartCorrection->setEnabled(false);
    ui->btnStopCorrection->setEnabled(true);
    ui->lblLogs->setText("");
    ui->lblCurrentIterationLimit->setText("out of " + correction_iterations_lbl);
    //for (QString param : params)
    //{
    //    std::cout << param.toStdString() << " ";
    //}

    if (!this->debug_mode->get().toBool())
        saveCorrectorsAndRF();

    QStringList sshArgs;
    sshArgs << remoteHost << "sofb" << params;

    if (!correction_process->startDetached("ssh", sshArgs)) {
        std::cout << "Error: Unable to start Correction\n";
    }
}

void MainWindow::saveCorrectorsAndRF()
{
    std::array<double, 65> correction;
    for (int i = 0; i < 64; ++i)
    {
        correction[i] = this->getCorrectors_currents[i]->get().toDouble();
    }
    correction[64] = this->getFrequency->get().toDouble();
    correctionStack.push(correction);
    emit stackLengthChanged(correctionStack.length());
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
    QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Confirmation", "Are you sure you want to stop the correction?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
      return;

    QString processName = "sofb$";
    QProcess pkillProcess;
    //pkillProcess.start("pkill", QStringList() << processName);

    QStringList sshArgs;
    sshArgs << remoteHost << "pkill" << processName;

    if (!pkillProcess.startDetached("ssh", sshArgs)) {
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
    this->ui->bump->setEnabled(false);
    this->ui->chkBoxInfIterations->setEnabled(false);
    this->ui->numIterations->setEnabled(false);
    this->ui->numSingularValues->setEnabled(false);
    this->ui->maxFreqChange->setEnabled(false);
    this->ui->maxCurrChange->setEnabled(false);
    this->ui->maxReadFail->setEnabled(false);
    this->ui->samplingFrequency->setEnabled(false);
    this->ui->chkBoxIncludeRf->setEnabled(false);
    this->ui->chkBoxApplyReg->setEnabled(false);
    this->ui->btnRemoveCorrection->setEnabled(false);
}

void MainWindow::enableInputs()
{
    this->ui->energy->setEnabled(true);
    this->ui->chkBoxInfIterations->setEnabled(true);
    this->ui->numSingularValues->setEnabled(true);
    this->ui->maxFreqChange->setEnabled(true);
    this->ui->maxCurrChange->setEnabled(true);
    this->ui->maxReadFail->setEnabled(true);
    this->ui->samplingFrequency->setEnabled(true);
    this->ui->chkBoxIncludeRf->setEnabled(true);
    this->ui->chkBoxApplyReg->setEnabled(true);

    if (this->inf_iterations)
        this->ui->numIterations->setEnabled(false);
    else
        this->ui->numIterations->setEnabled(true);

    if (this->correctionStack.length())
        this->ui->btnRemoveCorrection->setEnabled(true);
    else
        this->ui->btnRemoveCorrection->setEnabled(false);

    bumpInputEnableDisable(this->energy->get());
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

void MainWindow::on_btnRemoveCorrection_clicked()
{
    if (!correctionStack.isEmpty())
    {
        std::array<double, 65> correction = correctionStack.pop();
        emit stackLengthChanged(correctionStack.length());
        for (int i = 0; i < 64; ++i)
        {
            Client::writePV(this->setCorrectors_currents[i], correction[i]);
        }
        Client::writePV(this->setFrequency->pv(), correction[64]);
    } else
    {
        std::cout << "Stack is empty!" <<std::endl;
    }
}

void MainWindow::onStackLengthChanged(int stackLength)
{
    this->ui->btnRemoveCorrection->setText(QString::asprintf("Remove Correction (%d)", stackLength));
    if (!stackLength)
        this->ui->btnRemoveCorrection->setEnabled(false);
}

void MainWindow::on_bump_activated(const QString &type)
{
    this->golden_path = this->data_path + "/" + this->bumpFile.at(type);
}
