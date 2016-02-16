

#ifndef __T3D_SG_LIGHT_H__
#define __T3D_SG_LIGHT_H__


#include "SG/Visual/T3DSGVisual.h"
#include "Misc/T3DColor4.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGLight : public SGVisual
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
        virtual SGNodePtr clone() const override;

        void setLightType(LightType type);
        LightType getLightType() const;

        void setAmbientColor(const Color4 &ambient);
        const Color4 &getAmbientColor() const;

        void setDiffuseColor(const Color4 &diffuse);
        const Color4 &getDiffuseColor() const;

        void setSpecularColor(const Color4 &specular);
        const Color4 &getSpecularColor() const;

        void setDirection(const Vector3 &dir, bool isDerivedOrientation = false);
        Vector3 getDirection() const;

        void setSpotlightRangle(const Degree &inner, const Degree &outer, Real falloff = 1.0);

        const Degree &getSpotlightInnerAngle() const;
        const Real &getSpotlightOuterAngle() const;
        Real getSpotlightFalloff() const;

        void setAttenuation(Real range, Real constant, Real linear, Real quadratic);
        Real getAttenuationRange() const;
        Real getAttenuationConst() const;
        Real getAttenuationLinear() const;
        Real getAttenuationQuadric() const;

    protected:
        virtual void updateTransform() override;
        virtual void frustumCulling(const BoundPtr &bound, const RenderQueuePtr &queue) override;

    protected:
        SGLight(uint32_t uID = E_NID_AUTOMATIC);

        LightType   mLightType;

        Color4      mAmbientColor;
        Color4      mDiffuseColor;
        Color4      mSpecularColor;

        Vector3     mDirection;

        Degree      mSpotInner;
        Degree      mSpotOuter;
        Real        mFalloff;
        Real        mRange;
        Real        mAttenuationConst;
        Real        mAttenuationLinear;
        Real        mAttenuationQuad;

        bool        mIsDerivedOrientation;
    };
}


#endif  /*__T3D_SG_LIGHT_H__*/
