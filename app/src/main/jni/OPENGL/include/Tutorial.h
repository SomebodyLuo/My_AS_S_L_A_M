/* Tutorial.h
 *
 * Copyright (C) 2012 Abdallah DIB.
 * All rights reserved. Email: Abdallah.dib@virtual-vison.net
 * Web: <http://www.virutal-vision.net/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef vvision_Tutorial_h
#define vvision_Tutorial_h

#include "vvision.h"
#include "CameraEntity.h"
#include "MeshEntity.h"
#include <gl3stub.h>
#include <opencv2/opencv.hpp>
#include <native_app_glue/android_native_app_glue.h>
#include "CacheResourceManager.h"
#include <ORB_SLAM2/include/MapPoint.h>
//#include <vector>

namespace vvision
{
    class Tutorial
    {
    public:

        /** constructor*/
        Tutorial();

        /** destructor*/
        ~Tutorial();

        /** deploy tutorial content*/
        bool Deploy();

        void Frame(const cv::Mat& imgData, const mat4f& view, const mat4f& proj, vector<ORB_SLAM2::MapPoint *> &kMPs, vector<ORB_SLAM2::MapPoint *> &refMPs);

    private:

        /** cleanup*/
        void Cleanup();

        /** load shaders*/
        bool LoadShaders();

        /** load entities*/
        bool LoadEntities();

        /**render the scene from different position*/
        //渲染模型
        void RenderPoints(const mat4f &view, const mat4f &projection, vector<ORB_SLAM2::MapPoint *> &kMPs, vector<ORB_SLAM2::MapPoint *> &refMPs);

        //渲染模型
        void RenderFromPosition(const mat4f& view, const mat4f& projection);

        //渲染图像背景
        void RenderCamera(const mat4f &view, const cv::Mat& imgData);

        /** shaders*/
        CShader *m_pShaderMesh;

        /** m_pCamera entity*/
        CCameraEntity* m_pCamera;

        /** 3d point*/
        CMeshEntity* m_pPointMesh;

        /** mesh*/
        CMeshEntity* m_pMesh;

        /** camera mesh*/
        CMeshEntity* m_pCamMesh;

        /** shadow mesh*/
        CMeshEntity* m_pShadowMesh;

        CacheResourceManager* Crm;
    };
}


#endif
