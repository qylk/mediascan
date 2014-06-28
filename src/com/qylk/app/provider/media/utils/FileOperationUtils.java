package com.qylk.app.provider.media.utils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;

import android.content.Context;

public class FileOperationUtils {
	public static boolean copyAssetToLocal(Context context, String name,
			File dest) {
		FileOutputStream fos = null;
		InputStream is = null;
		try {
			is = context.getAssets().open(name);
			fos = new FileOutputStream(dest);
			byte[] buffer = new byte[1024];
			int len;
			while ((len = is.read(buffer)) != -1) {
				fos.write(buffer, 0, len);
			}
			fos.flush();
			fos.close();
			is.close();
			return true;
		} catch (Exception e) {
			e.printStackTrace();
			try {
				if (is != null)
					is.close();
				if (fos != null)
					fos.close();
			} catch (Exception ee) {
				ee.printStackTrace();
			}
			return false;
		}
	}

	public static boolean copyFiles(File src, File dest) {
		FileOutputStream fos = null;
		InputStream is = null;
		if (!src.exists())
			throw new IllegalArgumentException("source file dose not exist!");
		if (!dest.getParentFile().exists())
			dest.getParentFile().mkdirs();
		try {
			is = new FileInputStream(src);
			fos = new FileOutputStream(dest);
			byte[] buffer = new byte[1024];
			int len;
			while ((len = is.read(buffer)) != -1) {
				fos.write(buffer, 0, len);
			}
			fos.flush();
			fos.close();
			is.close();
			return true;
		} catch (Exception e) {
			e.printStackTrace();
			try {
				if (is != null)
					is.close();
				if (fos != null)
					fos.close();
			} catch (Exception ee) {
				ee.printStackTrace();
			}
			return false;
		}
	}

}
