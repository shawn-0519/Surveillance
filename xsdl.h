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
	bool Init(int w, int h, Format fmt = RGBA)override;

	////////////////////////////////////
	// <summary>
	// ��Ⱦͼ��  �̰߳�ȫ
	// <param name="data">  ��Ⱦ�Ķ���������
	// <param name="linesize">  һ�����ݵ��ֽ���������YUV420P����Yһ���ֽ�����linesize<=0 �͸��ݿ�Ⱥ����ظ�ʽ�Զ������С
	// <returns>  �Ƿ񴴽��ɹ���trueΪ�ɹ���falseΪʧ��
	bool Draw(const unsigned char* data, int linesize = 0)override;
	bool Draw(
		const unsigned char* y, int y_pitch,
		const unsigned char* u, int u_pitch,
		const unsigned char* v, int v_pitch
	)override;
	//���������������Դ�������رմ���
	void Close()override;

	//�������˳��¼�
	bool IsExit()override;

private:
	SDL_Window* win_ = nullptr;
	SDL_Renderer* render_ = nullptr;
	SDL_Texture* texture_ = nullptr;
};

