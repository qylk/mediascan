package com.qylk.app.provider.media;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.preference.PreferenceManager;

public class SyncOverReceiver extends BroadcastReceiver {

	@Override
	public void onReceive(Context context, Intent intent) {
		SimpleDateFormat sdf = new SimpleDateFormat("yyyy年MM月dd日HH时mm分ss秒",
				Locale.SIMPLIFIED_CHINESE);
		PreferenceManager.getDefaultSharedPreferences(context).edit()
				.putString("lastsync", sdf.format(new Date())).commit();
	}
}
