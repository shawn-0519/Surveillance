#pragma once
#include <mutex>

////////////////////////////////////
/// ��Ƶ��Ⱦ�ӿ���
/// ����SDLʵ��
/// ��Ⱦ���������
// �̰߳�ȫ
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
	/// ��ʼ����Ⱦ����  �̰߳�ȫ
	/// <param name="w">  ���ڿ��
	/// <param name="h">  ���ڸ߶�
	/// <param name="fmt">  ���Ƶ����ظ�ʽ��Ĭ��RGBA
	/// <param name="win_id">  ���ھ�������Ϊnullptr�򴴽��´���
	/// <returns>  �Ƿ񴴽��ɹ���trueΪ�ɹ���falseΪʧ��
	virtual bool Init(int w, int h, Format fmt = RGBA,void *win_id = nullptr) = 0;
	
	////////////////////////////////////
	/// <summary>
	/// ��Ⱦͼ��  �̰߳�ȫ
	/// <param name="data">  ��Ⱦ�Ķ���������
	/// <param name="linsize">  һ�����ݵ��ֽ���������YUV420P����Yһ���ֽ�����linesize<=0 �͸��ݿ�Ⱥ����ظ�ʽ�Զ������С
	/// <returns>  �Ƿ񴴽��ɹ���trueΪ�ɹ���falseΪʧ��
	virtual bool Draw(const char*data,int linsize = 0) = 0;
	
	
	//XVideoView();
	//virtual ~XVideoView();

protected:
	int width_ = 0;     //���ڿ��
	int height_ = 0;
	Format fmt_ = RGBA;  //���ظ�ʽ
	std::mutex mtx_;


};

