package com.rujiangtechcomcn.loraenvassistant_androidapp.SQLite;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import com.rujiangtechcomcn.loraenvassistant_androidapp.SQLite.Bean.User;

import java.util.Objects;

public class UserSQLiteHelper extends SQLiteOpenHelper {
    public enum insertUserStatus {
        REGISTER_SUCCESS,
        REGISTER_ERROR,
        ERR_TNUMBER_IS_DUPLICATE,
        ERR_PASSWORD_INCORRECT
    }
    public enum queryUserStatus {
        SUCCESS,
        ERR_TNUMBER_OR_USERNAME_INCORRECT,
        ERR_PASSWORD_INCORRECT,
        ERR_ANY
    }

    public static final String LOG_NAME = "AppSQLiteLogDebug";
    private static final String DB_NAME = "LoRaEnvAssistant_AndroidApp.db";
    private static final String TABLE_NAME = "CREATE TABLE user(" +
            "tnumber text PRIMARY KEY," +
            "username text," +
            "password text)";

    public UserSQLiteHelper(Context context) {
        super(context, DB_NAME, null, 1);
    }

    @Override
    public void onCreate(SQLiteDatabase sqLiteDatabase) {
        sqLiteDatabase.execSQL(TABLE_NAME);
    }

    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int oldVersion, int newVersion) {
        sqLiteDatabase.execSQL("DROP TABLE IF EXISTS user");
        onCreate(sqLiteDatabase);
    }

    public long insertUserData(User user) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put("tnumber", user.getTnumber());
        contentValues.put("username", user.getUsername());
        contentValues.put("password", user.getPassword());
        return sqLiteDatabase.insert("user", null, contentValues);
    }

    public queryUserStatus queryUserData(String tnumber, String username, String password) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        /* 查询电话号码或账户名 */
        Cursor cursorTnumber = sqLiteDatabase.query
                ("user", null, "tnumber=?", new String[]{tnumber}, null, null, null);
        Cursor cursorUserName = sqLiteDatabase.query
                ("user", null, "username=?", new String[]{username}, null, null, null);
        /* 查询到了 */
        if (cursorTnumber.getCount() != 0) {
            Log.d(LOG_NAME, "Telephone number is cursored!");
            while (cursorTnumber.moveToNext()) {                                                                                            // 每一行数据循环扫描
                String getSQLPassword = cursorTnumber.getString
                        (cursorTnumber.getColumnIndexOrThrow("password"));                                                      // 获取密码
                if (Objects.equals(getSQLPassword, password)) {
                    return queryUserStatus.SUCCESS;                    // 对比密码是否正确
                }
            }
            return queryUserStatus.ERR_PASSWORD_INCORRECT;
        }
        if (cursorUserName.getCount() != 0) {
            Log.d(LOG_NAME, "Username is cursored!");
            while (cursorUserName.moveToNext()) {                                                                                           // 每一行数据循环扫描
                String getSQLPassword = cursorUserName.getString
                        (cursorUserName.getColumnIndexOrThrow("password"));                                                      // 获取密码
                if (Objects.equals(getSQLPassword, password)) {
                    return queryUserStatus.SUCCESS;                    // 对比密码是否正确
                }
            }
            return queryUserStatus.ERR_PASSWORD_INCORRECT;
        }
        if (cursorTnumber.getCount() == 0 && cursorUserName.getCount() == 0) {
            Log.d(LOG_NAME, "Cursor telephone number or username error!");
            return queryUserStatus.ERR_TNUMBER_OR_USERNAME_INCORRECT;
        }
        return queryUserStatus.ERR_ANY;
    }

    public int updateUsername(User user) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        ContentValues contentValues = new ContentValues(); // 这个是干什么的
        contentValues.put("username", user.getUsername());
//        contentValues.put("password", user.getPassword());
        return sqLiteDatabase.update(TABLE_NAME, contentValues, "username = ?", new String[]{user.getUsername()});
    }

    public int updatePassword(User user) {
        SQLiteDatabase sqLiteDatabase = getWritableDatabase();
        ContentValues contentValues = new ContentValues();
        contentValues.put("username", user.getUsername());
        contentValues.put("password", user.getPassword());
        return sqLiteDatabase.update(TABLE_NAME, contentValues, "username = ?", new String[]{user.getUsername()});
    }
}
