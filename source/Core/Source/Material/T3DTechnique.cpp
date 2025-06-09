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


#include "Material/T3DTechnique.h"
#include "Material/T3DPass.h"
#include "Render/T3DRenderState.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TechniquePtr Technique::create(const String &name)
    {
        return new Technique(name);
    }

    //--------------------------------------------------------------------------

    Technique::Technique(const String &name)
        : mName(name)
    {
        
    }

    //--------------------------------------------------------------------------

    TechniquePtr Technique::clone() const
    {
        TechniquePtr tech = create(getName());
        tech->mLOD = mLOD;
        tech->mTags = mTags;
        for (auto pass : mPasses)
        {
            tech->mPasses.emplace_back(pass->clone());
        }
        return tech;
    }

    //--------------------------------------------------------------------------

    TResult Technique::compile()
    {
        TResult ret = T3D_OK;

        do
        {
            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Start compiling technique [%s] ...", getName().c_str());
            for (auto pass : mPasses)
            {
                ret = pass->compile();
                if (T3D_FAILED(ret))
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to compile pass [%s] ! ERROR [%d]", pass->getName().c_str(), ret);
                    break;
                }
            }
            T3D_LOG_DEBUG(LOG_TAG_RESOURCE, "Completed compiling technique !");
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult Technique::reflect()
    {
        TResult ret = T3D_OK;

        do
        {
            for (auto pass : mPasses)
            {
                ret = pass->reflect();
                if (T3D_FAILED(ret))
                {
                    T3D_LOG_ERROR(LOG_TAG_RESOURCE, "Failed to reflect pass [%s] ! ERROR [%d]", pass->getName().c_str(), ret);
                    break;
                }
            }
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool Technique::addTag(const String &key, const String &value)
    {
        auto rval = mTags.emplace(key, value);
        if (rval.second)
        {
            toTagValue(key, rval.first->second);
        }
        return rval.second;
    }
    
    //--------------------------------------------------------------------------

    void Technique::removeTag(const String &key)
    {
        mTags.erase(key);
        resetTagValue(key);
    }
    
    //--------------------------------------------------------------------------

    bool Technique::getTag(const String &key, String &value) const
    {
        bool ret = false;
        const auto itr = mTags.find(key);
        if (itr != mTags.end())
        {
            value = itr->second;
            ret = true;
        }
        return ret;
    }
    
    //--------------------------------------------------------------------------

    bool Technique::setTag(const String &key, const String &value)
    {
        bool ret = false;
        const auto itr = mTags.find(key);
        if (itr != mTags.end())
        {
            itr->second = value;
            toTagValue(key, value);
            ret = true;
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    bool Technique::addPass(PassPtr pass)
    {
        bool found = false;
        for (const auto p : mPasses)
        {
            if (p->getName() == pass->getName())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            mPasses.emplace_back(pass);
            pass->setTechnique(this);
        }

        return !found;
    }
    
    //--------------------------------------------------------------------------

    void Technique::removePass(const String &name)
    {
        for (auto itr = mPasses.begin(); itr != mPasses.end(); ++itr)
        {
            if ((*itr)->getName() == name)
            {
                (*itr)->setTechnique(nullptr);
                mPasses.erase(itr);
                break;
            }
        }
    }
    
    //--------------------------------------------------------------------------

    bool Technique::getPass(const String &name, PassPtr &pass) const
    {
        bool ret = false;
        for (const auto p : mPasses)
        {
            if (p->getName() == name)
            {
                pass = p;
                ret = true;
                break;
            }
        }
        return ret;
    }
    
    //--------------------------------------------------------------------------

    void Technique::toTagValues()
    {
        for (auto &kv : mTags)
        {
            toTagValue(kv.first, kv.second);
        }
    }
    
    //--------------------------------------------------------------------------

    void Technique::toTagValue(const String &key, const String &value)
    {
        if (key == ShaderLab::kBuiltinTagQueue)
        {
            if (ShaderLab::kBuiltinQueueBkgndStr == value)
            {
                mRenderQueue = ShaderLab::kBuiltinQueueBkgnd;
            }
            else if (ShaderLab::kBuiltinQueueGeometryStr == value)
            {
                mRenderQueue = ShaderLab::kBuiltinQueueGeometry;
            }
            else if (ShaderLab::kBuiltinQueueAlphaTestStr == value)
            {
                mRenderQueue = ShaderLab::kBuiltinQueueAlphaTest;
            }
            else if (ShaderLab::kBuiltinQueueTransparentStr == value)
            {
                mRenderQueue = ShaderLab::kBuiltinQueueTransparent;
            }
            else if (ShaderLab::kBuiltinQueueOverlayStr == value)
            {
                mRenderQueue = ShaderLab::kBuiltinQueueOverlay;
            }
        }
    }

    //--------------------------------------------------------------------------

    void Technique::resetTagValue(const String& key)
    {
        if (key == ShaderLab::kBuiltinTagQueue)
        {
            mRenderQueue = ShaderLab::kBuiltinQueueGeometry;
        }
    }
    
    //--------------------------------------------------------------------------

    void Technique::onPostLoad()
    {
        for (auto pass : mPasses)
        {
            pass->setTechnique(this);
        }
    }

    //--------------------------------------------------------------------------
}


