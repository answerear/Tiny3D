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

#ifndef __COMPUTE_CASES_H__
#define __COMPUTE_CASES_H__


#include <Tiny3D.h>

#include <memory>
#include <vector>


/**
 * \brief 用例对后端能力位的要求
 * \remarks 与 RHICapabilities 的字段一一对应。能力位为 false 时对应的 RHIContext
 *          接口是 T3D_RHI_UNSUPPORTED stub，跑下去只会拿到 T3D_ERR_NOT_IMPLEMENT。
 */
enum ComputeCapBits : uint32_t
{
    kNeedCompute          = (1 << 0),
    kNeedUnorderedAccess  = (1 << 1),
    kNeedStructuredBuffer = (1 << 2),
    kNeedReadback         = (1 << 3),
    kNeedIndirectDraw     = (1 << 4),
    kNeedIndirectDispatch = (1 << 5),
    kNeedAppendConsume    = (1 << 6),
};


/**
 * \brief 用例支持的后端
 * \remarks **与 requiredCaps 管的不是同一类问题，不要合并。** requiredCaps 挡的是
 *          「后端老实承认不支持」，backendMask 挡的是「能力位说支持、实现却静默
 *          no-op」—— 典型是 GL 的纹理 UAV：全仓库没有 glBindImageTexture，
 *          setCSUnorderedAccessBuffers 遇到非结构化缓冲会静默绑 0 且不报错，
 *          只能靠后端名字跳过（设计文档 §2.2 / §3.4）。
 */
enum ComputeBackendBits : uint32_t
{
    kBackendD3D11 = (1 << 0),
    kBackendGL4   = (1 << 1),
    kBackendGLES3 = (1 << 2),
    /// 其它后端（VK / Metal / Console）当前全是 stub
    kBackendOther = (1 << 3),

    kBackendAll   = kBackendD3D11 | kBackendGL4 | kBackendGLES3,
};


/**
 * \brief 一个自检用例
 * \remarks 三段式对应三个帧钩子：setup 在启动时建资源，record 在 onRender 里录制
 *          GPU 命令（含发起 readback），verify 在 onPostRender 里消费 readback 并比对。
 *          这个时序不是风格选择 —— map 必须在 beginRender / endRender 之间录制，
 *          unmap 会阻塞等 GPU，必须在 endRender 之后。
 */
class ComputeCase
{
public:
    virtual ~ComputeCase() = default;

    /// 用例名，形如 "K1 linear write"
    virtual const char *getName() const = 0;

    /// ComputeCapBits 组合
    virtual uint32_t getRequiredCaps() const = 0;

    /// ComputeBackendBits 组合
    virtual uint32_t getBackendMask() const = 0;

    /// 建 kernel 与缓冲，只在启动时跑一次；失败则该用例整体判失败
    virtual TResult setup() = 0;

    /// 在 onRender 里录制 GPU 命令
    virtual TResult record() = 0;

    /// 在 onPostRender 里回读比对
    virtual TResult verify() = 0;
};

using ComputeCasePtr = std::unique_ptr<ComputeCase>;


/// 建出全部自检用例，vector 顺序即执行顺序
void buildComputeCases(std::vector<ComputeCasePtr> &cases);

/// 当前 active 后端对应的 ComputeBackendBits
uint32_t currentBackendBit();

/**
 * \brief 判断用例在当前后端能否跑
 * \param [in] c : 待判定的用例
 * \param [in] caps : 当前后端能力位
 * \param [in] backendBit : currentBackendBit() 的结果
 * \param [out] reason : 不能跑时填入人类可读的原因，用于日志
 * \return 可跑返回 true
 */
bool isCaseRunnable(const ComputeCase &c, const Tiny3D::RHICapabilities &caps,
    uint32_t backendBit, String &reason);


#endif  /*__COMPUTE_CASES_H__*/
