#include "xsdl.h"
#include <SDL.h>
#include <iostream>
#pragma comment(lib,"SDL2.lib")

using namespace std;

/// <summary>
/// ��ʼ��SDL
/// <returns>�ɹ�����true
static bool InitVideo() 
{
    static bool is_first = true;
    static mutex mux;
    unique_lock<mutex> sdl_lock(mux);

    if (!is_first)return true;
    is_first = false;
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        cout << SDL_GetError << endl;
        return false;
    }
    //�趨�����㷨������������,���Բ�ֵ�㷨
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    return true;
}
bool XSDL::Init(int w, int h, Format fmt, void* win_id)
{
    if (w <= 0 || h <= 0)return false;
    //��ʼ������
    InitVideo();

    //�̰߳�ȫ
    unique_lock<mutex> sdl_lock(mtx_);
    width_ = w;
    height_ = h;
    fmt_ = fmt;
    
    //1 ��������
    if(!win_)
    {
        //�½�����
        if (!win_id)
        {
            win_ = SDL_CreateWindow("",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                w, h,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
            );
        }
        else
        {
            //��Ⱦ���ؼ�����
            win_ = SDL_CreateWindowFrom(win_id);
        }
    }
    if (!win_)
    {
        cerr << SDL_GetError << endl;
        return false;
    }

    //2 ������Ⱦ��
    render_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
    if (!render_)
    {
        cerr << SDL_GetError << endl;
        return false;
    }

    //3 �������ʣ��Դ棩Ĭ��RGBA8888
    unsigned int sdl_fmt = SDL_PIXELFORMAT_RGBA8888;
    switch (fmt)
    {
    case XVideoView::RGBA:
        break;
    case XVideoView::ARGB:
        sdl_fmt = SDL_PIXELFORMAT_ARGB32;
        break;
    case XVideoView::YUV420P:
        sdl_fmt = SDL_PIXELFORMAT_IYUV;
        break;
    default:
        break;
    }

    texture_ = SDL_CreateTexture(render_,
        sdl_fmt,                        //���ظ�ʽ
        SDL_TEXTUREACCESS_STREAMING,    //��ҪƵ���޸ĵ���Ⱦ��������
        w, h);                          //���ʴ�С
    if (!texture_) 
    {
        cerr << SDL_GetError << endl;
        return false;
    }


    return true;
}

bool XSDL::Draw(const unsigned char* data, int linesize)
{
    if (!data)return false;
    //�ݴ�
    unique_lock<mutex> sdl_lock(mtx_);
    if (!win_ || !render_ || !texture_ || width_ <= 0 || height_ <= 0)return false;

    // û�д�����Ч�� linesize,����ݵ�ǰ�����ظ�ʽ�Ϳ���Զ���������ʵ� linesize
    if (linesize <= 0)
    {
        switch (fmt_)
        {
        case XVideoView::RGBA:
        case XVideoView::ARGB:
            linesize = width_ * 4;
            break;
        case XVideoView::YUV420P:
            linesize = width_;
            break;
        default:
            break;
        }
    }
    if (linesize <= 0) return false;

    //���ڴ��е��������ݸ��µ�SDL������
    auto re = SDL_UpdateTexture(texture_, nullptr, data, linesize);
    if (re != 0) {
        cout << SDL_GetError << endl;
        return false;
    }

    //��յ�ǰ��ȾĿ�꣨ͨ���Ǵ��ڣ�
    re = SDL_RenderClear(render_);
    if (scale_h_ <= 0)scale_h_ = height_;
    if (scale_w_ <= 0)scale_w_ = width_;

    SDL_Rect rect;
    rect.w = scale_w_;//��Ⱦ�Ŀ�ߣ�������
    rect.h = scale_h_;
    rect.x = 0;
    rect.y = 0;

    //��һ������Texture�������ݸ��Ƶ���ǰ��ȾĿ�꣨Renderer��
    re = SDL_RenderCopy(render_, texture_,NULL, &rect);
    if (re != 0) {
        cout << SDL_GetError << endl;
        return false;
    }

    //��֮ǰ���е���Ⱦ�������� RenderCopy����ʾ����Ļ��
    SDL_RenderPresent(render_);
    
    return false;
}
