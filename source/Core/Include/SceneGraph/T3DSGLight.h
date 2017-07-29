

#ifndef __T3D_SG_LIGHT_H__
#define __T3D_SG_LIGHT_H__


#include "SceneGraph/T3DSGRenderable.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGLight : public SGRenderable
    {
    public:
        enum LightType
        {
            E_LT_POINT = 0,
            E_LT_SPOT,
            E_LT_DIRECTIONNAL,
        };

        static SGLightPtr create(uint32_t uID = E_NID_AUTOMATIC);

        virtual ~SGLight();

        virtual Type getNodeType() const override;
        virtual NodePtr clone() const override;

        void setLightType(LightType type)   { mLightType = type; }
        LightType getLightType() const      { return mLightType; }

        void setAmbientColor(const Color4 &ambient) { mAmbientColor = ambient; }
        const Color4 &getAmbientColor() const       { return mAmbientColor; }

        void setDiffuseColor(const Color4 &diffuse) { mDiffuseColor = diffuse; }
        const Color4 &getDiffuseColor() const       { return mDiffuseColor; }

        void setSpecularColor(const Color4 &specular)   { mSpecularColor = specular; }
        const Color4 &getSpecularColor() const          { return mSpecularColor; }

        void setDirection(const Vector3 &dir, bool isDerivedOrientation = false);
        Vector3 getDirection() const;

        void setSpotlightRangle(const Degree &inner, const Degree &outer, Real falloff = 1.0);

        const Degree &getSpotlightInnerAngle() const    { return mSpotInner; }
        const Degree &getSpotlightOuterAngle() const    { return mSpotOuter; }
        Real getSpotlightFalloff() const                { return mSpotFalloff; }

        void setAttenuation(Real range, Real constant, Real linear, Real quadratic);

        Real getAttenuationRange() const    { return mRange; }
        Real getAttenuationConst() const    { return mAttenuationConst; }
        Real getAttenuationLinear() const   { return mAttenuationLinear; }
        Real getAttenuationQuadric() const  { return mAttenuationQuad; }

    protected:
        virtual void updateTransform() override;
        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

        virtual MaterialPtr getMaterial() const override;
        virtual Renderer::PrimitiveType getPrimitiveType() const override;
        virtual VertexDataPtr getVertexData() const override;
        virtual IndexDataPtr getIndexData() const override;
        virtual bool isIndicesUsed() const override;

    protected:
        SGLight(uint32_t uID = E_NID_AUTOMATIC);

        LightType   mLightType;

        Color4      mAmbientColor;
        Color4      mDiffuseColor;
        Color4      mSpecularColor;

        Vector3     mDirection;

        Degree      mSpotInner;
        Degree      mSpotOuter;
        Real        mSpotFalloff;
        Real        mRange;
        Real        mAttenuationConst;
        Real        mAttenuationLinear;
        Real        mAttenuationQuad;

        bool        mIsDerivedOrientation;
    };
}


#endif  /*__T3D_SG_LIGHT_H__*/
