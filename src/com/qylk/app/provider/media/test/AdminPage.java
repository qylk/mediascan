package com.qylk.app.provider.media.test;

import java.io.File;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.qylk.app.provider.media.DataBaseHelper;
import com.qylk.app.provider.media.MEDIA;
import com.qylk.app.provider.media.MEDIALIB;
import com.qylk.app.provider.media.MEDIALIB.LIBRARY;
import com.qylk.app.provider.media.R;
import com.qylk.app.provider.media.utils.FileOperationUtils;

public class AdminPage extends Activity {
	private SharedPreferences preference;
	private BroadcastReceiver receiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			Toast.makeText(context, "扫描完毕", Toast.LENGTH_LONG).show();
		}
	};

	private OnClickListener scanlistener = new OnClickListener() {

		@Override
		public void onClick(View v) {
			sendBroadcast(new Intent(MEDIA.SCAN_ACTION));
		}
	};
	private OnClickListener updatelistener = new OnClickListener() {

		@Override
		public void onClick(View v) {
			if (!Environment.getExternalStorageState().equals(
					Environment.MEDIA_MOUNTED)) {
				Toast.makeText(AdminPage.this, "SD卡未挂载!", Toast.LENGTH_SHORT)
						.show();
				return;
			}
			boolean suc = updateDB(new File(Environment
					.getExternalStorageDirectory().getAbsolutePath()
					+ "/QMUSIC/database/update.db"));
			Toast.makeText(getApplicationContext(), suc ? "升级成功" : "升级失败",
					Toast.LENGTH_SHORT).show();
		}
	};

	private boolean updateDB(File newdbfile) {
		if (!newdbfile.exists())
			return false;
		try {
			SQLiteDatabase newdb = SQLiteDatabase.openDatabase(
					newdbfile.getAbsolutePath(), null,
					SQLiteDatabase.OPEN_READWRITE);
			newdb.execSQL("ATTACH \"" + DataBaseHelper.getLibDBFile()
					+ "\" AS old");
			newdb.execSQL("ATTACH \"" + DataBaseHelper.getMediaDBFile()
					+ "\" AS media");
			newdb.beginTransaction();
			newdb.execSQL("CREATE TABLE tb_update as select a.id,b.* from main.lib a left join media.search b on a.id=b.lib_id and a.time_modified < b.time_modified where b._id is not null;");
			newdb.execSQL("UPDATE main.lib set lang=(select lang from tb_update where tb_update.id=main.lib.id),"
					+ "genre=(select genre from tb_update where tb_update.id=main.lib.id),"
					+ "rhythm=(select rhythm from tb_update where tb_update.id=main.lib.id),"
					+ "sing_method=(select sing_method from tb_update where tb_update.id=main.lib.id),"
					+ "age=(select age from tb_update where tb_update.id=main.lib.id),"
					+ "subject=(select subject from tb_update where tb_update.id=main.lib.id),"
					+ "instrument=(select instrument from tb_update where tb_update.id=main.lib.id),"
					// +
					// "listentime=(select listentime from tb_update where tb_update.id=main.lib.id),"
					// +
					// "tag=(select tag from tb_update where tb_update.id=main.lib.id),"
					+ "time_modified=(select time_modified from tb_update where tb_update.id=main.lib.id) "
					+ "where id in(select id from tb_update)");
			newdb.execSQL("DROP TABLE tb_update");
			newdb.execSQL("CREATE TABLE tb_update as select a.id,b.* from main.lib a left join old.lib b on a.title=b.title and a.artist=b.artist and a.time_modified < b.time_modified where b.id is not null;");
			newdb.execSQL("UPDATE main.lib set lang=(select lang from tb_update where tb_update.id=main.lib.id),"
					+ "genre=(select genre from tb_update where tb_update.id=main.lib.id),"
					+ "rhythm=(select rhythm from tb_update where tb_update.id=main.lib.id),"
					+ "sing_method=(select sing_method from tb_update where tb_update.id=main.lib.id),"
					+ "age=(select age from tb_update where tb_update.id=main.lib.id),"
					+ "subject=(select subject from tb_update where tb_update.id=main.lib.id),"
					+ "instrument=(select instrument from tb_update where tb_update.id=main.lib.id),"
					// +
					// "listentime=(select listentime from tb_update where tb_update.id=main.lib.id),"
					// +
					// "tag=(select tag from tb_update where tb_update.id=main.lib.id),"
					+ "time_modified=(select time_modified from tb_update where tb_update.id=main.lib.id) "
					+ "where id in(select id from tb_update)");
			newdb.execSQL("DROP TABLE tb_update");
			newdb.execSQL("DROP TABLE media.tag");
			newdb.execSQL("CREATE TABLE media.[tag] AS SELECT _id,lang,genre,rhythm,sing_method,age,subject,instrument,lib.id AS lib_id,time_modified,play_times FROM media.audio LEFT JOIN main.lib ON media.audio.title=main.lib.title and media.audio.artist=main.lib.artist;");

			newdb.setTransactionSuccessful();
			newdb.endTransaction();
			newdb.execSQL("DETACH old;");
			newdb.execSQL("DETACH media;");
		} catch (SQLiteException ex) {
			ex.printStackTrace();
			return false;
		}
		return FileOperationUtils.copyFiles(newdbfile, new File(
				MEDIALIB.LIBRARY.getLibDBFilePath()));
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		IntentFilter ift = new IntentFilter(MEDIA.INTENT_SCAN_DONE);
		registerReceiver(receiver, ift);

		TextView info = (TextView) findViewById(R.id.info);
		StringBuffer sb = new StringBuffer();
		sb.append("上次扫描在：");
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
		Button scan = (Button) findViewById(R.id.scan);
		scan.setOnClickListener(scanlistener);
		Button update = (Button) findViewById(R.id.update);
		update.setOnClickListener(updatelistener);
	}

	@Override
	protected void onDestroy() {
		unregisterReceiver(receiver);
		super.onDestroy();
	}

}
