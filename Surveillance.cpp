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
static int pix_size = 4;
static ifstream yuv_file;
static XVideoView* view = nullptr;
void Surveillance::timerEvent(QTimerEvent* ev)
{
    yuv_file.read((char*)yuv, sdl_width * sdl_height * 1.5);
    view->Draw(yuv);
    //static unsigned char tmp = 255;
    //tmp--;  
    //for (int j = 0; j < sdl_height; j++)
    //{
    //    int b = j * sdl_width * pix_size;
    //    for (int i = 0; i < sdl_width * pix_size; i += pix_size)
    //    {
    //        //rgb[b + i] = 0;         //B
    //        //rgb[b + i + 1] = tmp;   //G
    //        //rgb[b + i + 2] = 0;     //R
    //        //rgb[b + i + 3] = 0;     //A
    //    }
    //}
    // 
    //把 rgb 指向的像素数据，按照每行的步长，全部更新到 sdl_texture 这个纹理对象里。
    //SDL_UpdateTexture(sdl_texture, nullptr, yuv, 
    //    sdl_width//一行 y的字节数
    //);
    ////清除当前渲染目标上的内容
    //SDL_RenderClear(sdl_render);
    //SDL_Rect rect;
    //rect.x = 0;
    //rect.y = 0;
    //rect.w = sdl_width;
    //rect.h = sdl_height;
    ////把 sdl_texture 里的内容，按照 rect 指定的位置和大小，渲染到 sdl_render 关联的窗口上。
    //SDL_RenderCopy(sdl_render, sdl_texture, nullptr, &rect);
    //SDL_RenderPresent(sdl_render);
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
    //ui.label->resize(sdl_width, sdl_height);
    view = XVideoView::Create();
    view->Init(sdl_width, sdl_height,
        XVideoView::YUV420P, (void*)ui.label->winId());
    //初始化SDL
    //SDL_Init(SDL_INIT_VIDEO);
    ////创建窗口
    //sdl_win = SDL_CreateWindowFrom((void*)ui.label->winId());
    ////创建渲染器
    //sdl_render = SDL_CreateRenderer(sdl_win, -1, SDL_RENDERER_ACCELERATED);
    ////创建材质 支持YUV
    //sdl_texture = SDL_CreateTexture(sdl_render,
    //    SDL_PIXELFORMAT_IYUV,
    //    SDL_TEXTUREACCESS_STREAMING,
    //    sdl_width,
    //    sdl_height
    //);
    yuv = new unsigned char[sdl_width * sdl_height * pix_size];
    startTimer(10);
}

void Surveillance::resizeEvent(QResizeEvent* ev)
{
    ui.label->resize(size());
    ui.label->move(0, 0);
    view->Scale(width(), height());

    // 输出窗口和控件的大小
    qDebug() << "this-size:" << this->width() << "x" << this->height();
    qDebug() << "label-size:" << ui.label->width() << "x" << ui.label->height();
}
   
Surveillance::~Surveillance()
{}

