#include "xencode.h"
#include <iostream>
using namespace std;
extern"C"{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

//��ӡ������
static void PrintErr(int err) 
{
	char buf[1024] = { 0 };
	av_strerror(err, buf, sizeof(buf) - 1);
	cerr << buf << endl;
}

AVCodecContext* XEncode::Create(int codec_id)
{
	//1 �ҵ�������  AV_CODEC_ID_HEVC(H265)
	auto codec = avcodec_find_encoder((AVCodecID)codec_id);
	if (!codec) {
		cerr << "avcodec_find_encoder failed!" << codec_id << endl;
		return nullptr;
	}

	//2 ����������
	auto ctx = avcodec_alloc_context3(codec);
	if (!ctx) {
		cerr << "avcodec_alloc_context3 failed!" << codec_id << endl;
		return nullptr;
	}
	//���ò���Ĭ��ֵ
	ctx->time_base = { 1,25 };
	ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	ctx->thread_count = 16;

	return ctx;
}

bool XEncode::Open()
{
	unique_lock<mutex>lock(mux_);
	if (!ctx_)return false;
	///4 �򿪱���������
	auto re = avcodec_open2(ctx_, NULL, NULL);
	if (re != 0) {
		PrintErr(re);
		return false;
	}
	return true;
}

AVFrame* XEncode::CreateFrame()
{
	unique_lock<mutex>lock(mux_);
	if (!ctx_)return nullptr;

	auto frame = av_frame_alloc();
	/// ������AVFrame�ռ� δѹ������
	frame->width = ctx_->width;
	frame->height = ctx_->height;
	frame->format = ctx_->pix_fmt;
	auto re = av_frame_get_buffer(frame, 0);
	if (re != 0)
	{
		av_frame_free(&frame);
		PrintErr(re);
		return nullptr;
	}
	return frame;
}

AVPacket* XEncode::Encode(const AVFrame* frame)
{
	unique_lock<mutex>lock(mux_);
	if (!ctx_)return nullptr;

	auto re = avcodec_send_frame(ctx_, frame);
	if (re != 0)return nullptr;

	AVPacket* pkt = av_packet_alloc();

	//ѭ���������п��õİ�
	//while (re >= 0) {
	//	re = avcodec_receive_packet(enc_ctx, pkt);
	//	if (re == AVERROR(EAGAIN) || re == AVERROR_EOF)
	//		break;
	//	// дpkt���ļ�����
	//	av_packet_unref(pkt);
	//}

	re = avcodec_receive_packet(ctx_, pkt);
	if (re == 0) 
	{
		return pkt;
	}
	av_packet_free(&pkt);
	if (re == AVERROR(EAGAIN) || re == AVERROR_EOF)
	{
		return nullptr;
	}
	if (re < 0)
	{
		PrintErr(re);
	}

	return nullptr;
}

vector<AVPacket*> XEncode::End()
{
	vector<AVPacket*> res;
	unique_lock<mutex>lock(mux_);
	if (!ctx_)return res;

	// ����NULL֡��֪ͨ���������������������ʣ������ݰ�
	auto re = avcodec_send_frame(ctx_, NULL);
	if (re != 0)return res;

	// ѭ����ȡ���л������е�AVPacket
	while (re >= 0)
	{
		AVPacket* pkt = av_packet_alloc();
		re = avcodec_receive_packet(ctx_, pkt);
		if (re != 0)
		{
			av_packet_free(&pkt);
			break;
		}
		res.push_back(pkt);
	}
	return res;
}



void XEncode::set_c(AVCodecContext* ctx)
{
	unique_lock<mutex> lock(mux_);
	if (ctx)
	{
		avcodec_free_context(&ctx);
	}
	this->ctx_ = ctx;
}

bool XEncode::SetOpt(const char* key, const char* val)
{
	unique_lock<mutex> lock(mux_);
	if (!ctx_)return false;
	auto re = av_opt_set(ctx_->priv_data, key, val, 0);
	if (re != 0)
	{
		cerr << "av_opt_set failure";
		PrintErr(re);
	}
	return true;
}

bool XEncode::SetOpt(const char* key, int val)
{
	unique_lock<mutex> lock(mux_);
	if (!ctx_)return false;
	auto re = av_opt_set_int(ctx_->priv_data, key, val, 0);
	if (re != 0)
	{
		cerr << "av_opt_set failure";
		PrintErr(re);
	}
	return true;
}
