package com.qylk.app.provider.media;

import android.net.Uri;

public final class MEDIA {
	
	public static String getDBFilePath() {
		return DataBaseHelper.getMediaDBPath();
	}
	
	public static final class AUDIO {

		public static final class TAG {
			public static final String FIELD_ID = "_id";
			public static final String FIELD_LIB_ID = "lib_id";
			public static final String FIELD_LANG = "lang";
			public static final String FIELD_GENRE = "genre";
			public static final String FIELD_RHYTHM = "rhythm";
			public static final String FIELD_SING_METHOD = "sing_method";
			public static final String FIELD_AGE = "age";
			public static final String FIELD_SUBJECT = "subject";
			public static final String FIELD_INSTRUMENT = "instrument";
			public static final String FIELD_PLAY_TIMES = "play_times";
			public static final String FIELD_TIME_MODIFIED = "time_modified";
			
			public static final String PATH_TAG = "tag";
			public static final Uri URI = Uri.parse("content://"
					+ AUTOHORITY_NAME + '/' + PATH_TAG);
		}
		
		public static final class ARTIST{
			public static final String FIELD_ID = "_id";
			public static final String FIELD_INDEX = "artindex";
			public static final String FIELD_NAME = "name";
			public static final String FIELD_TRACKNUM = "tracknum";
			
			public static final String PATH_ARTIST = "artists";
			public static final Uri URI = Uri.parse("content://"
					+ AUTOHORITY_NAME + '/' + PATH_ARTIST);
		}

		public static final String FIELD_ALBUM = "album";
		public static final String FIELD_ALBUM_ID = "album_id";
		public static final String FIELD_ARTIST = "artist";
		public static final String FIELD_ARTIST_ID = "artist_id";
		public static final String FIELD_DURATION = "duration";
		public static final String FIELD_ID = "_id";
		public static final String FIELD_PATH = "_data";
		public static final String FIELD_TITLE = "title";
		public static final String FIELD_TITLE_KEY = "title_key";
		public static final String FIELD_ARTIST_KEY = "artist_key";
		public static final String FIELD_ALBUM_KEY = "album_key";
		public static final String FIELD_LIB_ID = "lib_id";
		
		// public static final String FIELD_TAG = "tag";
		public static final String PATH_AUDIO = "audio";
		public static final String PATH_SEARCH = "search";
		public static final String PATH_TAG = "tag";

		public static final Uri URI = Uri.parse("content://" + AUTOHORITY_NAME
				+ '/' + PATH_AUDIO);
	}

	public static final String AUTOHORITY_NAME = "com.qylk.app.provider.media";
	public static final String CONTENT_AUDIO_ITEM_TYPE = "vnd.android.cursor.item/vnd.media.audio.id";
	public static final String CONTENT_TAG_ITEM_TYPE = "vnd.android.cursor.item/vnd.media.tag.id";
	public static final String CONTENT_AUDIO_ARTIST_TYPE = "vnd.android.cursor.dir/vnd.media.audio.artist";
	public static final String CONTENT_AUDIO_TYPE = "vnd.android.cursor.dir/vnd.media.audio";
	
	
	public static final String SCAN_ACTION = "com.qylk.app.provider.media.scan";
	public static final String INTENT_SCAN_START = "com.qylk.app.musicplayer.media.syncstart";
	public static final String INTENT_SCAN_DONE = "com.qylk.app.musicplayer.media.syncdone";
}
