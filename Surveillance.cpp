#include "Surveillance.h"
#include <QMessageBox>
#include "xvideo_view.h"
#include "xsdl.h"
#include <fstream>
#include <iostream>

using namespace std;

//static SDL_Window *sdl_win = nullptr;
//static SDL_Renderer* sdl_render = nullptr;
//static SDL_Texture* sdl_texture = nullptr;
static int sdl_width = 0;
static int sdl_height = 0;
static unsigned char* yuv = NULL;
static int pix_size = 2;
static ifstream yuv_file;
static XVideoView* view = nullptr;
void Surveillance::timerEvent(QTimerEvent* ev)
{
    yuv_file.read((char*)yuv, sdl_width * sdl_height * 1.5);
    view->Draw(yuv);
}


Surveillance::Surveillance(QWidget* parent)
    : QWidget(parent)
{
    //打开yuv文件
    yuv_file.open("400_300_25.yuv", ios::binary);
    if (!yuv_file)
    {
        QMessageBox::information(this, "", "open yuv failed!");
        return;
    }
    ui.setupUi(this);
    
    sdl_width = 400;
    sdl_height = 300;
    resize(sdl_width, sdl_height);
    cout << "窗口大小：" << this->width() << endl;
    cout << "窗口大小：" << this->height() << endl;
    //ui.label->resize(sdl_width, sdl_height);
    view = XVideoView::Create();
    view->Init(sdl_width, sdl_height,
        XVideoView::YUV420P, (void*)ui.label->winId());
    

    yuv = new unsigned char[sdl_width * sdl_height * pix_size];
    startTimer(10);
}

void Surveillance::resizeEvent(QResizeEvent* ev)
{
    ui.label->resize(size());
    ui.label->move(0, 0);
    view->Scale(width(), height());

    // 输出窗口和控件的大小
    cout << "this-size:" << this->width() << "x" << this->height();
    cout << "label-size:" << ui.label->width() << "x" << ui.label->height();
}
   
Surveillance::~Surveillance()
{}



