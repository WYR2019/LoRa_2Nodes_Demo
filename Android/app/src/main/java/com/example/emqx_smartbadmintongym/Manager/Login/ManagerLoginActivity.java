package com.example.emqx_smartbadmintongym.Manager.Login;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.example.emqx_smartbadmintongym.Customer.Login.CustomerLoginActivity;
import com.example.emqx_smartbadmintongym.MainActivity;
import com.example.emqx_smartbadmintongym.R;

public class ManagerLoginActivity extends AppCompatActivity implements View.OnClickListener {

    private Button btnManagerLogin;
    private EditText etWorknumber, etManagerPassword;
    private TextView tvReturn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_manager_login);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        Init();
    }

    /**
     * Called when a view has been clicked.
     *
     * @param v The view that was clicked.
     */
    @Override
    public void onClick(View v) {
        if (v.getId()==R.id.btn_manager_login) {
//            if (etWorknumber.equals("12345")&&etManagerPassword.equals("12345")) {
//                Intent intent = new Intent(ManagerLoginActivity.this, MainActivity.class);
//                startActivity(intent);
//                finish();
//            }
            Intent intent = new Intent(ManagerLoginActivity.this, MainActivity.class);
                startActivity(intent);
                finish();
        } else if (v.getId()==R.id.tv_return) {
            Intent intent = new Intent(ManagerLoginActivity.this, CustomerLoginActivity.class);
            startActivity(intent);
            finish();
        }
    }

    private void Init() {
        btnManagerLogin = findViewById(R.id.btn_manager_login);
        etWorknumber = findViewById(R.id.et_worknumber);
        etManagerPassword = findViewById(R.id.et_manager_password);
        tvReturn = findViewById(R.id.tv_return);
        btnManagerLogin.setOnClickListener(this);
        tvReturn.setOnClickListener(this);
    }
}