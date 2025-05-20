#include <iostream>
#include <fstream>
#include "xencode.h"
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

	XEncode encode;
	auto ctx = encode.Create(codec_id);
	ctx->width = 400;
	ctx->height = 300;
	encode.set_c(ctx);
	encode.SetOpt("crf", 18);
	encode.Open();
	AVFrame *frame = encode.CreateFrame();
	
	
	int count = 0;//写入文件的帧数 SPS PPS IDR放在一帧中
	//500帧
	for (int i = 0; i < 500; i++)
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
		auto pkt = encode.Encode(frame);
		if (pkt)
		{
			count++;
			ofs.write((char*)pkt->data, pkt->size);
			av_packet_free(&pkt);
		}
	}
	//解决丢帧问题：
	auto pkts = encode.End();
	for (auto pkt : pkts)
	{
		count++;
		ofs.write((char*)pkt->data, pkt->size);
		av_packet_free(&pkt);
	}


	ofs.close();
	encode.set_c(nullptr);
	cout << "encode " << count << endl;

	return 0;
}