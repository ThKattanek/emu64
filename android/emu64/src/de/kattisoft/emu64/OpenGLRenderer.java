package de.kattisoft.emu64;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView.Renderer;

public class OpenGLRenderer implements Renderer {

	@Override
	public void onDrawFrame(GL10 gl) {
		NativeClass.Renderer();
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		NativeClass.Resize(width, height);
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		NativeClass.Init();
	}

}
