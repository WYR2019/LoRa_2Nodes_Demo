package com.rujiangtechcomcn.loraenvassistant_androidapp;

import android.os.Bundle;
import android.view.MenuItem;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentTransaction;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.rujiangtechcomcn.loraenvassistant_androidapp.Fragments.AccountsFragment;
import com.rujiangtechcomcn.loraenvassistant_androidapp.Fragments.DevicesFragment;
import com.rujiangtechcomcn.loraenvassistant_androidapp.Fragments.MainDashboardFragment;

// 要改进的问题：
// 1、夜晚模式ui问题
// 2、语言
// 3、登录模块与注册模块的代码一致性（已完成）
// 4、登录模块的问题反馈（已完成）
// 5、注册模块materials ui重合问题
// 6、添加图形验证、手机号校验的功能
// 7、添加自动登录
// 8、添加修改密码和注销账户的功能
// 9、如何确保密码不明文传输

public class MainActivity extends AppCompatActivity {

    private AccountsFragment accountsFragment;
    private DevicesFragment devicesFragment;
    private MainDashboardFragment mainDashboardFragment;
    private BottomNavigationView bottomNavigationView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        bottomNavigationView = findViewById(R.id.BottomNavigationView);
        bottomNavigationView.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                if (item.getItemId() == R.id.dashboard) {
                    selectedFragment(0);
                } else if (item.getItemId() == R.id.devices) {
                    selectedFragment(1);
                } else {
                    selectedFragment(2);
                }
                return true;
            }
        });
        // 默认首页选中
        selectedFragment(0);
    }

    private void selectedFragment(int position) {
        // fragment布局加载器
        FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
        hidefragment(fragmentTransaction);

        // 第一个被选中
        if (position == 0) {
            if (mainDashboardFragment == null) {
                mainDashboardFragment = new MainDashboardFragment();
                fragmentTransaction.add(R.id.content, mainDashboardFragment);
            } else {
                fragmentTransaction.show(mainDashboardFragment);
            }
        } else if (position == 1) {
            if (devicesFragment == null) {
                devicesFragment = new DevicesFragment();
                fragmentTransaction.add(R.id.content, devicesFragment);
            } else {
                fragmentTransaction.show(devicesFragment);
            }
        } else {
            if (accountsFragment == null) {
                accountsFragment = new AccountsFragment();
                fragmentTransaction.add(R.id.content, accountsFragment);
            } else {
                fragmentTransaction.show(accountsFragment);
            }
        }

        // 提交
        fragmentTransaction.commit();
    }

    // 点击其中一个fragment的时候需要将其他fragment隐藏
    private void hidefragment(FragmentTransaction fragmentTransaction) {
        if (mainDashboardFragment != null) {
            fragmentTransaction.hide(mainDashboardFragment);
        }
        if (devicesFragment != null) {
            fragmentTransaction.hide(devicesFragment);
        }
        if (accountsFragment != null) {
            fragmentTransaction.hide(accountsFragment);
        }
    }
}