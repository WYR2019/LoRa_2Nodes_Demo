package com.rujiangiot.loraenvassistant_androidapp.Fragments;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Toast;

import com.rujiangiot.loraenvassistant_androidapp.MQTT.Interface.OnSubscribedListener;
import com.rujiangiot.loraenvassistant_androidapp.R;
import com.rujiangiot.loraenvassistant_androidapp.SQLite.Device.DeviceSQLiteHelper;
import com.rujiangiot.loraenvassistant_androidapp.Tools.Dialog.DialogListenerImpl;
import com.rujiangiot.loraenvassistant_androidapp.Tools.ListView.ListViewAdapter;

import java.util.ArrayList;
import java.util.List;

public class DevicesFragment extends Fragment implements OnSubscribedListener {
    private ImageView imgAddItems;
    private ListView listView;
    private DeviceSQLiteHelper deviceSQLiteHelper;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_devices, container, false);
        init(view);

        deviceSQLiteHelper = new DeviceSQLiteHelper(getContext());
        listViewManager();

        return view;
    }

    public void listViewManager() {
        ArrayList<String> itemList = new ArrayList<>();
        ListViewAdapter listViewAdapter = new ListViewAdapter(getActivity(), itemList);
        listView.setAdapter(listViewAdapter);

        /* 手动添加 */
        imgAddItems.setOnClickListener(v -> {
            String[] hints = {"请输入设备ID（deviceId）", "请输入网络模式（deviceMode）"};
            DialogListenerImpl.showMultiInputsDialog(requireActivity(), "查找设备", hints, inputs -> {
                /* 显示网关名称 */
                itemList.add(inputs[0]);
                listViewAdapter.notifyDataSetChanged();
                Toast.makeText(getActivity(), "已添加", Toast.LENGTH_SHORT).show();
            });
        });

        /* 自动添加 */
        List<String> deviceIds = queryDeviceIds();
        if (deviceIds != null) {
            for (int i = 0; i < deviceIds.size(); i++) {
                String id = deviceIds.get(i);
                // 在这里使用 id，比如上传、显示、保存
                itemList.add(id);
                if (onlineTest(id)) {
                    Log.v("Online Test", "'" + id + "'" + " is online.");
                } else {
                    Log.v("Online Test", "'" + id + "'" + " is offline.");
                }
            }
        }

        /* 列表项长按：删除项目（带确认对话框） */
        listView.setOnItemLongClickListener((parent, view2, position, id) -> {
            new AlertDialog.Builder(requireActivity())
                    .setTitle("删除确认")
                    .setMessage("确定要删除「" + itemList.get(position) + "」吗？")
                    .setPositiveButton("删除", (dialog, which) -> {
                        itemList.remove(position);
                        listViewAdapter.notifyDataSetChanged();
                        Toast.makeText(getActivity(), "已删除", Toast.LENGTH_SHORT).show();
                    })
                    .setNegativeButton("取消", null)
                    .show();
            return true;                                                                                // 消费长按事件，不再触发短按
        });
    }

    private void init(@NonNull View view) {
        listView = view.findViewById(R.id.listview);
        imgAddItems = view.findViewById(R.id.img_add_items);
    }

    public List<String> queryDeviceIds() {
        List<String> ids = deviceSQLiteHelper.queryAllDeviceIds();
        Log.v("ids", String.valueOf(ids));
//        ids.forEach(id -> Log.v("ids", "id = " + id));
        return ids;
    }

    public boolean onlineTest(String deviceId) {
        if (deviceSQLiteHelper == null || deviceId == null) return false;

        long getSavedTimestamp = deviceSQLiteHelper.queryDeviceTimestamp(deviceId);
        long timestamp = System.currentTimeMillis();
        long seconds = timestamp / 1000;
        return seconds - getSavedTimestamp <= 10;
    }

    @Override
    public void onDeviceConnected(String deviceId, String deviceMode, long timestamp) {

    }

    @Override
    public void onNodesConnected(boolean isNode1Connected) {

    }

    @Override
    public void onTemperatureReceived(String temperature) {

    }

    @Override
    public void onHumidityReceived(String humidity) {

    }
}