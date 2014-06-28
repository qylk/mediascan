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

	public static String getLibDBFile() {
		return DataBaseDIR + LIBRARY_DBNAME;
	}

	public static String getMediaDBFile() {
		return DataBaseDIR + DBNAME;
	}

	private Context context;

	public DataBaseHelper(Context context) {
		super(context, getMediaDBFile(), null, DATABASE_VERSION);
		File dbp = new File(getMediaDBFile());
		if (!dbp.exists())
			dbp.getParentFile().mkdirs();
		this.context = context;
	}

	private boolean copyDataBaseFromAsset(String name, File dest) {
		return FileOperationUtils.copyAssetToLocal(context, name, dest);
	}

	public SQLiteDatabase getWritableLibDatabase() {
		String lib = getLibDBFile();
		File libf = new File(lib);
		if (libf.exists())
			return SQLiteDatabase.openDatabase(lib, null,
					SQLiteDatabase.OPEN_READWRITE);
		else {
			if (copyDataBaseFromAsset(LIBRARY_DBNAME, libf))
				return SQLiteDatabase.openDatabase(lib, null,
						SQLiteDatabase.OPEN_READWRITE);
		}
		return null;
	}

	@Override
	public void onCreate(SQLiteDatabase db) {
		String dbpath = getLibDBFile();
		File dbfile = new File(dbpath);
		if (!dbfile.exists()) {
			dbfile.getParentFile().mkdirs();
			copyDataBaseFromAsset(LIBRARY_DBNAME, dbfile);
		}
		Log.i(TAG, "creating media.db into databases folder");
		db.execSQL("CREATE TABLE [audio](_id integer primary key autoincrement,title text,title_key text,artist text default 'unknown',artist_key text,album text default 'unknown',album_key text,duration long not null,_data text not null,last_modified long default 0)");
		db.execSQL("CREATE TABLE [tag](_id integer,lang int,genre int,rhythm int,sing_method int,age int,subject int,instrument int,lib_id int,time_modified long,play_times int);");
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
