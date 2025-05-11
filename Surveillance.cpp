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
    //ѭ������
    if (yuv_file.tellg() == file_size) //��ȡ���ļ���β
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
    //ѭ������
    if (yuv_file.tellg() == file_size) //��ȡ���ļ���β
    {
        yuv_file.seekg(0, ios::beg);
    }
    view->DrawFrame(frame);

    stringstream ss;
    ss << "fps" << view->render_fps();

    //ֻ���ڲۺ����е���
    view_fps->setText(ss.str().c_str());
    view_fps->adjustSize();// ��Ⱥ͸߶��Զ�����ʵ�����ݵ���
    view_fps->setStyleSheet("background: transparent; color: black;"); // ��ɫ���Զ���

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
    //��yuv�ļ�
    yuv_file.open("400_300_25.yuv", ios::binary);
    if (!yuv_file)
    {
        QMessageBox::information(this, "", "open yuv failed!");
        return;
    }

    //��ȡ�ļ�����
    yuv_file.seekg(0, ios::end);
    file_size = yuv_file.tellg();
    yuv_file.seekg(0, ios::beg);


    ui.setupUi(this);

    //����Ⱦ�źŲ�
    connect(this, SIGNAL(ViewS()), this, SLOT(View()));
    
    //��ʾfps�Ŀؼ�
    view_fps = new QLabel(this);
    view_fps->setText("fps:");
    
    
    sdl_width = 400;
    sdl_height = 300;
    resize(sdl_width, sdl_height);
    cout << "���ڴ�С��" << this->width() << endl;
    cout << "���ڴ�С��" << this->height() << endl;
    //ui.label->resize(sdl_width, sdl_height);
    view = XVideoView::Create();
    //view->Init(sdl_width, sdl_height, XVideoView::YUV420P);
    //view->Close();
    view->Init(sdl_width, sdl_height,XVideoView::YUV420P, (void*)ui.label->winId());
    
    

    //���䲢��ʼ��һ�� AVFrame �ṹ�����
    frame = av_frame_alloc();

	frame->width = sdl_width;
	frame->height = sdl_height;
	frame->format = AV_PIX_FMT_YUV420P;

    frame->linesize[0] = sdl_width;
	frame->linesize[1] = sdl_width / 2;
	frame->linesize[2] = sdl_width / 2;

    //Ϊ AVFrame ����ʵ�����ݻ��������ڴ�ռ䣩
	auto re = av_frame_get_buffer(frame, 0);
    if (re != 0) {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf));//������д��buf��
        cerr << buf << endl;
    }

    //����������һ���µ��߳�,����Ƶ��Ⱦ��֡�ʿ��ƵȺ�ʱ�����ں�̨�߳����첽ִ�У������������߳�
    th_ = std::thread(&Surveillance::Main, this);
    //startTimer(10);
    
}

void Surveillance::resizeEvent(QResizeEvent* ev)
{
    ui.label->resize(size());
    ui.label->move(0, 0);
    //view->Scale(width(), height());

    // ������ںͿؼ��Ĵ�С
    cout << "this-size:" << this->width() << "x" << this->height();
    cout << "label-size:" << ui.label->width() << "x" << ui.label->height();
}




