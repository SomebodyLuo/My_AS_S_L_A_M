/**
 * This file is part of ORB-SLAM2.
 *
 * Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
 * For more information see <https://github.com/raulmur/ORB_SLAM2>
 *
 * ORB-SLAM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ORB-SLAM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "MapDrawer.h"
#include "MapPoint.h"
#include "KeyFrame.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include<GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <mutex>
#include <android/log.h>
#define LOG_TAG "ORB_SLAM_TRACK"

#define LOG(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)

namespace ORB_SLAM2
{
    MapDrawer::MapDrawer(Map *pMap, const string &strSettingPath) :
            mpMap(pMap){
        cv::FileStorage fSettings(strSettingPath, cv::FileStorage::READ);

        mKeyFrameSize = fSettings["Viewer.KeyFrameSize"];
        mKeyFrameLineWidth = fSettings["Viewer.KeyFrameLineWidth"];
        mGraphLineWidth = fSettings["Viewer.GraphLineWidth"];
        mPointSize = fSettings["Viewer.PointSize"];
        mCameraSize = fSettings["Viewer.CameraSize"];
        mCameraLineWidth = fSettings["Viewer.CameraLineWidth"];

        float staticfloat[] =
                {0.3, 0, 0, 0,
                 0, 0.3, 0, 0,
                 0, 0, 0.3, 0,
                 0, 0, 0, 1};

        mStaticPose = cv::Mat(4,4,CV_32F,staticfloat);
    }

    MapDrawer::~MapDrawer()
    {
        delete mpTutorial;
    }

    void  MapDrawer::InitGL()
    {
        mpTutorial = new vvision::Tutorial();

        mpTutorial->Deploy();
    }

    void  MapDrawer::NewDraw(cv::Mat& OImg)
    {
        if (!mCameraPose.empty()) {
            cv::Mat R;
            cv::Mat T;

            R = mCameraPose.rowRange(0, 3).colRange(0, 3);
            T = mCameraPose.rowRange(0, 3).col(3);

            float qx, qy, qz, qw;
            qw = sqrt(1.0 + R.at<float>(0, 0) + R.at<float>(1, 1) + R.at<float>(2, 2)) / 2.0;
            qx = (R.at<float>(2, 1) - R.at<float>(1, 2)) / (4 * qw);
            qy = -(R.at<float>(0, 2) - R.at<float>(2, 0)) / (4 * qw);
            qz = -(R.at<float>(1, 0) - R.at<float>(0, 1)) / (4 * qw);

            vec4f r1(1 - 2 * qy * qy - 2 * qz * qz, 2 * qx * qy + 2 * qz * qw,
                     2 * qx * qz - 2 * qy * qw, 0);
            vec4f r2(2 * qx * qy - 2 * qz * qw, 1 - 2 * qx * qx - 2 * qz * qz,
                     2 * qy * qz + 2 * qx * qw, 0);
            vec4f r3(2 * qx * qz + 2 * qy * qw, 2 * qy * qz - 2 * qx * qw,
                     1 - 2 * qx * qx - 2 * qy * qy, 0);
            vec4f r4(T.at<float>(0), -T.at<float>(1), -T.at<float>(2), 1);

            float pose[16] = {r1.x, r1.y, r1.z, r1.w,
                              r2.x, r2.y, r2.z, r2.w,
                              r3.x, r3.y, r3.z, r3.w,
                              r4.x, r4.y, r4.z, r4.w};

            mat4f poseMatrix(pose);
            mat4f scaleMatrix = mat4f::createScale(0.03, 0.03, 0.03);
            poseMatrix = poseMatrix * scaleMatrix;

            mat4f projMatrix = mat4f::createPerspective(60.0, 640.0 / 480.0, 0.02, 100);

            mat4f newModelView;
            newModelView = poseMatrix/* * _translationX * _translationY * _translationZ * _rotationX * _rotationY * _rotationZ * _scale*/;

            vector<MapPoint *> vpMPs = mpMap->GetAllMapPoints();
            vector<MapPoint *> vpRefMPs = mpMap->GetReferenceMapPoints();

            //draw frame
            mpTutorial->Frame(OImg, newModelView, projMatrix,vpMPs,vpRefMPs);
        }
    }

    void MapDrawer::DrawMapPoints() {
        const vector<MapPoint *> &vpMPs = mpMap->GetAllMapPoints();
        const vector<MapPoint *> &vpRefMPs = mpMap->GetReferenceMapPoints();

        set < MapPoint * > spRefMPs(vpRefMPs.begin(), vpRefMPs.end());

        if (vpMPs.empty())
            return;

        for (int i = 0, iend = vpMPs.size(); i < iend; i++) {
            if (vpMPs[i]->isBad() || spRefMPs.count(vpMPs[i]))
                continue;
            cv::Mat pos = vpMPs[i]->GetWorldPos();

            GLfloat vertexArray[3] = {pos.at<float>(0), pos.at<float>(1), pos.at<float>(2)};
        }

        for (set<MapPoint *>::iterator sit = spRefMPs.begin(), send = spRefMPs.end();sit != send; sit++) {
            if ((*sit)->isBad())
                continue;
            cv::Mat pos = (*sit)->GetWorldPos();
            GLfloat vertexArray[] = {pos.at<float>(0), pos.at<float>(1), pos.at<float>(2)};
        }
    }

    void MapDrawer::SetCurrentCameraPose(const cv::Mat &Tcw) {
        unique_lock <mutex> lock(mMutexCamera);
        mCameraPose = Tcw.clone();
    }
} //namespace ORB_SLAM
