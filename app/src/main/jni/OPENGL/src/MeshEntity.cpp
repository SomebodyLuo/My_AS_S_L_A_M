/*
 *  MeshEntity.cpp
 *  Virtual Vision
 *
 *  Created by Abdallah Dib Abdallah.dib@virtual-vison.net
 *  Copyright 2011 Virtual Vision. All rights reserved.
 *
 */

#include "../include/MeshEntity.h"
#include <android/log.h>
#define LOG_TAG "ORB_SLAM_TRACK"

#define LOG(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

CMeshEntity::CMeshEntity(IMesh* mesh)
: m_pMesh(mesh)
{
    //m_pMesh = mesh->GetMesh();
    first_Data = false;
}

CMeshEntity::~CMeshEntity()
{
    SAFE_DELETE(m_pMesh);
}

void CMeshEntity::Update(float32 dt)
{
}
void CMeshEntity::Render(CShader* shader, const mat4f &view, const mat4f &projection)
{
    assert ( shader != NULL );
    
    //send modelViewprojection matrix to the current shader
    if(shader->matprojviewmodel != -1 )
    {
        mat4f mat = projection * view * m_mTransformationMatrix;
        glUniformMatrix4fv(shader->matprojviewmodel, 1, GL_FALSE, &mat[0]);
    }
    
    //send model matrix if the shader needs
    if(shader->matmodel != -1 )
    {
        glUniformMatrix4fv(shader->matmodel, 1, GL_FALSE, &m_mTransformationMatrix[0]);
    }
    
    if (shader->matnormal != -1) {
        mat4f matNormal = (view * m_mTransformationMatrix).inverse().transpose();
        glUniformMatrix4fv(shader->matnormal, 1, GL_FALSE, &matNormal[0]);
    }
    
    //this part can be merged with a renderer class
    //get the mesh buffer
    CMeshBuffer* buffer = m_pMesh->GetMeshBuffer();
    
    //go through all groups in the mesh buffer
    for(uint32 g = 0; g < buffer->GroupsCount(); ++g)
	{
        //render each group
		CMeshGroup* grp = buffer->GroupAtIndex(g);
        
        //get corresponding material for each group
        CMaterial* material = buffer->MaterialForGroup(grp);

        //the following code can be merged with a renderer class
        //map material to shader context, this can be merged in a renderer class, where the renderer go through all material properties and set the appropriate GL state and shader uniform, for ex, bump texture, detail texture, (diffuse, ambient, specular properties ), face culling, depth write, transparent materials (blending) etc...
        if(material != NULL )
        {
            if( material->diffuseTexture != NULL && shader->texture0 != -1)
            {
                material->diffuseTexture->ActivateAndBind(GL_TEXTURE0);
                //shader->SetUniform1i("texture0", 0, shader->texture0);
                //or
                glUniform1i(shader->texture0, 0);
            }
        }
        
        //enable vertex array object (vao)
        grp->MapToGPU(0);

        //perform GL draw for each group
        glDrawElements(grp->GetDrawingMode(), grp->GetIndices().size(), GL_UNSIGNED_SHORT, 0);

        grp->UnmapFromGPU();
    }
}

void CMeshEntity::RenderPoint(CShader* shader, const mat4f &view, const mat4f &projection, vector<ORB_SLAM2::MapPoint *> &kMPs, vector<ORB_SLAM2::MapPoint *> &refMPs)
{
    assert ( shader != NULL );

    //send modelViewprojection matrix to the current shader
    if(shader->matprojviewmodel != -1 )
    {
        mat4f mat = projection * view * m_mTransformationMatrix;
        glUniformMatrix4fv(shader->matprojviewmodel, 1, GL_FALSE, &mat[0]);
    }

    //send model matrix if the shader needs
    if(shader->matmodel != -1 )
    {
        glUniformMatrix4fv(shader->matmodel, 1, GL_FALSE, &m_mTransformationMatrix[0]);
    }

    if (shader->matnormal != -1) {
        mat4f matNormal = (view * m_mTransformationMatrix).inverse().transpose();
        glUniformMatrix4fv(shader->matnormal, 1, GL_FALSE, &matNormal[0]);
    }

    if (first_Data == false)
    {
        first_Data = true;

        if (m_pMesh->LoadMesh(shader, 0, refMPs) == false)
        {
            LOG("RenderPoint333");
        }
    }
    else
    {

    }

    //this part can be merged with a renderer class
    //get the mesh buffer
    CMeshBuffer* buffer = m_pMesh->GetMeshBuffer();

    //go through all groups in the mesh buffer
    for(uint32 g = 0; g < buffer->GroupsCount(); ++g)
    {
        //render each group
        CMeshGroup* grp = buffer->GroupAtIndex(g);

//        //get corresponding material for each group
//        CMaterial* material = buffer->MaterialForGroup(grp);
//
//        //the following code can be merged with a renderer class
//        //map material to shader context, this can be merged in a renderer class, where the renderer go through all material properties and set the appropriate GL state and shader uniform, for ex, bump texture, detail texture, (diffuse, ambient, specular properties ), face culling, depth write, transparent materials (blending) etc...
//        if(material != NULL )
//        {
//            if( material->diffuseTexture != NULL && shader->texture0 != -1)
//            {
//                material->diffuseTexture->ActivateAndBind(GL_TEXTURE0);
//                //shader->SetUniform1i("texture0", 0, shader->texture0);
//                //or
//
//                material->diffuseTexture->ActivateAndRefresh(mImOrigin.data, mImOrigin.cols, mImOrigin.rows);
//
//
//                glUniform1i(shader->texture0, 0);
//            }
//        }

        //enable vertex array object (vao)
        grp->MapToGPU(0);

        //perform GL draw for each group

        glDrawArrays(grp->GetDrawingMode(), 0, grp->GetPos().size() / 3);
//        glDrawElements(grp->GetDrawingMode(), grp->GetPos().size()/3, GL_UNSIGNED_SHORT, 0);

        grp->UnmapFromGPU();
    }
}


void CMeshEntity::RenderAndUpdateTex(CShader* shader, const mat4f &view, const mat4f &projection, const cv::Mat& imgMat)
{
    assert ( shader != NULL );

    //send modelViewprojection matrix to the current shader
    if(shader->matprojviewmodel != -1 )
    {
        mat4f mat = projection * view * m_mTransformationMatrix;
        glUniformMatrix4fv(shader->matprojviewmodel, 1, GL_FALSE, &mat[0]);
    }

    //send model matrix if the shader needs
    if(shader->matmodel != -1 )
    {
        glUniformMatrix4fv(shader->matmodel, 1, GL_FALSE, &m_mTransformationMatrix[0]);
    }

    if (shader->matnormal != -1) {
        mat4f matNormal = (view * m_mTransformationMatrix).inverse().transpose();
        glUniformMatrix4fv(shader->matnormal, 1, GL_FALSE, &matNormal[0]);
    }

    //this part can be merged with a renderer class
    //get the mesh buffer
    CMeshBuffer* buffer = m_pMesh->GetMeshBuffer();

    //go through all groups in the mesh buffer
    for(uint32 g = 0; g < buffer->GroupsCount(); ++g)
    {
        //render each group
        CMeshGroup* grp = buffer->GroupAtIndex(g);

        //get corresponding material for each group
        CMaterial* material = buffer->MaterialForGroup(grp);

        //the following code can be merged with a renderer class
        //map material to shader context, this can be merged in a renderer class, where the renderer go through all material properties and set the appropriate GL state and shader uniform, for ex, bump texture, detail texture, (diffuse, ambient, specular properties ), face culling, depth write, transparent materials (blending) etc...
        if(material != NULL )
        {
            if( material->diffuseTexture != NULL && shader->texture0 != -1)
            {
                //激活纹理
                material->diffuseTexture->ActivateAndBind(GL_TEXTURE0);
                mImOrigin = imgMat;

                cv::cvtColor(mImOrigin,mImOrigin,CV_BGRA2BGR);

                material->diffuseTexture->ActivateAndRefresh(mImOrigin.data, mImOrigin.cols, mImOrigin.rows);
                //shader->SetUniform1i("texture0", 0, shader->texture0);
                //or
                glUniform1i(shader->texture0, 0);
            }
        }

        //enable vertex array object (vao)
        grp->MapToGPU(0);

        //perform GL draw for each group
        glDrawElements(grp->GetDrawingMode(), grp->GetIndices().size(), GL_UNSIGNED_SHORT, 0);
        grp->UnmapFromGPU();
    }

}
