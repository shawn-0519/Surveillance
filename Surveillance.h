#pragma once
#include <thread>
#include <QtWidgets/QWidget>
#include "ui_Surveillance.h"

class Surveillance : public QWidget
{
    Q_OBJECT

public:
    Surveillance(QWidget *parent = nullptr);

    ~Surveillance()
    {
        is_exit_ = true;
        //�ȴ���Ⱦ�߳��˳�
        th_.join();
    }

    //
    void timerEvent(QTimerEvent *ev)override;

    //
    void resizeEvent(QResizeEvent *ev)override;

    //�̺߳���������ˢ����Ƶ
    void Main();

signals:
    void ViewS();   //�źź���������������б�

public slots:
    void View();    //��ʾ�Ĳۺ���

private:
    std::thread th_;
    bool is_exit_ = false;
    Ui::SurveillanceClass ui;
};
