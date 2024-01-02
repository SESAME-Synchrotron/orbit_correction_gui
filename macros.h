#ifndef _MACROS_H
#define _MACROS_H

#include <QDesktopWidget>
#include <QApplication>
#include <QStyle>
#include <QUiLoader>
#include <QFile>
#include <QPushButton>

void showUI(QWidget* widget);
void loadUI(QString fileName);

#define CONNECT_CLOSE_BUTTON \
    QObject::connect(this->ui->btnClose, SIGNAL(clicked()), this, SLOT(close())); \

#define SET_GROUP(CLASS) \
    foreach (CLASS* widget, this->findChildren<CLASS*>()) \
    { \
        widget->setVariableNameSubstitutionsProperty("group=" + this->group); \
    }

#define STRING(X) \
    QString::number(X) \

#define FIX_SIZE \
    setFixedSize(width(), height()) \

#define COLOR_DARKGREEN QColor(20, 60, 20)

#define OPEN_UI(object, CLASS, ...)      \
    if(!object || !object->isVisible())  \
        object = new CLASS(__VA_ARGS__); \
    showUI(object);                      \

#define TITLE_MAIN "SESAME Control System"

#define iterate_for(item, container) \
    for(auto item = container.begin(); item != container.end(); item++) \

// Nice lambda spells ....
// #define list_sum(container)         std::accumulate(container.begin(), container.end(), 0, [&](double a, QEpicsPV* b) { return a + b->get().toInt(); })
#define list_range_sum(container, end) std::accumulate(container.begin(), container.begin() + end, 0, [](double a, QEpicsPV* b) { return a + b->get().toInt(); })
#define list_iterator_sum(begin, end)  std::accumulate(begin, end, 0, [](double a, QEpicsPV* b) { return a + b->get().toInt(); })
#define list_sum(container)            list_range_sum(container, container.size())

// ui->ledTL2Vacuum->setValue( std::any_of(pvTL2Vacuum.begin(), pvTL2Vacuum.end(), [](QEpicsPV* pv){ return pv->get().toInt() == 1; }));
#define contains_any(container, value) std::any_of(std::begin(container), std::end(container), [](QEpicsPV* pv) { return pv->get().toInt() == value; })


#endif // _MACROS_H
