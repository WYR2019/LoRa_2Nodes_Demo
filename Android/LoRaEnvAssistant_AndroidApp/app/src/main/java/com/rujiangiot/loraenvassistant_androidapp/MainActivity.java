package com.rujiangiot.loraenvassistant_androidapp;

import android.os.Build;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentTransaction;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.rujiangiot.loraenvassistant_androidapp.Fragments.AccountsFragment;
import com.rujiangiot.loraenvassistant_androidapp.Fragments.DevicesFragment;
import com.rujiangiot.loraenvassistant_androidapp.Fragments.OverviewFragment;
import com.rujiangiot.loraenvassistant_androidapp.Tools.Permission.PermissionsHelper;

// 要改进的问题：
// 1、夜晚模式ui适配问题
// 2、语言
// 3、登录模块与注册模块的代码一致性（已完成）
// 4、登录模块的问题反馈（已完成）
// 5、注册模块materials ui重合问题
// 6、添加图形验证、手机号校验的功能
// 7、添加自动登录（已完成）
// 8、添加修改密码和注销账户的功能
// 9、如何确保密码不明文传输
// 10、（很重要）温湿度数据只能传输一次（已解决）
// 11、查询用户名并显示在用户界面上，通过密码查询用户名，但如果密码一样，则只会查到更早注册的用户名。需要使查询具有唯一性（已完成）。
// 12、sp取值即使是空字符串也只是“”而不是null，电话号码非null，但是字符串可以为null，因错误使用.equals()函数，导致登录时自动填充用户名失败。（已解决）
// 13、完善设备界面
// 14、优化json解析的bug（已解决）
// 15、完成设备存储、显示以及改名的功能（还差改名）
// 16、设备数据库创建失败（已解决）
// 17、添加网络状态和蓝牙状态监测
// 18、更改MQTT连接失败机制，添加断开重连机制。
// 19、添加数据库插入时间戳功能。(已完成)
// 20、添加判定设备在线功能。（已完成）
// 21、加一个启动界面。

public class MainActivity extends AppCompatActivity {

    private AccountsFragment accountsFragment;
    private DevicesFragment devicesFragment;
    private OverviewFragment overviewFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        BottomNavigationView bottomNavigationView = findViewById(R.id.BottomNavigationView);
        bottomNavigationView.setOnNavigationItemSelectedListener(item -> {
            if (item.getItemId() == R.id.overview) {
                selectedFragment(0);
            } else if (item.getItemId() == R.id.devices) {
                selectedFragment(1);
            } else {
                selectedFragment(2);
            }
            return true;
        });
        /* 修复因为Activity重建时没有区分“首次创建”和“从保存状态恢复”，导致系统恢复Fragment时造成重叠的问题。 */
        if (savedInstanceState == null) {
            /* 仅在首次创建时手动添加 Fragment，并设置默认选中 */
            selectedFragment(0);
        } else {
            /* 从 FragmentManager 中找回已恢复的 Fragment，更新成员变量 */
            overviewFragment = (OverviewFragment) getSupportFragmentManager()
                    .findFragmentByTag("dashboard");
            devicesFragment = (DevicesFragment) getSupportFragmentManager()
                    .findFragmentByTag("devices");
            accountsFragment = (AccountsFragment) getSupportFragmentManager()
                    .findFragmentByTag("accounts");
            selectedFragment(0);                                                            // 确保默认显示首页，隐藏其他
        }

        PermissionsHelper permissionsHelper = new PermissionsHelper(this);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            permissionsHelper.requestNotiPermission();
        }
        permissionsHelper.requestLocationPermission();
    }

    /**
     * 选中并切换fragment
     * @param position 第几个fragment
     */
    private void selectedFragment(int position) {
        /* fragment布局加载器 */
        FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
        hidefragment(fragmentTransaction);

        /* 第一个被选中 */
        if (position == 0) {
            if (overviewFragment == null) {
                overviewFragment = new OverviewFragment();
                fragmentTransaction.add(R.id.content, overviewFragment);
            } else {
                fragmentTransaction.show(overviewFragment);
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
        fragmentTransaction.commit();                                                               // 提交
    }

    /**
     * 点击其中一个fragment的时候需要将其他fragment隐藏
     */
    private void hidefragment(FragmentTransaction fragmentTransaction) {
        if (overviewFragment != null) {
            fragmentTransaction.hide(overviewFragment);
        }
        if (devicesFragment != null) {
            fragmentTransaction.hide(devicesFragment);
        }
        if (accountsFragment != null) {
            fragmentTransaction.hide(accountsFragment);
        }
    }
}