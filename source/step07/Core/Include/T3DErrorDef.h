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

        T3D_ERR_ZIP_FILE_INFO           = T3D_ERR_CORE + 0x0040, /**< 获取 zip 文件信息出错 */
        T3D_ERR_ZIP_FILE_GOTO_FILE      = T3D_ERR_CORE + 0x0041, /**< 跳转到第一个压缩文件失败 */
        T3D_ERR_ZIP_FILE_LOCATE_FILE    = T3D_ERR_CORE + 0x0042, /**< 定位指定文件失败 */
        T3D_ERR_ZIP_FILE_GET_FILE_INFO  = T3D_ERR_CORE + 0x0043, /**< 获取 zip 中的文件信息出错 */
        T3D_ERR_ZIP_FILE_READ_DATA      = T3D_ERR_CORE + 0x0044, /**< 获取 zip 中的单个文件数据出错 */
        T3D_ERR_ZIP_FILE_NOT_SUPPORT    = T3D_ERR_CORE + 0x0045, /**< 不支持该功能 */
    };
}


#endif    /*__T3D_ERROR_DEF_H__*/
