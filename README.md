# LoRa-P2P
LoRa-P2P CODE

LoRa 点对点 通讯的程序，程序基于 GROPE LoRa Dev.kit-v1.1 & Dev.kit-E-v1.1 开发板 

代码的文本编码格式为UTF8 Without signature,如果注释出现乱码，请自行修改IDE编码

IDE使用MDK,我的版本为5.23。

GROPE LoRa Dev系列由两块板构成：
	- Dev.kit-v1.1为射频地板，含有MCU(STM32L051C8T6)/RF MODULE/SWD下载口，并吧剩余未用引脚都引出来了
	- Dev.kit-E-v1.1为开发板的外设版，含有LED灯x2/KEYx1/OLEDx1/USB-TTL(CH340E)

LoRa 本身为半双工通信，若丢包需检查设备本身的射频参数是否配置正确，并且发送频率是否太高。

## 工作逻辑 ##

1. 当开发板烧录程序上电之后，每10s会发送一个数据包
2. 发送完成之后会进入持续接收模式，此时才能够有接收数据
3. 按键、向MCU发送数据都可以触发发包，其中向MCU发送数据会修改发送缓存
4. 本例程发送完成之后并没有清掉发送缓存的数据，所以串口修改缓存之后，会一直发送之前串口下发的数据
5. LoRa 单包最大能发送255字节




