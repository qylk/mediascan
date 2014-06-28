package com.qylk.app.provider.media.test;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.qylk.app.provider.media.MEDIA;
import com.qylk.app.provider.media.MEDIALIB.LIBRARY;
import com.qylk.app.provider.media.R;

public class AdminPage extends Activity {
	private SharedPreferences preference;
	private BroadcastReceiver receiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			Toast.makeText(context, "更新完毕", Toast.LENGTH_LONG).show();
		}
	};

	private OnClickListener synclistener = new OnClickListener() {

		@Override
		public void onClick(View v) {
			sendBroadcast(new Intent(MEDIA.SCAN_ACTION));
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		IntentFilter ift = new IntentFilter(MEDIA.INTENT_SCAN_DONE);
		registerReceiver(receiver, ift);

		TextView info = (TextView) findViewById(R.id.info);
		StringBuffer sb = new StringBuffer();
		sb.append("上次同步在：");
		preference = PreferenceManager.getDefaultSharedPreferences(this);
		sb.append(preference.getString("lastsync", "--")).append("\n");
		Cursor c = getContentResolver().query(LIBRARY.URI,
				new String[] { "count(" + LIBRARY.FIELD_ID + ")" }, null, null,
				null);
		if (c != null && c.getCount() != 0) {
			c.moveToFirst();
			sb.append("数据库拥有：");
			sb.append(c.getInt(0)).append("条记录\n");
			c.close();
		}
		info.setText(sb.toString());
		Button sync = (Button) findViewById(R.id.sync);
		sync.setOnClickListener(synclistener);
	}

	@Override
	protected void onDestroy() {
		unregisterReceiver(receiver);
		super.onDestroy();
	}

}
