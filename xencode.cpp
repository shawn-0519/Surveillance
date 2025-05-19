#include "xencode.h"
#include <iostream>
using namespace std;
extern"C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
static void PrintErr(int err) 
{
	char buf[1024] = { 0 };
	av_strerror(err, buf, sizeof(buf) - 1);
	cerr << buf << endl;
}

AVCodecContext* xencode::Create(int codec_id)
{
	//1 找到编码器  AV_CODEC_ID_HEVC(H265)
	auto codec = avcodec_find_encoder((AVCodecID)codec_id);
	if (!codec) {
		cerr << "avcodec_find_encoder failed!" << codec_id << endl;
		return nullptr;
	}

	//2 编码上下文
	auto ctx = avcodec_alloc_context3(codec);
	if (!ctx) {
		cerr << "avcodec_alloc_context3 failed!" << codec_id << endl;
		return nullptr;
	}
	//设置参数默认值
	ctx->time_base = { 1,25 };
	ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	ctx->thread_count = 16;

	return ctx;
}

void xencode::set_c(AVCodecContext* ctx)
{
	unique_lock<mutex> lock(mux_);
	if (ctx)
	{
		avcodec_free_context(&ctx);
	}
	this->ctx_ = ctx;
}

bool xencode::SetOpt(const char* key, const char* val)
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

bool xencode::SetOpt(const char* key, int val)
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
