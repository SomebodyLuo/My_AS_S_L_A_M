/* MeshEntity.h
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

#ifndef VVISION_MeshEntity_h
#define VVISION_MeshEntity_h

#include "vvision.h"
#include <opencv2/opencv.hpp>
#include <ORB_SLAM2/include/MapPoint.h>

class CMeshEntity
{
public:
    
    /** constructor*/
    CMeshEntity(IMesh* mesh);
    
    /** destructor*/
    ~CMeshEntity();
    
    /** update mesh*/
    virtual void Update(float32 dt);
    
    /** render*/
    virtual void Render(CShader* shader, const mat4f &view, const mat4f &projection);
    
    void RenderAndUpdateTex(CShader* shader, const mat4f &view, const mat4f &projection, const cv::Mat& imgMat);

    /** render*/
    virtual void RenderPoint(CShader* shader, const mat4f &view, const mat4f &projection, vector<ORB_SLAM2::MapPoint *> &kMPs, vector<ORB_SLAM2::MapPoint *> &refMPs);

    /**reference to transformation matrix*/
    mat4f& GetTransfromationMatrix() {return m_mTransformationMatrix;}
    
    /** reference to the mesh buffer*/
    CMeshBuffer& GetMeshBuffer() {return *m_pMesh->GetMeshBuffer();}
    
private:
    bool first_Data;
    cv::Mat mImOrigin;
    /** holds mesh buffer*/
    IMesh* m_pMesh;
    
    /** model matrix*/
    mat4f m_mTransformationMatrix;
};

#endif
