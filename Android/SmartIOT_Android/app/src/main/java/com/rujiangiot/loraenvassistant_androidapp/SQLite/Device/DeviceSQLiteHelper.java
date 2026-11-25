package com.rujiangiot.loraenvassistant_androidapp.SQLite.Device;

import android.annotation.SuppressLint;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.List;

public class DeviceSQLiteHelper extends SQLiteOpenHelper {

    public static final String LOG_NAME = "DeviceSQLiteDebug";
    private static final String DB_NAME = "LoRaEnvAssistant_AndroidApp.db";
    private static final String TABLE_NAME = "device";
    private static final String CREATE_TABLE_SQL = "CREATE TABLE " + TABLE_NAME + "(" +
            "device_id text PRIMARY KEY," +
            "device_nickname text," +
            "device_mode text," +
            "timestamp integer)";

    /**
     * Create a helper object to create, open, and/or manage a database.
     * This method always returns very quickly.  The database is not actually
     * created or opened until one of {@link #getWritableDatabase} or
     * {@link #getReadableDatabase} is called.
     *
     * @param context to use for locating paths to the database
     */
    public DeviceSQLiteHelper(@Nullable Context context) {
        super(context, DB_NAME, null, 6);
    }

    /**
     * Called when the database is created for the first time. This is where the
     * creation of tables and the initial population of the tables should happen.
     *
     * @param db The database.
     */
    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL(CREATE_TABLE_SQL);
    }

    /**
     * Called when the database needs to be upgraded. The implementation
     * should use this method to drop tables, add tables, or do anything else it
     * needs to upgrade to the new schema version.
     *
     * <p>
     * The SQLite ALTER TABLE documentation can be found
     * <a href="http://sqlite.org/lang_altertable.html">here</a>. If you add new columns
     * you can use ALTER TABLE to insert them into a live table. If you rename or remove columns
     * you can use ALTER TABLE to rename the old table, then create the new table and then
     * populate the new table with the contents of the old table.
     * </p><p>
     * This method executes within a transaction.  If an exception is thrown, all changes
     * will automatically be rolled back.
     * </p>
     * <p>
     * <em>Important:</em> You should NOT modify an existing migration step from version X to X+1
     * once a build has been released containing that migration step.  If a migration step has an
     * error and it runs on a device, the step will NOT re-run itself in the future if a fix is made
     * to the migration step.</p>
     * <p>For example, suppose a migration step renames a database column from {@code foo} to
     * {@code bar} when the name should have been {@code baz}.  If that migration step is released
     * in a build and runs on a user's device, the column will be renamed to {@code bar}.  If the
     * developer subsequently edits this same migration step to change the name to {@code baz} as
     * intended, the user devices which have already run this step will still have the name
     * {@code bar}.  Instead, a NEW migration step should be created to correct the error and rename
     * {@code bar} to {@code baz}, ensuring the error is corrected on devices which have already run
     * the migration step with the error.</p>
     *
     * @param db         The database.
     * @param oldVersion The old database version.
     * @param newVersion The new database version.
     */
    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_NAME);
        onCreate(db);
    }

    public void insertDeviceData(String deviceId, String deviceMode, long timestamp) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put("device_id", deviceId);
        contentValues.put("device_mode", deviceMode);
        contentValues.put("timestamp", timestamp);
        sqLiteDatabase.insert(TABLE_NAME, null, contentValues);
    }

//    public void insert(ContentValues contentValues, String tableName) {
//        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
//        sqLiteDatabase.insert(tableName, null, contentValues);
//    }

    @SuppressLint("Recycle")
    public boolean isDeviceExist(String deviceId) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        Cursor result = sqLiteDatabase.query(TABLE_NAME, null,
                "device_id=?", new String[]{deviceId}, null, null, null);
        return result.getCount() != 0;
    }

    /*
     * 查询全部deviceId数据
     * SELECT <deviceId> FROM <TABLE_NAME>
     */
    @SuppressLint("Recycle")
    public List<String> queryAllDeviceIds() {
        List<String> deviceIds = new ArrayList<>();
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        try (Cursor result = sqLiteDatabase.query(TABLE_NAME, new String[]{"device_id"},
                null, null, null, null, null)) {
            int index = result.getColumnIndexOrThrow("device_id");
            while (result.moveToNext()) {
                deviceIds.add(result.getString(index));
            }
            Log.v(LOG_NAME, "Query " + deviceIds.size() + " device ids");
        } catch (Exception e) {
            Log.e(LOG_NAME, "Cursor device ID error!", e);
        }
        return deviceIds;
    }

    /*
     * SELECT * FROM <TABLE_NAME> WHERE DEVICE_ID = <deviceId>
     */
    @SuppressLint("Recycle")
    public Long queryDeviceMode(String deviceId) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        try {
            Cursor result = sqLiteDatabase.query(TABLE_NAME, new String[]{"device_mode"},
                    "device_id=?", new String[]{deviceId}, null, null, null);
            if (result.moveToFirst()) {
                Log.v(LOG_NAME, "Device mode is cursored!");
                return result.getLong(result.getColumnIndexOrThrow("device_mode"));
            }
        } catch (Exception e) {
            Log.e(LOG_NAME, "Cursor device mode error!", e);
        }
        return -1L;
    }

    @SuppressLint("Recycle")
    public Long queryDeviceTimestamp(String deviceId) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        try {
            Cursor result = sqLiteDatabase.query(TABLE_NAME, new String[]{"timestamp"},
                    "device_id=?", new String[]{deviceId}, null, null, null);
            if (result.moveToFirst()) {
                Log.v(LOG_NAME, "Timestamp is cursored!");
                return result.getLong(result.getColumnIndexOrThrow("timestamp"));
            }
        } catch (Exception e) {
            Log.e(LOG_NAME, "Cursor timestamp error!", e);
        }
        return -1L;
    }

    public void updateDeviceTimestamp(String deviceId, long timestamp) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put("timestamp", timestamp);
        sqLiteDatabase.update(TABLE_NAME, contentValues, "device_id = ?", new String[]{deviceId});
    }
}
