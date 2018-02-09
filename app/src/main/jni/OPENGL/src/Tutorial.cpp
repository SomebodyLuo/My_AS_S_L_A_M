/*
 *  Tutorial.cpp
 *  Virtual Vision
 *
 *  Created by Abdallah Dib Abdallah.dib@virtual-vison.net
 *  Copyright 2011 Virtual Vision. All rights reserved.
 *
 */

#include "Tutorial.h"
#include <android/log.h>
#define LOG_TAG "ORB_SLAM_TRACK"

#define LOG(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

namespace vvision {

    Tutorial::Tutorial()
            : m_pCamera(NULL), m_pMesh(NULL), m_pShaderMesh(NULL), m_pCamMesh(NULL), m_pPointMesh(NULL),
              m_pShadowMesh(NULL) {
        Crm = new CacheResourceManager();
    }

    Tutorial::~Tutorial() {
        delete  Crm;
        Cleanup();
    }

    void Tutorial::Cleanup() {
        //this will delete all cached resources ( textures, shaders and meshes)
        SAFE_DELETE(m_pCamera);
        SAFE_DELETE(m_pMesh);
        SAFE_DELETE(m_pCamMesh);
        SAFE_DELETE(m_pPointMesh);
        SAFE_DELETE(m_pShadowMesh);
    }

    bool Tutorial::LoadShaders() {
        //load shader.
        m_pShaderMesh = Crm->LoadShader("Resources/Shaders/nolighting");
        return (m_pShaderMesh != NULL);
    }

    bool Tutorial::LoadEntities() {
        //static mesh
        IMesh *assimpMesh = IMesh::LoadMeshFromFile("Resources/Leather_Arm_Chair/Leather_Arm_Chair");
        IMesh *assimpCamMesh = IMesh::LoadMeshFromFile("Resources/Camera/camera");
        IMesh *assimpShadowMesh = IMesh::LoadMeshFromFile("Resources/Shadow/fakeshadow");
        IMesh *assimpPointMesh = IMesh::LoadMeshFromPoint();

        if (assimpMesh == NULL || assimpCamMesh == NULL || assimpShadowMesh == NULL || assimpPointMesh == NULL)
            return false;

        m_pMesh = new CMeshEntity(assimpMesh);
        m_pCamMesh = new CMeshEntity(assimpCamMesh);
        m_pShadowMesh = new CMeshEntity(assimpShadowMesh);
        m_pPointMesh = new CMeshEntity(assimpPointMesh);

        //m_pCamera
        m_pCamera = new CCameraEntity(vec3f(0.0f, 0.0f, 2.0f), vec3f(0.0f, 0.0f, 0.0f), 60.f);

        return true;
    }

    void printGLString(const char *name, GLenum s) {
        const char *v = (const char *) glGetString(s);
        LOG("new GL %s = %s\n", name, v);
    }

    bool Tutorial::Deploy() {
        /************verify opengl***************/
        printGLString("Version", GL_VERSION);
        printGLString("Vendor", GL_VENDOR);
        printGLString("Renderer", GL_RENDERER);
        printGLString("Extensions", GL_EXTENSIONS);

        /************Shaders***************/
        if (!LoadShaders()) {
            Cleanup();
            return false;
        }

        // luoyouren
        /************Model***************/
        if (!LoadEntities()) {
            Cleanup();
            return false;
        }

        //setup opengl
        glClearColor(1.0, 0.0, 0.0, 1.f);
        //enable depth test an back face culling
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        LOG("Deploy Finish");
        return true;
    }

    void Tutorial::RenderFromPosition(const mat4f &view, const mat4f &projection) {
        //activate shader
        m_pShaderMesh->Begin();
        m_pMesh->Render(m_pShaderMesh, view, projection);
        mat4f shadowView = view * mat4f::createTranslation(0.0, -1.3, 0.0);
        shadowView = shadowView * mat4f::createScale(5.0, 5.0, 5.0);
        m_pShadowMesh->Render(m_pShaderMesh, shadowView, projection);
        //disable shader
        m_pShaderMesh->End();

        //roate model arround vertical axis
//    static float angle = 0.f;
//    mat4f & model = m_pMesh->GetTransfromationMatrix();
//    model = mat4f::createRotationAroundAxis(0, angle, angle);
//    angle += 5.5f;

//    mat4f & model = m_pMesh->GetTransfromationMatrix();
//    model = mat4f::createScale(2.0, 2.0, 2.0);
    }

    void Tutorial::RenderCamera(const mat4f &view, const cv::Mat& imgData) {
        m_pShaderMesh->Begin();
        static mat4f viewMat, projMat;
        projMat = mat4f::createOrtho(-1, 1, -1, 1, -100, 100);
        m_pCamMesh->RenderAndUpdateTex(m_pShaderMesh, viewMat, projMat, imgData);
        m_pShaderMesh->End();
    }

    void Tutorial::RenderPoints(const mat4f &view, const mat4f &projection, vector<ORB_SLAM2::MapPoint *> &kMPs, vector<ORB_SLAM2::MapPoint *> &refMPs) {
        m_pShaderMesh->Begin();
        m_pPointMesh->RenderPoint(m_pShaderMesh, view, projection, kMPs, refMPs);

        m_pShaderMesh->End();
    }

    void Tutorial::Frame(const cv::Mat& imgData, const mat4f &view, const mat4f &proj, vector<ORB_SLAM2::MapPoint *> &kMPs, vector<ORB_SLAM2::MapPoint *> &refMPs) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderCamera(view, imgData);
        RenderFromPosition(view, proj);
        RenderPoints(view, proj, kMPs, refMPs);
    }
}