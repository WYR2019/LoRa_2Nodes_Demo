package com.rujiangiot.loraenvassistant_androidapp.Fragments;

import android.content.Intent;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.rujiangiot.loraenvassistant_androidapp.Accounts.AboutUsActivity;
import com.rujiangiot.loraenvassistant_androidapp.Login.LoginActivity;
import com.rujiangiot.loraenvassistant_androidapp.R;
import com.rujiangiot.loraenvassistant_androidapp.SQLite.User.UserSQLiteHelper;
import com.rujiangiot.loraenvassistant_androidapp.Settings.SettingsActivity;
import com.rujiangiot.loraenvassistant_androidapp.Tools.SharedPreferences.User.UserSPHelper;

public class AccountsFragment extends Fragment implements View.OnClickListener{

    private TextView tvUsername;
    private Button btnLogout;
    private RelativeLayout rlAboutUs, rlSettings;
    private UserSPHelper userSPHelper;
    private UserSQLiteHelper userSQLiteHelper;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        userSPHelper = new UserSPHelper(requireContext());
        userSQLiteHelper = new UserSQLiteHelper(getContext());
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_accounts, container, false);
        init(view);
        setTvUsername();
        return view;
    }

    private void init(@NonNull View view) {
        tvUsername = view.findViewById(R.id.tv_username);
        rlAboutUs = view.findViewById(R.id.rl_about_us);
        rlAboutUs.setOnClickListener(this);
        rlSettings = view.findViewById(R.id.rl_settings);
        rlSettings.setOnClickListener(this);
        btnLogout = view.findViewById(R.id.btn_logout);
        btnLogout.setOnClickListener(this);
    }

    @Override
    public void onClick(@NonNull View v) {
        if (v.getId() == R.id.btn_logout) {
            userSPHelper.spLogout();
            Intent intent = new Intent(getActivity(), LoginActivity.class);
            startActivity(intent);
            requireActivity().finish();
        } else if (v.getId() == R.id.rl_about_us) {
            Intent intent = new Intent(getActivity(), AboutUsActivity.class);
            startActivity(intent);
        } else if (v.getId() == R.id.rl_settings) {
            Intent intent = new Intent(getActivity(), SettingsActivity.class);
            startActivity(intent);
        }
    }

    private void setTvUsername() {
        String username = userSPHelper.spGetUsername();
        String telNumber = userSPHelper.spGetTelNumber();
        if (!username.isEmpty()) {
            tvUsername.setText(username);
        } else {
            username = userSQLiteHelper.queryUsernameByTelNumber(telNumber);
            tvUsername.setText(username);
        }
    }
}
