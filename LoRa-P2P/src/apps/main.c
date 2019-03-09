/*
    2018 Grope

Description: app Layer code & main 
License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Derek Chen
*/
#include <string.h>

#include "board.h"

#include "LoRaPhy_interface.h"


typedef struct 
{
    uint8_t tx_buf[256];
    uint8_t tx_len;
    bool tx_done;//false:no_tx_done true:tx_done
}tx_info_t;

typedef struct 
{
    uint8_t rx_buf[256];
    uint8_t rx_len;
    bool rx_done;//false:no_rx_done true:rx_done
    int16_t rssi;
    int8_t snr;
}rx_info_t;

typedef struct 
{
    tx_info_t tx_info;
    rx_info_t rx_info;
}rf_info_t;

typedef enum 
{
    DEVICE_SEND,
    DEVICE_RX,
    DEVICE_CYCLE,
    DEVICE_IDLE,
}DeviceState_t;

static LoRa_notification_t LoRa_callback;

static rf_info_t rf_info;

static DeviceState_t device_state;


void uart_rx_complete()
{
    MSG_DBG("uart recv len:%d",uart1.rx_len);
}


void LoRa_TxDone(void)
{
    static uint32_t tx_count = 0;
    
    tx_count++;
    
    MSG_DBG("TXDONE");

    rf_info.tx_info.tx_done = true;

#ifdef USE_OLED
    uint8_t buf[64] = {0};
    sprintf((char*)buf,"tx_count:%ld",tx_count);
    OLED_ShowString_Line(1,buf);    
#endif      
    
    device_state = DEVICE_RX;
}

void LoRa_RxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    static uint32_t rx_count = 0;
    
    rx_count++;
    
    MSG_DBG("RXDONE");    

    //接收到的数据和信号强度保存在rf_info.rx_info中
    memcpy(rf_info.rx_info.rx_buf,payload,size);
    rf_info.rx_info.rx_len = size;
    rf_info.rx_info.rssi = rssi;
    rf_info.rx_info.snr = snr;

    rf_info.rx_info.rx_done = true;

#ifdef USE_OLED
    uint8_t buf[64] = {0};
    sprintf((char*)buf,"rx_count:%ld",rx_count);
    OLED_ShowString_Line(2,buf);    
#endif        
    
}

/**
 * Main application entry point.
 */
int main( void )
{
    HAL_Init();//硬件初始化  
    
    OLED_Show_Logo();
    OLED_Clean_All_Slowly();
    
    uart1.rx_complete_callback = uart_rx_complete;    
    
    uint32_t last_send_tick = HAL_GetTick();    
    
/****************************节点射频参数配置*****************************************/
    //发送接收参数可以配置成不一样，这样不会互相干扰
    RF_Paramater_t tx_rf_parameter;
    RF_Paramater_t rx_rf_parameter;
    
    //发送参数
    tx_rf_parameter.modulation = MODULATION_LORA;
    tx_rf_parameter.Frequency = 465000000;
    tx_rf_parameter.TxPower = 20;
    tx_rf_parameter.LoRa_parameter.bandwidth = LORA_125KHZ_BANDWIDTH;
    tx_rf_parameter.LoRa_parameter.data_rate = LoRa_SF8;
    tx_rf_parameter.LoRa_parameter.SYNC_WORD = 0x25;

    //接收参数
    rx_rf_parameter.modulation = MODULATION_LORA;
    rx_rf_parameter.Frequency = 465000000;
    rx_rf_parameter.TxPower = 20;
    rx_rf_parameter.LoRa_parameter.bandwidth = LORA_125KHZ_BANDWIDTH;
    rx_rf_parameter.LoRa_parameter.data_rate = LoRa_SF8;
    rx_rf_parameter.LoRa_parameter.SYNC_WORD = 0x25;

    interface_LoRaPhy_init(&tx_rf_parameter, &rx_rf_parameter);
    /****************************节点射频参数配置 END*****************************************/
    
    LoRa_callback.TxDone = LoRa_TxDone;
    LoRa_callback.RxDone = LoRa_RxDone;
    LoRa_callback.RxError = NULL;
    LoRa_callback.RxTimeout = NULL;
    interface_LoRaPhy_register_notification(LoRa_callback);
    
    device_state = DEVICE_SEND;    
    
    while(1)
    {       
        switch(device_state)
        {
            case DEVICE_SEND:
                MSG_DBG("Start Send");

            #ifdef USE_OLED
                OLED_ShowString_Line(0,"TX RUNNING...");    
            #endif
                
                //LoRa PHY 层发送完成之后会产生TXDONE中断
                interface_LoRaPhy_SendFrame("12345",sizeof("12345"));
                
                last_send_tick = HAL_GetTick();
            
                device_state = DEVICE_IDLE;
                break;
            
            case DEVICE_RX:
            
            #ifdef USE_OLED
                OLED_ShowString_Line(0,"RX RUNNING...");    
            #endif              
            
                 //必须要在发送完成之后再打开接收模式，否则会造成发送失败
                interface_LoRaPhy_start_rx();               
            
                device_state = DEVICE_CYCLE;
                break;
            
            case DEVICE_CYCLE:
                //每三秒发一个包
                if(HAL_GetTick() - last_send_tick > 5000)
                {
                    device_state = DEVICE_SEND;
                }                
            
                break;
            
            case DEVICE_IDLE:
            default:
            
                break;
        }
        
    }
}

