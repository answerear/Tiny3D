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


#include "ProjectSettings.h"
#include "ProjectManager.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    //--------------------------------------------------------------------------

    const char *ProjectSettings::PROJECT_SETTINGS_FOLDER = "ProjectSettings";
    const char *ProjectSettings::PROJECT_SETTINGS_NAME = "ProjectSettings.tasset";
    
    //--------------------------------------------------------------------------

    void ProjectSettings::ensure()
    {
        String path = PROJECT_MGR.getAssetsPath() + Dir::getNativeSeparator() + PROJECT_SETTINGS_FOLDER;
        if (!Dir::exists(path))
        {
            Dir::makeDir(path);
        }
    }

    //--------------------------------------------------------------------------

    TResult ProjectSettings::save()
    {
        TResult ret = T3D_OK;

        do
        {
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(PROJECT_MGR.getAssetsPath(), ARCHIVE_TYPE_METAFS, Archive::AccessMode::kTruncate);
            if (archive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load archive [%s]", PROJECT_MGR.getAssetsPath().c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            String filename = String(PROJECT_SETTINGS_FOLDER) + Dir::getNativeSeparator() + String(PROJECT_SETTINGS_NAME);
            ret = archive->write(filename,
                [this](DataStream &stream, const String &filename, void *userData)
                {
                    return T3D_SERIALIZER_MGR.serialize(stream, *this);
                },
                nullptr);
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    TResult ProjectSettings::load()
    {
        TResult ret = T3D_OK;

        do
        {
            ArchivePtr archive = T3D_ARCHIVE_MGR.loadArchive(PROJECT_MGR.getAssetsPath(), ARCHIVE_TYPE_METAFS, Archive::AccessMode::kRead);
            if (archive == nullptr)
            {
                EDITOR_LOG_ERROR("Failed to load archive [%s] !", PROJECT_MGR.getAssetsPath().c_str());
                ret = T3D_ERR_RES_LOAD_FAILED;
                break;
            }

            String filename = String(PROJECT_SETTINGS_FOLDER) + Dir::getNativeSeparator() + String(PROJECT_SETTINGS_NAME);
            ret = archive->read(filename,
                [this](DataStream &stream, const String &filename, void *userData)
                {
                    return T3D_SERIALIZER_MGR.deserialize(stream, *this);
                },
                nullptr);
        } while (false);

        return ret;
    }
    
    //--------------------------------------------------------------------------

    NS_END
}
