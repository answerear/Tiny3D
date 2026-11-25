/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef __HDR_APP_H__
#define __HDR_APP_H__


#include "../Common/SampleApp.h"


/**
 * \brief Night street lit by sodium lamps and neon signs, rendered into a floating-point camera buffer
 * \remarks
 *   Lamp bulbs and neon tubes emit well above 1.0. With a floating-point buffer the bloom
 *   picks them up and ACES rolls them off while they keep their hue; an 8-bit buffer
 *   would clip them to flat white before post-processing ever sees them.
 */
class HDRApp : public SampleWindowApp
{
public:
    HDRApp();
    ~HDRApp() override;

protected:  /// from Tiny3D::Application
    TResult applicationDidFinishLaunching(int32_t argc, char *argv[]) override;

    void applicationWillTerminate() override;

protected:
    Tiny3D::ShaderPtr buildLitShader();
    Tiny3D::ShaderPtr buildEmissiveShader();

    Tiny3D::MaterialPtr buildLitMaterial(const String &name, Tiny3D::Texture2D *texture, Tiny3D::Real smoothness);
    Tiny3D::MaterialPtr buildEmissiveMaterial(const String &name, Tiny3D::Texture2D *texture, const Tiny3D::ColorRGBA &emission);

    Tiny3D::Texture2DPtr buildColorTexture(const String &name, uint32_t width, uint32_t height, const uint8_t *bgra);
    Tiny3D::Texture2DPtr buildBrickTexture(const String &name, uint8_t r, uint8_t g, uint8_t b, uint32_t seed);
    Tiny3D::Texture2DPtr buildAsphaltTexture();
    Tiny3D::Texture2DPtr buildPavementTexture();
    Tiny3D::Texture2DPtr buildSolidTexture(const String &name, uint8_t r, uint8_t g, uint8_t b);
    Tiny3D::Texture2DPtr buildWindowTexture();
    Tiny3D::Texture2DPtr buildNeonTexture(const String &name, uint32_t width, uint32_t height, int32_t shape, Tiny3D::Real &emissionScale);

    void buildCamera(Tiny3D::Transform3D *parent);
    void buildLights(Tiny3D::Transform3D *parent);
    void buildStreet(Tiny3D::Transform3D *parent);
    void buildSigns(Tiny3D::Transform3D *parent);

    Tiny3D::GameObjectPtr addMeshObject(Tiny3D::Transform3D *parent, const String &name, Tiny3D::MeshPtr mesh);

    Tiny3D::ShaderPtr mLitShader {nullptr};
    Tiny3D::ShaderPtr mEmissiveShader {nullptr};
    TArray<Tiny3D::MeshPtr> mMeshes;
    TArray<Tiny3D::MaterialPtr> mMaterials;
    TArray<Tiny3D::Texture2DPtr> mTextures;
};


#endif  /*__HDR_APP_H__*/
