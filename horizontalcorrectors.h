#ifndef HORIZONTALCORRECTORS_H
#define HORIZONTALCORRECTORS_H

#include <QMainWindow>

namespace Ui {
class HorizontalCorrectors;
}

class HorizontalCorrectors : public QMainWindow
{
    Q_OBJECT

public:
    explicit HorizontalCorrectors(QWidget *parent = nullptr);
    ~HorizontalCorrectors();

private:
    Ui::HorizontalCorrectors *ui;
};

#endif // HORIZONTALCORRECTORS_H
