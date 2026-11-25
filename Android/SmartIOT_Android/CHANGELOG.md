# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- 细化设备自动显示功能。
- 增加数据备份功能。
- 增加查看设备历史数据功能。
- 增加消息告知功能。

### Fixed
- 优化夜间模式bug。
- 优化切换界面时重合的bug。

## [1.0.0 beta1] - 2026-09-16

### Added
- 添加自动登录功能，勾选自动登录可实现，不勾选则只会自动补全用户名/手机号码。
- 创建了设备数据库，添加了设备自动显示的功能，通过判断时间戳查看是否在线功能。
- 添加了其他小功能。

### Changed
- 完善了App的操作逻辑，仿照汽车App远程控制的功能，便于管理多个IOT设备。
- 完善了SharedPreferences的信息自动补全功能，去除了补全密码，确保更安全（用于以后拓展后端）。
- 优化了安卓App注册登录模块的问题反馈：例如添加了输入框非空判断、密码判断等提示。
- 修改部分界面及功能。

### Fixed
- 更改部分代码冗余，增强代码规范性。
- 优化json解析的bug。
