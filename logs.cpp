#include "logs.h"
#include "ui_logs.h"

Logs::Logs(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Logs)
{
    ui->setupUi(this);

    this->base_path = "/home/control/Documents/sofb/orbit_correction/";
    this->logs_path = this->base_path + "logs";
    this->logFile = new QFile(this->logs_path + "/history.log");

    textEdit = new QPlainTextEdit(this);
    setCentralWidget(textEdit);

    if (logFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(logFile);
      textEdit->setPlainText(in.readAll());
      QTextCursor cursor = textEdit->textCursor();
      cursor.movePosition(QTextCursor::End);
      textEdit->setTextCursor(cursor);
    }
}

Logs::~Logs()
{
    if (logFile->isOpen())
        logFile->close();
    delete ui;
}
