/*
 *  MeshGroup.cpp
 *  Virtual Vision
 *
 *  Created by Abdallah Dib Abdallah.dib@virtual-vison.net
 *  Copyright 2011 Virtual Vision. All rights reserved.
 *
 */

#include "../include/MeshBuffer.h"
namespace vvision
{
    CMeshGroup::~CMeshGroup()
    {
        m_vVertices.clear();
        m_vIndices.clear();
        m_iMaterialIndex = -1;
        m_cVboVertices.Destroy(0);
        m_cVboIndices.Destroy(0);
    }
    
    CMeshBuffer::~CMeshBuffer()
    {
        for (int32 i=0; i<groups.size(); i++) {
            delete groups[i];
            groups[i] = NULL;
        }
        groups.clear();
        
        for (int32 i=0; i<materials.size(); i++)
        {
            delete materials[i];
            materials[i] = NULL;
        }
        materials.clear();
    }

    bool CMeshGroup::AllocateOnGpuMemory( GL_BUFFER_USAGE_HINT usage_)
    {
        PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC) eglGetProcAddress("glGenVertexArraysOES");
        PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC) eglGetProcAddress("glBindVertexArrayOES");

        glGenVertexArrays(1, &m_uVao);
        glBindVertexArray(m_uVao);
//                glGenVertexArraysOES(1, &m_uVao);
//                glBindVertexArrayOES(m_uVao);

        bool res;

        res = m_cVboVertices.AllocateStorage(kGL_BUFFER_TYPE_ARRAY, usage_, sizeof(CGpuVertex) * m_vVertices.size(), &m_vVertices[0],0);
        if(!res)
            return false;

        res = m_cVboIndices.AllocateStorage(kGL_BUFFER_TYPE_ELEMENT, usage_, sizeof(ushort16) * m_vIndices.size(), &m_vIndices[0],0);
        if(!res)
            return false;

        m_cVboVertices.Bind(0);
        CVertexBuffer::MapAttribLocation(ATTRIB_VERTEX,         3, kDATA_TYPE_FLOAT, sizeof(CGpuVertex), 0);
        CVertexBuffer::MapAttribLocation(ATTRIB_TEXTURE0,       2, kDATA_TYPE_FLOAT, sizeof(CGpuVertex), (const GLvoid*)12);
//                CVertexBuffer::MapAttribLocation(ATTRIB_NORMAL,         3, kDATA_TYPE_FLOAT, sizeof(CGpuVertex), (const GLvoid*)20);
//                CVertexBuffer::MapAttribLocation(ATTRIB_BONES_INDICES,  4, kDATA_TYPE_FLOAT, sizeof(CGpuVertex), (const GLvoid*)32);
//                CVertexBuffer::MapAttribLocation(ATTRIB_BONES_WEIGHTS,  4, kDATA_TYPE_FLOAT, sizeof(CGpuVertex), (const GLvoid*)48);
//                CVertexBuffer::MapAttribLocation(ATTRIB_TANGENT,        3, kDATA_TYPE_FLOAT, sizeof(CGpuVertex), (const GLvoid*)64);

        m_cVboIndices.Bind(0);
        m_cVboVertices.Unbind();
//                m_cVboIndices.Unbind();

        glBindVertexArray(0);
        //glBindVertexArrayOES(0);

        return true;
    }

    bool CMeshGroup::AllocateOnGpuMemory_InitPoint(CShader* shader, GL_BUFFER_USAGE_HINT usage_)
    {
        PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC) eglGetProcAddress("glGenVertexArraysOES");
        PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC) eglGetProcAddress("glBindVertexArrayOES");

        //vao生成与绑定，使用时，还需再继续绑定vao
        glGenVertexArrays(1, &m_uVao);
        glBindVertexArray(m_uVao);
//                glGenVertexArraysOES(1, &m_uVao);
//                glBindVertexArrayOES(m_uVao);

        LOG("GPU:%d , %d , %f , %f , %f", shader->position, m_vPos.size(),m_vPos[0],m_vPos[1],m_vPos[2]);

        //vbo生成，绑定，赋值
        bool res = m_cVboVertices.AllocateStorage(kGL_BUFFER_TYPE_ARRAY, usage_, sizeof(float) * m_vPos.size(), &m_vPos[0],0);

        if(!res)
        {
            return false;
        }


        res = m_cVboVertices.AllocateStorage(kGL_BUFFER_TYPE_ARRAY, usage_, sizeof(float) * m_vPosColor.size(), &m_vPosColor[0],1);

        if(!res)
        {
            return false;
        }

        m_cVboVertices.Bind(0);
        CVertexBuffer::MapAttribLocation(ATTRIB_VERTEX, 3, kDATA_TYPE_FLOAT, 0, 0);
        m_cVboVertices.Bind(1);
        CVertexBuffer::MapAttribLocation(ATTRIB_COLOR, 3, kDATA_TYPE_FLOAT, 0, 0);

//                m_cVboVertices.Unbind();

        glBindVertexArray(0);
        //glBindVertexArrayOES(0);

        return true;
    }

    bool CMeshGroup::AllocateOnGpuMemory_UpdatePoint(CShader* shader, GL_BUFFER_USAGE_HINT usage_)
    {
        PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC) eglGetProcAddress("glGenVertexArraysOES");
        PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC) eglGetProcAddress("glBindVertexArrayOES");

        glGenVertexArrays(1, &m_uVao);
        glBindVertexArray(m_uVao);
//                glGenVertexArraysOES(1, &m_uVao);
//                glBindVertexArrayOES(m_uVao);

        m_cVboVertices.UpdateContent(0, sizeof(float) * m_vPos.size(), &m_vPos[0], 0);

        m_cVboVertices.Bind(0);
        CVertexBuffer::MapAttribLocation(shader->position, 3, kDATA_TYPE_FLOAT, 0, NULL);

        m_cVboVertices.Unbind();

        glBindVertexArray(0);
        //glBindVertexArrayOES(0);

        return true;
    }

    void CMeshGroup::MapToGPU(uint32 flags)
    {
        PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC) eglGetProcAddress("glBindVertexArrayOES");
        glBindVertexArray(m_uVao);
        //glBindVertexArrayOES(m_uVao);
    }

    void CMeshGroup::UnmapFromGPU()
    {
        PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC) eglGetProcAddress("glBindVertexArrayOES");
        glBindVertexArray(0);
        //glBindVertexArrayOES(0);
    }
}