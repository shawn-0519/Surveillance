#include <thread>
#include <iostream>
#include "xvideo_view.h"
#include "xsdl.h"
extern"C" {
#include<libavcodec/avcodec.h>
}
#pragma comment(lib,"avutil.lib")
using namespace std;
XVideoView* XVideoView::Create(Rendertype type)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}

	return nullptr;
}

bool XVideoView::DrawFrame(AVFrame* frame)
{
	//容错
	if (!frame || !frame->data[0]) {
		return false;
	}
	count_++;
	if (beg_ms_ <= 0)
	{
		beg_ms_ = clock();
	}
	//计算显示帧率
	else if ((clock() - beg_ms_) / (CLOCKS_PER_SEC / 1000) >= 1000) //一秒计算一次fps
	{
		render_fps_ = count_;
		std::cout << "fps:" << count_<<"  ";
		count_ = 0;
		beg_ms_ = clock();
	}

	switch (frame->format)
	{
	case AV_PIX_FMT_YUV420P:
		return Draw(
			frame->data[0], frame->linesize[0],
			frame->data[1], frame->linesize[1],
			frame->data[2], frame->linesize[2]
			);
	case AV_PIX_FMT_BGRA:
	case AV_PIX_FMT_ARGB:
	case AV_PIX_FMT_RGBA:
		return Draw(frame->data[0], frame->linesize[0]);
	default:
		break;
	}

	return false;
}

bool XVideoView::Open(std::string filepath)
{	
	//确保流处于“未打开”状态
	if (ifs_)
	{
		ifs_.close();
	}
	ifs_.open(filepath, ios::binary);
	return ifs_.is_open();

}

AVFrame* XVideoView::Read()
{
	if (width_ <= 0 || height_ <= 0 || !ifs_)return NULL;
	//AVFrame空间已经申请，如果参数发生变化，需要释放空间
	if (frame_)
	{
		if (frame_->width != width_
			|| frame_->height != height_
			|| frame_->format != fmt_)
		{
			av_frame_free(&frame_);//释放AVFrame对象空间，和buf引用计数减一
		}
	}
	if (!frame_)
	{
		//分配对象空间
		frame_ = av_frame_alloc();
		frame_->width = width_;
		frame_->height = height_;
		frame_->format = fmt_;
		frame_->linesize[0] = width_ * 4;//默认格式是RGBA每个像素4字节
		if (frame_->format == AV_PIX_FMT_YUV420P)
		{
			frame_->linesize[0] = width_;
			frame_->linesize[1] = width_/2;
			frame_->linesize[2] = width_/2;
		}
		//生成AVFrame空间，使用默认对齐
		auto re = av_frame_get_buffer(frame_, 0);
		if (re != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(re, buf, sizeof(buf) - 1);
			cout << buf << endl;
			av_frame_free(&frame_);
			return NULL;
		}
	}

	if (!frame_)return NULL;

	//读一帧数据
	if (frame_->format == AV_PIX_FMT_YUV420P)
	{
		ifs_.read((char*)frame_->data[0], frame_->linesize[0] * height_);	//Y
		ifs_.read((char*)frame_->data[1], frame_->linesize[1] * height_/2);	//U
		ifs_.read((char*)frame_->data[2], frame_->linesize[2] * height_/2);	//V
	}
	else //RGBA ARGB BGRA 32
	{
		ifs_.read((char*)frame_->data[0], frame_->linesize[0] * height_);
	}

	//读完了
	if (ifs_.gcount() == 0)return NULL;

	return frame_;
}

void MSleep(unsigned int ms)  
{  
   auto beg = clock();  
   for (unsigned int i = 0; i < ms; i++)
   {  
       this_thread::sleep_for(chrono::milliseconds(1));
       if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
           break;  
   }  
}

long long NowMs() {
	return clock() / (CLOCKS_PER_SEC / 1000);
}