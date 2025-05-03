package com.example.emqx_smartbadmintongym.Customer;

import android.os.Bundle;
import android.view.MenuItem;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.fragment.app.FragmentTransaction;

import com.example.emqx_smartbadmintongym.Customer.Fragment.CustomerAccountFragment;
import com.example.emqx_smartbadmintongym.Customer.Fragment.CustomerOrderFragment;
import com.example.emqx_smartbadmintongym.R;
import com.google.android.material.bottomnavigation.BottomNavigationView;

public class CustomerMainActivity extends AppCompatActivity {
    private CustomerOrderFragment customerOrderFragment;
    private CustomerAccountFragment customerAccountFragment;
    private BottomNavigationView bottomNavigationView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_customer_main);
        bottomNavigationView = findViewById(R.id.BottomNavigationView);
        bottomNavigationView.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                if(item.getItemId()==R.id.ordering){
                    selectedFragment(0);
                } else {
                    selectedFragment(1);
                }
                return true;
            }
        });
        //默认首页选中
        selectedFragment(0);
    }
    private void selectedFragment(int position){
        //fragment布局加载器
        FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
        hidefragment(fragmentTransaction);

        //第一个被选中
        if(position == 0){
            if(customerOrderFragment == null) {
                customerOrderFragment = new CustomerOrderFragment();
                fragmentTransaction.add(R.id.content,customerOrderFragment);
            }else {
                fragmentTransaction.show(customerOrderFragment);
            }
        } else {
            if (customerAccountFragment == null) {
                customerAccountFragment = new CustomerAccountFragment();
                fragmentTransaction.add(R.id.content, customerAccountFragment);
            } else {
                fragmentTransaction.show(customerAccountFragment);
            }
        }

        //提交
        fragmentTransaction.commit();
    }
    //点击其中一个fragment的时候需要将其他fragment隐藏K
    private void hidefragment(FragmentTransaction fragmentTransaction){
        if (customerOrderFragment!=null){
            fragmentTransaction.hide(customerOrderFragment);
        }if (customerAccountFragment!=null){
            fragmentTransaction.hide(customerAccountFragment);
        }
    }
}