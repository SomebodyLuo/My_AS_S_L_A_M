LOCAL_PATH := $(call my-dir)


###########################    import opencv     #############################

#### libopencv_core.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := opencv_core
LOCAL_SRC_FILES := ./lib/opencv/libopencv_core.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### libopencv_features2d.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := opencv_features2d
LOCAL_SRC_FILES := ./lib/opencv/libopencv_features2d.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### libopencv_highgui.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := opencv_highgui
LOCAL_SRC_FILES := ./lib/opencv/libopencv_highgui.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### libopencv_flann.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := opencv_flann
LOCAL_SRC_FILES := ./lib/opencv/libopencv_flann.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### libopencv_imgproc.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := opencv_imgproc
LOCAL_SRC_FILES := ./lib/opencv/libopencv_imgproc.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### libopencv_calib3d.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := opencv_calib3d
LOCAL_SRC_FILES := ./lib/opencv/libopencv_calib3d.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)


#### libtbb.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := tbb
LOCAL_SRC_FILES := ./lib/3rdparty/libtbb.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### libIlmImf.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := libIlmlmf
LOCAL_SRC_FILES := ./lib/3rdparty/libIlmImf.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### liblibjasper.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := libjasper
LOCAL_SRC_FILES := ./lib/3rdparty/liblibjasper.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### liblibjpeg.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := libjpeg
LOCAL_SRC_FILES := ./lib/3rdparty/liblibjpeg.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### liblibpng.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := libpng
LOCAL_SRC_FILES := ./lib/3rdparty/liblibpng.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)

#### liblibtiff.a ####
include $(CLEAR_VARS)
LOCAL_MODULE := libtiff
LOCAL_SRC_FILES := ./lib/3rdparty/liblibtiff.a
LOCAL_EXPORT_C_INCLUDES := ./include
include $(PREBUILT_STATIC_LIBRARY)


#### assimp ####
include $(CLEAR_VARS)
LOCAL_MODULE := assimp
LOCAL_SRC_FILES := ./OPENGL/externals/assimp-3.0/libs/armeabi-v7a/libassimp.so
LOCAL_EXPORT_C_INCLUDES := ./OPENGL/externals/assimp-3.0/include
include $(PREBUILT_SHARED_LIBRARY)


# 遍历目录及子目录的函数
define walk
$(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef

#### native_app_glue ####
include $(CLEAR_VARS)
# 遍历Classes目录
ALLFILES = $(call walk, ./native_app_glue)
# 从所有文件中提取出所有.cpp文件
FILE_LIST := $(filter %.cpp, $(ALLFILES))

LOCAL_MODULE:= android_native_app_glue
LOCAL_SRC_FILES := $(FILE_LIST:./%=%)
LOCAL_EXPORT_C_INCLUDES := ./native_app_glue
LOCAL_EXPORT_LDLIBS := -llog -landroid
LOCAL_EXPORT_LDFLAGS := -u ANativeActivity_onCreate

include $(BUILD_STATIC_LIBRARY)


#### cpufeatures ####
include $(CLEAR_VARS)
# 遍历Classes目录
ALLFILES = $(call walk, ./cpufeatures)
# 从所有文件中提取出所有.cpp文件
FILE_LIST := $(filter %.cpp, $(ALLFILES))
LOCAL_MODULE := cpufeatures
LOCAL_SRC_FILES := $(FILE_LIST:./%=%)
LOCAL_CFLAGS := -Wall -Wextra -Werror
LOCAL_EXPORT_C_INCLUDES := ./cpufeatures
LOCAL_EXPORT_LDLIBS := -ldl
include $(BUILD_STATIC_LIBRARY)


#### ndk_helper ####
include $(CLEAR_VARS)
# 遍历Classes目录
ALLFILES = $(call walk, ./ndk_helper)
# 从所有文件中提取出所有.cpp文件
FILE_LIST := $(filter %.cpp, $(ALLFILES))
LOCAL_MODULE:= ndk_helper
LOCAL_SRC_FILES:= $(FILE_LIST:./%=%)

LOCAL_EXPORT_C_INCLUDES := ./ndk_helper
#LOCAL_EXPORT_LDLIBS    := -llog -landroid -lEGL -lGLESv2

LOCAL_STATIC_LIBRARIES := cpufeatures android_native_app_glue

include $(BUILD_STATIC_LIBRARY)


#### Main File mono_camera.cpp ####
include $(CLEAR_VARS)
LOCAL_MODULE := mono
#LOCAL_CPPFLAGS  += -std=c++11
LOCAL_LDLIBS    := -llog -landroid -lz -lEGL -lGLESv2
#-lGLESv1_CM opengl es1.0
#-lGLESv2    opengl es2.0
#  -lEGL

###############  SHARED_OPENCV  ################

LOCAL_STATIC_LIBRARIES := ndk_helper
LOCAL_STATIC_LIBRARIES += cpufeatures android_native_app_glue NDKHelper


LOCAL_SHARED_LIBRARIES := opencv_features2d
LOCAL_SHARED_LIBRARIES += opencv_highgui
LOCAL_SHARED_LIBRARIES += opencv_imgproc
LOCAL_SHARED_LIBRARIES += opencv_core
LOCAL_SHARED_LIBRARIES += opencv_flann
LOCAL_SHARED_LIBRARIES += opencv_calib3d

LOCAL_SHARED_LIBRARIES += libIlmlmf
LOCAL_SHARED_LIBRARIES += libjasper
LOCAL_SHARED_LIBRARIES += libjpeg
LOCAL_SHARED_LIBRARIES += libpng
LOCAL_SHARED_LIBRARIES += libtiff
LOCAL_SHARED_LIBRARIES += tbb
LOCAL_SHARED_LIBRARIES += assimp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/OPENGL/externals/assimp-3.0/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/OPENGL/externals/glm-0.9.7.5/glm
LOCAL_C_INCLUDES += $(LOCAL_PATH)/OPENGL/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ORB_SLAM2/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Thirdparty
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ndk_helper
LOCAL_C_INCLUDES += $(LOCAL_PATH)/native_app_glue
LOCAL_C_INCLUDES += $(LOCAL_PATH)/cpufeatures

# 遍历Classes目录
ALLFILES = $(call walk, $(LOCAL_PATH)/Thirdparty/DBoW2)
# 从所有文件中提取出所有.cpp文件
FILE_LIST := $(filter %.cpp, $(ALLFILES))

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

# 遍历Classes目录
ALLFILES = $(call walk, $(LOCAL_PATH)/Thirdparty/g2o/g2o)
# 从所有文件中提取出所有.cpp文件
FILE_LIST := $(filter %.cpp, $(ALLFILES))

LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

# 遍历Classes目录
ALLFILES = $(call walk, $(LOCAL_PATH)/OPENGL/externals/glm-0.9.7.5/glm)
# 从所有文件中提取出所有.cpp文件
FILE_LIST := $(filter %.cpp, $(ALLFILES))

LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

# 遍历Classes目录
ALLFILES = $(call walk, $(LOCAL_PATH)/ORB_SLAM2/src)
# 从所有文件中提取出所有.cpp文件
FILE_LIST := $(filter %.cpp, $(ALLFILES))

LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

# 遍历Classes目录
ALLFILES = $(call walk, $(LOCAL_PATH)/OPENGL/src)
# 从所有文件中提取出所有.cpp文件
FILE_LIST := $(filter %.cpp, $(ALLFILES))

LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES += ./mono_camera.cpp

#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

include $(BUILD_SHARED_LIBRARY)

####################################################################################################