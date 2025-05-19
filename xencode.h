#pragma once
#include <mutex>
struct AVCodecContext;

class xencode
{
   //////////////////////////////////////////
   /// 创建编码上下文
   /// 
   /// @para codec_id 编码器ID号，对应ffmpeg
   /// @return 编码上下文 ,失败返回nullptr
    static AVCodecContext* Create(int codec_id);

    //////////////////////////////////////////
    /// 设置对象的编码器上下文 上下文传递到对象中，资源由XEncode维护
    /// 
    /// @para c 编码器上下文 如果c_不为nullptr，则先清理资源
    void set_c(AVCodecContext* ctx);

    /////////////////////////////////////////////
    /// 设置编码参数，线程安全
    /// @param key 参数名
    /// @param val 参数值
    /// @return 设置成功返回true，失败返回false
    bool SetOpt(const char* key,const char* val);
    bool SetOpt(const char* key,int val);


private:
    AVCodecContext* ctx_;   //编码器上下文
    std::mutex mux_;        //编码器上下文锁
};

