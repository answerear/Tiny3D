/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#ifndef __T3D_SG_MODEL_H__
#define __T3D_SG_MODEL_H__


#include "SceneGraph/T3DSGNode.h"
#include "Render/T3DHardwareVertexBuffer.h"


namespace Tiny3D
{
    class T3D_ENGINE_API SGModel : public SGNode
    {
    public:
        enum RenderMode
        {
            E_RENDER_UNKNOWN = 0,
            E_RENDER_ENTITY,
            E_RENDER_SKELETON,
            E_RENDER_BOUND,
        };

        struct ActionInfo
        {
            String  mName;          /// 动作名称
            int64_t mDuration;      /// 该动作动画持续时间
            int32_t mTotalFrames;   /// 该动作动画总共关键帧帧数
        };

        typedef std::vector<ActionInfo>     ActionList;
        typedef ActionList::iterator        ActionListItr;
        typedef ActionList::const_iterator  ActionListConstItr;

        static SGModelPtr create(const String &modelName, uint32_t unID = E_NID_AUTOMATIC);

        virtual ~SGModel();

        void setRenderMode(RenderMode mode);

        virtual Type getNodeType() const override;

        virtual NodePtr clone() const override;

        /**
         * @brief 枚举该模型所有的动画动作
         * @param [out] actions : 动画动作列表
         * @return void
         */
        void enumerateActionList(ActionList &actions);

        /**
         * @brief 按照指定名称运行动画动作
         * @param [in] name : 动作名称
         * @return 调用成功，返回true
         */
        bool runAction(const String &name, bool repeat = true, int32_t frame = 0);

        bool stopAction(const String &name);

        bool isActionRunning(const String &name) const;
        bool isActionRunning() const    { return mIsActionRunning; }

    protected:
        SGModel(uint32_t unID = E_NID_AUTOMATIC);

        virtual bool init(const String &modelName);

        virtual void updateTransform() override;

        void updatePoses();
        void updatePose(int64_t time, ObjectPtr skeleton);
        void updateSkeletons();
        void updateSkins();
        void updateSkinData(ObjectPtr data, VertexDataPtr vertexData);
        void updateSkinVertex(ObjectPtr buffer, void *vertex, const VertexElement &posElem, const VertexElement &weightElem, const VertexElement &indicesElem);

        bool getVertexElement(ObjectPtr buffer, VertexElement::Semantic semantic, VertexElement &element);

        VertexDataPtr createVertexData(ObjectPtr data);
        bool createSkeletons();
        bool createNodes();

        bool searchMesh(const String &meshName, const String &submeshName, SGMeshPtr &mesh);

        void enableRenderingEntities();
        void enableRenderingSkeletons();

    protected:
        typedef std::vector<VertexDataPtr>      VertexDataList;
        typedef VertexDataList::iterator        VerticesDataItr;
        typedef VertexDataList::const_iterator  VerticesDataConstItr;

        typedef std::vector<SGMeshPtr>          MeshList;
        typedef MeshList::iterator              MeshListItr;
        typedef MeshList::const_iterator        MeshListConstItr;

        typedef std::vector<SGBonePtr>          BoneList;
        typedef BoneList::iterator              BoneListItr;
        typedef BoneList::const_iterator        BoneListConstItr;

        typedef std::vector<SGTransformNodePtr> NodeList;
        typedef NodeList::iterator              NodeListItr;
        typedef NodeList::const_iterator        NodeListConstItr;

        ModelPtr        mModel;
        RenderMode      mRenderMode;

        VertexDataList  mVertexDataList;
        BoneList        mBones;
        NodeList        mNodes;
        MeshList        mMeshes;

        SGBonePtr       mRootBone;

        SGSkeletonPtr   mSkeleton;

        int64_t         mStartTime;

        int32_t         mCurKeyFrameT;
        int32_t         mCurKeyFrameR;
        int32_t         mCurKeyFrameS;

        String          mCurActionName;
        bool            mIsActionRunning;
        bool            mIsLoop;

        ObjectPtr       mCurActionData;
    };
}


#endif  /*__T3D_SG_MODEL_H__*/
