#pragma once

#include <QtWidgets/QWidget>
#include "ui_Surveillance.h"

class Surveillance : public QWidget
{
    Q_OBJECT

public:
    Surveillance(QWidget *parent = nullptr);
    ~Surveillance();
    void timerEvent(QTimerEvent *ev)override;
    void resizeEvent(QResizeEvent *ev)override;
private:
    Ui::SurveillanceClass ui;
};
