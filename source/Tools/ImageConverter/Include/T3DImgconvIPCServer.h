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

#ifndef __T3D_IMGCONV_IPC_SERVER_H__
#define __T3D_IMGCONV_IPC_SERVER_H__


#include "T3DImageConverterPrerequisites.h"


namespace Tiny3D
{
    /**
     * \brief imgconv 端 IPC 服务：主动连接调用方监听的 127.0.0.1:<port>，处理 ReqImport 与 ReqShutdown
     */
    class ImgconvIPCServer : public Allocator
    {
    public:
        ImgconvIPCServer(ImageConverterApp *app, uint16_t port, const String &token);
        ~ImgconvIPCServer();

        /// 运行 IPC 事件循环直至收到 ReqShutdown 或连接断开；成功返回 0，连接/协议错误返回 2
        int32_t run();

    private:
        ImageConverterApp  *mApp {nullptr};
        uint16_t            mPort {0};
        String              mToken {};
    };
}


#endif  /*__T3D_IMGCONV_IPC_SERVER_H__*/
