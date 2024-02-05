#include "logs.h"
#include "ui_logs.h"
#include <iostream>

Logs::Logs(QWidget *parent)
    : QMainWindow(parent)
    ,  ui(new Ui::Logs)
    , remoteHost("dev.control@10.2.0.23")
{
    ui->setupUi(this);

    this->base_path = "/home/dev.control/orbit-correction/";
    this->logs_path = this->base_path + "logs";

    textEdit = new QPlainTextEdit(this);
    setCentralWidget(textEdit);

    QProcess logProcess;
    QStringList sshArgs;
    sshArgs << remoteHost << "cat" << this->logs_path + "/history.log";
    logProcess.start("ssh", sshArgs);
    if (logProcess.waitForFinished())
    {
        QString output = logProcess.readAll();
        textEdit->setPlainText(output);
        textEdit->moveCursor(QTextCursor::End);
    } else {
        std::cout << "SSH Command: ssh " << sshArgs.join(" ").toStdString() << std::endl;
        std::cout << "SSH Exit Code: " << logProcess.exitCode()  << std::endl;
        std::cout << "SSH Error: " << logProcess.errorString().toStdString() << std::endl;
        std::cout << "Error: Unable to execute SSH command\n";
    }

    /* // local file reading
    this->logFile = new QFile(this->logs_path + "/history.log");
    if (logFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(logFile);
      textEdit->setPlainText(in.readAll());
      QTextCursor cursor = textEdit->textCursor();
      cursor.movePosition(QTextCursor::End);
      textEdit->setTextCursor(cursor);
    }
    */
}

Logs::~Logs()
{
//    if (logFile->isOpen())
//        logFile->close();
    delete ui;
}
