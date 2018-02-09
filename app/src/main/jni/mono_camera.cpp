#include "orb_slam2_android_MyNDK.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include<chrono>
#include <errno.h>
#include <android/asset_manager_jni.h>
#include<opencv2/core/core.hpp>

#include<GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "System.h"
#include "myJNIHelper.h"

using namespace cv;

#define WM_SendNums 32

static ORB_SLAM2::System *s;
bool init_end = false;
MyJNIHelper * gHelperObject=NULL;


void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOG("new GL %s = %s\n", name, v);
}


JNIEXPORT void JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_initOPENGL
(JNIEnv *env, jclass cls, jint w, jint h)
{
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
}

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    initSystemWithParameters
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_initSystemWithParameters
(JNIEnv *env, jclass cls, jstring VOCPath, jstring calibrationPath,jobject assetManager,jstring pathToInternalDir) {
    const char *calChar = env->GetStringUTFChars(calibrationPath, JNI_FALSE);
    const char *vocChar = env->GetStringUTFChars(VOCPath, JNI_FALSE);
    // use your string
    std::string voc_string(vocChar);
    std::string cal_string(calChar);

    LOG("000%s , %s",vocChar,calChar);
    env->GetJavaVM(&jvm);
    jvm->AttachCurrentThread(&env, NULL);
    gHelperObject = new MyJNIHelper(env, assetManager, pathToInternalDir);

    s=new ORB_SLAM2::System(voc_string, cal_string,ORB_SLAM2::System::MONOCULAR,true);
    env->ReleaseStringUTFChars(calibrationPath, calChar);
    env->ReleaseStringUTFChars(VOCPath, vocChar);
    init_end = true;
}

JNIEXPORT jintArray JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_Uninit
        (JNIEnv *, jclass) {
    if (gHelperObject != NULL) {
        delete gHelperObject;
    }
    gHelperObject = NULL;
}

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    glesInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_glesInit
(JNIEnv *env, jclass cls) {
    s->InitGL();
}

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    glesRender
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_glesRender
(JNIEnv * env, jclass cls, jlong addr) {
    cv::Mat *im = (cv::Mat *) addr;
    if(init_end)
    s->drawGL(*im);
}

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    glesResize
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_glesResize
(JNIEnv *env, jclass cls, jint width, jint height) {

    //GLfloat fovy = 45.0f,  aspect = 640.0f/480.0f, zNear = 0.1f, zFar = 100.0f;
    //
    //GLfloat top = zNear * ((GLfloat) tan(fovy * 3.1415f / 360.0));
    //GLfloat bottom = -top;
    //GLfloat left = bottom * aspect;
    //GLfloat right = top * aspect;
    //glFrustumf(left, right, bottom, top, zNear, zFar);
    //
    //// 选择模型观察矩阵
    //glMatrixMode(GL_MODELVIEW);
    //
    //// 重置模型观察矩阵
    //glLoadIdentity();
}

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    readShaderFile
 * Signature: (Landroid/content/res/AssetManager;)V
 */
JNIEXPORT jintArray JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_startCurrentORBForCamera
        (JNIEnv *env, jclass cls,jdouble timestamp, jlong addr,jint w,jint h)
{
    const cv::Mat *im = (cv::Mat *) addr;

    imwrite("/storage/emulated/0/Movies/1.bmp",*im);
    cv::Mat ima = s->TrackMonocular(*im, timestamp);

    int iSize = ima.rows * ima.cols;
    jintArray resultArray = env->NewIntArray(iSize);
    int* pDst = new int[iSize];
    for (int i = 0; i < iSize; i++)
    {
    unsigned char* pSrc = ima.ptr() + i * 3;
    pDst[i] = 0xff000000 | (pSrc[0] << 16) | (pSrc[1] << 8) | pSrc[2];
    }
    env->SetIntArrayRegion(resultArray, 0, iSize, (jint*) pDst);
    delete [] pDst;
    //env->ReleaseIntArrayElements(resultArray, resultPtr, 0);
    return resultArray;
}

JNIEXPORT jintArray JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_ChangeTxt2Bin
        (JNIEnv *env, jclass cls,jstring VOCPath)
{
    const char *vocChar = env->GetStringUTFChars(VOCPath, JNI_FALSE);
    std::string voc_string(vocChar);
    LOG("000:%s",vocChar);
    s=new ORB_SLAM2::System(vocChar);
    env->ReleaseStringUTFChars(VOCPath, vocChar);
}





