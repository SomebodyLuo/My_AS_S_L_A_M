/* AssimpMesh.h
 *
 * Virtual Vision Engine . Copyright (C) 2012 Abdallah DIB.
 * All rights reserved. Email: Abdallah.dib@virtual-vison.net
 * Web: <http://www.virutal-vision.net/>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ASSIMP_MESH_INCLUDED
#define ASSIMP_MESH_INCLUDED


#include "Mesh.h"
#include "AssimpSceneAnimator.h"
#include "CacheResourceManager.h"


namespace vvision
{
    
    class CAssimpMesh: public IMesh
    {
        
    public:
        
        /** constructor*/
        CAssimpMesh();
        
        /** destructor*/
        ~CAssimpMesh();
        
        /** clear/delete cached resources*/
        void DeleteCachedResources();
        
        /** get animator*/
        inline AssimpSceneAnimator* GetAnimator() {return m_pAnimator;}
    protected:
        /**load assimp mesh
         @retrun true if success
         */
        virtual bool LoadMesh(const std::string FileName);
        virtual bool LoadMesh(CShader* shader, int Type, vector<ORB_SLAM2::MapPoint *> &refMPs);

    private:
        /** not allowed*/
        CAssimpMesh& operator=(const CAssimpMesh& mesh);

        /** init mesh*/
        bool ExtractMeshGroup_InitPoint(CShader* shader, PRIMITIVE_TYPE _PriType, vector<ORB_SLAM2::MapPoint *> &refMPs);
        void ExtractMeshGroup_Point(CShader* shader, int No, vector<ORB_SLAM2::MapPoint *> &refMPs);

        /** init mesh*/
        bool ExtractMeshGroup(PRIMITIVE_TYPE _PriType, const aiMesh* paiMesh);

        /** init materials*/
        bool ExtractMaterials(const aiScene* pScene,const std::string FileName);
        bool ExtractMaterials();
        aiScene* m_pAssimpScene;
        
        string m_sMeshName;
        
        /** assim mesh animator*/
        AssimpSceneAnimator *m_pAnimator;

        CacheResourceManager* Crm;
    };
}
#endif