package orb.slam2.android;


import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.Image;
import android.media.ImageReader;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.example.castoryan.orb.R;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import orb.slam2.android.nativefunc.OrbNdkHelper;
import orb.slam2.android.CameraUtil;


public class MainActivity extends Activity implements CvCameraViewListener2, OnClickListener, SensorEventListener {

    private static final String TAG = "ORB_SLAM_TRACK";
    Button changeMode;
    String VOCPath, TUMPath;
    private CameraBridgeViewBase mOpenCvCameraView;

    private SensorManager mSensorManager;
    private Sensor mRotation;
    final private float[] temporaryQuaternion = new float[4];

    private static final int INIT_FINISHED = 0x00010001;
    private ImageView imgDealed;

    private Bitmap resultImg;
    private double timestamp;

    private long addr;
    private int w, h;
    private boolean isSLAMRunning = true;

    private LinearLayout linear;

    private ORBSLAMForCameraActivity mView;

    private Mat mYuv;
    private Mat mRgba;


    public static CameraManager mCameraManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);//隐藏标题
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);//设置全屏
        setContentView(R.layout.activity_main);
        changeMode = (Button) findViewById(R.id.changeTxt2Bin);
        changeMode.setOnClickListener(this);

//		DisplayMetrics dm = new DisplayMetrics();
//		getWindowManager().getDefaultDisplay().getMetrics(dm);//display = getWindowManager().getDefaultDisplay();display.getMetrics(dm)（把屏幕尺寸信息赋值给DisplayMetrics dm）;
//		int width = dm.widthPixels;
//		int height = dm.heightPixels;
//		float xdpi = dm.xdpi;
//		float ydpi = dm.ydpi;
//		int density = dm.densityDpi;
//		float fdensity = dm.density;
//
//		Log.i(TAG, "onCreate: width="+width+"// height="+height +"// xdpi = "+xdpi
//				+"// ydp= "+ydpi + "// density = "+density +"// fdensity="+fdensity);

//		mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.tutorial1_activity_java_surface_view);
//		mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
//		mOpenCvCameraView.setMaxFrameSize(640,480);
//		mOpenCvCameraView.setCvCameraViewListener(this);


        // Create the camera manager
        mCameraManager = new CameraManager(this);

        imgDealed = (ImageView) findViewById(R.id.img_dealed);

        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        //todo 判断是否存在rotation vector sensor
        mRotation = mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);

        linear = (LinearLayout) findViewById(R.id.surfaceLinear);

        mView = new ORBSLAMForCameraActivity(this);
        linear.addView(mView);

        mView.assetManager = getAssets();
        mView.pathToInternalDir = getFilesDir().getAbsolutePath();
        Log.i(TAG, "pathToInternalDir = " + mView.pathToInternalDir);

        new Thread(new Runnable() {
            @Override
            public void run() {
                // TODO Auto-generated method stub
                while (true) {
                    if (mView.InitFinish == true) {
                        mView.InitFinish = false;

                        myHandler.sendEmptyMessage(INIT_FINISHED);

                        break;
                    }
                }
            }
        }).start();
    }

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.i(TAG, "OpenCV loaded successfully");
//					mOpenCvCameraView.enableView();


                    mYuv = new Mat(480 + (480 / 2), 640, CvType.CV_8UC1);
                    mRgba = new Mat();
                }
                break;
                default: {
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    Handler myHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case INIT_FINISHED:
                    //Toast.makeText(ORBSLAMForCameraActivity.this,"init has been finished!",Toast.LENGTH_LONG).show();
                    new Thread(new Runnable() {

                        @Override
                        public void run() {
                            while (isSLAMRunning) {
                                if (w != 0) {
                                    timestamp = (double) System.currentTimeMillis() / 1000.0;
                                    // TODO Auto-generated method stub
                                    int[] resultInt = OrbNdkHelper.startCurrentORBForCamera(timestamp, addr, w, h);
                                    resultImg = Bitmap.createBitmap(resultInt, w, h, Bitmap.Config.ARGB_8888);
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            // TODO Auto-generated method stub
                                            imgDealed.setImageBitmap(resultImg);
                                        }
                                    });
                                }
                            }
                        }
                    }).start();
                    break;
            }
            super.handleMessage(msg);
        }
    };

    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
        mView.onResume();
//		mSensorManager.registerListener(this,mRotation, SensorManager.SENSOR_DELAY_GAME);

//		OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_10, this, mLoaderCallback);

        //添加一下代码即可    （mLoaderCallback为BaseLoaderCallback对象）
        if (OpenCVLoader.initDebug()) {
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);

            if (mCameraManager != null) {
                // Start the background thread
                mCameraManager.startBackgroundThread();
                // Open the camera
                // This should take care of the permissions requests

                mCameraManager.openCamera(640, 480);
            }

        } else {
            Log.i(TAG, "OpenCVLoader.initDebug() failed!");
        }


    }

    @Override
    protected void onStart() {
        // TODO Auto-generated method stub
        super.onStart();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

//		mView.onPause();
        mSensorManager.unregisterListener(this);

//		if (mOpenCvCameraView != null)
//			mOpenCvCameraView.disableView();

        if (mCameraManager != null) {
            // Stop background thread
            mCameraManager.stopBackgroundThread();
            // Close our camera, note we will get permission errors if we try to reopen
            // And we have not closed the current active camera
            mCameraManager.closeCamera();
        }
    }


    @Override
    public void onClick(View v) {
        // TODO Auto-generated method stub
        switch (v.getId()) {
            case R.id.changeTxt2Bin:
                VOCPath = "/storage/emulated/0/Movies/ORBvoc.txt";

                Log.i(TAG, "VOCPath: " + VOCPath);

                if (!TextUtils.isEmpty(VOCPath)) {
                    OrbNdkHelper.ChangeTxt2Bin(VOCPath);
                }
                break;
        }
    }

    @Override
    public void onSensorChanged(SensorEvent event) {

        if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {
            // Process rotation vector (just safe it)
            // Calculate angle. Starting with API_18, Android will provide this value as event.values[3], but if not, we have to calculate it manually.
            SensorManager.getQuaternionFromVector(temporaryQuaternion, event.values);

            //Log.i(TAG,"rotation:"+ temporaryQuaternion[0]+","+temporaryQuaternion[1]+","+temporaryQuaternion[2]+","+temporaryQuaternion[3]);

        } else if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {

            float axisX = event.values[0];
            float axisY = event.values[1];
            float axisZ = event.values[2];
            float axisT = event.values[3];

            //Log.i(TAG,"scope:"+ axisX+","+axisY+","+axisZ+","+axisT);
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }

    public void onCameraViewStarted(int width, int height) {
    }

    public void onCameraViewStopped() {
    }

    //没用到了
    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
        Mat im = inputFrame.rgba();
        synchronized (im) {
            mView.imagedata = addr = im.getNativeObjAddr();
        }
        w = im.cols();
        h = im.rows();
        return inputFrame.rgba();
    }


    // Taken from OpenCamera project
    // URL: https://github.com/almalence/OpenCamera/blob/master/src/com/almalence/opencam/cameracontroller/Camera2Controller.java#L3455
    public final ImageReader.OnImageAvailableListener imageAvailableListener = new ImageReader.OnImageAvailableListener() {

        @Override
        public void onImageAvailable(ImageReader ir) {

            // Get the next image from the queue
            Image img = ir.acquireNextImage();

            Mat im = imageToMat(img);

            synchronized (im) {
                mView.imagedata = addr = im.getNativeObjAddr();
            }

            w = im.cols();
            h = im.rows();

            img.close();
        }
    };

    /**
     * @param vertexs float 数组
     * @return 获取浮点形缓冲数据
     */
    public static FloatBuffer getFloatBuffer(float[] vertexs) {
        FloatBuffer buffer;
        ByteBuffer qbb = ByteBuffer.allocateDirect(vertexs.length * 4);
        qbb.order(ByteOrder.nativeOrder());
        buffer = qbb.asFloatBuffer();
        //写入数组
        buffer.put(vertexs);
        //设置默认的读取位置
        buffer.position(0);
        return buffer;
    }

    public Mat imageToMat(Image image) {
        ByteBuffer buffer;
        int rowStride;
        int pixelStride;
        int width = image.getWidth();
        int height = image.getHeight();
        int offset = 0;

        Image.Plane[] planes = image.getPlanes();
        byte[] data = new byte[image.getWidth() * image.getHeight() * ImageFormat.getBitsPerPixel(ImageFormat.YUV_420_888) / 8];
        byte[] rowData = new byte[planes[0].getRowStride()];

        for (int i = 0; i < planes.length; i++) {
            buffer = planes[i].getBuffer();
            rowStride = planes[i].getRowStride();
            pixelStride = planes[i].getPixelStride();
            int w = (i == 0) ? width : width / 2;
            int h = (i == 0) ? height : height / 2;
            for (int row = 0; row < h; row++) {
                int bytesPerPixel = ImageFormat.getBitsPerPixel(ImageFormat.YUV_420_888) / 8;
                if (pixelStride == bytesPerPixel) {
                    int length = w * bytesPerPixel;
                    buffer.get(data, offset, length);

                    // Advance buffer the remainder of the row stride, unless on the last row.
                    // Otherwise, this will throw an IllegalArgumentException because the buffer
                    // doesn't include the last padding.
                    if (h - row != 1) {
                        buffer.position(buffer.position() + rowStride - length);
                    }
                    offset += length;
                } else {

                    // On the last row only read the width of the image minus the pixel stride
                    // plus one. Otherwise, this will throw a BufferUnderflowException because the
                    // buffer doesn't include the last padding.
                    if (h - row == 1) {
                        buffer.get(rowData, 0, width - pixelStride + 1);
                    } else {
                        buffer.get(rowData, 0, rowStride);
                    }

                    for (int col = 0; col < w; col++) {
                        data[offset++] = rowData[col * pixelStride];
                    }
                }
            }
        }

        // Finally, create the Mat.
//		mYuv = new Mat(height + height / 2, width, CvType.CV_8UC1);

        synchronized (this) {
            mYuv.put(0, 0, data);
        }

//        Mat im = new Mat();
//
        Imgproc.cvtColor(mYuv, mRgba, Imgproc.COLOR_YUV2RGB_I420);

        return mRgba;
    }
}
