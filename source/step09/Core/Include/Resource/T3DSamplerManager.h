/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
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


#ifndef __T3D_SAMPLER_MANAGER_H__
#define __T3D_SAMPLER_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DSampler.h"


namespace Tiny3D
{
    /**
     * @class   SamplerManager
     * @brief   采样器资源管理器
     */
    class T3D_ENGINE_API SamplerManager
        : public Singleton<SamplerManager>
        , public ResourceManager
    {
        T3D_DECLARE_CLASS();

    public:
        /**
         * @fn  static SamplerManagerPtr create();
         * @brief   创建采样器管理器对象
         * @return  A SamplerManagerPtr.
         */
        static SamplerManagerPtr create();

        /**
         * @fn  virtual ~SamplerManager();
         * @brief   析构函数
         */
        virtual ~SamplerManager();

        /**
         * @fn  void setSamplerCreator(SamplerCreator *creator);
         * @brief   设置采样器创建器
         * @param [in,out]  creator If non-null, the creator.
         */
        void setSamplerCreator(SamplerCreator *creator);

        /**
         * @fn  virtual SamplerPtr loadSampler(const String &name);
         * @brief   加载采样器资源
         * @param [in]  name    : 采样器资源名称.
         * @return  返回新建的纹理对象.
         */
        virtual SamplerPtr loadSampler(const String &name);

        /**
         * @fn  virtual TResult unloadSampler(SamplerPtr texture);
         * @brief   卸载采样器资源
         * @param [in]  texture : 要卸载的采样器资源对象.
         * @return  void.
         */
        virtual TResult unloadSampler(SamplerPtr texture);

    protected:
        /**
         * @fn  SamplerManager();
         * @brief   构造函数
         */
        SamplerManager();

        /**
         * @fn  virtual ResourcePtr create(const String &name, 
         *      int32_t argc, va_list args) override;
         * @brief   重写基类接口，实现创建采样器对象
         * @param   name    The name.
         * @param   argc    The argc.
         * @param   args    The arguments.
         * @return  A ResourcePtr.
         * @sa  ResourcePtr Resource::create(const String &amp;name, int32_t argc,
         *  va_list args)
         */
        virtual ResourcePtr create(const String &name, int32_t argc,
            va_list args) override;

    protected:
        SamplerCreator   *mCreator;      /**< GPU程序创建器对象 */
    };

    #define T3D_SAMPLER_MGR         (SamplerManager::getInstance())
}


#endif  /*__T3D_SAMPLER_MANAGER_H__*/
