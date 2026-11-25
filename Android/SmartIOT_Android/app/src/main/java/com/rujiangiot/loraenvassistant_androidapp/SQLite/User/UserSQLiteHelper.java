package com.rujiangiot.loraenvassistant_androidapp.SQLite.User;

import android.annotation.SuppressLint;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import com.rujiangiot.loraenvassistant_androidapp.SQLite.User.Entity.User;

import java.util.Objects;

public class UserSQLiteHelper extends SQLiteOpenHelper {
    public enum insertUserStatus {
        REGISTER_SUCCESS,
        REGISTER_ERROR
    }
    public enum queryUserStatus {
        TEL_NUMBER_QUERY_SUCCESS,
        USERNAME_QUERY_SUCCESS,
        ERR_TEL_NUMBER_OR_USERNAME_INCORRECT,
        ERR_PASSWORD_INCORRECT,
        ERR_ANY
    }

    public static final String LOG_NAME = "UserSQLiteDebug";
    private static final String DB_NAME = "LoRaEnvAssistant_AndroidApp.db";
    private static final String TABLE_NAME = "user";
    private static final String CREATE_TABLE_SQL = "CREATE TABLE " + TABLE_NAME + "(" +
            "tel_number text PRIMARY KEY," +
            "username text," +
            "password text)";

    public UserSQLiteHelper(Context context) {
        super(context, DB_NAME, null, 6);
    }

    @Override
    public void onCreate(SQLiteDatabase sqLiteDatabase) {
        sqLiteDatabase.execSQL(CREATE_TABLE_SQL);
    }

    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int oldVersion, int newVersion) {
        sqLiteDatabase.execSQL("DROP TABLE IF EXISTS " + TABLE_NAME);
        onCreate(sqLiteDatabase);
    }

    public long insertUserData(User user) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put("tel_number", user.getTelNumber());
        contentValues.put("username", user.getUsername());
        contentValues.put("password", user.getPassword());
        return sqLiteDatabase.insert(TABLE_NAME, null, contentValues);
    }

    public queryUserStatus queryPassword(String telNumber, String username, String password) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        /* 查询电话号码或账户名 */
        @SuppressLint("Recycle") Cursor telNumberResult = sqLiteDatabase.query
                (TABLE_NAME, null, "tel_number=?",
                        new String[]{telNumber}, null, null, null);
        /* 查询到了 */
        if (telNumberResult.getCount() != 0) {
            Log.v(LOG_NAME, "Telephone number is cursored!");
            while (telNumberResult.moveToNext()) {                                                  // 每一行数据循环扫描
                String getSQLPassword = telNumberResult.getString
                        (telNumberResult.getColumnIndexOrThrow("password"));            // 获取密码
                if (Objects.equals(getSQLPassword, password)) {                                     // 对比密码是否正确
                    return queryUserStatus.TEL_NUMBER_QUERY_SUCCESS;
                }
            }
            return queryUserStatus.ERR_PASSWORD_INCORRECT;
        }
        @SuppressLint("Recycle") Cursor usernameResult = sqLiteDatabase.query
                (TABLE_NAME, null, "username=?",
                        new String[]{username}, null, null, null);
        if (usernameResult.getCount() != 0) {
            Log.v(LOG_NAME, "Username is cursored!");
            while (usernameResult.moveToNext()) {                                                   // 每一行数据循环扫描
                String getSQLPassword = usernameResult.getString
                        (usernameResult.getColumnIndexOrThrow("password"));             // 获取密码
                if (Objects.equals(getSQLPassword, password)) {                                     // 对比密码是否正确
                    return queryUserStatus.USERNAME_QUERY_SUCCESS;
                }
            }
            return queryUserStatus.ERR_PASSWORD_INCORRECT;
        }
        if (telNumberResult.getCount() == 0 && usernameResult.getCount() == 0) {
            Log.e(LOG_NAME, "Cursor telephone number or username error!");
            return queryUserStatus.ERR_TEL_NUMBER_OR_USERNAME_INCORRECT;
        }
        return queryUserStatus.ERR_ANY;
    }

    public String queryUsernameByTelNumber(String telNumber) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        String username = null;
        Cursor telNumberResult = sqLiteDatabase.query
                (TABLE_NAME, new String[]{"username"}, "tel_number=?",
                        new String[]{telNumber}, null, null, null);
        if (telNumberResult.getCount() != 0) {
            Log.v(LOG_NAME, "Username is cursored!");
            if (telNumberResult.moveToNext()) {
                username = telNumberResult.getString
                        (telNumberResult.getColumnIndexOrThrow("username"));
            }
        }
        telNumberResult.close();
        sqLiteDatabase.close();
        return username;
    }

    public int updateUsername(User user) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        ContentValues contentValues = new ContentValues();                                          // 组装数据
        contentValues.put("username", user.getUsername());
//        contentValues.put("password", user.getPassword());
        return sqLiteDatabase.update(TABLE_NAME, contentValues, "username=?",
                new String[]{user.getUsername()});
    }

    public int updatePassword(User user) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        ContentValues contentValues = new ContentValues();
//        contentValues.put("username", user.getUsername());
        contentValues.put("password", user.getPassword());
        return sqLiteDatabase.update(TABLE_NAME, contentValues, "username=?",
                new String[]{user.getUsername()});
    }
}
