/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __T3D_IMGCONV_CLIENT_H__
#define __T3D_IMGCONV_CLIENT_H__


#include "T3DImgconvArgs.h"
#include "ImageConverter.pb.h"
#include <Network/T3DTCPConnection.h>
#include <Process/T3DProcess.h>


namespace Tiny3D
{
    /**
     * \brief 一次 imgconv 导入调用的结构化汇总结果
     */
    struct ImgconvBatchReport
    {
        Tools::MsgDone                  done {};
        TArray<Tools::MsgResult>        results {};
        TArray<Tools::MsgLog>           logs {};
    };

    using OnImgconvProgress = TFunction<void(const Tools::MsgProgress &)>;
    using OnImgconvLog      = TFunction<void(const Tools::MsgLog &)>;
    using OnImgconvResult   = TFunction<void(const Tools::MsgResult &)>;
    using OnImgconvDone     = TFunction<void(const ImgconvBatchReport &)>;

    /**
     * \brief imgconv IPC 客户端（封装监听、子进程拉起、Token 握手与请求收发）
     * \remarks 同一实例在生命周期内可复用同一个常驻 imgconv 子进程发起多次 importRequest
     */
    class ImgconvClient : public Allocator, public Noncopyable
    {
    public:
        ImgconvClient();
        ~ImgconvClient() override;

        /**
         * \brief 启动本地 TCPListener，拉起 imgconv 子进程并完成 Token 握手（若已就绪则直接返回 T3D_OK）
         * \param [in] exePath : imgconv 可执行文件路径（为空时默认按 Dir::getAppPath() 下查找 imgconv）
         * \param [in] handshakeTimeoutMs : 握手超时毫秒数
         * \return 成功返回 T3D_OK
         */
        TResult ensureStarted(const String &exePath = "", uint32_t handshakeTimeoutMs = 10000);

        /**
         * \brief 同步阻塞执行一批导入（内部轮询 TCPListener::poll / TCPConnection::poll 直至收到 MsgDone 或超时/断连）
         * \param [in] args : 参数构造器
         * \param [out] outReport : 汇总输出
         * \param [in] onProgress : 可选进度回调
         * \param [in] onLog : 可选日志回调
         * \param [in] timeoutMs : 单批超时毫秒数
         * \return 成功返回 outReport.done.result()，通信/超时失败返回错误码
         */
        TResult importSync(
            const ImgconvArgs &args,
            ImgconvBatchReport &outReport,
            const OnImgconvProgress &onProgress = nullptr,
            const OnImgconvLog &onLog = nullptr,
            uint32_t timeoutMs = 120000);

        /**
         * \brief 异步发起导入请求（供已有主循环的调用方如 TinyEditor 使用，调用方在主循环调用 poll()）
         * \param [in] args : 参数构造器
         * \param [in] onDone : 完成回调
         * \param [in] onProgress : 可选进度回调
         * \param [in] onLog : 可选日志回调
         * \param [in] onResult : 可选单张结果回调
         * \return 成功入队/发送返回 T3D_OK
         */
        TResult importAsync(
            const ImgconvArgs &args,
            const OnImgconvDone &onDone,
            const OnImgconvProgress &onProgress = nullptr,
            const OnImgconvLog &onLog = nullptr,
            const OnImgconvResult &onResult = nullptr);

        /**
         * \brief 轮询网络事件（异步模式下由外部主循环定期调用）
         */
        void poll();

        /**
         * \brief 发送 ReqShutdown 并关闭连接、等待子进程退出
         */
        void shutdown();

        /**
         * \brief 当前是否已完成握手且连接有效
         */
        bool isReady() const { return mHandshaked && mConnection != nullptr && mConnection->isConnected(); }

        /**
         * \brief 解析默认 imgconv 可执行文件路径
         */
        static String resolveDefaultExePath();

    protected:
        void onAccepted(TCPListener *listener, TCPConnection *connection);
        void onRecv(TCPConnection *connection, uint32_t seq, const void *data, int32_t dataSize);
        void onDisconnected(TCPConnection *connection, TResult result);
        TResult sendMessage(uint32_t seq, const Tools::ImgconvRequestBody &req);

    protected:
        struct PendingRequest
        {
            uint32_t            seq {0};
            ImgconvBatchReport  report {};
            OnImgconvProgress   onProgress {nullptr};
            OnImgconvLog        onLog {nullptr};
            OnImgconvResult     onResult {nullptr};
            OnImgconvDone       onDone {nullptr};
            bool                completed {false};
        };

        TCPListener                        *mListener {nullptr};
        TCPConnection                      *mConnection {nullptr};
        Process                            *mProcess {nullptr};
        String                              mExePath {};
        String                              mToken {};
        uint32_t                            mNextSeq {1};
        bool                                mHandshaked {false};
        bool                                mDisconnected {false};
        TUnorderedMap<uint32_t, PendingRequest> mPendingRequests {};
        TArray<uint8_t>                     mSendScratch {};
    };
}


#endif  /*__T3D_IMGCONV_CLIENT_H__*/
