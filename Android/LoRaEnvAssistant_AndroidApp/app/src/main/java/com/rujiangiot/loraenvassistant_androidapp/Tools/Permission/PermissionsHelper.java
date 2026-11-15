package com.rujiangiot.loraenvassistant_androidapp.Tools.Permission;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Build;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;

import java.util.List;

import pub.devrel.easypermissions.AfterPermissionGranted;
import pub.devrel.easypermissions.EasyPermissions;

public class PermissionsHelper implements EasyPermissions.PermissionCallbacks {
    private static final int REQUEST_FINE_LOCATION_PERMISSION = 1;
    private static final int REQUEST_CAMERA_PERMISSION = 2;
    private static final int REQUEST_NOTIFICATION_PERMISSION = 3;
    private final Activity activity;  // 必须持有 Activity 引用

    // 构造函数传入 Activity
    public PermissionsHelper(Activity activity) {
        this.activity = activity;
    }

    @AfterPermissionGranted(REQUEST_FINE_LOCATION_PERMISSION)
    public void requestLocationPermission() {
        String permission = Manifest.permission.ACCESS_FINE_LOCATION;
        if (EasyPermissions.hasPermissions(activity, permission)) {
            Toast.makeText(activity, "已获取到定位权限", Toast.LENGTH_SHORT).show();
        } else {
            EasyPermissions.requestPermissions(activity, "需要定位权限", REQUEST_FINE_LOCATION_PERMISSION, permission);
        }
    }

    @AfterPermissionGranted(REQUEST_CAMERA_PERMISSION)
    public void requestCameraPermission() {
        String permission = Manifest.permission.CAMERA;
        if (EasyPermissions.hasPermissions(activity, permission)) {
            Toast.makeText(activity, "已获取到相机权限", Toast.LENGTH_SHORT).show();
        } else {
            EasyPermissions.requestPermissions(activity, "需要相机权限", REQUEST_CAMERA_PERMISSION, permission);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.TIRAMISU)
    @AfterPermissionGranted(REQUEST_NOTIFICATION_PERMISSION)
    public void requestNotiPermission() {
        String permission = Manifest.permission.POST_NOTIFICATIONS;
        if (EasyPermissions.hasPermissions(activity, permission)) {
            Toast.makeText(activity, "已获取到通知权限", Toast.LENGTH_SHORT).show();
        } else {
            EasyPermissions.requestPermissions(activity, "需要通知权限", REQUEST_NOTIFICATION_PERMISSION, permission);
        }
    }

    @Override
    public void onPermissionsGranted(int i, @NonNull List<String> list) {
        if (i == REQUEST_FINE_LOCATION_PERMISSION) {
            Toast.makeText(activity, "定位权限已授予", Toast.LENGTH_SHORT).show();
        } else if (i == REQUEST_CAMERA_PERMISSION) {
            Toast.makeText(activity, "相机权限已授予", Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(activity, "通知权限已授予", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    public void onPermissionsDenied(int i, @NonNull List<String> list) {
        if (i == REQUEST_FINE_LOCATION_PERMISSION) {
            Toast.makeText(activity, "定位权限被拒绝", Toast.LENGTH_SHORT).show();
        } else if (i == REQUEST_CAMERA_PERMISSION) {
            Toast.makeText(activity, "相机权限被拒绝", Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(activity, "通知权限被拒绝", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Callback for the result from requesting permissions. This method
     * is invoked for every call on .
     * <p>
     * <strong>Note:</strong> It is possible that the permissions request interaction
     * with the user is interrupted. In this case you will receive empty permissions
     * and results arrays which should be treated as a cancellation.
     * </p>
     *
     * @param requestCode  The request code passed in
     * @param permissions  The requested permissions. Never null.
     * @param grantResults The grant results for the corresponding permissions
     *                     which is either {@link PackageManager#PERMISSION_GRANTED}
     *                     or {@link PackageManager#PERMISSION_DENIED}. Never null.
     */
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {

    }
}
