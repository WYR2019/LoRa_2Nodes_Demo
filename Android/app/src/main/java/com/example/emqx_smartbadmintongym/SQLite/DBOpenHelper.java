package com.example.emqx_smartbadmintongym.SQLite;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import com.example.emqx_smartbadmintongym.SQLite.Bean.User;

public class DBOpenHelper extends SQLiteOpenHelper {
    private SQLiteDatabase sqLiteDatabase;
    private static final String DB_NAME = "Sports_Assistant_System";
    private static final String TABLE_NAME = "CREATE TABLE userstatus(" +
            "tnumber text PRIMARY KEY," +
            "username text," +
            "password text)";
    public DBOpenHelper(Context context) {
        super(context, DB_NAME, null, 1);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL(TABLE_NAME);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS userstatus");
        onCreate(db);
    }

    public long mDataBaseRegister(User userPut){
        sqLiteDatabase = getWritableDatabase();
        ContentValues cv = new ContentValues();
        cv.put("tnumber",userPut.getTnumber());
        cv.put("username",userPut.getUsername());
        cv.put("password",userPut.getPassword());
        long users = sqLiteDatabase.insert("userstatus",null,cv);
        return users;
    }

    public Boolean mDataBaseLogin(String tnumber,String password){
        SQLiteDatabase writeSQLUserData = getWritableDatabase();
        boolean resPassword = false;
        //得到一个返回值
        Cursor cursorTnumber = writeSQLUserData.query("userstatus",null,"tnumber like ?",new String[]{tnumber},null,null,null);
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
