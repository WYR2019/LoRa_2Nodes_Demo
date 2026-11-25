package com.rujiangiot.loraenvassistant_androidapp.SQLite.User.Entity;


import androidx.annotation.NonNull;

public class User {
    private String telNumber;
    private String username;
    private String password;

    public User() {
    }

    public User(String telNumber, String username, String password) {
        this.telNumber = telNumber;
        this.username = username;
        this.password = password;
    }

    public String getTelNumber() {
        return telNumber;
    }


    public String getUsername() {
        return username;
    }


    public String getPassword() {
        return password;
    }


    @NonNull
    @Override
    public String toString() {
        return "User{" +
                "telNumber='" + telNumber + '\'' +
                ", username='" + username + '\'' +
                ", password='" + password + '\'' +
                '}';
    }
}
