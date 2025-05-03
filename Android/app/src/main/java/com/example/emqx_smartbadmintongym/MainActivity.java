package com.example.emqx_smartbadmintongym;

import android.os.Bundle;
import android.view.MenuItem;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentTransaction;

import com.example.emqx_smartbadmintongym.Manager.Fragments.ManagerAccountFragment;
import com.example.emqx_smartbadmintongym.Manager.Fragments.ManagerNodesFragment;
import com.example.emqx_smartbadmintongym.Manager.Fragments.ManagerGymManFragment;
import com.google.android.material.bottomnavigation.BottomNavigationView;

public class MainActivity extends AppCompatActivity {

    private ManagerNodesFragment managerNodesFragment;
    private ManagerGymManFragment managerGymManFragment;
    private ManagerAccountFragment managerAccountFragment;
    private BottomNavigationView bottomNavigationView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        bottomNavigationView = findViewById(R.id.BottomNavigationView);
        bottomNavigationView.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                if(item.getItemId()==R.id.nodes){
                    selectedFragment(0);
                } else if (item.getItemId()==R.id.gymmanagement) {
                    selectedFragment(1);
                } else {
                    selectedFragment(2);
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
            if(managerNodesFragment == null) {
                managerNodesFragment = new ManagerNodesFragment();
                fragmentTransaction.add(R.id.content,managerNodesFragment);
            }else {
                fragmentTransaction.show(managerNodesFragment);
            }
        } else if (position == 1) {
            if(managerGymManFragment == null) {
                managerGymManFragment = new ManagerGymManFragment();
                fragmentTransaction.add(R.id.content,managerGymManFragment);
            }else {
                fragmentTransaction.show(managerGymManFragment);
            }
        } else {
            if (managerAccountFragment == null) {
                managerAccountFragment = new ManagerAccountFragment();
                fragmentTransaction.add(R.id.content, managerAccountFragment);
            } else {
                fragmentTransaction.show(managerAccountFragment);
            }
        }

        //提交
        fragmentTransaction.commit();
    }
    //点击其中一个fragment的时候需要将其他fragment隐藏K
    private void hidefragment(FragmentTransaction fragmentTransaction){
        if (managerNodesFragment!=null){
            fragmentTransaction.hide(managerNodesFragment);
        }if (managerGymManFragment!=null){
            fragmentTransaction.hide(managerGymManFragment);
        }if (managerAccountFragment!=null){
            fragmentTransaction.hide(managerAccountFragment);
        }
    }
}