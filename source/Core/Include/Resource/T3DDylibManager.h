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


#ifndef __T3D_DYLIB_MANAGER_H__
#define __T3D_DYLIB_MANAGER_H__


#include "T3DPrerequisites.h"
#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    /**
     * @class   DylibManager
     * @brief   動態庫資源管理器
     */
    class T3D_ENGINE_API DylibManager 
        : public Singleton<DylibManager>
        , public ResourceManager
    {
    public:
        /**
         * @fn  static DylibManagerPtr create();
         * @brief   創建動態庫管理器對象
         * @return  A DylibManagerPtr.
         */
        static DylibManagerPtr create();

        /**
         * @fn  virtual ~DylibManager();
         * @brief   析構函數
         */
        ~DylibManager() override = default;

        /**
         * @fn  virtual DylibPtr loadDylib(const String &name);
         * @brief   加載動態庫
         * @param   name    The name.
         * @return  The dylib.
         */
        DylibPtr loadDylib(const String &name);

        /**
         * @fn  virtual TResult unloadDylib(DylibPtr dylib);
         * @brief   卸載動態庫
         * @param   dylib   The dylib.
         * @return  A TResult.
         */
        TResult unloadDylib(DylibPtr dylib);

    protected:
        /**
         * @brief 构造函数
         */
        DylibManager() = default;

        /**
         * @brief 实现基类函数
         */
        ResourcePtr newResource(const String &name, int32_t argc, va_list args) override;

        /**
         * @brief 实现基类函数
         */
        ResourcePtr loadResource(Archive *archive, const String &filename) override;
        
        /**
         * @brief 实现基类函数
         */
        ResourcePtr loadResource(const String &filename, DataStream &stream) override;

        /**
         * @brief 实现基类函数
         */
        TResult saveResource(DataStream &stream, Resource *res) override;
    };

    #define T3D_DYLIB_MGR   (DylibManager::getInstance())
}


#endif  /*__T3D_DYLIB_MANAGER_H__*/
