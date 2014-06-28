package com.qylk.app.provider.media;

import java.io.File;

import com.qylk.app.provider.media.utils.FileOperationUtils;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.os.Environment;
import android.util.Log;

public class DataBaseHelper extends SQLiteOpenHelper {

	private static final int DATABASE_VERSION = 2;
	private static String DataBaseDIR = Environment
			.getExternalStorageDirectory().getAbsolutePath()
			+ "/QMUSIC/database/";
	private static final String DBNAME = "media.db";
	private static final String LIBRARY_DBNAME = "lib.db";
	private static final String TAG = "DataBaseHelper";

	public static String getLibDBPath() {
		return DataBaseDIR + LIBRARY_DBNAME;
	}

	public static String getMediaDBPath() {
		return DataBaseDIR + DBNAME;
	}

	private Context context;

	public DataBaseHelper(Context context) {
		super(context, getMediaDBPath(), null, DATABASE_VERSION);
		this.context = context;
	}

	private boolean copyDataBaseFromAsset(String name, File dest) {
		return FileOperationUtils.copyAssetToLocal(context, name, dest);
	}

	public SQLiteDatabase getWritableLibDatabase() {
		String dbpath = getLibDBPath();
		File dbfile = new File(dbpath);
		if (dbfile.exists())
			return SQLiteDatabase.openDatabase(dbpath, null,
					SQLiteDatabase.OPEN_READWRITE);
		else {
			dbfile.getParentFile().mkdirs();
			if (copyDataBaseFromAsset(LIBRARY_DBNAME, dbfile))
				return SQLiteDatabase.openDatabase(dbpath, null,
						SQLiteDatabase.OPEN_READWRITE);
		}
		return null;
	}

	@Override
	public void onCreate(SQLiteDatabase db) {
		Log.i(TAG, "creating media.db into databases folder");
		db.execSQL("CREATE TABLE [audio](_id integer primary key autoincrement,title text,title_key text,artist text default 'unknown',artist_key text,album text default 'unknown',album_key text,duration long not null,_data text not null)");
		db.execSQL("CREATE TABLE [tag](_id integer,lang int,genre int,rhythm int,sing_method int,age int,subject int,instrument int,lib_id int,time_modified long,play_times int);");
		
		String dbpath = getLibDBPath();
		File dbfile = new File(dbpath);
		if (!dbfile.exists()) {
			dbfile.getParentFile().mkdirs();
			copyDataBaseFromAsset(LIBRARY_DBNAME, dbfile);
		}
	}

	@Override
	public void onOpen(SQLiteDatabase db) {
		super.onOpen(db);
		Log.i(TAG, "database opened");
	}

	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

	}

}
