package jp.saka.memcpyspeed;

import android.app.Activity;
import android.os.Bundle;

public class MemcpySpeedActivity extends Activity
{
	static {
		System.loadLibrary("memcpyspeed");
	}

	public native void memcpyspeedtest();

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		(new MemcpySpeedTestThread()).start();
	}

	class MemcpySpeedTestThread extends Thread {
		public void run() {
			memcpyspeedtest();
		}
	}
}
