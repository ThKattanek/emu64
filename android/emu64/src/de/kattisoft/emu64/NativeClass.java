package de.kattisoft.emu64;

public class NativeClass {
	static
	{
		System.loadLibrary("emu64");
	}
	public static native void Init();
	public static native void Resize(int xw, int yw);
	public static native void Renderer();
	public static native void Pause();
	public static native void Resume();
	public static native void Destroy();
}
