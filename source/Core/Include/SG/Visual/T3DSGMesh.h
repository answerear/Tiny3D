
#ifndef __T3D_SG_MESH_H__
#define __T3D_SG_MESH_H__


#include "SG/Visual/T3DSGVisual.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGMesh : public SGVisual
    {
    public:
        enum Type
        {
            E_TYPE_NONE = 0,            /// 空mesh
            E_TYPE_BUILT_IN_BOX,        /// 内置立方体mesh
            E_TYPE_BUILT_IN_SPHERE,     /// 内置球体mesh
            E_TYPE_BUILT_IN_PLANE,      /// 内置平面mesh
            E_TYPE_CUSTOM_MESH,         /// 外部加载的mesh
        };
        static SGNodePtr create(uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGMesh();

        bool loadBuiltInMesh(Type meshType);
        
    protected:
        SGMesh(uint32_t unID = E_NID_AUTOMATIC);


    };
}


#endif  /*__T3D_SG_MESH_H__*/
