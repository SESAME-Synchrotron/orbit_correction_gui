#ifndef PIDGAINWIDGET_H
#define PIDGAINWIDGET_H

#include <QWidget>

namespace Ui {
class PIDGainWidget;
}

class PIDGainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PIDGainWidget(int bpmIndex, QString type, QWidget *parent = nullptr);
    ~PIDGainWidget();

private:
    Ui::PIDGainWidget *ui;
    QString pvName;
};

#endif // PIDGAINWIDGET_H
