package de.kattisoft.emu64;

import de.kattisoft.emu64.ImageManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.app.Activity;
import android.content.pm.ActivityInfo;
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