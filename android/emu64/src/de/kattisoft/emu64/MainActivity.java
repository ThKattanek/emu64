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
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.MotionEvent.PointerCoords;
import android.view.Window;
import android.view.WindowManager;

public class MainActivity extends Activity {

	private GLSurfaceView view;
	private OpenGLRenderer renderer;
	private ImageManager imageMgr;
	private boolean rendererSet = false;
	
	private Builder back;
	
	private PointerCoords coord = new PointerCoords();
	
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
			CopyFileFromAssets("1_pixel_fld.prg",app_data_path);
			CopyFileFromAssets("flexgrid.prg",app_data_path);
			CopyFileFromAssets("plexer_dycp.prg",app_data_path);
		}
		
		NativeClass.SetSDExtPath(app_data_path);
		
		// Vollbild
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		// Ständig Landscape
		 setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		// Ständig Portrait
		//setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		
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
	
	/*
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event){
		super.onKeyDown(keyCode, event);
	if (keyCode == KeyEvent.KEYCODE_MENU) {
		NativeClass.Menue();
	}
	
	if (keyCode == KeyEvent.KEYCODE_HOME) {
		NativeClass.Home();
		return false;
	}
	
		return true;
	}
	*/
	
	@Override
    public boolean onKeyDown(int keyCode, KeyEvent event){
		super.onKeyDown(keyCode, event);

		if (keyCode == KeyEvent.KEYCODE_MENU) {
			NativeClass.Menue();
		}	
		
    if ((keyCode == KeyEvent.KEYCODE_BACK)) {

//----------------------------------------------------------------
// erstellt einen Alert Dialog
//----------------------------------------------------------------        

        back = new AlertDialog.Builder(this);
        //back.setIcon(R.drawable.alert); //verlinkt Icon aus drawable Ordner
        back.setTitle("Sie haben die Zurück Taste gedrückt");
        back.setMessage("Möchten Sie Emu64 wirklich beenden?");

        back.setPositiveButton("Ja", new DialogInterface.OnClickListener() 
        {

            public void onClick(DialogInterface dialog, int whichButton) 
            {
            	finish();
            }

        });

        back.setNegativeButton("Nein", new DialogInterface.OnClickListener() 
        {
            public void onClick(DialogInterface dialog, int whichButton) {
            }

        });

        back.show();
    }
        return true;
}

	public boolean onTouchEvent(MotionEvent event)
	{
		super.onTouchEvent(event);
		
		Integer idx = event.getActionIndex();
		Integer id = event.getPointerId(idx);
		Integer action = event.getAction();
		
		switch(action & MotionEvent.ACTION_MASK)
		{
		case MotionEvent.ACTION_DOWN:
		case MotionEvent.ACTION_POINTER_DOWN:
			event.getPointerCoords(idx, coord);
			NativeClass.TouchDown(id, coord.x, coord.y);
			break;
		case MotionEvent.ACTION_UP:
		case MotionEvent.ACTION_POINTER_UP:
			NativeClass.TouchUp(id, coord.x, coord.y);
			break;
		}
		return true;
	}
	
}