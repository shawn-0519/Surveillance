#include <iostream>
#include <fstream>
using namespace std;
extern "C" { 
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")

static int width = 400;
static int height = 400;

int learn(int argc, char* argv[])
{
	string filename = "400_300_25";
	AVCodecID codec_id = AV_CODEC_ID_H264;
	if(argc>1)
	{
		string codec = argv[1];
		if (codec == "h265" || codec == "hevc")
		{
			codec_id = AV_CODEC_ID_HEVC;
		}
	}

	if (codec_id == AV_CODEC_ID_HEVC)
	{
		filename += ".h265";
	}
	else if (codec_id == AV_CODEC_ID_H264)
	{
		filename += ".h264";
	}
	ofstream ofs;
	ofs.open(filename, ios::binary);

	char buf[1024] = { 0 };
	//1 找到编码器  AV_CODEC_ID_HEVC(H265)
	auto codec = avcodec_find_encoder(codec_id);
	if (!codec) {
		cerr << "codec not find!" << endl;
		return -1;
	}

	//2 编码上下文
	auto ctx = avcodec_alloc_context3(codec);
	if (!ctx) {
		cerr << "avcodec_alloc_context3 failed!" << endl;
		return -1;
	}

	//3 设定上下文参数
	ctx->width = width;//视频宽高
	ctx->height = height;


	//帧时间戳的时间单位  pts*time_base = 播放时间（秒）
	ctx->time_base = { 1,25 };//分数 1/25

	ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	ctx->thread_count = 16;	//编码线程数，可以通过调用系统接口获取cpu核心数量

	///4 打开编码上下文
	auto re = avcodec_open2(ctx, codec, NULL);
	if (re != 0) {
		//buf[1024] = { 0 };av_strerror 会把错误信息写入 buf，并自动在字符串末尾加上 \0，
		// 不会出现脏数据问题。只要每次调用前 buf 没有被其他地方写入无效内容，就不会影响输出。
		av_strerror(re, buf, sizeof(buf) - 1);
		cerr << "avcodec_open2 failed!" << buf << endl;
		return -1;
	}

	auto frame = av_frame_alloc();
	/// 创建好AVFrame空间 未压缩数据
	frame->width = ctx->width;
	frame->height = ctx->height;
	frame->format = ctx->pix_fmt;
	re = av_frame_get_buffer(frame, 0);
	if (re != 0)
	{
		av_strerror(re, buf, sizeof(buf) - 1);
		cerr << "avcodec_open2 failed!" << buf << endl;
		return -1;
	}

	auto pkt = av_packet_alloc();

	//10s视频，250帧
	for (int i = 0; i < 250; i++)
	{
		//生成AVFrame 数据 每帧数据不同
		//Y
		for (int y = 0; y < ctx->height; y++)
		{
			for (int x = 0; x < ctx->width; x++)
			{
				frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
			}
		}
		//UV
		for (int y = 0; y < ctx->height / 2; y++)
		{
			for (int x = 0; x < ctx->width / 2; x++)
			{
				frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
				frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
			}
		}
		frame->pts = i;//显示的时间

		//发送未压缩帧到线程中压缩
		re = avcodec_send_frame(ctx, frame);
		if (re != 0)
		{
			break;
		}

		//返回多帧
		while (re >= 0)
		{
			//接收压缩帧，一遍前几次调用返回空（缓冲，立刻返回，编码未完成）
			//编码是在独立的线程中
			// 每次调用会重新分配pkt中的空间
			re = avcodec_receive_packet(ctx, pkt);
			if (re == AVERROR(EAGAIN) || //当前没有可用的输出数据
				re == AVERROR_EOF)		 //编码器已经被刷新（flushed），不会再有更多输出数据
				break;
			if (re != 0)
			{
				av_strerror(re, buf, sizeof(buf) - 1);
				cerr << "avcodec_receive_packet failed!" << buf << endl;
				break;
			}
			cout << pkt->size << " " << flush;
			ofs.write((char*)pkt->data, pkt->size);
			av_packet_unref(pkt);
		}

	}
	ofs.close();
	//释放frame/pkt
	av_packet_free(&pkt);
	av_frame_free(&frame);
	//释放编码器上下文
	avcodec_free_context(&ctx);

	return 0;
}