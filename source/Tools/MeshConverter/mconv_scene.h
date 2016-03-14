

#ifndef __MCONV_SCENE_H__
#define __MCONV_SCENE_H__


#include "mconv_node.h"


namespace mconv
{
    class Scene : public Node
    {
    public:
        Scene(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Scene()
        {

        }
    };
}


#endif  /*__MCONV_SCENE_H__*/
