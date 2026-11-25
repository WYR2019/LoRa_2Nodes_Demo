# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.0.1-beta2]
2026.11.18

### Added
- 新增 Environment Node 和 Gateway Node 的温度控制模式.

### Fixed
- 修复一些 bug .

## [0.0.1-beta1]
2026.11.15

### Added
- 完善了 Environment Node 对风扇和加湿器的控制代码.

### Fixed
- 改进了 Environment Node 由于开启空闲中断却没配置导致的，接收 LoRa 网关节点下发的命令后停止发送数据的bug .
- 改进了 Environment Node 接收命令函数中 switch case 表达式错误的问题.