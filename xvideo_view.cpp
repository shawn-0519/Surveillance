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
	case AV_PIX_FMT_ARGB:
		return Draw(frame->data[0], frame->linesize[0]);
	default:
		break;
	}

	return true;
}

void XVideoView::MSleep(unsigned int ms)  
{  
   auto beg = clock();  
   for (unsigned int i = 0; i < ms; i++)
   {  
       this_thread::sleep_for(chrono::milliseconds(1));
       if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
           break;  
   }  
}
