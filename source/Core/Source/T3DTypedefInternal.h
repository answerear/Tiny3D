

#ifndef __T3D_TYPEDEF_INTERNAL_H__
#define __T3D_TYPEDEF_INTERNAL_H__



#include "Misc/T3DSmartPtr.h"


namespace Tiny3D
{
    T3D_DECLARE_SMART_PTR(GeometryData);
    T3D_DECLARE_SMART_PTR(BoneData);
    T3D_DECLARE_SMART_PTR(ActionData);
    T3D_DECLARE_SMART_PTR(KeyFrameData);
    T3D_DECLARE_SMART_PTR(KeyFrameDataT);
    T3D_DECLARE_SMART_PTR(KeyFrameDataR);
    T3D_DECLARE_SMART_PTR(KeyFrameDataS);

    #define T3D_BIN_MODEL_FILE_EXT              "tmb"
    #define T3D_TXT_MODEL_FILE_EXT              "tmt"

    #define T3D_BIN_MATERIAL_FILE_EXT           "mtb"
    #define T3D_TXT_MATERIAL_FILE_EXT           "mtt"
}

#endif  /*__T3D_TYPEDEF_INTERNAL_H__*/
