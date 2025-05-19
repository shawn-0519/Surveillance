#pragma once
#include <mutex>
struct AVCodecContext;

class xencode
{
   //////////////////////////////////////////
   /// ��������������
   /// 
   /// @para codec_id ������ID�ţ���Ӧffmpeg
   /// @return ���������� ,ʧ�ܷ���nullptr
    static AVCodecContext* Create(int codec_id);

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

