#pragma once
#include <mutex>
#include <fstream>
struct AVFrame;

//��ʱ��
void MSleep(unsigned int ms);

////////////////////////////////////
/// ��Ƶ��Ⱦ�ӿ���
/// ����SDLʵ��
/// ��Ⱦ���������
// �̰߳�ȫ
class XVideoView
{
public:
	enum Format //ö�ٵ�ֵ��ffmpeg��һ��
	{
		YUV420P = 0,
		ARGB = 25,
		RGBA = 26,
		BGRA = 28
	};
	enum Rendertype
	{
		SDL = 0
	};

	//ʵ�֡�����ģʽ����������� OpenGL��DirectX ����Ⱦ����
	static XVideoView* Create(Rendertype type = SDL);

	////////////////////////////////////
	/// <summary>
	/// ��ʼ����Ⱦ����  �̰߳�ȫ
	/// <param name="w">  ���ڿ��
	/// <param name="h">  ���ڸ߶�
	/// <param name="fmt">  ���Ƶ����ظ�ʽ��Ĭ��RGBA
	/// <param name="win_id">  ���ھ�������Ϊnullptr�򴴽��´���
	/// <returns>  �Ƿ񴴽��ɹ���trueΪ�ɹ���falseΪʧ��
	virtual bool Init(int w, int h, Format fmt = RGBA) = 0;
	
	////////////////////////////////////
	/// <summary>
	/// ��Ⱦͼ��  �̰߳�ȫ
	/// <param name="data">  ��Ⱦ�Ķ���������
	/// <param name="linsize">  һ�����ݵ��ֽ���������YUV420P����Yһ���ֽ�����linesize<=0 �͸��ݿ�Ⱥ����ظ�ʽ�Զ������С
	/// <returns>  �Ƿ񴴽��ɹ���trueΪ�ɹ���falseΪʧ��
	virtual bool Draw(const unsigned char*data,int linsize = 0) = 0;
	virtual bool Draw(
		const unsigned char* y, int y_pitch,
		const unsigned char* u, int u_pitch,
		const unsigned char* v, int v_pitch
			) = 0;
	
	//���ݸ�ʽ��Ⱦ
	bool DrawFrame(AVFrame* frame);

	int render_fps() { return render_fps_; }

	//���ļ�
	bool Open(std::string filepath);

	//���������������Դ�������رմ���
	virtual void Close() = 0;

	//�������˳��¼�
	virtual bool IsExit() = 0;
	
	//��ʾ����
	void Scale(int w, int h)
	{
		scale_w_ = w;
		scale_h_ = h;
	}

	AVFrame* Read();

	void set_win_id(void* win) { win_id_ = win; }

protected:
	void* win_id_ = nullptr;
	int render_fps_ = 0;       //��ʾ֡��
	long long beg_ms_ = 0;       //��ʱ��ʼʱ��
	int count_ = 0;              //ͳ����ʾ����

	int width_ = 0;     //���ʿ��
	int height_ = 0;
	Format fmt_ = RGBA;  //���ظ�ʽ
	std::mutex mtx_;    //ȷ���̰߳�ȫ
	int scale_w_ = 0;   //��ʾ��С
	int scale_h_ = 0;

private:
	std::ifstream ifs_;
	AVFrame* frame_ = nullptr;//AVFrame�Ŀռ���ʱ����˽������ȷ���ڲ�ȷ���Ƿ���ⲿʹ�õ�����°�ȫ�ͷš�

};

