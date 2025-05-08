#pragma once
#include "xvideo_view.h"
#include <mutex>
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class XSDL:public XVideoView
{
public:
	////////////////////////////////////
	// <summary>
	// 初始化渲染窗口  线程安全
	// <param name="w">  窗口宽度
	// <param name="h">  窗口高度
	// <param name="fmt">  绘制的像素格式，默认RGBA
	// <param name="win_id">  窗口句柄，如果为nullptr则创建新窗口
	// <returns>  是否创建成功，true为成功，false为失败
	bool Init(int w, int h, Format fmt = RGBA, void* win_id = nullptr)override;

	////////////////////////////////////
	// <summary>
	// 渲染图像  线程安全
	// <param name="data">  渲染的二进制数据
	// <param name="linesize">  一行数据的字节数，对于YUV420P就是Y一行字节数，linesize<=0 就根据宽度和像素格式自动算出大小
	// <returns>  是否创建成功，true为成功，false为失败
	bool Draw(const unsigned char* data, int linesize = 0)override;

	//清理所有申请的资源，包括关闭窗口
	void Close()override;

	//处理窗口退出事件
	void IsExit()override;

private:
	SDL_Window* win_ = nullptr;
	SDL_Renderer* render_ = nullptr;
	SDL_Texture* texture_ = nullptr;
};

