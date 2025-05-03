package com.example.emqx_smartbadmintongym.Customer.Login;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.example.emqx_smartbadmintongym.R;

public class CustomerLoginbyTnumberActivity extends AppCompatActivity implements View.OnClickListener{
    private TextView tvLoginByPassword;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_customer_login_by_tnumber);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        Init();
    }

    public void Init()
    {
        tvLoginByPassword = findViewById(R.id.tv_login_by_password);
        tvLoginByPassword.setOnClickListener(this);
    }

    @Override
    public void onClick(View v){
        if (v.getId()==R.id.tv_login_by_password){
            Intent intent = new Intent(this, CustomerLoginActivity.class);
            startActivity(intent);
            finish();
        }
    }
}