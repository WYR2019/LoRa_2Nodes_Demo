package com.rujiangiot.loraenvassistant_androidapp.Login;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.rujiangiot.loraenvassistant_androidapp.MainActivity;
import com.rujiangiot.loraenvassistant_androidapp.R;
import com.rujiangiot.loraenvassistant_androidapp.SQLite.User.UserSQLiteHelper;
import com.rujiangiot.loraenvassistant_androidapp.Tools.SharedPreferences.User.UserSPHelper;

public class LoginActivity extends AppCompatActivity implements View.OnClickListener {
    private Button btnLogin;
    private EditText etTelNumberOrUsername, etPassword;
    protected TextView tvRegister, tvForgotPassword;
    private SwitchCompat swAutoLogin;
    private UserSQLiteHelper userSQLiteHelper;
    private UserSPHelper userSPHelper;

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
        userSPHelper = new UserSPHelper(this);
        String telNumber = userSPHelper.spGetTelNumber();
        String username = userSPHelper.spGetUsername();
        /* 自动登录 */
        if (!userSPHelper.spGetLoginStatus()) {
            Intent intent = new Intent(LoginActivity.this, MainActivity.class);
            startActivity(intent);
        } else {
            if (!telNumber.isEmpty()) {
                etTelNumberOrUsername.setText(telNumber);
            } else if (!username.isEmpty()) {
                etTelNumberOrUsername.setText(username);
            }
        }
    }

    /**
     * Called when a view has been clicked.
     *
     * @param v The view that was clicked.
     */
    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_login) {
            String telNumber = etTelNumberOrUsername.getText().toString();
            String username = etTelNumberOrUsername.getText().toString();
            String password = etPassword.getText().toString();
            if (TextUtils.isEmpty(telNumber)) {
                Toast.makeText(this, "请填写用户名或手机号", Toast.LENGTH_SHORT).show();
            } else if (TextUtils.isEmpty(password)) {
                Toast.makeText(this, "请填写密码", Toast.LENGTH_SHORT).show();
            } else if (login(telNumber, username, password) == UserSQLiteHelper.queryUserStatus.TEL_NUMBER_QUERY_SUCCESS) {
                Toast.makeText(this, "登录成功，登录方式：电话号码", Toast.LENGTH_SHORT).show();
                userSPHelper.spLogout();
                userSPHelper.spAutoInput(telNumber, null);
                if (swAutoLogin.isChecked())
                    userSPHelper.spSaveLoginTime();
                Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                startActivity(intent);
                finish();
            } else if (login(telNumber, username, password) == UserSQLiteHelper.queryUserStatus.USERNAME_QUERY_SUCCESS) {
                Toast.makeText(this, "登录成功，登录方式：用户名", Toast.LENGTH_SHORT).show();
                userSPHelper.spLogout();
                userSPHelper.spAutoInput(null, username);
                if (swAutoLogin.isChecked())
                    userSPHelper.spSaveLoginTime();
                Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                startActivity(intent);
                finish();
            } else if (login(telNumber, username, password) == UserSQLiteHelper.queryUserStatus.ERR_TEL_NUMBER_OR_USERNAME_INCORRECT) {
                Toast.makeText(this, "电话号码或账户名错误", Toast.LENGTH_SHORT).show();
            } else if (login(telNumber, username, password) == UserSQLiteHelper.queryUserStatus.ERR_PASSWORD_INCORRECT) {
                Toast.makeText(this, "密码错误", Toast.LENGTH_SHORT).show();
            } else if (login(telNumber, username, password) == UserSQLiteHelper.queryUserStatus.ERR_ANY) {
                Toast.makeText(this, "系统故障", Toast.LENGTH_SHORT).show();
            }
        } else if (v.getId() == R.id.tv_register) {
            Intent intent = new Intent(LoginActivity.this, RegisterActivity.class);
            startActivity(intent);
        } else if (v.getId() == R.id.tv_forgot_password) {
            Intent intent = new Intent(LoginActivity.this, ForgotPasswordActivity.class);
            startActivity(intent);
        }
    }

    private void init() {
        btnLogin = findViewById(R.id.btn_login);
        tvRegister = findViewById(R.id.tv_register);
        tvForgotPassword = findViewById(R.id.tv_forgot_password);
        etTelNumberOrUsername = findViewById(R.id.et_tel_number_or_username);
        etPassword = findViewById(R.id.et_password);
        swAutoLogin = findViewById(R.id.sw_autologin);
        btnLogin.setOnClickListener(this);
        tvRegister.setOnClickListener(this);
        tvForgotPassword.setOnClickListener(this);
    }

    private UserSQLiteHelper.queryUserStatus login(String telNumber, String username, String password) {
        // 传输入框获取的账号密码
        return userSQLiteHelper.queryPassword(telNumber, username, password);
    }
}