package orb.slam2.android;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;
import orb.slam2.android.nativefunc.OrbNdkHelper;

import com.example.castoryan.orb.R;

public class MainActivity extends Activity implements OnClickListener{

	private static final String TAG = "ORB_SLAM_TRACK";
	Button datasetMode,cameraMode,changeMode;
	public static OrbNdkHelper indtouchNDK = null;
	private String VOCPath,TUMPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);//隐藏标题
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
          WindowManager.LayoutParams.FLAG_FULLSCREEN);//设置全屏
        setContentView(R.layout.activity_main);
        datasetMode=(Button)findViewById(R.id.dataset_mode);
        cameraMode=(Button)findViewById(R.id.camera_mode);
		changeMode=(Button)findViewById(R.id.changeTxt2Bin);
        datasetMode.setOnClickListener(this);
        cameraMode.setOnClickListener(this);
		changeMode.setOnClickListener(this);

		if(indtouchNDK == null)
		{
			indtouchNDK = new OrbNdkHelper();
		}
    }

    @Override
	public void onDestroy(){
		super.onDestroy();
		if(indtouchNDK != null) {
			indtouchNDK = null;
		}
	}


	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch(v.getId()){
		case R.id.dataset_mode:
			startActivity(new Intent(MainActivity.this,DataSetModeActivity.class));
			break;
		case R.id.camera_mode:
			//Toast.makeText(MainActivity.this, "on the way...", Toast.LENGTH_LONG).show();
			//startActivity(new Intent(MainActivity.this,CameraModeActivity.class));
			TUMPath = "/storage/emulated/0/Movies/KITTI03.yaml";
			VOCPath = "/storage/emulated/0/Movies/ORBvoc.bin";
			if(!TextUtils.isEmpty(TUMPath)&&!TextUtils.isEmpty(VOCPath)){
				Bundle bundle=new Bundle();
				bundle.putString("voc",VOCPath );
				bundle.putString("calibration",TUMPath );
				Intent intent =new Intent(this,ORBSLAMForCameraActivity.class);
				intent.putExtras(bundle);
				startActivity(intent);
				finish();
			}else{
				Toast.makeText(this, "None of image path or Calibration path can be empty!", Toast.LENGTH_LONG).show();
			}
			break;
		case R.id.changeTxt2Bin:
		VOCPath = "/storage/emulated/0/Movies/ORBvoc.txt";

		Log.i(TAG,"VOCPath: "+VOCPath);

		if(!TextUtils.isEmpty(VOCPath)) {
			OrbNdkHelper.ChangeTxt2Bin(VOCPath);
		}
		break;
		}
	}
}
