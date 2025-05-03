package com.example.emqx_smartbadmintongym.Customer.Fragment;

import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.example.emqx_smartbadmintongym.Customer.Accounts.CustomerSettingsActivity;
import com.example.emqx_smartbadmintongym.Customer.Login.CustomerLoginActivity;
import com.example.emqx_smartbadmintongym.R;

public class CustomerAccountFragment extends Fragment implements View.OnClickListener{
    private Button btnCustomerQuit;
    private RelativeLayout rlCustomerSettings;
    private TextView tvUsername;
    private SQLiteDatabase sqLiteDatabase;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_customer_account, container, false);
        Init(view);
        return view;
    }

    /**
     * Called when a view has been clicked.
     *
     * @param v The view that was clicked.
     */
    @Override
    public void onClick(View v) {
//        sqLiteDatabase = new DBOpenHelper(getContext());
//        tvUsername.setText(dbOpenHelper.mDataBaseLogin());
        if (v.getId()==R.id.btn_customer_quit){
            Intent Intent_quit = new Intent(getActivity(), CustomerLoginActivity.class);
            startActivity(Intent_quit);
            onDestroy();
        }else if (v.getId()==R.id.rl_customer_settings){
            Intent Intent_setting = new Intent(getActivity(), CustomerSettingsActivity.class);
            startActivity(Intent_setting);
        }
    }

    public void Init(View view){
        btnCustomerQuit = view.findViewById(R.id.btn_customer_quit);
        rlCustomerSettings = view.findViewById(R.id.rl_customer_settings);
        tvUsername = view.findViewById(R.id.tv_uname);
        btnCustomerQuit.setOnClickListener(this);
        rlCustomerSettings.setOnClickListener(this);
    }
}