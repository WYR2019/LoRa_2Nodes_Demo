# LoRa_2Nodes_Demo #
## 项目概述 ##
该项目是基于STM32F103C8T6最小系统板和正点原子ATK-LORA-01模块的智能环境监测与控制系统，可实现2个LoRa节点采集温湿度等传感器数据，通过定点模式与LoRa网关进行组网通信，LoRa网关通过ESP8266-01s Wi-Fi模块将数据上传到基于MQTT协议的EMQX Broker服务器中。同时搭建一个Android App，实现远程实时查看数据、远程控制模块、安全监控、报警等功能。

## 项目要点 ##
### STM32芯片 ###
* 本项目借鉴了B站江科大的STM32项目框架，使用标准库进行裸机编程。本项目分为1个LoRa网关和2个采集数据的LoRa节点，其中LoRa网关模块开发板分别有STM32F103C8T6最小系统板、LoRa模块和ESP8266-01s Wi-Fi模块，2个采集数据的LoRa节点分别为温湿度采集与控制模块和室内安全监测模块，温湿度采集与控制模块有STM32最小系统板、LoRa模块、DHT11温湿度监测传感器、0.95英寸OLED显示屏和3个分别负责风扇、加热片和加湿器的继电器，室内安全监测模块有STM32最小系统板、LoRa模块、MQ2烟雾浓度传感器、0.95英寸OLED显示屏和蜂鸣器。该项目使用洞洞板和焊接排母的方式连接每一个系统的所有模块。本系统可实现每一个系统的所有传感器都能监测数据，并通过LoRaWAN组网协议、MQTT协议上传到EMQX服务器中实现数据的传输功能。
### LoRaWAN组网协议 ###
* 由于检测的功能较为分散,并且考虑到可能会用到占地面积较大的场地中，同时对系统延迟的要求并不高，所以采用LoRaWAN组网协议，通过不同的LoRa节点，将不同的功能分门别类的同时能覆盖较大的面积。
* 传输报文：
&nbsp; 以节点1为例：
&nbsp; 1、基础信息：
&nbsp; 模块地址（十进制）：1002
&nbsp; 通信信道（十进制）：23
&nbsp; 模块地址（十六进制）：0x03 0xEA
&nbsp; 通信信道（十六进制）：0x17
&nbsp; 2、数据传输：
&nbsp; （1）向网关发送数据：
&nbsp; 以十六进制传输消息，格式为模块地址（十六进制）+通信信道（十六进制）+节点序号，也是命令头（D1-D4）+传感器名称（DHT11为EA、烟雾浓度为EB、光照强度为EC）+数值（xx）+执行器状态（加湿器、风扇、蜂鸣器、LED、舵机和步进电机分别为FA、FB、FC、FD、FE、FF）+所有执行器的状态：01为开、00为关+命令尾BA
&nbsp; 例：节点1向网关传输消息：
&nbsp; 03 E9 17 D1 EA 20 70 FA 01 FB 01 BA
&nbsp; （2）接收数据：
&nbsp; 网关定义发送命令头0C+想要控制的执行器（如上文）+状态（打开是01，关闭是00）+命令尾AB：
&nbsp; 例：03 EA 17 0C FA 01 AB

### Wi-Fi数据上云 ###
* 
### MQTT协议、消息订阅与发布 ###
### 搭建SpringBoot后端、MySQL数据库以保存、编辑用户数据 ###

## 项目部署 ##
### SpringBoot后端： ###
* 本项目后端基于SpringBoot和MybatisPlus搭建。在IDEA中新建项目，其中：
  Project下选择“Maven”，“Spring Boot”下选择3.5.0，名字自拟，Java选择17，“Dependencies”分别选择“Lombok”、“Spring Web”和“MySQL Driver”三项，选择后点击生成即可。
* 在pom.xml中补充MybatisPlus依赖。
```
<dependency>
    <groupId>com.baomidou</groupId>
    <artifactId>mybatis-plus-spring-boot3-starter</artifactId>
    <version>3.5.12</version>
</dependency>
<dependency>
    <groupId>com.baomidou</groupId>
    <artifactId>mybatis-plus-jsqlparser</artifactId>
    <version>3.5.12</version>
</dependency>
```
* 在application.yml中补充MySQL数据库的连接信息和MybatisPlus的log包。
```
server:
  port: 8079

spring:
  application:
    name: <Your_Application>
  datasource:
    driver-class-name: com.mysql.cj.jdbc.Driver
    url: jdbc:mysql://localhost:3306/<your_database>?useSSL=false&serverTimezone=UTC&&allowPublicKeyRetrieval=true
    username: <your_dbuser>
    password: <your_dbpassword>

#log包
mybatis-plus:
  configuration:
    log-impl: org.apache.ibatis.logging.stdout.StdOutImpl
```
