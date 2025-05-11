#pragma once
#include <mutex>
struct AVFrame;
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
	virtual bool Draw(const unsigned char*data,int linsize = 0) = 0;
	virtual bool Draw(
		const unsigned char* y, int y_pitch,
		const unsigned char* u, int u_pitch,
		const unsigned char* v, int v_pitch
			) = 0;
	
	virtual bool DrawFrame(AVFrame* frame);

	//清理所有申请的资源，包括关闭窗口
	virtual void Close() = 0;

	//处理窗口退出事件
	virtual bool IsExit() = 0;
	
	//显示缩放
	void Scale(int w, int h)
	{
		scale_w_ = w;
		scale_h_ = h;
	}

	//定时器
	void MSleep(unsigned int ms);

	int render_fps() { return render_fps_; }

protected:
	
	int render_fps_ = 0;       //显示帧率
	long long beg_ms_ = 0;       //计时开始时间
	int count_ = 0;              //统计显示次数

	int width_ = 0;     //材质宽高
	int height_ = 0;
	Format fmt_ = RGBA;  //像素格式
	std::mutex mtx_;    //确保线程安全
	int scale_w_ = 0;   //显示大小
	int scale_h_ = 0;


};

