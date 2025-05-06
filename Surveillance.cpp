#include "Surveillance.h"
#include <QMessageBox>
#include <SDL.h>
#pragma comment(lib,"SDL2.lib")
static int sdl_width = 0;
static int sdl_height = 0;
static SDL_Window *sdl_win = nullptr;
static SDL_Renderer* sdl_render = nullptr;
static SDL_Texture* sdl_texture = nullptr;
static unsigned char* rgb = NULL;
static int pix_size = 4;
void Surveillance::timerEvent(QTimerEvent* ev)
{
    static unsigned char tmp = 255;
    tmp--;
    for (int j = 0; j < sdl_height; j++)
    {
        int b = j * sdl_width * pix_size;
        for (int i = 0; i < sdl_width * pix_size; i += pix_size)
        {
            rgb[b + i] = 0;         //B
            rgb[b + i + 1] = tmp;   //G
            rgb[b + i + 2] = 0;     //R
            rgb[b + i + 3] = 0;     //A
        }
    }
    //�� rgb ָ����������ݣ�����ÿ�еĲ�����ȫ�����µ� sdl_texture �����������
    SDL_UpdateTexture(sdl_texture, nullptr, rgb, pix_size * sdl_width);
    //�����ǰ��ȾĿ���ϵ�����
    SDL_RenderClear(sdl_render);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = sdl_width;
    rect.h = sdl_height;
    //�� sdl_texture ������ݣ����� rect ָ����λ�úʹ�С����Ⱦ�� sdl_render �����Ĵ����ϡ�
    SDL_RenderCopy(sdl_render, sdl_texture, nullptr, &rect);
    SDL_RenderPresent(sdl_render);
}


Surveillance::Surveillance(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    sdl_width = ui.label->width();
    sdl_height = ui.label->height();
    //Init
    SDL_Init(SDL_INIT_VIDEO);
    //create window
    sdl_win = SDL_CreateWindowFrom((void*)ui.label->winId());
    //create render
    sdl_render = SDL_CreateRenderer(sdl_win, -1, SDL_RENDERER_ACCELERATED);
    //create Texture 
    sdl_texture = SDL_CreateTexture(
        sdl_render, 
        SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_STREAMING, 
        sdl_width, 
        sdl_height
    );
    rgb = new unsigned char[sdl_height * sdl_width * pix_size];
    startTimer(10);
}

Surveillance::~Surveillance()
{}

