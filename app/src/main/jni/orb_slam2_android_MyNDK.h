
#include <jni.h>
/* Header for class orb_slam2_android_MyNDK */

#ifndef _Included_orb_slam2_android_MyNDK
#define _Included_orb_slam2_android_MyNDK
#ifdef __cplusplus
extern "C" {
#endif

JavaVM* jvm;

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    initSystemWithParameters
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_initSystemWithParameters
(JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    startCurrentORB
 * Signature: (D[III)[I
 */
JNIEXPORT jintArray JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_startCurrentORB
        (JNIEnv *, jclass, jdouble, jintArray, jint, jint);

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    glesInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_glesInit
(JNIEnv *, jclass);

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    glesRender
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_glesRender
(JNIEnv *, jclass);

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    glesResize
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_glesResize
(JNIEnv *, jclass, jint, jint);

/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    readShaderFile
 * Signature: (Landroid/content/res/AssetManager;)V
 */
JNIEXPORT jintArray JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_startCurrentORBForCamera
        (JNIEnv *, jclass,jdouble, jlong,jint,jint);
/*
 * Class:     orb_slam2_android_nativefunc_OrbNdkHelper
 * Method:    readShaderFile
 * Signature: (Landroid/content/res/AssetManager;)V
 */
JNIEXPORT jintArray JNICALL Java_orb_slam2_android_nativefunc_OrbNdkHelper_ChangeTxt2Bin
        (JNIEnv *, jclass,jstring);

#ifdef __cplusplus
}
#endif
#endif
