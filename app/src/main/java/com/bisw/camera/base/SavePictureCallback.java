package com.bisw.camera.base;

import android.Manifest;
import android.app.Activity;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.MediaStore;
import android.text.format.DateUtils;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class SavePictureCallback implements IPictureBufferCallback {
    static final int REQUEST_CODE_SAVE_IMG = 0;

    /**
     * @param bitmap
     */
    @Override
    public void onPictureToken(Bitmap bitmap, String savedPath, Context context) {
        if (bitmap == null || savedPath == null || context == null) {
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                int sIdx = savedPath.lastIndexOf(".") + 1;
                int eIdx = savedPath.lastIndexOf("/");
                String name = savedPath.substring(sIdx, eIdx);
                savePicture(context, bitmap, name);
            }
        }).start();
    }

    private void savePicture(Context context, Bitmap bitmap, String name) {
        String[] mPermissionList = new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE};
        if (hasPermissions(context, mPermissionList)) {
            saveImage(context, bitmap, name);
        } else {
            ActivityCompat.requestPermissions((Activity)context, mPermissionList, REQUEST_CODE_SAVE_IMG);
        }
    }

    public boolean hasPermissions(Context context, String... perms) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            return true;
        }
        for (String perm : perms) {
            boolean hasPerm = (ContextCompat.checkSelfPermission(context, perm) ==
                    PackageManager.PERMISSION_GRANTED);
            if (!hasPerm) {
                return false;
            }
        }
        return true;
    }

    public void saveImage(Context context, Bitmap bitmap, String name) {
        if (bitmap == null) {
            return;
        }
        if (Build.VERSION.SDK_INT < 29) {
            saveImageToGallery0(context, bitmap, name);
        } else {
            saveImageToGallery1(context, bitmap, name);
        }
    }

    /**
     * Android versions below 10
     */
    public boolean saveImageToGallery0(Context context, Bitmap image, String name) {
        String storePath = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "dearxy";
        File appDir = new File(storePath);
        if (!appDir.exists()) {
            appDir.mkdir();
        }
        String fileName = name  + System.currentTimeMillis() + ".jpg";
        File file = new File(appDir, fileName);
        try {
            FileOutputStream fos = new FileOutputStream(file);
            // use iostream to save picture
            boolean isSuccess = image.compress(Bitmap.CompressFormat.JPEG, 60, fos);
            fos.flush();
            fos.close();

            // after save picture, then send update message to database
            Uri uri = Uri.fromFile(file);
            context.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, uri));
            if (isSuccess) {
                return true;
            } else {
                return false;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    /**
     * Android 10 and above
     */
    public boolean saveImageToGallery1(Context context, Bitmap image, String name) {
        Long mImageTime = System.currentTimeMillis();
        String mImageFileName = name + mImageTime + ".jpg";

        final ContentValues values = new ContentValues();
        values.put(MediaStore.MediaColumns.RELATIVE_PATH, Environment.DIRECTORY_PICTURES
                + File.separator + name);
        values.put(MediaStore.MediaColumns.DISPLAY_NAME, mImageFileName);
        values.put(MediaStore.MediaColumns.MIME_TYPE, "image/jpg");
        values.put(MediaStore.MediaColumns.DATE_ADDED, mImageTime / 1000);
        values.put(MediaStore.MediaColumns.DATE_MODIFIED, mImageTime / 1000);
        values.put(MediaStore.MediaColumns.DATE_EXPIRES, (mImageTime + DateUtils.DAY_IN_MILLIS) / 1000);
        values.put(MediaStore.MediaColumns.IS_PENDING, 1);

        ContentResolver resolver = context.getContentResolver();
        final Uri uri = resolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
        try {
            // First, write the actual data for our screenshot
            try (OutputStream out = resolver.openOutputStream(uri)) {
                if (!image.compress(Bitmap.CompressFormat.JPEG, 100, out)) {
                    return false;
                }
            }
            // Everything went well above, publish it!
            values.clear();
            values.put(MediaStore.MediaColumns.IS_PENDING, 0);
            values.putNull(MediaStore.MediaColumns.DATE_EXPIRES);
            resolver.update(uri, values, null, null);
        } catch (IOException e) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R){
                resolver.delete(uri, null);
            }
            return false;
        }
        return true;
    }



}
