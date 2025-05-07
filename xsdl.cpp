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
        cerr << SDL_GetError << endl;
        return false;
    }

    //3 创建材质（显存）
    unsigned int sdl_fmt = SDL_PIXELFORMAT_RGBA8888;
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

bool XSDL::Draw(const char* data, int linsize)
{
    return false;
}
