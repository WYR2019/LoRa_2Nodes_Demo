package com.example.emqx_smartbadmintongym.Customer.Login;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.example.emqx_smartbadmintongym.Customer.CustomerMainActivity;
import com.example.emqx_smartbadmintongym.Manager.Login.ManagerLoginActivity;
import com.example.emqx_smartbadmintongym.R;
import com.example.emqx_smartbadmintongym.SQLite.DBOpenHelper;

public class CustomerLoginActivity extends AppCompatActivity implements View.OnClickListener {

    private Button btnLogin;
    private EditText etTnumber,etPassword;
    private TextView tvRegister,tvChangePwd,tvLoginByTnumber,tvManagerLogin;
    private Switch swRememberPwd;
    /*sharedpreference功能*/
    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor editor;

    private DBOpenHelper dbOpenHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_customer_login);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right,systemBars.bottom);
            return insets;
        });
        Init();
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        dbOpenHelper = new DBOpenHelper(this);
        sharedPreferences=getSharedPreferences("login",MODE_PRIVATE);
        sharedPreferences = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        Boolean isremember = sharedPreferences.getBoolean("remember_password",false);
        if (isremember){
            String username = sharedPreferences.getString("username","");
            String tnumber = sharedPreferences.getString("tnumber","");
            String password = sharedPreferences.getString("password","");
            etTnumber.setText(tnumber);
            etPassword.setText(password);
            swRememberPwd.setChecked(true);
        }
    }

    @Override
    public void onClick(View v) {
        if(v.getId()==R.id.btn_login){
            String getTnumber = etTnumber.getText().toString();
            String getPassword = etPassword.getText().toString();

            boolean loginStatus = dbOpenHelper.mDataBaseLogin(getTnumber,getPassword);//传输入框获取的账号密码
            if (loginStatus)//判断布尔类型传的是成功还是失败了 当login是对的，说明登录成功了
            {
                Toast.makeText(this, "登录成功", Toast.LENGTH_SHORT).show();

                editor = sharedPreferences.edit();
                if (swRememberPwd.isChecked()) {
                    editor.putBoolean("remember_password", true);
                    editor.putString("tnumber",getTnumber);
                    editor.putString("password",getPassword);
                }
                else{
                    editor.clear();
                }
                editor.commit();

                Intent intent = new Intent(CustomerLoginActivity.this, CustomerMainActivity.class);

                intent.putExtra("tnumber",getTnumber);
                intent.putExtra("password",getPassword);

                startActivity(intent);
                finish();
            }else {
                Toast.makeText(this, "登陆失败", Toast.LENGTH_SHORT).show();
            }
        } else if (v.getId()==R.id.tv_register) {
            Intent intent1 = new Intent(CustomerLoginActivity.this, CustomerRegisterActivity.class);
            startActivity(intent1);
//            finish();
        } else if (v.getId()==R.id.tv_changepwd) {
            Intent intent = new Intent(CustomerLoginActivity.this, CustomerChangePasswordActivity.class);
            startActivity(intent);
        } else if (v.getId()==R.id.tv_login_by_tnumber) {
            Intent intent = new Intent(CustomerLoginActivity.this, CustomerLoginbyTnumberActivity.class);
            startActivity(intent);
        } else if (v.getId()==R.id.tv_manager_login) {
            Intent intent = new Intent(CustomerLoginActivity.this, ManagerLoginActivity.class);
            startActivity(intent);
        }
    }

    private void Init() {
        btnLogin = findViewById(R.id.btn_login);
        tvRegister = findViewById(R.id.tv_register);
        etTnumber = findViewById(R.id.et_tnumber);
        etPassword = findViewById(R.id.et_password);
        swRememberPwd = findViewById(R.id.sw_rememberpwd);
        tvChangePwd = findViewById(R.id.tv_changepwd);
        tvLoginByTnumber = findViewById(R.id.tv_login_by_tnumber);
        tvManagerLogin = findViewById(R.id.tv_manager_login);
        tvChangePwd.setOnClickListener(this);
        btnLogin.setOnClickListener(this);
        tvRegister.setOnClickListener(this);
        tvLoginByTnumber.setOnClickListener(this);
        tvManagerLogin.setOnClickListener(this);
    }
}