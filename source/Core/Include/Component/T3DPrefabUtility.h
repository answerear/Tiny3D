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

#ifndef __T3D_PREFAB_UTILITY_H__
#define __T3D_PREFAB_UTILITY_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
#if defined(T3D_EDITOR)
    /**
     * \brief 编辑器侧 Prefab 创建 / Variant / Prefab Mode 辅助
     */
    class T3D_ENGINE_API PrefabUtility
    {
    public:
        /**
         * \brief 从场景物体创建 Prefab 资产，并将原物体转为链接实例
         * \param [in] root : 场景中选中的根 GameObject
         * \param [in] prefabPath : 保存路径（如 Assets/xxx.tprefab）
         * \return 新 Prefab；失败返回 nullptr
         */
        static PrefabPtr createPrefab(GameObject *root, const String &prefabPath);

        /**
         * \brief 基于已有 Prefab 创建 Variant 资产
         * \param [in] basePrefab : 基 Prefab
         * \param [in] variantPath : Variant 保存路径
         * \return 新 Variant Prefab；失败返回 nullptr
         */
        static PrefabPtr createPrefabVariant(Prefab *basePrefab, const String &variantPath);

        /**
         * \brief 查找实例的 Prefab 根（向上找带 PrefabInstance 的节点）
         */
        static GameObject *getPrefabRoot(GameObject *go);

        /**
         * \brief Prefab Mode：打开隔离编辑（将 Prefab 根放入临时编辑场景）
         */
        static TResult enterPrefabMode(Prefab *prefab);

        /**
         * \brief 退出 Prefab Mode；save 为 true 时写回资产
         */
        static TResult exitPrefabMode(bool save);

        static bool isInPrefabMode();

        static Prefab *getPrefabModePrefab();
    };
#endif
}


#endif  /*__T3D_PREFAB_UTILITY_H__*/