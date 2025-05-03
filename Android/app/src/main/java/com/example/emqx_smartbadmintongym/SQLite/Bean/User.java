package com.example.emqx_smartbadmintongym.SQLite.Bean;


public class User {
    private String tNumber;
    private String userName;
    private String passWord;

    public User() {
    }

    public User(String tnumber, String username, String password) {
        this.tNumber = tnumber;
        this.userName = username;
        this.passWord = password;
    }

    public String getTnumber() {
        return tNumber;
    }

    public void setTnumber(String tnumber) {
        this.tNumber = tnumber;
    }

    public String getUsername() {
        return userName;
    }

    public void setUsername(String username) {
        this.userName = username;
    }

    public String getPassword() {
        return passWord;
    }

    public void setPassword(String password) {
        this.passWord = password;
    }

    @Override
    public String toString() {
        return "User{" +
                "tnumber='" + tNumber + '\'' +
                ", username='" + userName + '\'' +
                ", password='" + passWord + '\'' +
                '}';
    }
}
