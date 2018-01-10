package orb.slam2.android.nativefunc;

import android.content.res.AssetManager;
import android.provider.Settings;

/**
 *
 * @author buptzhaofang@163.com Mar 26, 2016 8:48:13 PM
 *
 */
public class OrbNdkHelper {

	static{
		System.loadLibrary("mono");
	}
	/**
	 * jni中初始化SLAM系统
	 * @param VOCPath
	 * @param calibrationPath
	 */
	public static native void initSystemWithParameters(String VOCPath,String calibrationPath,AssetManager assetManager, String pathToInternalDir);
	public static native void Uninit();

	public static native int[] startCurrentORBForCamera(double curTimeStamp,long addr,int w,int h);

	public static native void glesInit();
	public static native void glesRender(long addr);
    public static native void glesResize(int width, int height);

	public static native void ChangeTxt2Bin(String VOCPath);
}
