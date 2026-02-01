#include "esp8266.h"

volatile uint8_t ucTcpClosedFlag = 0;
char cStr [ 1500 ] = { 0 };
struct SerialFrame_t xSerialFrameRecord = { 0 };

/**
  * @brief  vEsp8266GpioConfig
  * @note   ESP8266模块连接引脚配置函数，该函数被vEsp8266Init调用。
  * @param  无
  * @retval 无
  */
void vEsp8266GpioConfig ( void )
{
    /* 定义一个GPIO_InitTypeDef类型的结构体 */
    GPIO_InitTypeDef GPIO_InitStructure;
    /* 配置 CH_PD 引脚*/
    RCC_APB2PeriphClockCmd ( macESP8266_CH_PD_CLK, ENABLE );
    GPIO_InitStructure.GPIO_Pin = macESP8266_CH_PD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init ( macESP8266_CH_PD_PORT, & GPIO_InitStructure );
    /* 配置 RST 引脚*/
    RCC_APB2PeriphClockCmd ( macESP8266_RST_CLK, ENABLE );
    GPIO_InitStructure.GPIO_Pin = macESP8266_RST_PIN;
    GPIO_Init ( macESP8266_RST_PORT, & GPIO_InitStructure );
}

/**
  * @brief  vEsp8266Rst
  * @note   调用这个函数，就可以重启ESP8266模块，该函数被vEsp8266AtTest调用。
  * @param  无
  * @retval 无
  */
void vEsp8266Rst ( void )
{
    #if 0
        bEsp8266Command ( "AT+RST", "OK", "ready", 2500 );
    #else
        macESP8266_RST_LOW_LEVEL();
        vDelayMs ( 500 ); 
    macESP8266_RST_HIGH_LEVEL();
    #endif
}

/**
 * @brief  bEsp8266Command
 * @note   对ESP8266模块发送AT指令,并等待响应。该函数被外部调用。
 * @param  pcCmd：待发送的指令
 * @param  pcReply1，pcReply2：期待的响应，为NULL表不需响应，两者为或逻辑关系。
 * @param  ulWaittime：等待响应的时间
 * @retval true：指令发送成功
 * @retval false：指令发送失败
 */
bool bEsp8266Command ( char * pcCmd, char * pcAck1, char * pcAck2, uint32_t ulWaittime )
{    
    xSerialFrameRecord.Bits_t.usFrameLength = 0;                                                    //从新开始接收新的数据包
    vUsartPrintf ( USART2, "%s\r\n", pcCmd );
    if ( ( pcAck1 == 0 ) && ( pcAck2 == 0 ) )                                                       //不需要接收数据
        return true;
    vDelayMs( ulWaittime );                                                                         //延时
    xSerialFrameRecord.cSerialReceivedBuffer [ xSerialFrameRecord.Bits_t.usFrameLength ]  = '\0';
    if ( ( pcAck1 != 0 ) && ( pcAck2 != 0 ) )
        return ( ( bool ) strstr ( xSerialFrameRecord.cSerialReceivedBuffer, pcAck1 ) || 
                            ( bool ) strstr ( xSerialFrameRecord.cSerialReceivedBuffer, pcAck2 ) );
    else if ( pcAck1 != 0 )
        return ( ( bool ) strstr ( xSerialFrameRecord.cSerialReceivedBuffer, pcAck1 ) );
    else
        return ( ( bool ) strstr ( xSerialFrameRecord.cSerialReceivedBuffer, pcAck2 ) );
}

/**
  * @brief  vEsp8266AtTest
  * @note   对ESP8266模块进行AT测试启动，该函数被外部调用。
  * @param  无
  * @retval 无
  */
void vEsp8266AtTest ( void )
{
    char count=0;
    macESP8266_RST_HIGH_LEVEL();
    vDelayMs( 1000 );
    while ( count < 3 )
    {
        if( bEsp8266Command ( "AT", "OK", NULL, 500 ) != true ) 
            return;
        vEsp8266Rst();
        ++ count;
    }
}

/** 
  * @brief  bEsp8266NetModeChoose
  * @note   选择ESP8266模块的工作模式,该函数被外部调用。
  * @param  xMode，工作模式
  * @retval 1，选择成功
  * @retval 0，选择失败
  */
bool bEsp8266NetModeChoose ( eNetMode_t xMode )
{
    switch ( xMode )
    {
        case STA:
            return bEsp8266Command ( "AT+CWMODE=1", "OK", "no change", 2500 );
        case AP:
            return bEsp8266Command ( "AT+CWMODE=2", "OK", "no change", 2500 );
        case STA_AP:
            return bEsp8266Command ( "AT+CWMODE=3", "OK", "no change", 2500 );
        default:
            return false;
    }
}

/**
  * @brief  bEsp8266JoinAp
  * @note   ESP8266模块连接外部WiFi，该函数被外部调用。
  * @param  pcSsid，WiFi名称字符串
  * @param  pcPassWord，WiFi密码字符串
  * @retval 1，连接成功
  * @retval 0，连接失败
  */
bool bEsp8266JoinAp ( char * pcSsid, char * pcPassWord )
{
    char cCmd [128];
    sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pcSsid, pcPassWord );
    if (strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "WIFI CONNECTED" ) != NULL)
    {
        /* code */
        return true;
    }
    return bEsp8266Command ( cCmd, "OK", "WIFI CONNECTED", 8000 );
}

/**
  * @brief  bEsp8266MqttInit
  * @note   ESP8266模块MQTT初始化，该函数被外部调用。
  * @param  pcMqttUserName，MQTT用户名字符串
  * @param  pcMqttPassword，MQTT密码字符串
  * @param  pcMqttClientId，MQTT客户端ID字符串
  * @param  pcMqttServerIp，MQTT服务器IP地址字符串
  * @param  usMqttServerPort，MQTT服务器端口号
  * @retval 1，初始化成功
  * @retval 0，初始化失败
  */
bool bEsp8266MqttInit ( char * pcMqttUserName, char * pcMqttPassword, char * pcMqttClientId, 
                        char * pcMqttServerIp, uint16_t usMqttServerPort, 
                        char * pcMqttSubscribeTopic )
{
    char cCmd[256] = {0};
    
    /* 1. 配置MQTT用户信息 */
    snprintf(cCmd, sizeof(cCmd), "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"", 
             pcMqttUserName, pcMqttPassword);
    if (bEsp8266Command(cCmd, "OK", NULL, 2000) != true)
        return false;
    
    /* 2. 设置客户端ID */
    memset(cCmd, 0, sizeof(cCmd));
    snprintf(cCmd, sizeof(cCmd), "AT+MQTTCLIENTID=0,\"%s\"", pcMqttClientId);
    if (bEsp8266Command(cCmd, "OK", NULL, 2000) != true)
        return false;
    
    /* 3. 连接到MQTT服务器 */
    memset(cCmd, 0, sizeof(cCmd));
    snprintf(cCmd, sizeof(cCmd), "AT+MQTTCONN=0,\"%s\",%u,1", 
             pcMqttServerIp, usMqttServerPort);
    if (bEsp8266Command(cCmd, "OK", NULL, 8000) != true && bEsp8266Command(cCmd, "+MQTTCONN", NULL, 8000) != true)
        return false;
    
    /* 4. 订阅主题 */
    memset(cCmd, 0, sizeof(cCmd));
    snprintf(cCmd, sizeof(cCmd), "AT+MQTTSUB=0,\"%s\",1", pcMqttSubscribeTopic);
    if (bEsp8266Command(cCmd, "OK", NULL, 2000) != true)
        return false;
    
    return true;
}

/**
  * @brief  bEsp8266BuildAp
  * @note   ESP8266模块创建AP热点，该函数被外部调用。
  * @param  pcSsid，AP名称字符串
  * @param  pcPassWord，AP密码字符串
  * @param  xPsdMode，AP加密方式
  * @retval 1，创建成功
  * @retval 0，创建失败
  */
bool bEsp8266BuildAp ( char * pcSsid, char * pcPassWord, eApPsdMode_t xPsdMode )
{
    char cCmd [128];
    sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%d", pcSsid, pcPassWord, xPsdMode );
    return bEsp8266Command ( cCmd, "OK", 0, 1000 );
}

/**
  * @brief  bEsp8266EnableMultipleId
  * @note   ESP8266模块使能/禁止多连接，该函数被外部调用。
  * @param  xEnumEnUnvarnishTx，ENABLE：使能多连接；DISABLE：禁止多连接
  * @retval 1，操作成功
  * @retval 0，操作失败
  */
bool bEsp8266EnableMultipleId ( FunctionalState xEnumEnUnvarnishTx )
{
    return bEsp8266Command ( "AT+CIPMUX=%d", "OK", 0, 500 );
}

/**
  * @brief  bEsp8266LinkServer
  * @note   ESP8266模块连接服务器，该函数被外部调用。
  * @param  xNetProtocol，连接协议类型
  * @param  pcIp，服务器IP地址字符串
  * @param  pcComNum，服务器端口号字符串
  * @param  xId，连接ID号
  * @retval 1，连接成功
  * @retval 0，连接失败
  */
bool bEsp8266LinkServer ( eNetPro_t xNetProtocol, char * pcIp, char * pcComNum, eIdNo_t xId)
{
    char cStr [128] = { 0 }, cCmd [128];
    switch (  xNetProtocol )
    {
        case enumTCP:
            sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", pcIp, pcComNum );
            break;
        case enumUDP:
            sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", pcIp, pcComNum );
            break;
        default:
            break;
    }
    if ( xId < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", xId, cStr);
    else
        sprintf ( cCmd, "AT+CIPSTART=%s", cStr );
    return bEsp8266Command ( cCmd, "OK", "ALREADY CONNECT", 4000 );
}

/**
  * @brief  bEsp8266StartOrShutServer
  * @note   ESP8266模块启动/关闭服务器，该函数被外部调用。
  * @param  xMode，ENABLE：启动服务器；DISABLE：关闭服务器
  * @param  pcPortNum，服务器端口号字符串
  * @param  pcTimeOver，服务器超时时间字符串
  * @retval 1，操作成功
  * @retval 0，操作失败
  */
bool bEsp8266StartOrShutServer ( FunctionalState xMode, char * pcPortNum, char * pcTimeOver )
{
    char cCmd1 [128], cCmd2 [128];
    if ( xMode )
    {
        sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pcPortNum );
        sprintf ( cCmd2, "AT+CIPSTO=%s", pcTimeOver );
        return ( bEsp8266Command ( cCmd1, "OK", 0, 500 ) &&
                            bEsp8266Command ( cCmd2, "OK", 0, 500 ) );
    }
    else
    {
        sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pcPortNum );
        return bEsp8266Command ( cCmd1, "OK", 0, 500 );
    }
}

/**
  * @brief  ucEsp8266GetLinkStatus
  * @note   获取ESP8266的连接状态，该函数被外部调用。
  * @param  无
  * @retval 0，未连接
  * @retval 2，已连接至AP但未连接服务器
  * @retval 3，已连接至AP且已连接服务器
  * @retval 4，连接断开
  */
uint8_t ucEsp8266GetLinkStatus ( void )
{
    if ( bEsp8266Command ( "AT+CIPSTATUS", "OK", 0, 500 ) )
    {
        if ( strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "STATUS:2\r\n" ) )
            return 2;
        else if ( strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "STATUS:3\r\n" ) )
            return 3;
        
        else if ( strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "STATUS:4\r\n" ) )
            return 4;
    }
    return 0;
}

/**
  * @brief  ucEsp8266GetIdLinkStatus
  * @note   获取ESP8266的各ID连接状态，该函数被外部调用。
  * @param  无
  * @retval 各ID连接状态，位0~4分别表示ID0~ID4的连接状态，1表示已连接，0表示未连接
  */
uint8_t ucEsp8266GetIdLinkStatus ( void )
{
    uint8_t ucIdLinkStatus = 0x00;
    if ( bEsp8266Command ( "AT+CIPSTATUS", "OK", 0, 500 ) )
    {
        if ( strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "+CIPSTATUS:0," ) )
            ucIdLinkStatus |= 0x01;
        else 
            ucIdLinkStatus &= ~ 0x01;
        if ( strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "+CIPSTATUS:1," ) )
            ucIdLinkStatus |= 0x02;
        else 
            ucIdLinkStatus &= ~ 0x02;
        if ( strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "+CIPSTATUS:2," ) )
            ucIdLinkStatus |= 0x04;
        else 
            ucIdLinkStatus &= ~ 0x04;
        if ( strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "+CIPSTATUS:3," ) )
            ucIdLinkStatus |= 0x08;
        else 
            ucIdLinkStatus &= ~ 0x08;
        if ( strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "+CIPSTATUS:4," ) )
            ucIdLinkStatus |= 0x10;
        else 
            ucIdLinkStatus &= ~ 0x10;
    }
    return ucIdLinkStatus;
}

/**
  * @brief  ucEsp8266InquireApIp
  * @note   获取ESP8266的AP IP，该函数被外部调用。
  * @param  pcApIp，存放 AP IP 的数组的首地址
  * @param  ucArrayLength，存放 AP IP 的数组的长度
  * @retval 0，获取失败
  * @retval 1，获取成功
  */
uint8_t ucEsp8266InquireApIp ( char * pcApIp, uint8_t ucArrayLength )
{
    char c;
    char * pCh;
    bEsp8266Command ( "AT+CIFSR", "OK", 0, 500 );
    pCh = strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "APIP,\"" );
    if ( pCh )
        pCh += 6;
    else
        return 0;
    for ( c = 0; c < ucArrayLength; c ++ )
    {
        pcApIp [ c ] = * ( pCh + c);
        if ( pcApIp [ c ] == '\"' )
        {
            pcApIp [ c ] = '\0';
            break;
        }
    }
    return 1;
}

/**
  * @brief  bEsp8266UnvarnishSend
  * @note   配置ESP8266模块进入透传模式，该函数被外部调用。
  * @param  无
  * @retval 1，配置成功
  * @retval 0，配置失败
  */
bool bEsp8266UnvarnishSend ( void )
{
    return bEsp8266Command ( "AT+CIPMODE=1", "OK", 0, 500 );
}

/**
  * @brief  vEsp8266ExitUnvarnishSend
  * @note   使ESP8266模块退出透传模式，该函数被外部调用。
  * @param  无
  * @retval 无
  */
void vEsp8266ExitUnvarnishSend ( void )
{
    vDelayMs( 1000 );
    vUsartPrintf ( USART2, "+++" );
    vDelayMs( 500 ); 
}

/**
  * @brief  bEsp8266SendString
  * @note   ESP8266模块发送字符串，该函数被外部调用。
  * @param  xEnumEnUnvarnishTx，声明是否已使能了透传模式
  * @param  pcStr，待发送的字符串首地址
  * @param  ulStrLength，待发送的字符串长度
  * @param  ucId，连接ID号
  * @retval 1，发送成功
  * @retval 0，发送失败
  */
bool bEsp8266SendString ( FunctionalState xEnumEnUnvarnishTx, char * pcStr, uint32_t ulStrLength, eIdNo_t xId )
{
    char cStr [256];
    bool bRet = false;
    if ( xEnumEnUnvarnishTx )
    {
        vUsartPrintf ( USART2, "%s", pcStr );
        bRet = true;
    }
    else
    {
        if ( xId < 5 )
            sprintf ( cStr, "AT+CIPSEND=%d,%d", xId, ulStrLength + 2 );
        else
            sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
        bEsp8266Command ( cStr, "> ", 0, 1000 );
        bRet = bEsp8266Command ( pcStr, "SEND OK", 0, 1000 );
    }
    return bRet;
}

/**
  * @brief  pcEsp8266ReceiveString
  * @note   ESP8266模块接收字符串，该函数被外部调用。
  * @param  xEnumEnUnvarnishTx，声明是否已使能了透传模式
  * @retval 接收到的字符串首地址，未接收到返回0
  */
char * pcEsp8266ReceiveString ( FunctionalState xEnumEnUnvarnishTx )
{
    char * pRecStr = 0;
    xSerialFrameRecord.Bits_t.usFrameLength = 0;
    xSerialFrameRecord.Bits_t.usFrameFinishFlag = 0;
    while ( ! xSerialFrameRecord.Bits_t.usFrameFinishFlag );
    xSerialFrameRecord.cSerialReceivedBuffer [ xSerialFrameRecord.Bits_t.usFrameLength ] = '\0';
    if ( xEnumEnUnvarnishTx )
        pRecStr = xSerialFrameRecord.cSerialReceivedBuffer;
    else 
    {
        if ( strstr ( xSerialFrameRecord.cSerialReceivedBuffer, "+IPD" ) )
            pRecStr = xSerialFrameRecord.cSerialReceivedBuffer;
    }
    return pRecStr;
}
