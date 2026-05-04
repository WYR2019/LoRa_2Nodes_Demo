package com.example.emqx_smartbadmintongym.SQLite;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import com.example.emqx_smartbadmintongym.SQLite.Bean.User;

public class DBOpenHelper extends SQLiteOpenHelper {
    private SQLiteDatabase sqLiteDatabase;
    private static final String DB_NAME = "EMQXSmartBadmintonGym";
    private static final String TABLE_NAME_USER_CUSTOMER = "CREATE TABLE UserCustomer(" +
            "tnumber text PRIMARY KEY," +
            "username text," +
            "password text)";
    private static final String TABLE_NAME_USER_MANAGER = "CREATE TABLE UserManager(" +
            "employeeid text PRIMARY KEY," +
            "password text)";
    private static final String TABLE_NAME_GYM_Management = "CREATE TABLE GymManagement(" +
            "employeeid text PRIMARY KEY," +
            "password text)";
    public DBOpenHelper(Context context) {
        super(context, DB_NAME, null, 1);
    }

    /* 建表 */
    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL(TABLE_NAME_USER_CUSTOMER);
        db.execSQL(TABLE_NAME_USER_MANAGER);
        db.execSQL(TABLE_NAME_GYM_Management);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS UserCustomer");
        db.execSQL("DROP TABLE IF EXISTS UserManager");
        db.execSQL("DROP TABLE IF EXISTS GymManagement");
        onCreate(db);
    }

    public long mDataBaseRegister(User user) {
        sqLiteDatabase = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put("tnumber",user.getTnumber());
        cv.put("username",user.getUsername());
        cv.put("password",user.getPassword());
        long userCustomers = sqLiteDatabase.insert("UserCustomer",null,cv);
        return userCustomers;
    }

    public Boolean mDataBaseLogin(String tnumber,String password) {
        SQLiteDatabase writeSQLUserData = getWritableDatabase();
        boolean resPassword = false;
        //得到一个返回值
        Cursor cursorTnumber = writeSQLUserData.query("UserCustomer",null,"tnumber like ?",new String[]{tnumber},null,null,null);
        if (cursorTnumber != null){
            while (cursorTnumber.moveToNext()){//每一行数据循环扫描
                String strPassword = cursorTnumber.getString(2);//获取密码
                resPassword = strPassword.equals(password);//对比密码是否正确
                return resPassword;
            }
        }
        return false;
    }
}
