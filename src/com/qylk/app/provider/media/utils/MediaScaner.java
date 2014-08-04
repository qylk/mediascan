package com.qylk.app.provider.media.utils;

import android.content.Context;
import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

import com.qylk.app.provider.media.DataBaseHelper;
import com.qylk.app.provider.media.MEDIA;

public class MediaScaner implements MediaScanerClient {
	static {
		System.loadLibrary("mediascaner");
	}
	private Context mContext;

	private native static boolean processDirectory(String dir,
			MediaScanerClient client);

	private native static boolean setup(String db);

	public MediaScaner(Context context) {
		this.mContext = context;
	}

	@Override
	public void postFile(String file) {

	}

	@Override
	public void postTag(String id, String value) {
		
	}

	public void scanDirectory(String dir) {
		if (!setup(MEDIA.getDBFilePath())) {
			Log.e(getClass().getSimpleName(), "native setup failed");
			return;
		}
		mContext.sendBroadcast(new Intent(MEDIA.INTENT_SCAN_START));
		scan(dir);
	}

	private void initBeforeScan(SQLiteDatabase db) {
		db.execSQL("ATTACH \"" + DataBaseHelper.getLibDBFile() + "\" AS lib0");
		db.beginTransaction();
		db.execSQL("CREATE TABLE tb_update as select a.id,b.* from lib0.lib a left join tag b on a.id=b.lib_id and a.time_modified < b.time_modified where b._id is not null;");
		db.execSQL("UPDATE lib0.lib set lang=(select lang from tb_update where tb_update.id=lib0.lib.id),"
				+ "genre=(select genre from tb_update where tb_update.id=lib0.lib.id),"
				+ "rhythm=(select rhythm from tb_update where tb_update.id=lib0.lib.id),"
				+ "sing_method=(select sing_method from tb_update where tb_update.id=lib0.lib.id),"
				+ "age=(select age from tb_update where tb_update.id=lib0.lib.id),"
				+ "subject=(select subject from tb_update where tb_update.id=lib0.lib.id),"
				+ "instrument=(select instrument from tb_update where tb_update.id=lib0.lib.id),"
				//+ "listentime=(select listentime from tb_update where tb_update.id=lib0.lib.id),"
				//+ "tag=(select tag from tb_update where tb_update.id=lib0.lib.id),"
				+ "time_modified=(select time_modified from tb_update where tb_update.id=lib0.lib.id) "
				+ "where id in(select id from tb_update)");
		db.execSQL("DROP TABLE tb_update");
		db.execSQL("DROP TABLE IF EXISTS [tag];");
		db.execSQL("DROP TABLE IF EXISTS [artists];");
		db.execSQL("DROP TABLE IF EXISTS [albums];");
		db.setTransactionSuccessful();
		db.endTransaction();
	}

	private void afterScan(SQLiteDatabase db) {
		db.beginTransaction();
		db.execSQL("CREATE TABLE albums(_id integer primary key,name text,tracknum integer default 0);");
		db.execSQL("INSERT INTO albums(name,tracknum) select album,count(album) from audio group by album;");
		db.execSQL("CREATE TABLE [tag] AS SELECT _id,lang,genre,rhythm,sing_method,age,subject,instrument,lib.id AS lib_id,time_modified,play_times FROM [audio] LEFT JOIN [lib] ON audio.title=lib.title and audio.artist=lib.artist;");
		db.execSQL("UPDATE tag set lang=0,genre=0,rhythm=0,sing_method=0,age=0,subject=0,instrument=0,lib_id=-1,play_times=0 where lang is null");
		db.execSQL("CREATE TABLE artists(_id integer primary key,name text,tracknum integer default 0,artindex text)");
		db.execSQL("INSERT INTO artists(name,tracknum,artindex) SELECT audio.artist,count(audio.title),artist.artindex from audio LEFT JOIN [artist] ON audio.artist=artist.name group by audio.artist");
		db.execSQL("CREATE VIEW IF NOT EXISTS [search] AS SELECT audio.*,artists._id as artist_id,albums._id as album_id, tag.* FROM [audio] LEFT JOIN [tag] ON audio._id=tag._id left join [artists] ON audio.artist=artists.name left join [albums] ON audio.album=albums.name;");
		db.setTransactionSuccessful();
		db.endTransaction();
		db.execSQL("DETACH lib0");
	}

	private void scan(String dir) {
		SQLiteDatabase db = new DataBaseHelper(mContext).getWritableDatabase();
		initBeforeScan(db);
		long start = System.currentTimeMillis();
		boolean success = processDirectory(dir, this);
		if (success) {
			Log.v(getClass().getSimpleName(),
					"scan complete:" + (System.currentTimeMillis() - start));
		}
		afterScan(db);
	}
}
