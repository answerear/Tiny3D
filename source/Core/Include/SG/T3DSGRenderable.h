

#ifndef __T3D_SG_RENDERABLE_H__
#define __T3D_SG_RENDERABLE_H__


#include "SG//T3DSGNode.h"
#include "Resource/T3DMaterial.h"
#include "Misc/T3DIndexData.h"
#include "Misc/T3DVertexData.h"
#include "Render/T3DRenderer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGRenderable : public SGNode
    {
    protected:
        SGRenderable(uint32_t unID = E_NID_AUTOMATIC);

    public:
        /**
         * @brief Destructor
         */
        virtual ~SGRenderable();

        /**
         * @brief 返回渲染材质对象。
         * @note 所有可渲染派生类需要重写本接口以返回对应的渲染材质对象。
         */
        virtual MaterialPtr getMaterial() const = 0;

        /**
         * @brief 返回当前渲染对象世界变化矩阵
         */
        virtual const Matrix4 &getWorldMatrix() const;

        /**
         * @brief 返回渲染元类型
         * @see Renderer::PrimitiveType
         */
        virtual Renderer::PrimitiveType getPrimitiveType() const = 0;

        /**
         * @brief 返回顶点数据对象
         */
        virtual VertexDataPtr getVertexData() const = 0;

        /**
         * @brief 返回索引数据对象
         */
        virtual IndexDataPtr getIndexData() const = 0;

        /**
         * @brief 是否使用顶点索引
         */
        virtual bool isIndicesUsed() const = 0;
    };
}


#endif  /*__T3D_SG_RENDERABLE_H__*/

