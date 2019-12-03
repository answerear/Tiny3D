/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#ifndef __T3D_GPU_CONST_BUFFER_H__
#define __T3D_GPU_CONST_BUFFER_H__


#include "Resource/T3DResource.h"
#include "Render/T3DHardwareConstantBuffer.h"


namespace Tiny3D
{
    /**
     * @class   GPUConstBuffer
     * @brief   GPU 常量緩衝區代理類，讓常量緩衝區有資源行為，用於資源管理.
     */
    class T3D_ENGINE_API GPUConstBuffer : public Resource
    {
    public:
        /**
         * @enum    BuiltinType
         * @brief   GPU常量緩衝區中可使用的數據類型
         */
        enum class BuiltinType : uint8_t
        {
            NONE = 0,   /**< 未定義 */
            REAL,       /**< 實數型 */
            INT,        /**< 整型 */
            VECTOR4,    /**< 4D向量 */
            MATRIX4x4,  /**< 4x4矩陣 */
            MATRIX4x3,  /**< 4x3矩陣 */
        };

        /**
         * @struct  DataDeclaration
         * @brief   A data type.
         */
        struct DataDeclaration
        {
            DataDeclaration() :type(BuiltinType::NONE), count(0) {}

            BuiltinType type;
            uint8_t     count;
        };

        typedef TList<DataDeclaration>          DataDeclList;
        typedef DataDeclList::iterator          DataTypeListItr;
        typedef DataDeclList::const_iterator    DataTypeListConstItr;

        /**
         * @fn  static GPUConstBufferPtr 
         *      GPUConstBuffer::create(const String& name);
         * @brief   創建一個新的常量緩衝區資源對象
         * @param   name    資源名稱.
         * @returns 調用成功返回一個新建對象.
         */
        static GPUConstBufferPtr create(const String& name);

        /**
         * @fn  virtual GPUConstBuffer::~GPUConstBuffer();
         * @brief   析構函數
         */
        virtual ~GPUConstBuffer();

        /**
         * @fn  virtual Type GPUConstBuffer::getType() const override;
         * @brief   重寫 Resource::getType()
         * @returns 返回資源類型.
         */
        virtual Type getType() const override;

        /**
         * @fn  virtual TResult GPUConstBuffer::initWithData(size_t bufSize, 
         *      const void* buffer, HardwareBuffer::Usage usage, uint32_t mode);
         * @brief   用數據初始化硬件常量緩衝區
         * @param   bufSize 緩衝區大小.
         * @param   buffer  緩衝區數據.
         * @param   usage   緩衝區用法.
         * @param   mode    缓冲区访问方式.
         * @returns 調用成功返回T3D_OK.
         */
        virtual TResult initWithData(size_t bufSize, const void* buffer, 
            HardwareBuffer::Usage usage, uint32_t mode);

        /**
         * @fn  TResult GPUConstBuffer::addDataDeclaration(
         *      BuiltinType type, uint8_t count);
         * @brief   添加一個數據類型聲明
         * @param   type    數據類型枚舉.
         * @param   count   數據數量.
         * @returns 調用成功返回 T3D_OK.
         * @sa  enum BuiltinType
         */
        TResult addDataDeclaration(BuiltinType type, uint8_t count);

        /**
         * @fn  TResult GPUConstBuffer::removeDataDeclaration(size_t index);
         * @brief   根據索引移除一個數據類型聲明
         * @param   index   位置索引.
         * @returns 調用成功返回 T3D_OK.
         */
        TResult removeDataDeclaration(size_t index);

        /**
         * @fn  size_t GPUConstBuffer::getBufferSize() const;
         * @brief   獲取緩衝區的字節數大小
         * @returns 返回緩衝區的字節數大小.
         */
        size_t getBufferSize() const 
        { 
            if (!mHasData)
                return mBufSize * 4;
            return mBufSize; 
        }

        /**
         * @fn  DataDeclaration GPUConstBuffer::getDataType(size_t index) const;
         * @brief   根據索引獲取對應的索引位置的數據類型
         * @param   index   索引位置.
         * @returns 返回對應索引位置的數據聲明.
         */
        DataDeclaration getDataDeclaration(size_t index) const;

        /**
         * @fn  const DataTypeList GPUConstBuffer::&getDataTypeList() const;
         * @brief   獲取數據類型列表
         * @returns 返回數據類型列表.
         */
        const DataDeclList& getDataDeclList() const { return mDataDeclList; }

        /**
         * @fn  HardwareConstantBufferPtr GPUConstBuffer::getBufferImpl() const;
         * @brief   獲取具體跟平台有關的對象
         * @returns 返回具體跟平台有關的對象.
         */
        HardwareConstantBufferPtr getBufferImpl() const { return mBufferImpl; }

    protected:
        /**
         * @fn  GPUConstBuffer::GPUConstBuffer(const String& name);
         * @brief   構造函數
         * @param   name    資源名稱.
         */
        GPUConstBuffer(const String& name);

        /**
         * @fn  virtual TResult GPUConstBuffer::load() override;
         * @brief   重寫 Resource::load()
         * @returns 調用成功返回T3D_OK.
         */
        virtual TResult load() override;

        /**
         * @fn  virtual TResult GPUConstBuffer::unload() override;
         * @brief   重寫 Resource::unlaod()
         * @returns 調用成功返回T3D_OK.
         */
        virtual TResult unload() override;

        /**
         * @fn  virtual ResourcePtr GPUConstBuffer::clone() const override;
         * @brief   重寫 Resource::clone()
         * @returns 調用成功返回一個新資源對象.
         */
        virtual ResourcePtr clone() const override;

    protected:
        size_t                      mBufSize;           /**< 緩衝區大小 */
        const void                  *mBuffer;           /**< 緩衝區數據 */
        HardwareBuffer::Usage       mUsage;             /**< 緩衝區用法 */
        uint32_t                    mAccessMode;        /**< 缓冲区访问方式 */
        bool                        mHasData;           /**< 是否初始化數據 */
        HardwareConstantBufferPtr   mBufferImpl;        /**< 具體實現類 */

        DataDeclList                mDataDeclList;      /**< 數據聲明列表 */
    };
}


#endif  /*__T3D_GPU_CONST_BUFFER_H__*/
