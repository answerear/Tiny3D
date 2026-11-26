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


#ifndef __T3D_RHI_QUERY_H__
#define __T3D_RHI_QUERY_H__


#include "RHI/T3DRHIResource.h"
#include <atomic>


namespace Tiny3D
{
    /**
     * \brief GPU 查询类型
     */
    enum class QueryType : uint32_t
    {
        /// 通过深度 / 模板测试的样本数
        kOcclusion = 0,
        /// 是否有样本通过深度 / 模板测试，结果为 0 或 1，可用于 RHIContext::setPredication
        kBinaryOcclusion,
        /// GPU 时间戳，单位纳秒，只有同一帧内两个时间戳的差值有意义
        kTimestamp,
        /// 各着色器阶段的调用次数与图元数
        kPipelineStatistics,
    };

    /// 返回查询类型的名字，用于日志
    T3D_ENGINE_API const char *getQueryTypeName(QueryType type);

    /**
     * \brief RHIContext::resolveQuery 的返回状态
     */
    enum class QueryStatus : uint32_t
    {
        /// 结果已就绪，QueryResult 有效
        kReady = 0,
        /// GPU 还没执行完，或者后端还没取回结果，下一帧再查
        kPending,
        /// 时间戳所在区间内 GPU 计时被打断（D3D11 的 Disjoint、GLES 的 GPU_DISJOINT），本次结果作废
        kDisjoint,
        /// 查询为空、从未发起过，或后端取结果失败
        kInvalid,
    };

    /**
     * \brief 管线统计查询的结果
     * \remarks 字段与 D3D11_QUERY_DATA_PIPELINE_STATISTICS 一一对应。各后端都尽量填满，
     *          GL 的 hull 阶段只有 patch 数（GL_TESS_CONTROL_SHADER_PATCHES），语义上是 HS 的输出 patch 数而非调用次数
     */
    struct PipelineStatistics
    {
        /// 输入装配读取的顶点数
        uint64_t inputVertices {0};
        /// 输入装配读取的图元数
        uint64_t inputPrimitives {0};
        /// 顶点着色器调用次数
        uint64_t vertexShaderInvocations {0};
        /// 外壳着色器调用次数（GL / Vulkan 为 patch 数）
        uint64_t hullShaderInvocations {0};
        /// 域着色器调用次数
        uint64_t domainShaderInvocations {0};
        /// 几何着色器调用次数
        uint64_t geometryShaderInvocations {0};
        /// 几何着色器输出的图元数
        uint64_t geometryShaderPrimitives {0};
        /// 送入裁剪阶段的图元数
        uint64_t clippingInvocations {0};
        /// 裁剪后输出的图元数
        uint64_t clippingPrimitives {0};
        /// 像素着色器调用次数
        uint64_t pixelShaderInvocations {0};
        /// 计算着色器调用次数
        uint64_t computeShaderInvocations {0};
    };

    /**
     * \brief GPU 查询结果
     */
    struct QueryResult
    {
        /// 查询类型
        QueryType           type {QueryType::kOcclusion};
        /// kOcclusion 为通过的样本数，kBinaryOcclusion 为 0 或 1
        uint64_t            samplesPassed {0};
        /// kTimestamp 的时间戳，单位纳秒
        uint64_t            timestamp {0};
        /// kPipelineStatistics 的统计结果
        PipelineStatistics  pipelineStatistics {};
    };

    /**
     * \brief RHI 层 GPU 查询对象
     * \remarks 一个对象对应一个查询，发起一次得到一个结果，可以反复发起。
     *          主线程按 begin / end 登记发起序号，后端在 GPU 完成后（可能在 RHI 线程上）用 publish
     *          发布对应序号的结果，主线程的 resolve 只认最近一次发起的序号，所以结果永远不会张冠李戴，
     *          也不需要等待 GPU。
     */
    class T3D_ENGINE_API RHIQuery : public RHIResource
    {
    public:
        /// 返回资源类型 kQuery
        ResourceType getResourceType() const override { return ResourceType::kQuery; }

        /// 查询类型
        QueryType getQueryType() const { return mQueryType; }

        /// 是否处于 begin 与 end 之间
        bool isActive() const { return mActive; }

        /// 最近一次 end 登记的序号，0 表示从未发起过
        uint32_t getIssuedSerial() const { return mIssuedSerial; }

        /**
         * \brief 主线程登记一次 begin，规则不满足时打错误日志
         * \param [in] tag : 日志 tag
         * \return 时间戳查询或已经 begin 过时返回 false
         */
        bool markBegin(const char *tag);

        /**
         * \brief 主线程登记一次 end，并分配本次发起的序号
         * \param [in] tag : 日志 tag
         * \param [out] serial : 本次发起的序号，后端 publish 时原样传回
         * \return 非时间戳查询没有 begin 时返回 false
         */
        bool markEnd(const char *tag, uint32_t &serial);

        /**
         * \brief 后端发布某次发起的结果
         * \param [in] serial : markEnd 分配的序号
         * \param [in] status : kReady 或 kDisjoint / kInvalid
         * \param [in] result : 结果，status 不是 kReady 时被忽略
         * \note 可在 RHI 线程调用。同一个查询的 publish 必须按序号递增的顺序调用
         */
        void publish(uint32_t serial, QueryStatus status, const QueryResult &result);

        /**
         * \brief 主线程读取最近一次发起的结果，不等待 GPU
         * \param [out] result : 结果，返回 kReady 时有效
         * \return 见 QueryStatus
         */
        QueryStatus resolve(QueryResult &result) const;

    protected:
        /// 受保护构造，由后端 RHIContext::createQuery 创建
        explicit RHIQuery(QueryType type);

        /// 查询类型
        QueryType               mQueryType {QueryType::kOcclusion};
        /// 主线程：是否处于 begin 与 end 之间
        bool                    mActive {false};
        /// 主线程：最近一次 end 登记的序号
        uint32_t                mIssuedSerial {0};
        /// 后端：已发布结果对应的序号，先写结果再 release 写序号
        std::atomic<uint32_t>   mPublishedSerial {0};
        /// 后端：已发布结果的状态
        QueryStatus             mPublishedStatus {QueryStatus::kPending};
        /// 后端：已发布的结果
        QueryResult             mPublishedResult {};
    };
}


#endif  /*__T3D_RHI_QUERY_H__*/
