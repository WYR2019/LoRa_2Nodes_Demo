package com.rujiangiot.loraenvassistant_androidapp.Tools.ListView;

import android.content.Context;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ListView extends ListViewAdapter{
    ArrayList<String> itemList = new ArrayList<>();

    /*
     * 继承父类
     */
    public ListView(Context context, List<String> dataList) {
        super(context, dataList);
    }

    public void addItems(Context context, String[] inputs) {
        ListViewAdapter listViewAdapter = new ListViewAdapter(context, itemList);
        itemList.addAll(Arrays.asList(inputs));
        listViewAdapter.notifyDataSetChanged();
        Toast.makeText(context, "已添加", Toast.LENGTH_SHORT).show();
    }

    public void deleteListItem() {

    }
}
