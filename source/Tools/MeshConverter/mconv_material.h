

#ifndef __MCONV_MATERIAL_H__
#define __MCONV_MATERIAL_H__


#include "mconv_node.h"


namespace mconv
{
    class Material : public Node
    {
    public:
        Material(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Material()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_MATERIAL;
        }
    };
}


#endif  /*__MCONV_MATERIAL_H__*/
