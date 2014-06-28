package com.qylk.app.provider.media.utils;

public interface MediaScanerClient {
	void postFile(String file);

	void postTag(String id, String value);
}
