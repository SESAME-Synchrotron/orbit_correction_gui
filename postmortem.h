#ifndef POSTMORTEM_H
#define POSTMORTEM_H

#include <QMainWindow>

namespace Ui {
class Postmortem;
}

class Postmortem : public QMainWindow
{
    Q_OBJECT

public:
    explicit Postmortem(QWidget *parent = nullptr);
    ~Postmortem();

private:
    Ui::Postmortem *ui;
};

#endif // POSTMORTEM_H
