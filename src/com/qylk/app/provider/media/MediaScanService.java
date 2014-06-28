package com.qylk.app.provider.media;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.PowerManager;

import com.qylk.app.provider.media.utils.MediaScaner;

public class MediaScanService extends Service {
	private static final String TAG = "MediaScannerService";
	static final String SCAN_ACTION = "com.qylk.app.provider.media.scanservice";
	private PowerManager.WakeLock mWakeLock;
	private boolean processing;
	private ServiceHandler mHander;

	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}

	@Override
	public void onCreate() {
		super.onCreate();
		PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);
		if (!Environment.getExternalStorageState().equals(
				Environment.MEDIA_MOUNTED)) {
			stopSelf();
			return;
		}
		mHander = new ServiceHandler();
		mHander.sendEmptyMessage(0);
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		if (processing)
			return Service.START_NOT_STICKY;
		else
			return Service.START_REDELIVER_INTENT;
	}

	private final class ServiceHandler extends Handler {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			mWakeLock.acquire();
			processing = true;
			MediaScaner scanner = new MediaScaner(MediaScanService.this);
			scanner.scanDirectory(Environment.getExternalStorageDirectory()
					.getAbsolutePath());
			processing = false;
			mWakeLock.release();
			sendBroadcast(new Intent(MEDIA.INTENT_SCAN_DONE));
			stopSelf();
		}
	}

}
