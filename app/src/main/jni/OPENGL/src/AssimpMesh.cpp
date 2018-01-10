/*
 *  AssimpMesh.cpp
 *  Virtual Vision
 *
 *  Created by Abdallah Dib Abdallah.dib@virtual-vison.net
 *  Copyright 2011 Virtual Vision. All rights reserved.
 *
 */


#include "../include/AssimpMesh.h"
#include "Helpers.h"
#include "myLogger.h"
#include "../include/misc.h"

#ifndef __cplusplus
#define __cplusplus
#endif

#include "../externals/assimp-3.0/include/assimp/color4.h"

namespace vvision
{
    CAssimpMesh::CAssimpMesh()
    :m_pAssimpScene(NULL), m_sMeshName(""), m_pAnimator(NULL)
    {
        Crm = new CacheResourceManager();
    }
    CAssimpMesh::~CAssimpMesh()
    {
        //managed by the resource manager
        delete Crm;

        m_pAssimpScene = NULL;
        
        if(m_pAnimator != NULL)
        {
            delete m_pAnimator;
            m_pAnimator = NULL;
        }
    }
    bool CAssimpMesh::LoadMesh(const std::string FileName)
    {
        m_sMeshName = FileName;

        m_pAssimpScene = Crm->LoadMesh(FileName);

        if(m_pAssimpScene == NULL)
            return false;
        
        // extract mesh groups
        LOG("m_pAssimpScene->mNumMeshes:%d", m_pAssimpScene->mNumMeshes);
        for (uint32 i = 0 ; i < m_pAssimpScene->mNumMeshes ; i++)
        {
            const aiMesh* paiMesh = m_pAssimpScene->mMeshes[i];
            
            if(!ExtractMeshGroup(kPRIMITIVE_TYPE_TRIANGLE, paiMesh))
                return false;
        }

        //extract materials ( groups can share materials
        bool ret = ExtractMaterials(m_pAssimpScene,FileName);

        //extract animations
//        if(m_pAssimpScene->HasAnimations())
//        {
//            m_pAnimator = new AssimpSceneAnimator(m_pAssimpScene);
//        }
        
        return ret;
    }

    bool CAssimpMesh::LoadMesh(CShader* shader, int Type, vector<ORB_SLAM2::MapPoint *> &refMPs)
    {
        bool ret = false;

        switch (Type)
        {
            case 0:
            {
                LOG("1");
                ret = ExtractMeshGroup_InitPoint(shader,kPRIMITIVE_TYPE_POINTS, refMPs);

                LOG("2");
            }
                break;
            case 1:
            {
                LOG("3");
                ExtractMeshGroup_Point(shader,0,refMPs);

                LOG("4");
            }
                break;
        }

        //extract materials ( groups can share materials
//        bool ret = ExtractMaterials();
        return ret;
    }
    
    void CAssimpMesh::DeleteCachedResources()
    {
        Crm->DeleteResource(kRESOURCE_TYPE_MESH, m_sMeshName);

        if(m_pMeshBuffer != NULL)
        {
            std::vector<CMaterial*>& materials = m_pMeshBuffer->GetMaterialsContainerRef();
            
            for(std::vector<CMaterial*>::iterator it = materials.begin(); it != materials.end(); ++it)
            {
                if( (*it)->diffuseTexture != NULL)
                {
                    if( Crm->DeleteResourceByMemoryAddress(kRESOURCE_TYPE_TEXTURE2D, (*it)->diffuseTexture) )
                        (*it)->diffuseTexture = NULL;
                }
                
                if( (*it)->bumpTexture != NULL)
                {
                    if( Crm->DeleteResourceByMemoryAddress(kRESOURCE_TYPE_TEXTURE2D, (*it)->bumpTexture) )
                        (*it)->bumpTexture = NULL;
                }
                
                if( (*it)->detailTexture != NULL)
                {
                    if( Crm->DeleteResourceByMemoryAddress(kRESOURCE_TYPE_TEXTURE2D, (*it)->detailTexture) )
                        (*it)->detailTexture = NULL;
                }
                
                if( (*it)->cubeMapTexture != NULL)
                {
                    if(Crm->DeleteResourceByMemoryAddress(kRESOURCE_TYPE_TEXTURE2D, (*it)->cubeMapTexture) )
                        (*it)->cubeMapTexture = NULL;
                }
            }
        }
        
        m_pAssimpScene = NULL;
    }
    
    bool CAssimpMesh::ExtractMeshGroup(PRIMITIVE_TYPE _PriType, const aiMesh* paiMesh)
    {
        CMeshGroup *group = new CMeshGroup(_PriType);
        group->SetMaterialIndex(paiMesh->mMaterialIndex );//材料索引

        bool hasTangents = paiMesh->HasTangentsAndBitangents();
        bool hasNormals = paiMesh->HasNormals();

   
        std::vector<CGpuVertex> &Vertices = group->GetVertices();
        std::vector<ushort16> &Indices = group->GetIndices();

        const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

        //collect weights on all vertices
        std::vector<std::vector<aiVertexWeight> > weightsPerVertex(paiMesh->mNumVertices);
        for (uint32 a = 0; a < paiMesh->mNumBones; a++) {
            const aiBone* bone = paiMesh->mBones[a];

            if(bone == NULL)
                continue;

            for (uint32 b = 0; b<bone->mNumWeights; b++) {
                weightsPerVertex[bone->mWeights[b].mVertexId].push_back(aiVertexWeight(a, bone->mWeights[b].mWeight));
            }
        }

        //extract vertex attributes ( pos, normal, tex coord, bone indices and weights)
        for (uint32 i = 0 ; i < paiMesh->mNumVertices ; i++)
        {
            //extract vertex pos normal texture coordinates (actually tangent are not extracted)
            const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
            const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
            const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
            const aiVector3D* pTangCoord = &(paiMesh->mTangents[i]);

            //fill vertex attributes
            CGpuVertex v;

            if(pPos != NULL)
                v.pos = vec3f(pPos->x, pPos->y, pPos->z);

            if(pTexCoord != NULL)
                v.texCoord = vec2f(pTexCoord->x, pTexCoord->y);

            if(hasNormals && pNormal != NULL)
                v.normal = vec3f(pNormal->x, pNormal->y, pNormal->z);

            if(hasTangents && pTangCoord != NULL)
                v.tangent = vec3f(pTangCoord->x, pTangCoord->y, pTangCoord->z);

            //extract bone indices and weights
            if(paiMesh->HasBones())
            {
                //assert(weightsPerVertex[i].size() <= 4);
                for(uint32 a = 0; a < weightsPerVertex[i].size(); a++)
                {
                    //fill vertex attributes
                    v.boneIndices[a] = (float32)weightsPerVertex[i][a].mVertexId;
                    v.boneWeights[a] = weightsPerVertex[i][a].mWeight /** 255.f*/;
                }
            }

            //push vertex into buffer
            Vertices.push_back(v);
        }

        //extract indices
        for (uint32 i = 0 ; i < paiMesh->mNumFaces ; i++) {
            const aiFace& Face = paiMesh->mFaces[i];
            //assert(Face.mNumIndices == 3);
            Indices.push_back((ushort16)Face.mIndices[0]);
            Indices.push_back((ushort16)Face.mIndices[1]);
            Indices.push_back((ushort16)Face.mIndices[2]);
        }

        //allocate buffer on gpu
        if(!group->AllocateOnGpuMemory(kGL_BUFFER_USAGE_HINT_STATIC))
        {
            delete group;
            return false;
        }
        
        //create mesh bounding box
//        group->CreateBoundingBox();
        
        //push mesh group
        m_pMeshBuffer->AddGroup(group);
        
        return true;
    }

    bool CAssimpMesh::ExtractMeshGroup_InitPoint(CShader* shader, PRIMITIVE_TYPE _PriType, vector<ORB_SLAM2::MapPoint *> &refMPs)
    {
        CMeshGroup *group = new CMeshGroup(_PriType);

        std::vector<float > &nPos = group->GetPos();
        std::vector<float > &nPosColor = group->GetPosColor();

        set < ORB_SLAM2::MapPoint * > spRefMPs(refMPs.begin(), refMPs.end());

        nPos.clear();
        nPosColor.clear();

        for (set<ORB_SLAM2::MapPoint *>::iterator sit = spRefMPs.begin(), send = spRefMPs.end();sit != send; sit++) {
            if ((*sit)->isBad())
                continue;

            cv::Mat pos = (*sit)->GetWorldPos();

            nPos.push_back(pos.at<float>(0));
            nPos.push_back(pos.at<float>(1));
            nPos.push_back(pos.at<float>(2));

            nPosColor.push_back(0);
            nPosColor.push_back(1);
            nPosColor.push_back(0);
        }

        //allocate buffer on gpu
        if (group->AllocateOnGpuMemory_InitPoint(shader,kGL_BUFFER_USAGE_HINT_DYNAMIC) == false)
        {
            delete group;
            return false;
        }

        //create mesh bounding box
//        group->CreateBoundingBox();

        //push mesh group
        m_pMeshBuffer->AddGroup(group);

        return true;
    }

    void CAssimpMesh::ExtractMeshGroup_Point(CShader* shader, int No, vector<ORB_SLAM2::MapPoint *> &refMPs)
    {
        CMeshGroup* grp = m_pMeshBuffer->GroupAtIndex(No);

        std::vector<float > &nPos = grp->GetPos();

        set < ORB_SLAM2::MapPoint * > spRefMPs(refMPs.begin(), refMPs.end());

        nPos.clear();

        for (set<ORB_SLAM2::MapPoint *>::iterator sit = spRefMPs.begin(), send = spRefMPs.end();sit != send; sit++) {
            if ((*sit)->isBad())
                continue;

            cv::Mat pos = (*sit)->GetWorldPos();

            nPos.push_back(pos.at<float>(0));
            nPos.push_back(pos.at<float>(1));
            nPos.push_back(pos.at<float>(2));
        }

        //allocate buffer on gpu
        grp->AllocateOnGpuMemory_UpdatePoint(shader, kGL_BUFFER_USAGE_HINT_DYNAMIC);
    }

    bool CAssimpMesh::ExtractMaterials(const aiScene* pScene,const std::string FileName)
    {
        LOG("ExtractMaterials:%d", pScene->mNumMaterials);
        // Initialize the materials
        for (uint32 i = 0 ; i < pScene->mNumMaterials ; i++) {
            CMaterial* material = new CMaterial();
            const aiMaterial* pMaterial = pScene->mMaterials[i];
            
            material->diffuseTexture = NULL;
            
            if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString path;
                
                if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
                {
                    // Extract the directory part from the file name
                    // will be used to read the texture
                    std::string textureFullPath = GetDirectoryName(FileName) + "/" + path.data;

                    LOG("Texture:%s",textureFullPath.c_str());
                    material->diffuseTexture = (CTexture*)Crm->LoadTexture2D(textureFullPath);
                }
            }
            
            aiColor4D specular, diffuse, ambient;
            vec4f zero;
            
            //diffuse
            if((AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse)))
                material->diffuse = vec4f(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
            else
                material->diffuse = zero;

            //ambiant
            if((AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient)))
                material->ambient = vec4f(ambient.r, ambient.g, ambient.b, ambient.a);
            else
                material->ambient = zero;

            //specular
            if((AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &specular)))
                material->specular = vec4f(specular.r, specular.g, specular.b, specular.a);
            else
                material->specular = zero;

            //shininess
            aiGetMaterialFloat(pMaterial,AI_MATKEY_SHININESS,&material->shininess);
            if(material->shininess <1.0f)
                material->shininess = 15;

            aiGetMaterialFloat(pMaterial,AI_MATKEY_OPACITY,&material->opacity);
            if(material->opacity< 1.f)
                material->isTransparent = true;
            
            aiGetMaterialInteger(pMaterial,AI_MATKEY_TWOSIDED,&material->twoSided);
            m_pMeshBuffer->AddMaterial(material);
        }
        return true;
    }

    bool CAssimpMesh::ExtractMaterials()
    {
        // Initialize the materials
//        for (uint32 i = 0 ; i < pScene->mNumMaterials ; i++) {
//            CMaterial* material = new CMaterial();
//            const aiMaterial* pMaterial = pScene->mMaterials[i];
//
//            material->diffuseTexture = NULL;
//
//            if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
//                aiString path;
//
//                if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
//                {
//                    // Extract the directory part from the file name
//                    // will be used to read the texture
//                    std::string textureFullPath = GetDirectoryName(FileName) + "/" + path.data;
//
//                    LOG("Texture:%s",textureFullPath.c_str());
//                    material->diffuseTexture = (CTexture*)Crm->LoadTexture2D(textureFullPath);
//                }
//            }
//            m_pMeshBuffer->AddMaterial(material);
//        }
        return true;
    }
}



