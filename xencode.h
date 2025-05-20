#pragma once
#include <mutex>
#include <vector>
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
class XEncode
{
public:

   //////////////////////////////////////////
   /// ��������������
   /// 
   /// @para codec_id ������ID�ţ���Ӧffmpeg
   /// @return ���������� ,ʧ�ܷ���nullptr
    static AVCodecContext* Create(int codec_id);


    //////////////////////////////////////////
    /// �򿪱�����
    bool Open();

    ///////////////////////////////////////////////////////////////
    //����AVCodecContext ����һ��AVFrame����Ҫ�������ͷ�av_frame_free
    // @return frame 
    AVFrame* CreateFrame();

    ///////////////////////////////////////////////////////////////
    /// �������� ÿ���´���AVPacket
    /// @para frame �ռ����û�ά��
    /// @return ʧ�ܷ�Χnullptr ���ص�AVPacket�û���Ҫͨ��av_packet_free ����
    AVPacket* Encode(const AVFrame *frame);

    //////////////////////////////////////////////////////////////
    //�������б��뻺����AVPacket
    std::vector<AVPacket*> End();

    //////////////////////////////////////////
    /// ���ö���ı����������� �����Ĵ��ݵ������У���Դ��XEncodeά��
    /// 
    /// @para c ������������ ���c_��Ϊnullptr������������Դ
    void set_c(AVCodecContext* ctx);

    /////////////////////////////////////////////
    /// ���ñ���������̰߳�ȫ
    /// @param key ������
    /// @param val ����ֵ
    /// @return ���óɹ�����true��ʧ�ܷ���false
    bool SetOpt(const char* key,const char* val);
    bool SetOpt(const char* key,int val);


private:
    AVCodecContext* ctx_;   //������������
    std::mutex mux_;        //��������������
};

