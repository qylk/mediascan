package com.qylk.app.provider.media;

import android.content.ContentProvider;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.os.Environment;

import com.qylk.app.provider.media.MEDIA.AUDIO;

public class MediaProvider extends ContentProvider {
	private static final int CODE_AUDIO_ALL = 0;
	private static final int CODE_AUDIO_ONE = 1;
	private static final int CODE_TAG_ONE = 2;
	private static final int CODE_ARTIST_ONE = 3;

	private static final UriMatcher sMatcher;

	static {
		sMatcher = new UriMatcher(UriMatcher.NO_MATCH);
		sMatcher.addURI(MEDIA.AUTOHORITY_NAME, AUDIO.PATH_AUDIO, CODE_AUDIO_ALL);
		sMatcher.addURI(MEDIA.AUTOHORITY_NAME, AUDIO.PATH_AUDIO + "/#",
				CODE_AUDIO_ONE);
		sMatcher.addURI(MEDIA.AUTOHORITY_NAME, AUDIO.PATH_AUDIO + "/#",
				CODE_AUDIO_ONE);
		sMatcher.addURI(MEDIA.AUTOHORITY_NAME, AUDIO.PATH_TAG + "/#",
				CODE_TAG_ONE);
		sMatcher.addURI(MEDIA.AUTOHORITY_NAME, AUDIO.ARTIST.PATH_ARTIST + "/#",
				CODE_ARTIST_ONE);
	}
	private SQLiteDatabase db;

	@Override
	public int delete(Uri uri, String selection, String[] selectionArgs) {
		if(db==null)
			return 0;
		int count = 0;
		switch (sMatcher.match(uri)) {
		case CODE_AUDIO_ALL:
			count = db.delete(AUDIO.PATH_AUDIO, selection, selectionArgs);
			break;
		case CODE_AUDIO_ONE:
			String id = uri.getPathSegments().get(1);
			count = db.delete(AUDIO.PATH_AUDIO, "_id=" + id, null);
			break;
		case CODE_TAG_ONE:
			id = uri.getPathSegments().get(1);
			count = db.delete(AUDIO.PATH_TAG, "_id=" + id, null);
			break;
		case CODE_ARTIST_ONE:
			id = uri.getPathSegments().get(1);
			count = db.delete(AUDIO.ARTIST.PATH_ARTIST, "_id=" + id, null);
			break;
		default:
			break;
		}
		return count;
	}

	@Override
	public String getType(Uri uri) {
		switch (sMatcher.match(uri)) {
		case CODE_AUDIO_ALL:
			return MEDIA.CONTENT_AUDIO_TYPE;
		case CODE_AUDIO_ONE:
			return MEDIA.CONTENT_AUDIO_ITEM_TYPE;
		case CODE_TAG_ONE:
			return MEDIA.CONTENT_TAG_ITEM_TYPE;
		case CODE_ARTIST_ONE:
			return MEDIA.CONTENT_AUDIO_ARTIST_TYPE;
		default:
			throw new IllegalArgumentException("Unknown URI:" + uri);
		}
	}

	@Override
	public Uri insert(Uri uri, ContentValues values) {
		if(db==null)
			return null;
		long rowId;
		if (sMatcher.match(uri) == CODE_AUDIO_ALL) {
			rowId = db.insert(AUDIO.PATH_AUDIO, null, values);
			if (rowId > 0) {
				Uri noteUri = ContentUris.withAppendedId(AUDIO.URI, rowId);
				getContext().getContentResolver().notifyChange(noteUri, null);
				return noteUri;
			}
		}
		throw new IllegalArgumentException("Unknown URI:" + uri);
	}

	@Override
	public boolean onCreate() {
		if (!Environment.getExternalStorageState().equals(
				Environment.MEDIA_MOUNTED))
			return false;
		DataBaseHelper dbhelper = new DataBaseHelper(getContext());
		db = dbhelper.getWritableDatabase();
		return (db != null);
	}

	@Override
	public Cursor query(Uri uri, String[] projection, String selection,
			String[] selectionArgs, String sortOrder) {
		if(db==null)
			return null;
		Cursor c = null;
		switch (sMatcher.match(uri)) {
		case CODE_AUDIO_ALL:
			c = db.query(AUDIO.PATH_SEARCH, projection, selection,
					selectionArgs, null, null, sortOrder);
			break;
		case CODE_AUDIO_ONE:
			String id = uri.getPathSegments().get(1);
			c = db.query(AUDIO.PATH_SEARCH, projection, "_id=" + id, null,
					null, null, null);
		case CODE_TAG_ONE:
			id = uri.getPathSegments().get(1);
			c = db.query(AUDIO.PATH_SEARCH, projection, "_id=" + id, null,
					null, null, null);
			break;
		case CODE_ARTIST_ONE:
			id = uri.getPathSegments().get(1);
			c = db.query(AUDIO.ARTIST.PATH_ARTIST, projection, "_id=" + id,
					null, null, null, null);
			break;
		default:
			break;
		}
		return c;
	}

	@Override
	public int update(Uri uri, ContentValues values, String selection,
			String[] selectionArgs) {
		if(db==null)
			return 0;
		int count = 0;
		switch (sMatcher.match(uri)) {
		case CODE_AUDIO_ALL:
			count = db.update(AUDIO.PATH_AUDIO, values, selection,
					selectionArgs);
			break;
		case CODE_AUDIO_ONE:
			String id = uri.getPathSegments().get(1);
			count = db.update(AUDIO.PATH_AUDIO, values, "_id=" + id, null);
			break;
		case CODE_TAG_ONE:
			id = uri.getPathSegments().get(1);
			count = db.update(AUDIO.PATH_TAG, values, "_id=" + id, null);
			break;
		case CODE_ARTIST_ONE:
			id = uri.getPathSegments().get(1);
			count = db.update(AUDIO.ARTIST.PATH_ARTIST, values, "_id=" + id,
					null);
			break;
		default:
			break;
		}
		return count;
	}
}
