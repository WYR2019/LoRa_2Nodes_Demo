package com.rujiangiot.loraenvassistant_androidapp.Accounts;

import android.content.Intent;
import android.os.Bundle;
import android.widget.ListView;

import androidx.appcompat.app.AppCompatActivity;

import com.rujiangiot.loraenvassistant_androidapp.R;
import com.rujiangiot.loraenvassistant_androidapp.Tools.ListView.ListViewAdapter;

import java.util.ArrayList;
import java.util.Arrays;

public class AboutUsActivity extends AppCompatActivity {

    private ListView lvAboutUs;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_about_us);
        init();
        listViewManager();
    }

    public void listViewManager() {
        ArrayList<String> itemList = new ArrayList<>();
        ListViewAdapter listViewAdapter = new ListViewAdapter(this, itemList);
        lvAboutUs.setAdapter(listViewAdapter);

        String[] list = {"联系开发者", "使用教程", "软件版本"};
        itemList.addAll(Arrays.asList(list).subList(0, 3));
        /* 列表项短按：跳转页面（传递数据） */
        lvAboutUs.setOnItemClickListener((parent, view1, position, id) -> {
            String selectedItem = itemList.get(position);
            Intent intent;
            if (position == 0) {
                intent = new Intent(AboutUsActivity.this, ContactToDeveloperActivity.class);
                intent.putExtra("item_content", selectedItem);
                startActivity(intent);
            } else if (position == 1) {
                intent = new Intent(AboutUsActivity.this, UserManualActivity.class);
                intent.putExtra("item_content", selectedItem);
                startActivity(intent);
            } else {
                intent = new Intent(AboutUsActivity.this, VersionActivity.class);
                intent.putExtra("item_content", selectedItem);
                startActivity(intent);
            }
        });
    }

    public void init() {
        lvAboutUs = findViewById(R.id.lv_about_us);
    }
}