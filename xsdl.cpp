#include "xsdl.h"
#include <SDL.h>
#include <iostream>
#pragma comment(lib,"SDL2.lib")

using namespace std;

/// <summary>
/// 初始化SDL
/// <returns>成功返回true
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
    //设定缩放算法，解决锯齿问题,线性插值算法
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    return true;
}
bool XSDL::Init(int w, int h, Format fmt, void* win_id)
{
    if (w <= 0 || h <= 0)return false;
    //初始化窗口
    InitVideo();

    //线程安全
    unique_lock<mutex> sdl_lock(mtx_);
    width_ = w;
    height_ = h;
    fmt_ = fmt;
    
    if (texture_)
        SDL_DestroyTexture(texture_);
    if (render_)
        SDL_DestroyRenderer(render_);

    //1 创建窗口
    if(!win_)
    {
        //新建窗口
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
            //渲染到控件窗口
            win_ = SDL_CreateWindowFrom(win_id);
        }
    }
    if (!win_)
    {
        cerr << SDL_GetError << endl;
        return false;
    }

    //2 创建渲染器
    render_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
    if (!render_)
    {
        cout << SDL_GetError << endl;
        return false;
    }

    //3 创建材质（显存）默认RGBA8888
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
        sdl_fmt,                        //像素格式
        SDL_TEXTUREACCESS_STREAMING,    //需要频繁修改的渲染（带锁）
        w, h);                          //材质大小
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
    //容错
    unique_lock<mutex> sdl_lock(mtx_);
    if (!win_ || !render_ || !texture_ || width_ <= 0 || height_ <= 0)return false;

    // 没有传递有效的 linesize,则根据当前的像素格式和宽度自动计算出合适的 linesize
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

    //将内存中的像素数据更新到SDL的纹理
    auto re = SDL_UpdateTexture(texture_, nullptr, data, linesize);
    if (re != 0) {
        cout << SDL_GetError << endl;
        return false;
    }

    //清空当前渲染目标（通常是窗口）
    re = SDL_RenderClear(render_);

    SDL_Rect rect;
    SDL_Rect* prect = nullptr;
    if (scale_w_ > 0) //用户手动设置缩放
    {
        rect.x = 0; rect.y = 0;
        rect.w = scale_w_;//渲染的宽高，可缩放
        rect.h = scale_h_;
        prect = &rect;
    }    

    //将一个纹理（Texture）的内容复制到当前渲染目标（Renderer）
    re = SDL_RenderCopy(render_, texture_,NULL, prect);
    if (re != 0) {
        cout << SDL_GetError << endl;
        return false;
    }

    //将之前所有的渲染操作（如 RenderCopy）显示到屏幕上
    SDL_RenderPresent(render_);
    
    return false;
}

bool XSDL::Draw(const unsigned char* y, int y_pitch, const unsigned char* u, int u_pitch, const unsigned char* v, int v_pitch)
{

    if (!y||!u||!v)return false;
    //容错
    unique_lock<mutex> sdl_lock(mtx_);
    if (!win_ || !render_ || !texture_ || width_ <= 0 || height_ <= 0)return false;

    
   

    //将内存中的像素数据更新到SDL的纹理
    auto re = SDL_UpdateYUVTexture(texture_, NULL, y, y_pitch, u, u_pitch, v, v_pitch);
    if (re != 0) {
        cout << SDL_GetError << endl;
        return false;
    }

    //清空当前渲染目标（通常是窗口）
    re = SDL_RenderClear(render_);

    SDL_Rect rect;
    SDL_Rect* prect = nullptr;
    if (scale_w_ > 0) //用户手动设置缩放
    {
        rect.x = 0; rect.y = 0;
        rect.w = scale_w_;//渲染的宽高，可缩放
        rect.h = scale_h_;
        prect = &rect;
    }

    //将一个纹理（Texture）的内容复制到当前渲染目标（Renderer）
    re = SDL_RenderCopy(render_, texture_, NULL, prect);
    if (re != 0) {
        cout << SDL_GetError << endl;
        return false;
    }

    //将之前所有的渲染操作（如 RenderCopy）显示到屏幕上
    SDL_RenderPresent(render_);

    return true;
}

void XSDL::Close()
{
    //确保线程安全
    unique_lock<mutex> sdl_lock(mtx_);
    if (texture_)
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (render_)
    {
        SDL_DestroyRenderer(render_);
        render_ = nullptr;
    }
    if (win_)
    {
        SDL_DestroyWindow(win_);
        win_ = nullptr;
    }

}

bool XSDL::IsExit()
{
    //SDL_Event ev;
    ////等待事件队列中出现新事件，并将事件信息写入 ev
    //SDL_WaitEventTimeout(&ev, 1);
    //if (ev.type == SDL_QUIT) {
    //    return true;
    //}
    return false;
}
