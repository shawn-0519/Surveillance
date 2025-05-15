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
        //等待渲染线程退出
        th_.join();
    }

    //
    void timerEvent(QTimerEvent *ev)override;

    //
    void resizeEvent(QResizeEvent *ev)override;

    //线程函数，用于刷新视频
    void Main();

signals:
    void ViewS();   //信号函数，将任务放入列表

public slots:
    void View();    //显示的槽函数
    void Open(int i);
    void Open1();
    void Open2();

private:
    std::thread th_;
    bool is_exit_ = false;//处理线程退出
    Ui::SurveillanceClass ui;
};
