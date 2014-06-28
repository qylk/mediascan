package com.qylk.app.provider.media;

import android.content.ContentProvider;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.os.Environment;

import com.qylk.app.provider.media.MEDIALIB.LIBRARY;

public class LibProvider extends ContentProvider {
	private static final int CODE_LIBRARY_ALL = 0;
	private static final int CODE_LIBRARY_ONE = 1;
	private static final UriMatcher sMatcher;
	private SQLiteDatabase db;
	static {
		sMatcher = new UriMatcher(UriMatcher.NO_MATCH);
		sMatcher.addURI(MEDIALIB.AUTOHORITY_NAME, LIBRARY.PATH_LIBRARY,
				CODE_LIBRARY_ALL);
		sMatcher.addURI(MEDIALIB.AUTOHORITY_NAME, LIBRARY.PATH_LIBRARY + "/#",
				CODE_LIBRARY_ONE);
	}

	@Override
	public int delete(Uri uri, String selection, String[] selectionArgs) {
		throw new IllegalArgumentException(
				"delete function has not implemented");
	}

	@Override
	public String getType(Uri uri) {
		switch (sMatcher.match(uri)) {
		case CODE_LIBRARY_ONE:
			return MEDIALIB.CONTENT_LIBRARY_TYPE;
		case CODE_LIBRARY_ALL:
			return MEDIALIB.CONTENT_LIBRARY_ITEM_TYPE;
		default:
			throw new IllegalArgumentException("Unknown URI:" + uri);
		}
	}

	@Override
	public Uri insert(Uri uri, ContentValues values) {
		if (db == null)
			return null;
		long rowId;
		if (sMatcher.match(uri) == CODE_LIBRARY_ALL) {
			rowId = db.insert(LIBRARY.PATH_LIBRARY, null, values);
			if (rowId > 0) {
				Uri noteUri = ContentUris.withAppendedId(LIBRARY.URI, rowId);
				getContext().getContentResolver().notifyChange(noteUri, null);
				return noteUri;
			}
		}
		throw new IllegalArgumentException("invalid Uri for Insert operation:"
				+ uri);
	}

	@Override
	public boolean onCreate() {
		if (!Environment.getExternalStorageState().equals(
				Environment.MEDIA_MOUNTED))
			return false;
		DataBaseHelper dbhelper = new DataBaseHelper(getContext());
		db = dbhelper.getWritableLibDatabase();
		return (db != null);
	}

	@Override
	public Cursor query(Uri uri, String[] projection, String selection,
			String[] selectionArgs, String sortOrder) {
		if (db == null)
			return null;
		Cursor c = null;
		switch (sMatcher.match(uri)) {
		case CODE_LIBRARY_ALL:
			c = db.query(LIBRARY.PATH_LIBRARY, projection, selection,
					selectionArgs, null, null, sortOrder);
			break;
		case CODE_LIBRARY_ONE:
			String id = uri.getPathSegments().get(1);
			c = db.query(LIBRARY.PATH_LIBRARY, projection, "id=" + id, null,
					null, null, null);
		default:
			break;
		}
		return c;
	}

	@Override
	public int update(Uri uri, ContentValues values, String selection,
			String[] selectionArgs) {
		if (db == null)
			return 0;
		int count = 0;
		switch (sMatcher.match(uri)) {
		case CODE_LIBRARY_ALL:
			count = db.update(LIBRARY.PATH_LIBRARY, values, selection,
					selectionArgs);
			break;
		case CODE_LIBRARY_ONE:
			String id = uri.getPathSegments().get(1);
			count = db.update(LIBRARY.PATH_LIBRARY, values, "id=" + id, null);
			break;
		default:
			break;
		}
		return count;
	}

}
