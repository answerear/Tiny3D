

namespace Tiny3D
{
    inline void Renderer::setViewTransform(const Matrix4 &mat)
    {
        setTransform(E_TS_VIEW, mat);
    }

    inline void Renderer::setWorldTransform(const Matrix4 &mat)
    {
        setTransform(E_TS_WORLD, mat);
    }

    inline void Renderer::setProjectionTransform(const Matrix4 &mat)
    {
        setTransform(E_TS_PROJECTION, mat);
    }

    inline void Renderer::setTextureTransform(int32_t textureIdx, const Matrix4 &mat)
    {
        TransformState ts = TransformState(textureIdx - (int32_t)E_TS_TEXTURE_0);
        setTransform(ts, mat);
    }
}
