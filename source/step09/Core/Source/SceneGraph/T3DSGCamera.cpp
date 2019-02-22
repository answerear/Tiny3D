/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
 * Copyright (C) 2015-2019  Answer Wong
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


#include "SceneGraph/T3DSGCamera.h"
#include "SceneGraph/T3DSceneManager.h"
#include "Render/T3DViewport.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SGCameraPtr SGCamera::create(ID uID /* = E_NID_AUTOMATIC */)
    {
        SGCameraPtr camera = new SGCamera(uID);
        camera->release();
        return camera;
    }

    //--------------------------------------------------------------------------

    SGCamera::SGCamera(ID uID /* = E_NID_AUTOMATIC */)
        : SGTransform3D(uID)
    {

    }

    //--------------------------------------------------------------------------

    SGCamera::~SGCamera()
    {

    }

    //--------------------------------------------------------------------------

    void SGCamera::renderScene(ViewportPtr viewport)
    {
        mViewport = viewport;
        T3D_SCENE_MGR.renderScene(this);
        mViewport = nullptr;
    }

    //--------------------------------------------------------------------------

    void SGCamera::lookAt(const Vector3 &pos, const Vector3 &obj, 
        const Vector3 &up)
    {
        Vector3 N = obj - pos;
        N.normalize();
        Vector3 V = up;
        V.normalize();
        Vector3 U = N.cross(V);
        U.normalize();
        V = U.cross(N);

        setPosition(pos);

        Matrix3 mat;
        mat.setColumn(0, U);
        mat.setColumn(1, V);
        mat.setColumn(2, -N);
        Quaternion orientation;
        orientation.fromRotationMatrix(mat);
        setOrientation(orientation);

        setScaling(Vector3::UNIT_SCALE);
    }
}
