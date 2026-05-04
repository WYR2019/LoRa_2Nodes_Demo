package com.rujiangtechcomcn.loraenvassistant_androidapp.Login;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.rujiangtechcomcn.loraenvassistant_androidapp.R;
import com.rujiangtechcomcn.loraenvassistant_androidapp.SQLite.Bean.User;
import com.rujiangtechcomcn.loraenvassistant_androidapp.SQLite.UserSQLiteHelper;

public class RegisterActivity extends AppCompatActivity implements View.OnClickListener {
    private Button btnRegister, btnReturn;
    private EditText etTnumber, etUsername, etPassword, etConfirmPassword;
    private UserSQLiteHelper userSQLiteHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_register);
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
    }

    private void init() {
        btnRegister = findViewById(R.id.btn_register);
        btnReturn = findViewById(R.id.btn_return);
        etTnumber = findViewById(R.id.et_reg_tnumber);
        etUsername = findViewById(R.id.et_reg_username);
        etPassword = findViewById(R.id.et_reg_password);
        etConfirmPassword = findViewById(R.id.et_reg_confirm_password);
        btnRegister.setOnClickListener(this);
        btnReturn.setOnClickListener(this);
    }

    private UserSQLiteHelper.insertUserStatus register(String tnumber, String username, String password) {
        User user = new User(tnumber, username, password);
        long status = userSQLiteHelper.insertUserData(user);
        if (status == -1)                                                               // 找到cursor游标
            return UserSQLiteHelper.insertUserStatus.REGISTER_ERROR;
        return UserSQLiteHelper.insertUserStatus.REGISTER_SUCCESS;
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_register) {
            String tnumber = etTnumber.getText().toString();
            String username = etUsername.getText().toString();
            String password = etPassword.getText().toString();
            String confirmPassword = etConfirmPassword.getText().toString();
            UserSQLiteHelper.insertUserStatus result = register(tnumber, username, password);                                      // 重点：只调用一次register方法
            if (TextUtils.isEmpty(tnumber)) {
                etTnumber.setError("请填写手机号码");
                Toast.makeText(getApplicationContext(), "请填写手机号码", Toast.LENGTH_SHORT).show();
            } else if (TextUtils.isEmpty(username)) {
                etUsername.setError("请填写用户名");
                Toast.makeText(getApplicationContext(), "请填写用户名", Toast.LENGTH_SHORT).show();
            } else if (TextUtils.isEmpty(password)) {
                etPassword.setError("请填写密码");
                Toast.makeText(getApplicationContext(), "请填写密码", Toast.LENGTH_SHORT).show();
            } else if (TextUtils.isEmpty(confirmPassword)) {
                etConfirmPassword.setError("请确认密码");
                Toast.makeText(getApplicationContext(), "请确认密码", Toast.LENGTH_SHORT).show();
            } else if (!TextUtils.isEmpty(confirmPassword) && !confirmPassword.equals(password)) {
                etConfirmPassword.setError("两次密码填写不一致");
                Toast.makeText(getApplicationContext(), "两次密码填写不一致", Toast.LENGTH_SHORT).show();
            } else if (result == UserSQLiteHelper.insertUserStatus.REGISTER_SUCCESS) {
                Toast.makeText(this, "注册成功", Toast.LENGTH_SHORT).show();
                Intent intent = new Intent(RegisterActivity.this, LoginActivity.class);
                startActivity(intent);
                finish();
            } else if (result == UserSQLiteHelper.insertUserStatus.REGISTER_ERROR) {
                etTnumber.setError("该手机号已经被注册了");
                Toast.makeText(this, "该手机号已经被注册了", Toast.LENGTH_SHORT).show();
            }
        } else if (v.getId() == R.id.btn_return) {
            Intent intent = new Intent(RegisterActivity.this, LoginActivity.class);
            startActivity(intent);
            finish();
        }
    }
}