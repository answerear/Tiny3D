/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#ifndef __T3D_ERROR_DEF_H__
#define __T3D_ERROR_DEF_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    enum CoreErrorCode
    {
        T3D_ERR_CFG_FILE_PARSING_XML    = T3D_ERR_CORE + 0x0000, /**< 解析 XML 出错 */
        T3D_ERR_CFG_FILE_XML_FORMAT     = T3D_ERR_CORE + 0x0001, /**< 错误 XML 格式 */
        T3D_ERR_CFG_FILE_BUILDING_XML   = T3D_ERR_CORE + 0x0002, /**< 构建 XML 出错 */

        T3D_ERR_PLUGIN_LOAD_FAILED      = T3D_ERR_CORE + 0x0020, /**< 加载插件出错 */

        T3D_ERR_PLG_NOT_EXISTS          = T3D_ERR_CORE + 0x0040, /**< 插件不存在 */
        T3D_ERR_PLG_DUPLICATED          = T3D_ERR_CORE + 0x0041, /**< 插件已经存在 */
        T3D_ERR_PLG_NOT_DYLIB           = T3D_ERR_CORE + 0x0042, /**< 不是插件资源*/
        T3D_ERR_PLG_NO_FUNCTION         = T3D_ERR_CORE + 0x0043, /**< 获取插件函数失败 */
        T3D_ERR_PLG_NO_PATH             = T3D_ERR_CORE + 0x0044, /**< 无法获取到插件路径 */
        T3D_ERR_PLG_NOT_LOADED          = T3D_ERR_CORE + 0x0045, /**< 没有加载插件 */

        T3D_ERR_IMG_NOT_FOUND           = T3D_ERR_CORE + 0x0060, /**< 找不到对应的图像编解码器 */

        T3D_ERR_SETTINGS_NOT_FOUND      = T3D_ERR_CORE + 0x0080, /**< 找不到设置项 */

        T3D_ERR_RES_LOAD_FAILED         = T3D_ERR_CORE + 0x00A0, /**< 加载资源失败 */

        T3D_ERR_RENDER_CREATE_WINDOW    = T3D_ERR_CORE + 0x00C0, /**< 创建渲染窗口失败 */
    };
}


#endif    /*__T3D_ERROR_DEF_H__*/
