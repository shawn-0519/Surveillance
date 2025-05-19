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
	//1 �ҵ�������  AV_CODEC_ID_HEVC(H265)
	auto codec = avcodec_find_encoder(codec_id);
	if (!codec) {
		cerr << "codec not find!" << endl;
		return -1;
	}

	//2 ����������
	auto ctx = avcodec_alloc_context3(codec);
	if (!ctx) {
		cerr << "avcodec_alloc_context3 failed!" << endl;
		return -1;
	}

	//3 �趨�����Ĳ���
	ctx->width = width;//��Ƶ���
	ctx->height = height;


	//֡ʱ�����ʱ�䵥λ  pts*time_base = ����ʱ�䣨�룩
	ctx->time_base = { 1,25 };//���� 1/25

	ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	ctx->thread_count = 16;	//�����߳���������ͨ������ϵͳ�ӿڻ�ȡcpu��������

	///4 �򿪱���������
	auto re = avcodec_open2(ctx, codec, NULL);
	if (re != 0) {
		//buf[1024] = { 0 };av_strerror ��Ѵ�����Ϣд�� buf�����Զ����ַ���ĩβ���� \0��
		// ����������������⡣ֻҪÿ�ε���ǰ buf û�б������ط�д����Ч���ݣ��Ͳ���Ӱ�������
		av_strerror(re, buf, sizeof(buf) - 1);
		cerr << "avcodec_open2 failed!" << buf << endl;
		return -1;
	}

	auto frame = av_frame_alloc();
	/// ������AVFrame�ռ� δѹ������
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

	//10s��Ƶ��250֡
	for (int i = 0; i < 250; i++)
	{
		//����AVFrame ���� ÿ֡���ݲ�ͬ
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
		frame->pts = i;//��ʾ��ʱ��

		//����δѹ��֡���߳���ѹ��
		re = avcodec_send_frame(ctx, frame);
		if (re != 0)
		{
			break;
		}

		//���ض�֡
		while (re >= 0)
		{
			//����ѹ��֡��һ��ǰ���ε��÷��ؿգ����壬���̷��أ�����δ��ɣ�
			//�������ڶ������߳���
			// ÿ�ε��û����·���pkt�еĿռ�
			re = avcodec_receive_packet(ctx, pkt);
			if (re == AVERROR(EAGAIN) || //��ǰû�п��õ��������
				re == AVERROR_EOF)		 //�������Ѿ���ˢ�£�flushed�����������и����������
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
	//�ͷ�frame/pkt
	av_packet_free(&pkt);
	av_frame_free(&frame);
	//�ͷű�����������
	avcodec_free_context(&ctx);

	return 0;
}