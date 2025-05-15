#include "Surveillance.h"
#include <QMessageBox>
#include <fstream>
#include <iostream>
#include <sstream>
#include <QSpinBox>
#include <QFileDialog>
#include <vector>
#include <Qstring>
#include "xvideo_view.h"
#include "xsdl.h"

extern"C" {
#include <libavcodec/avcodec.h>
}
using namespace std;
static std::vector<XVideoView*> views;
//static SDL_Window *sdl_win = nullptr;
//static SDL_Renderer* sdl_render = nullptr;
//static SDL_Texture* sdl_texture = nullptr;
//static int sdl_width = 0;
//static int sdl_height = 0;
//static int pix_size = 2;
//static ifstream yuv_file;
//static XVideoView* view = nullptr;
//static AVFrame* frame = nullptr;
//static QLabel* view_fps = nullptr;
//static long long file_size = 0;
//static QSpinBox* set_fps = nullptr;
//int fps = 25;
void Surveillance::timerEvent(QTimerEvent* ev)
{
    
}

void Surveillance::View() 
{
    for (int i = 0; i < views.size(); i++)
    {
        auto frame = views[i]->Read();
        if (!frame)continue;
        views[i]->DrawFrame(frame);
    }
}



void Surveillance::Main()
{
    while(!is_exit_)
    {
        ViewS();
        MSleep(10);
    }
}

void Surveillance::Open(int i)
{
    QFileDialog* fd;
    auto filename = fd->getOpenFileName();
    if (filename.isEmpty())return;
    cout << filename.toLocal8Bit().data() << endl;

    //打开文件
    if (!views[i]->Open(filename.toStdString())) {
        return;
    }

    //配置参数
    int w = 0;
    int h = 0;
    QString pix = 0;//YUV420P RGBA
    if (i == 0)
    {
        w = ui.width1->value();
        h = ui.height1->value();
        pix = ui.pix1->currentText(); //像素格式
    }
    else if (i == 1)
    {
        w = ui.width2->value();
        h = ui.height2->value();
        pix = ui.pix2->currentText();
    }

    XVideoView::Format fmt = XVideoView::YUV420P;
    if (pix == "YUV420P")
    {

    }
    else if (pix == "ARGB")
    {
        fmt = XVideoView::ARGB;
    }
    else if (pix == "BGRA")
    {
        fmt = XVideoView::BGRA;
    }
    else if (pix == "RGBA")
    {
        fmt = XVideoView::RGBA;
    }

    //初始化
    views[i]->Init(w, h, fmt);
    
}

void Surveillance::Open1()
{
    Open(0);
}

void Surveillance::Open2()
{
    Open(1);
}

Surveillance::Surveillance(QWidget* parent)
    : QWidget(parent)
{
    

    ui.setupUi(this);

    //绑定渲染信号槽
    connect(this, SIGNAL(ViewS()), this, SLOT(View()));
    views.push_back(XVideoView::Create());
    views.push_back(XVideoView::Create());
    views[0]->set_win_id((char*)ui.video1->winId());
    views[1]->set_win_id((char*)ui.video2->winId());


    //创建并启动一个新的线程,让视频渲染、帧率控制等耗时操作在后台线程中异步执行，不会阻塞主线程
    th_ = std::thread(&Surveillance::Main, this);
    
}

void Surveillance::resizeEvent(QResizeEvent* ev)
{

}




