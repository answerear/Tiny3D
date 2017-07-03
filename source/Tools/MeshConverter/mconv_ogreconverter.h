

#ifndef __MCONV_OGRE_CONVERTER_H__
#define __MCONV_OGRE_CONVERTER_H__


#include "mconv_converter.h"
#include "mconv_vertex.h"
#include "mconv_ogredata.h"


namespace mconv
{
    class Serializer;
    class Node;
    class Mesh;
    class SubMeshes;
    class SubMesh;
    class Model;
    class Vertex;
    class VertexBuffers;
    class VertexBuffer;
    class Bone;
    class Skeleton;

    struct OgreMesh;
    struct OgreSubMesh;
    struct OgreGeometry;
    struct OgreVertexBuffer;
    struct OgreVertexElement;
    struct OgreSkeleton;

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

        bool processOgreMesh(const OgreMesh &mesh, Node *pRoot);
        bool processOgreGeometry(const OgreGeometry &geometry, Mesh *pMesh);
        bool processOgreVertexAttributes(const OgreGeometry &geometry, VertexBuffer *pVertexBuffer, size_t source);
        bool putVertexAttribute(const OgreVertexElement &element, VertexBuffer *pVertexBuffer);
        bool processOgreVertexBuffer(const OgreVertexBuffer &buffer, VertexBuffer *pVertexBuffer, size_t source);
        bool putVertexData(const std::vector<float> &vertices, size_t &index, const VertexAttributes &attributes, Vertex &vertex);
        bool processVertexSemantic(uint16_t semantic, VertexAttribute &attribute);
        bool processVertexType(uint16_t type, VertexAttribute &attribute);
        bool createMesh(Model *pModel, Mesh *&pMesh, SubMeshes *&pSubMeshes, size_t index);
        bool processOgreSubMeshes(const OgreMesh &mesh, Model *pModel);
        bool processOgreSubMesh(const OgreSubMesh &submesh, Mesh *pMesh, SubMeshes *pSubMeshes);
        bool processOgreBoneAssignment(const std::vector<OgreBoneAssignment> &assignments, Mesh *pMesh);

        bool processOgreSkeleton(const OgreSkeleton &skeleton, Model *pModel);
        bool processOgreSkin(const OgreSkeleton &skeleton, Model *pModel);
        bool processBone(Bone *pBone, const Matrix4 &m);
        bool processOgreBones(const OgreSkeleton &skeleton, Model *pModel);

        bool processOgreAnimations(const OgreSkeleton &skeleton, Model *pModel);

        bool searchVertexBuffer(Mesh *pMesh, VertexBuffer *&pVertexBuffer);
        bool searchVertexAttribute(VertexBuffer *pVertexBuffer, VertexAttribute::VertexType type);
        void putVertexBlendAndWeightAttributes(VertexBuffer *pVB);

    protected:
        std::vector<Bone*> mBones;
        
        void    *mSrcData;
        void    *mDstData;
    };
}


#endif  /*__MCONV_OGRE_CONVERTER_H__*/
