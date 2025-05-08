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
	// ��ʼ����Ⱦ����  �̰߳�ȫ
	// <param name="w">  ���ڿ��
	// <param name="h">  ���ڸ߶�
	// <param name="fmt">  ���Ƶ����ظ�ʽ��Ĭ��RGBA
	// <param name="win_id">  ���ھ�������Ϊnullptr�򴴽��´���
	// <returns>  �Ƿ񴴽��ɹ���trueΪ�ɹ���falseΪʧ��
	bool Init(int w, int h, Format fmt = RGBA, void* win_id = nullptr);

	////////////////////////////////////
	// <summary>
	// ��Ⱦͼ��  �̰߳�ȫ
	// <param name="data">  ��Ⱦ�Ķ���������
	// <param name="linesize">  һ�����ݵ��ֽ���������YUV420P����Yһ���ֽ�����linesize<=0 �͸��ݿ�Ⱥ����ظ�ʽ�Զ������С
	// <returns>  �Ƿ񴴽��ɹ���trueΪ�ɹ���falseΪʧ��
	bool Draw(const unsigned char* data, int linesize = 0);

protected:
	int width_ = 0;     //���ʿ��
	int height_ = 0;
	Format fmt_ = RGBA;  //���ظ�ʽ
	std::mutex mtx_;

	SDL_Window* win_;
	SDL_Renderer* render_;
	SDL_Texture* texture_;
};

