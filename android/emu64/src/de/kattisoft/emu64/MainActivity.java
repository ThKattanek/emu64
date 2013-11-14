package de.kattisoft.emu64;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

import de.kattisoft.emu64.ImageManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

public class MainActivity extends Activity {

	private GLSurfaceView view;
	private OpenGLRenderer renderer;
	private ImageManager imageMgr;
	private boolean rendererSet = false;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		// Daten Verzeichnis anlegen wenn es e
		Context context = getApplicationContext();
		String packageName = context.getPackageName();
		File externalPath = Environment.getExternalStorageDirectory();
		String app_data_path = externalPath.getAbsolutePath() + "/Android/data/" + packageName;
		File appFiles = new File(app_data_path);
		
		boolean isCreatet = false;
		
		if (!(appFiles.exists() && appFiles.isDirectory())) 
        {
            if (!appFiles.mkdir())
            {
            	Log.v("emu64","Emu64 Daten Verzeichnis konnte nicht erstellt werden.");
            }
            else
            {
        		app_data_path += "/files";
        		appFiles = new File(app_data_path);
        		if (!(appFiles.exists() && appFiles.isDirectory())) 
                {
	                if (!appFiles.mkdir()) 
	                {
	                	Log.v("emu64","Emu64 files Verzeichnis konnte nicht erstellt werden.");
	                }
	                else isCreatet = true;
                }
            }
        }
		
		app_data_path = externalPath.getAbsolutePath() + "/Android/data/" + packageName + "/files";
		
		if(isCreatet)
		{	
			// Daten aus Assets nach "files"
			CopyFileFromAssets("1541.rom",app_data_path);
			CopyFileFromAssets("basic.rom",app_data_path);
			CopyFileFromAssets("kernal.rom",app_data_path);
			CopyFileFromAssets("char.rom",app_data_path);
		}
		
		NativeClass.SetSDExtPath(app_data_path);
		
		// Vollbild
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		view = new GLSurfaceView(this);
		renderer = new OpenGLRenderer();
		
		view.setRenderer(renderer);
		
		imageMgr = new ImageManager();
		imageMgr.setAssetManager(this.getAssets());
		
		rendererSet = true;
		
		setContentView(view);
	}
	
	private void CopyFileFromAssets(String filename, String dest_path) {
	    AssetManager assetManager = this.getAssets();

	    InputStream in = null;
	    OutputStream out = null;
	    try {
	        in = assetManager.open(filename);
	        String newFileName = dest_path + "/" + filename;
	        out = new FileOutputStream(newFileName);

	        byte[] buffer = new byte[1024];
	        int read;
	        while ((read = in.read(buffer)) != -1) {
	            out.write(buffer, 0, read);
	        }
	        in.close();
	        in = null;
	        out.flush();
	        out.close();
	        out = null;
	    } catch (Exception e) {
	        Log.e("tag", e.getMessage());
	    }

	}
	
	@Override
	protected void onPause() {
	    super.onPause();
	 
	    if (rendererSet) {
	        NativeClass.Pause();
	    }
	}
	 
	@Override
	protected void onResume() {
	    super.onResume();
	 
	    if (rendererSet) {
	    	NativeClass.Resume();
	    }
	}
	
	@Override
	protected void onDestroy(){
		super.onDestroy();
		NativeClass.Destroy();
	}
}