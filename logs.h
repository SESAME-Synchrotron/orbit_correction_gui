#ifndef LOGS_H
#define LOGS_H

#include <QMainWindow>
#include <qfile.h>
#include <qtextstream.h>
#include <qfiledialog.h>
#include <qplaintextedit.h>

namespace Ui {
class Logs;
}

class Logs : public QMainWindow
{
    Q_OBJECT

public:
    explicit Logs(QWidget *parent = nullptr);
    ~Logs();

private:
    Ui::Logs *ui;

    QString base_path;
    QString logs_path;
    QFile* logFile;
    QPlainTextEdit *textEdit;
};

#endif // LOGS_H
