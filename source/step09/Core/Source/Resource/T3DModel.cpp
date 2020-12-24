/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "Resource/T3DModel.h"
#include "Kernel/T3DAgent.h"
#include "Resource/T3DArchive.h"
#include "Resource/T3DArchiveManager.h"
#include "Serializer/T3DSerializerManager.h"
#include "protobuf/FileScriptObject.pb.h"
#include "Scene/T3DSceneManager.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    T3D_IMPLEMENT_CLASS_1(Model, Resource);

    //--------------------------------------------------------------------------

    ModelPtr Model::create(const String &name)
    {
        ModelPtr mesh = new Model(name);
        mesh->release();
        return mesh;
    }

    //--------------------------------------------------------------------------

    Model::Model(const String &name)
        : Resource(name)
        , mModelData(nullptr)
    {
        mModelData = new Script::FileFormat::FileModel();
    }

    //--------------------------------------------------------------------------

    Model::~Model()
    {

    }

    //--------------------------------------------------------------------------

    Resource::Type Model::getType() const
    {
        return Type::E_RT_MESHDATA;
    }

    //--------------------------------------------------------------------------

    TResult Model::load()
    {
        TResult ret = T3D_OK;

        do 
        {
            // 加载模型文件
            ArchivePtr archive = T3D_AGENT.getAssetsArchive(mName);
            if (archive == nullptr)
            {
                ret = T3D_ERR_IMG_NOT_FOUND;
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Could not find the archive for file %s !",
                    mName.c_str());
                break;
            }

            String path = T3D_AGENT.getMainAssetsPath(mName);
            MemoryDataStream stream;
            ret = archive->read(path, stream);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Read material content failed from file %s ! ",
                    mName.c_str());
                break;
            }

            // 交给模型解析器解析出来
            ret = T3D_SERIALIZER_MGR.parseModel(stream, this);
            if (T3D_FAILED(ret))
            {
                T3D_LOG_ERROR(LOG_TAG_RESOURCE,
                    "Could not parse material file %s !", mName.c_str());
                break;
            }

            ret = buildData();
            if (T3D_FAILED(ret))
            {
                break;
            }
        } while (false);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    TResult Model::unload()
    {
        T3D_SAFE_DELETE(mModelData);
        return Resource::unload();
    }

    //--------------------------------------------------------------------------

    ResourcePtr Model::clone() const
    {
        ModelPtr model = Model::create(mName);
        return model;
    }

    //--------------------------------------------------------------------------

    void Model::setModelData(void *data)
    {
        Script::FileFormat::FileModel *src = (Script::FileFormat::FileModel *)data;
        Script::FileFormat::FileModel *dst = (Script::FileFormat::FileModel *)mModelData;
        dst->CopyFrom(*src);
    }

    //--------------------------------------------------------------------------

    TResult Model::buildData()
    {
        TResult ret = T3D_OK;

        Script::FileFormat::FileModel *model = (Script::FileFormat::FileModel *)mModelData;

        auto body = model->data();
        auto uuid = body.root();
        auto nodes = body.nodes();
        auto root = nodes[uuid];

        mRoot = T3D_SCENE_MGR.createSceneNode(nullptr);
        mRoot->setName(root.name());

        auto children = root.children();

        for (int32_t i = 0; i < root.children_size(); ++i)
        {
            auto child = children[i];
            auto node = nodes[child];
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult buildNode(const google::protobuf::Map<String, Script::SceneSystem::Node> &nodes, const std::string &uuid, SceneNodePtr parent, SceneNodePtr &node)
    {
        TResult ret = T3D_OK;

        auto itr = nodes.find(uuid);
        const Script::SceneSystem::Node &src = itr->second;

        node = T3D_SCENE_MGR.createSceneNode(parent);
        node->setName(src.name());

        return ret;
    }
}
