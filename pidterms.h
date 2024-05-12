#ifndef PIDTERMS_H
#define PIDTERMS_H

#include <QMainWindow>

namespace Ui {
class PIDTerms;
}

class PIDTerms : public QMainWindow
{
    Q_OBJECT

public:
    explicit PIDTerms(QWidget *parent = nullptr);
    ~PIDTerms();

private:
    Ui::PIDTerms *ui;
};

#endif // PIDTERMS_H
