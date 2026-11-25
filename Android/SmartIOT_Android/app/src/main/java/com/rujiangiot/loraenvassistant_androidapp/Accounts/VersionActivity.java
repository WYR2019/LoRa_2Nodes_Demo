package com.rujiangiot.loraenvassistant_androidapp.Accounts;

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.rujiangiot.loraenvassistant_androidapp.R;
import com.rujiangiot.loraenvassistant_androidapp.Tools.GetAppVersion.GetAppVersion;

public class VersionActivity extends AppCompatActivity {

    private TextView tvVersionCode;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_version);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        init();
        try {
            int appVersionCode = GetAppVersion.getAppVersionCode(this);
            String appVersionName = GetAppVersion.getAppVersionName(this);
            tvVersionCode.setText(appVersionName);
        } catch (PackageManager.NameNotFoundException e) {
            throw new RuntimeException(e);
        }
    }

    private void init() {
        tvVersionCode = findViewById(R.id.tv_version_code);
    }
}