package com.qylk.app.provider.media;

import android.net.Uri;

public class MEDIALIB {
	public static final class LIBRARY {
		public static final String FIELD_ALBUM = "album";
		public static final String FIELD_ARTIST = "artist";
		public static final String FIELD_ID = "id";
		public static final String FIELD_TITLE = "title";
		public static final String FIELD_PLAY_TIMES = "play_times";
		public static final String FIELD_TAG_LANG = "lang";
		public static final String FIELD_TAG_GENRE = "genre";
		public static final String FIELD_TAG_RHYTHM = "rhythm";
		public static final String FIELD_TAG_SING_METHOD = "sing_method";
		public static final String FIELD_TAG_AGE = "age";
		public static final String FIELD_TAG_SUBJECT = "subject";
		public static final String FIELD_TAG_INSTRUMENT = "instrument";
		public static final String FIELD_TIME_MODIFIED = "time_modified";
		public static final String PATH_LIBRARY = "lib";
		
		public static final Uri URI = Uri.parse("content://" + AUTOHORITY_NAME
				+ '/' + PATH_LIBRARY);
		public static String getLibDBFilePath() {
			return DataBaseHelper.getLibDBPath();
		}
	}
	public static final String AUTOHORITY_NAME = "com.qylk.app.provider.medialib";
	public static final String CONTENT_LIBRARY_ITEM_TYPE = "vnd.android.cursor.item/vnd.media.library.title";
	public static final String CONTENT_LIBRARY_TYPE = "vnd.android.cursor.dir/vnd.media.library";
}
