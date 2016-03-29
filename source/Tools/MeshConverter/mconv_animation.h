

#ifndef __MCONV_ANIMATION_H__
#define __MCONV_ANIMATION_H__


#include "mconv_node.h"


namespace mconv
{
    class Keyframe
    {
    public:
        Keyframe(const String &ID)
        {

        }

        String  mID;
    };

    class Action : public Node
    {
    public:
        Action(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Action()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_ACTION;
        }


    };

    class Animation : public Node
    {
    public:
        Animation(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Animation()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_ANIMATION;
        }
    };
}


#endif  /*__MCONV_ANIMATION_H__*/
