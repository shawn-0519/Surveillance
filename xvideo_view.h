#pragma once
#include <mutex>

////////////////////////////////////
/// 视频渲染接口类
/// 隐藏SDL实现
/// 渲染方案可替代
// 线程安全
class XVideoView
{
public:
	enum Format
	{
		RGBA = 0,
		ARGB,
		YUV420P
	};
	enum Rendertype
	{
		SDL = 0
	};
	static XVideoView* Create(Rendertype type = SDL);

	////////////////////////////////////
	/// <summary>
	/// 初始化渲染窗口  线程安全
	/// <param name="w">  窗口宽度
	/// <param name="h">  窗口高度
	/// <param name="fmt">  绘制的像素格式，默认RGBA
	/// <param name="win_id">  窗口句柄，如果为nullptr则创建新窗口
	/// <returns>  是否创建成功，true为成功，false为失败
	virtual bool Init(int w, int h, Format fmt = RGBA,void *win_id = nullptr) = 0;
	
	////////////////////////////////////
	/// <summary>
	/// 渲染图像  线程安全
	/// <param name="data">  渲染的二进制数据
	/// <param name="linsize">  一行数据的字节数，对于YUV420P就是Y一行字节数，linesize<=0 就根据宽度和像素格式自动算出大小
	/// <returns>  是否创建成功，true为成功，false为失败
	virtual bool Draw(const char*data,int linsize = 0) = 0;
	
	
	//XVideoView();
	//virtual ~XVideoView();

protected:
	int width_ = 0;     //窗口宽高
	int height_ = 0;
	Format fmt_ = RGBA;  //像素格式
	std::mutex mtx_;


};

