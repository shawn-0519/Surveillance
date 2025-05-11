#include "Surveillance.h"
#include <QMessageBox>
#include <fstream>
#include <iostream>
#include <sstream>
#include "xvideo_view.h"
#include "xsdl.h"

extern"C" {
#include <libavcodec/avcodec.h>
}
using namespace std;

//static SDL_Window *sdl_win = nullptr;
//static SDL_Renderer* sdl_render = nullptr;
//static SDL_Texture* sdl_texture = nullptr;
static int sdl_width = 0;
static int sdl_height = 0;
static int pix_size = 2;
static ifstream yuv_file;
static XVideoView* view = nullptr;
static AVFrame* frame = nullptr;
static QLabel* view_fps = nullptr;
static long long file_size = 0;
void Surveillance::timerEvent(QTimerEvent* ev)
{
    //yuv_file.read((char*)yuv, sdl_width * sdl_height * 1.5);
    yuv_file.read((char*)frame->data[0],frame->width*frame->height);          //Y
	yuv_file.read((char*)frame->data[1], frame->width * frame->height / 4);   //U
	yuv_file.read((char*)frame->data[2], frame->width * frame->height / 4);   //V   
    
    if (view->IsExit())
    {
        view->Close();
        exit(0);
    }
    //循环播放
    if (yuv_file.tellg() == file_size) //读取到文件结尾
    {
        yuv_file.seekg(0, ios::beg);
    }

    view->DrawFrame(frame);
}

void Surveillance::View() {
    yuv_file.read((char*)frame->data[0], frame->width * frame->height);       //Y
    yuv_file.read((char*)frame->data[1], frame->width * frame->height / 4);   //U
    yuv_file.read((char*)frame->data[2], frame->width * frame->height / 4);   //V   
  
    if (view->IsExit())
    {
        view->Close();
        exit(0);
    }
    //循环播放
    if (yuv_file.tellg() == file_size) //读取到文件结尾
    {
        yuv_file.seekg(0, ios::beg);
    }
    view->DrawFrame(frame);

    stringstream ss;
    ss << "fps" << view->render_fps();

    //只能在槽函数中调用
    view_fps->setText(ss.str().c_str());
    view_fps->adjustSize();// 宽度和高度自动根据实际内容调整
    view_fps->setStyleSheet("background: transparent; color: black;"); // 颜色可自定义

    //cout << "ss:" << ss.str().c_str()<<" ";
}



void Surveillance::Main()
{
    while(!is_exit_)
    {
        ViewS();
        view->MSleep(40);
    }
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

    //获取文件长度
    yuv_file.seekg(0, ios::end);
    file_size = yuv_file.tellg();
    yuv_file.seekg(0, ios::beg);


    ui.setupUi(this);

    //绑定渲染信号槽
    connect(this, SIGNAL(ViewS()), this, SLOT(View()));
    
    //显示fps的控件
    view_fps = new QLabel(this);
    view_fps->setText("fps:");
    
    
    sdl_width = 400;
    sdl_height = 300;
    resize(sdl_width, sdl_height);
    cout << "窗口大小：" << this->width() << endl;
    cout << "窗口大小：" << this->height() << endl;
    //ui.label->resize(sdl_width, sdl_height);
    view = XVideoView::Create();
    //view->Init(sdl_width, sdl_height, XVideoView::YUV420P);
    //view->Close();
    view->Init(sdl_width, sdl_height,XVideoView::YUV420P, (void*)ui.label->winId());
    
    

    //分配并初始化一个 AVFrame 结构体对象
    frame = av_frame_alloc();

	frame->width = sdl_width;
	frame->height = sdl_height;
	frame->format = AV_PIX_FMT_YUV420P;

    frame->linesize[0] = sdl_width;
	frame->linesize[1] = sdl_width / 2;
	frame->linesize[2] = sdl_width / 2;

    //为 AVFrame 分配实际数据缓冲区（内存空间）
	auto re = av_frame_get_buffer(frame, 0);
    if (re != 0) {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf));//将错误写入buf中
        cerr << buf << endl;
    }

    //创建并启动一个新的线程,让视频渲染、帧率控制等耗时操作在后台线程中异步执行，不会阻塞主线程
    th_ = std::thread(&Surveillance::Main, this);
    //startTimer(10);
    
}

void Surveillance::resizeEvent(QResizeEvent* ev)
{
    ui.label->resize(size());
    ui.label->move(0, 0);
    //view->Scale(width(), height());

    // 输出窗口和控件的大小
    cout << "this-size:" << this->width() << "x" << this->height();
    cout << "label-size:" << ui.label->width() << "x" << ui.label->height();
}




