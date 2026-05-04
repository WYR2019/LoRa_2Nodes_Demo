package com.rujiangtechcomcn.loraenvassistant_androidapp.Login;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.rujiangtechcomcn.loraenvassistant_androidapp.MainActivity;
import com.rujiangtechcomcn.loraenvassistant_androidapp.R;
import com.rujiangtechcomcn.loraenvassistant_androidapp.SQLite.UserSQLiteHelper;

public class LoginActivity extends AppCompatActivity implements View.OnClickListener {
    private Button btnLogin;
    private EditText etTnumberOrUserName, etPassword;
    protected TextView tvRegister;
    private SwitchCompat swRememberPwd;
    private SharedPreferences sharedPreferences;
    private UserSQLiteHelper userSQLiteHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_login);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        init();
    }

    @Override
    protected void onStart() {
        super.onStart();
        userSQLiteHelper = new UserSQLiteHelper(this);
        sharedPreferences = getSharedPreferences("login", MODE_PRIVATE);
        sharedPreferences = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        boolean isRemember = sharedPreferences.getBoolean("remember_password", false);
        if (isRemember) {
            String tnumberOrUsername = sharedPreferences.getString("tnumber", "");
            String password = sharedPreferences.getString("password", "");
            etTnumberOrUserName.setText(tnumberOrUsername);
            etPassword.setText(password);
            swRememberPwd.setChecked(true);
        }
//        Boolean isRemember = false;
//        String tnumberOrUsername;
//        String password;
//        if (spGetText(isRemember, tnumberOrUsername, password)) {
//            etTnumber.setText(tnumberOrUsername);
//            etPassword.setText(password);
//            swRememberPwd.setChecked(true);
//        }
    }

    /**
     * Called when a view has been clicked.
     *
     * @param v The view that was clicked.
     */
    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_login) {
            String tnumber = etTnumberOrUserName.getText().toString();
            String username = etTnumberOrUserName.getText().toString();
            String password = etPassword.getText().toString();
            if (TextUtils.isEmpty(tnumber)) {
                Toast.makeText(this, "请填写用户名或手机号", Toast.LENGTH_SHORT).show();
            } else if (TextUtils.isEmpty(password)) {
                Toast.makeText(this, "请填写密码", Toast.LENGTH_SHORT).show();
            } else if (login(tnumber, username, password) == UserSQLiteHelper.queryUserStatus.SUCCESS) {
                spGetText(tnumber, password, swRememberPwd.isChecked());
                Toast.makeText(this, "登录成功", Toast.LENGTH_SHORT).show();
                /* 跳转页面同时传递数据 */
                Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                intent.putExtra("tnumber", tnumber);
                intent.putExtra("username", username);
                intent.putExtra("password", password);
                startActivity(intent);
                finish();
            } else if (login(tnumber, username, password) == UserSQLiteHelper.queryUserStatus.ERR_TNUMBER_OR_USERNAME_INCORRECT) {
                Toast.makeText(this, "电话号码或账户名错误", Toast.LENGTH_SHORT).show();
            } else if (login(tnumber, username, password) == UserSQLiteHelper.queryUserStatus.ERR_PASSWORD_INCORRECT) {
                Toast.makeText(this, "密码错误", Toast.LENGTH_SHORT).show();
            } else if (login(tnumber, username, password) == UserSQLiteHelper.queryUserStatus.ERR_ANY) {
                Toast.makeText(this, "系统故障", Toast.LENGTH_SHORT).show();
            }
        } else if (v.getId() == R.id.tv_register) {
            Intent intent = new Intent(LoginActivity.this, RegisterActivity.class);
            startActivity(intent);
        }
    }

    private void init() {
        btnLogin = findViewById(R.id.btn_login);
        tvRegister = findViewById(R.id.tv_register);
        etTnumberOrUserName = findViewById(R.id.et_tnumber);
        etPassword = findViewById(R.id.et_password);
        swRememberPwd = findViewById(R.id.sw_rememberpwd);
        btnLogin.setOnClickListener(this);
        tvRegister.setOnClickListener(this);
    }

    private UserSQLiteHelper.queryUserStatus login(String tnumber, String username, String password) {
        // 传输入框获取的账号密码
        return userSQLiteHelper.queryUserData(tnumber, username, password);
    }

    public void spGetText(String tnumberOrUsername, String password, boolean condition) {
        SharedPreferences.Editor editor;
        editor = sharedPreferences.edit();
        if (condition) {
            editor.putBoolean("remember_password", true);
            editor.putString("tnumber", tnumberOrUsername);
            editor.putString("password", password);
        } else {
            editor.clear();
        }
        editor.apply();
    }

    public boolean spAutoPutText() {
        sharedPreferences = getSharedPreferences("login", MODE_PRIVATE);
        sharedPreferences = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        boolean isRemember = sharedPreferences.getBoolean("remember_password", false);
        if (isRemember) {
            String tnumberOrUsername = sharedPreferences.getString("tnumber", "");
            String password = sharedPreferences.getString("password", "");
            return true;
        }
        return false;
    }
}