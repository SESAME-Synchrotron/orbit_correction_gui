#ifndef VERTICALCORRECTORS_H
#define VERTICALCORRECTORS_H

#include <QMainWindow>

namespace Ui {
class VerticalCorrectors;
}

class VerticalCorrectors : public QMainWindow
{
    Q_OBJECT

public:
    explicit VerticalCorrectors(QWidget *parent = nullptr);
    ~VerticalCorrectors();

private:
    Ui::VerticalCorrectors *ui;
};

#endif // VERTICALCORRECTORS_H
