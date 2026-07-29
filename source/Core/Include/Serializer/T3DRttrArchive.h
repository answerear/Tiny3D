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

#ifndef __T3D_RTTR_ARCHIVE_H__
#define __T3D_RTTR_ARCHIVE_H__


#include "Serializer/T3DSerializeArchive.h"


namespace Tiny3D
{
    /**
     * \brief RTTR 对象与归档后端之间的共享遍历层。
     *
     * \remarks 这里是(反)序列化语义的**唯一**实现处，JSON / 二进制等具体格式只需
     *          实现 IArchiveWriter / IArchiveReader 的编解码原语。新增格式不应、
     *          也不需要在后端里夹带任何语义判断。
     *
     * ## 行为基线
     *
     * 以下是两种格式**共同**遵守的语义。改动其中任何一条都会同时影响 JSON 与
     * 二进制，且多半会破坏既有资产的读取或字节级往返一致性。
     *
     * ### 写出
     * - 跳过带 `NO_SERIALIZE` 元数据的属性。
     * - 空裸指针与空 SmartPtr 属性**不写出**（而非写成 null），读取端因此看不到
     *   该属性、保留其默认值。
     * - wrapper(SmartPtr) 会解包，落盘的类型名是**运行时派生类型**，多态由此还原。
     * - `Buffer` / `UUID` 走专用表达（文本转 base64 / 字符串，二进制写裸字节）。
     * - 数值按 RTTR 声明类型的**确切宽度**落盘，不统一提升到最宽类型。
     * - 输出必须是**确定性**的：同一对象图重复写出的字节完全一致，否则 bundle 无法
     *   做增量比对。无序容器（unordered_map 等）的元素顺序由容器自身决定，本层不
     *   重排，但不得引入额外的不确定性（例如遍历中对 variant 用移动而非拷贝）。
     *
     * ### 读入
     * - 结构与类型一律由 **RTTR 声明类型驱动**，落盘的类型标签只用于判空、取数值
     *   确切宽度、以及多态建对象时取派生类型名。容器是顺序还是关联，看声明类型，
     *   不看落盘标记。
     * - 属性改名回退查找 `SERIALIZE_ALIAS` 记录的曾用名。
     * - **前向兼容**：未知属性整值跳过；标签与声明类型不兼容时丢弃载荷并告警。两者
     *   都必须保持流对齐——跳过是"消费掉"而不是"忽略"，否则后续属性全部读坏。
     * - 容器元素按**拉取**读取（个数只有二进制后端能预知），顺序容器按需增长。
     * - 生命周期回调 `onPostInit` / `onPostLoad` /
     *   `onAddComponentForLoadingResource`（写出侧为 `onPreSave` / `onPostSave`）
     *   受 SerializerManager::isInvokeLifecycleCallbacks 控制，离线工具应关闭，
     *   原因见该 setter 的说明。
     *
     * ### 格式版本
     * 两种格式的版本号描述的都只是**容器编码**，与资产 schema 无关；schema 变化由
     * 上述前向兼容规则承担，不需要升版本。详见 T3DBinArchive.h / T3DJsonArchive.h。
     */
    class T3D_ENGINE_API RttrArchive
    {
    public:
        /**
         * @brief 把可反射对象经归档后端写出（含文件头）。
         * @param [in,out] writer : 目标格式的写入归档后端
         * @param [in] obj        : 待序列化的可反射对象
         * @return 成功返回 T3D_OK .
         */
        static TResult write(IArchiveWriter &writer, const RTTRObject &obj);

        /**
         * @brief 经归档后端读入并新建可反射对象（含文件头校验）。
         * @param [in,out] reader : 源格式的读取归档后端
         * @param [out] obj       : 还原出的对象；文件头正常但内容无法还原时
         *                          返回无效变体，由调用方决定如何报错
         * @return 文件头校验失败返回对应错误码，否则返回 T3D_OK .
         */
        static TResult read(IArchiveReader &reader, RTTRVariant &obj);
    };
}


#endif    /*__T3D_RTTR_ARCHIVE_H__*/
