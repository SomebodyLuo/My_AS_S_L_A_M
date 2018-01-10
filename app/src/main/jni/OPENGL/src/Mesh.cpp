/*
 *  Mesh.cpp
 *  Virtual Vision
 *
 *  Created by Abdallah Dib Abdallah.dib@virtual-vison.net
 *  Copyright 2011 Virtual Vision. All rights reserved.
 *
 */


#include "../include/Mesh.h"
#include "AssimpMesh.h"
#include "myJNIHelper.h"
#include "AssimpMesh.h"
#include "Helpers.h"

namespace vvision
{
    IMesh* IMesh::LoadMeshFromFile(const string meshName)
    {
        std::string objFilename,mtlFilename,texFilename;
//        bool isFilesPresent  = gHelperObject->ExtractAssetReturnFilename(meshName, objFilename);

        std::string objFil,mtlFile,texFile;
        objFil = mtlFile = texFile = meshName;
        StringManipulator::AddCharArrayToString(objFil, ".obj");
        StringManipulator::AddCharArrayToString(mtlFile, ".mtl");
        StringManipulator::AddCharArrayToString(texFile, ".png");

        bool isFilesPresent  = gHelperObject->ExtractAssetReturnFilename(objFil, objFilename) &&
                                gHelperObject->ExtractAssetReturnFilename(mtlFile, mtlFilename) &&
                                gHelperObject->ExtractAssetReturnFilename(texFile, texFilename);
        if( !isFilesPresent ) {
            LOG("Model %s does not exist!", objFilename.c_str());
            return NULL;
        }

        IMesh* mesh = new CAssimpMesh();

        if(!mesh->LoadMesh(objFilename))
        {
            delete mesh;
            return NULL;
        }

        return mesh;
    }

    IMesh* IMesh::LoadMeshFromPoint()
    {
        IMesh* mesh = new CAssimpMesh();

        return mesh;
    }
}