

#ifndef __MCONV_TRANSFORM_H__
#define __MCONV_TRANSFORM_H__


#include "mconv_node.h"


namespace mconv
{
    class Hierarchy : public Node
    {
    public:
        Hierarchy(const String &ID)
            : Node(ID)
        {
        }

        ~Hierarchy()
        {

        }

        virtual Type getNodeType() const override
        {
            return E_TYPE_HIERARCHY;
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
