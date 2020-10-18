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


#ifndef __T3D_FBX_READER_H__
#define __T3D_FBX_READER_H__


#include "T3DConverterPrerequisites.h"


namespace Tiny3D
{
    class FBXReader;

    T3D_DECLARE_SMART_PTR(FBXReader);

    class FBXReader : public ModelReader
    {
    public:
        static FBXReaderPtr create();

        virtual ~FBXReader();

    protected:
        FBXReader();

        virtual TResult parse(DataStream &stream, Model *model) override;

        TResult initFbxObjects();
        TResult destroyFbxObjects();

        TResult importFbxScene(DataStream &stream, FbxScene *pFbxScene);
        TResult setupMetricSystem(FbxScene *pFbxScene);

        TResult processFbxScene(FbxScene *pFbxScene, Model *model);

        TResult processFbxNode(FbxNode *pFbxNode);
        TResult processFbxMesh(FbxNode *pFbxNode, FbxMesh *pFbxMesh);
        TResult processFbxMeshAttributes(FbxMesh *pFbxMesh);
        TResult processFbxMeshData(FbxMesh *pFbxMesh);
        TResult processFbxMaterial(FbxNode *pFbxNode, FbxMesh *pFbxMesh);
        TResult processFbxCamera(FbxNode *pFbxNode);
        TResult processFbxLight(FbxNode *pFbxNode);

        TResult processFbxAnimation(FbxScene *pFbxScene);
        TResult processFbxAnimation(FbxAnimStack *pFbxAnimStack, FbxNode *pFbxNode, int32_t idx);
        TResult processFbxAnimation(FbxAnimLayer *pFbxAnimLayer, FbxNode *pFbxNode, int32_t idx);
        TResult processFbxAnimationChannels(FbxAnimLayer *pFbxAnimLayer, FbxNode *pFbxNode);
        TResult processFbxAnimationCurve(FbxAnimCurve *pFbxAnimCurve);

        TResult processFbxBindPose(FbxScene *pFbxScene);

        TResult processFbxSkin(FbxGeometry *pFbxGeometry);

        int InterpolationFlagToIndex(int flags);
        int ConstantmodeFlagToIndex(int flags);
        int TangentmodeFlagToIndex(int flags);
        int TangentweightFlagToIndex(int flags);
        int TangentVelocityFlagToIndex(int flags);

		void convertMatrix(const FbxAMatrix &src, Matrix4 &dst);
        void convertMatrix(const FbxMatrix &src, Matrix4 &dst);

        TResult readPosition(FbxMesh *pFbxMesh, int32_t ctrlPointIdx,
            Vector3 &pos);
        TResult readColor(FbxMesh *pFbxMesh, int32_t ctrlPointIdx, 
            int32_t vertexIdx, int32_t layer, ColorRGBA &color);
        TResult readUV(FbxMesh *pFbxMesh, int32_t ctrlPointIdx, int32_t uvIdx,
            int32_t layer, Vector2 &uv);
        TResult readNormal(FbxMesh *pFbxMesh, int32_t ctrlPointIdx,
            int32_t vertexIdx, int32_t layer, Vector3 &normal);
        TResult readBinormal(FbxMesh *pFbxMesh, int32_t ctrlPointIdx,
            int32_t vertexIdx, int32_t layer, Vector3 &binormal);
        TResult readTangent(FbxMesh *pFbxMesh, int32_t ctrlPointIdx,
            int32_t vertexIdx, int32_t layer, Vector3 &tangent);

    protected:
        FbxManager  *mFbxManager;
        size_t      mTabCount;
    };
}


#endif  /*__T3D_FBX_READER_H__*/

