

#ifndef __MCONV_TRANSFORM_H__
#define __MCONV_TRANSFORM_H__


#include "mconv_node.h"


namespace mconv
{
    class Hiarachy : public Node
    {
    public:
        Hiarachy(const String &ID)
            : Node(ID)
        {
        }

        ~Hiarachy()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_HIARACHY;
        }
    };


    class Transform : public Node
    {
    public:
        typedef std::pair<Mesh*, SubMesh*>  Entity;
        typedef std::list<Entity>           Entities;
        typedef Entities::iterator          EntitiesItr;

        Transform(const String &ID)
            : Node(ID)
        {

        }

        virtual ~Transform()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_TRANSFORM;
        }

        Matrix4     mMatrix;
        Entities    mEntities;
    };
}


#endif  /*__MCONV_TRANSFORM_H__*/
