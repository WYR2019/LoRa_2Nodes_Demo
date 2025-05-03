package com.example.emqx_smartbadmintongym.Customer.Login;

import android.content.Intent;
import android.content.SharedPreferences;
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

import com.example.emqx_smartbadmintongym.SQLite.Bean.User;
import com.example.emqx_smartbadmintongym.R;
import com.example.emqx_smartbadmintongym.SQLite.DBOpenHelper;

public class CustomerRegisterActivity extends AppCompatActivity {

    private Button btnRegister,btnReturn;
    private EditText etTnumber,etUsername,etPassword,etConfirmPassword;
    private DBOpenHelper dbOpenHelperRegister;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_customer_register);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right,systemBars.bottom);
            return insets;
        });
        Init();
    }

    @Override
    protected void onStart() {
        super.onStart();
        dbOpenHelperRegister = new DBOpenHelper(this);
    }

    private void Init() {
        btnRegister = findViewById(R.id.btn_register);
        btnReturn = findViewById(R.id.btn_return);
        etTnumber = findViewById(R.id.et_reg_tnumber);
        etUsername = findViewById(R.id.et_reg_username);
        etPassword = findViewById(R.id.et_reg_password);
        etConfirmPassword = findViewById(R.id.et_reg_confirm_password);
    }
    public void mRegister(View view){
        String strTnumber = etTnumber.getText().toString();
        String strUsername = etUsername.getText().toString();
        String strPassword = etPassword.getText().toString();
        String strConfirmPassword = etConfirmPassword.getText().toString();
        User u = new User (strTnumber,strUsername,strPassword);
        long l = dbOpenHelperRegister.mDataBaseRegister(u);
        if (l ==-1){
            Toast.makeText(this, "注册失败", Toast.LENGTH_SHORT).show();
        } else if (TextUtils.isEmpty(strUsername)) {
            etUsername.setError("账号信息填写错误");
        }else if (TextUtils.isEmpty(strPassword)) {
            Toast.makeText(getApplicationContext(),
                    "请确认密码", Toast.LENGTH_SHORT).show();
        } else if (!strConfirmPassword.equals(strPassword)) {
            Toast.makeText(getApplicationContext(),
                    "请注意密码格式", Toast.LENGTH_SHORT).show();
        } else {
            SharedPreferences.Editor editor = getSharedPreferences("password",MODE_PRIVATE).edit();
            editor.putString("tnumber",etTnumber.getText().toString());
            editor.putString("password",etPassword.getText().toString());
            editor.commit();

            Toast.makeText(this, "注册成功", Toast.LENGTH_SHORT).show();
            Intent intent = new Intent(CustomerRegisterActivity.this, CustomerLoginActivity.class);
            startActivity(intent);
            finish();
        }
    }

    public void mReturn(View v) {
        if (v.getId()==R.id.btn_return){
            Intent intent = new Intent(CustomerRegisterActivity.this, CustomerLoginActivity.class);
            startActivity(intent);
            finish();
        }
    }
}
