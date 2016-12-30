

#ifndef __MCONV_OGRE_CONVERTER_H__
#define __MCONV_OGRE_CONVERTER_H__


#include "mconv_converter.h"
#include "mconv_vertex.h"


namespace mconv
{
    class Serializer;
    class Node;
    class Mesh;

    struct OgreMesh;
    struct OgreGeometry;
    struct OgreVertexElement;

    class OgreConverter : public ConverterImpl
    {
    public:
        OgreConverter(const Settings &settings);
        virtual ~OgreConverter();

    protected:
        virtual bool importScene() override;
        virtual bool convertToT3D() override;
        virtual bool exportScene() override;
        virtual void cleanup() override;

        bool processOgreMesh(OgreMesh *pOgreMesh, Node *pRoot);
        bool processOgreGeometry(const OgreGeometry &geometry, Mesh *pMesh);
        bool processOgreVertexAttributes(const OgreGeometry &geometry, Mesh *pMesh);
        bool putVertexAttribute(const OgreVertexElement &element, Mesh *pMesh);
        bool processVertexSemantic(uint16_t semantic, VertexAttribute &attribute);
        bool processVertexType(uint16_t type, VertexAttribute &attribute);
        bool processOgreSubMesh();

    protected:
        void    *mSrcData;
        void    *mDstData;
    };
}


#endif  /*__MCONV_OGRE_CONVERTER_H__*/
