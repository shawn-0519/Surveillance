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
   /// 创建编码上下文
   /// 
   /// @para codec_id 编码器ID号，对应ffmpeg
   /// @return 编码上下文 ,失败返回nullptr
    static AVCodecContext* Create(int codec_id);


    //////////////////////////////////////////
    /// 打开编码器
    bool Open();

    ///////////////////////////////////////////////////////////////
    //根据AVCodecContext 创建一个AVFrame，需要调用者释放av_frame_free
    // @return frame 
    AVFrame* CreateFrame();

    ///////////////////////////////////////////////////////////////
    /// 编码数据 每次新创建AVPacket
    /// @para frame 空间由用户维护
    /// @return 失败范围nullptr 返回的AVPacket用户需要通过av_packet_free 清理
    AVPacket* Encode(const AVFrame *frame);

    //////////////////////////////////////////////////////////////
    //返回所有编码缓存中AVPacket
    std::vector<AVPacket*> End();

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

