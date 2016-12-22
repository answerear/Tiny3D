

#ifndef __MCONV_OGRE_CONVERTER_H__
#define __MCONV_OGRE_CONVERTER_H__


#include "mconv_converter.h"


namespace mconv
{
    class Serializer;
    class Node;

    struct OgreMesh;

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
        bool processOgreGeometry(const OgreGeometry &geometry, Node *pParent, bool bSharedVertices);
        bool processOgreSubMesh();

    protected:
        void    *mSrcData;
        void    *mDstData;
    };
}


#endif  /*__MCONV_OGRE_CONVERTER_H__*/
