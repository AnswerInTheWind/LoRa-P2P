/*
    2018 Grope

Description: LoRa PHY interface
License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Derek Chen
*/
#include <stdbool.h>
#include <string.h>

#include "board.h"

#include "LoRaPhy_interface.h"


static uint8_t phyMacStatus = PHY_STATUS_IDEL;
static RF_Paramater_t tx_parameters;
static RF_Paramater_t rx_parameters;
static LoRa_notification_t LoRa_notification;
static RadioEvents_t RadioEvents;

static void phy_switch_to_rx_mode(void)
{
    Radio.Standby();//切换成standBy模式
    HAL_Delay(2);//延时1ms
    
    Radio.SetChannel( rx_parameters.Frequency );
    
    if ( rx_parameters.modulation ==  MODULATION_LORA )
	{
		Radio.SetRxConfig( MODEM_LORA, rx_parameters.LoRa_parameter.bandwidth, rx_parameters.LoRa_parameter.data_rate,
							   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
							   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
							   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
        
        Radio.SetMaxPayloadLength( MODEM_LORA, 255);//1 LoRa
        
        SX1276Write( REG_LR_SYNCWORD, rx_parameters.LoRa_parameter.SYNC_WORD );//设置同步字
	}

	else if ( rx_parameters.modulation ==  MODULATION_FSK )
	{
		Radio.SetRxConfig( MODEM_FSK, rx_parameters.FSK_parameter.fskBandwidth, rx_parameters.FSK_parameter.fskDatarate,
							  0, rx_parameters.FSK_parameter.fskAfcBandwidth, FSK_PREAMBLE_LENGTH,
							  0, false, 0, true,
							  0, 0,false, true );
                              
        Radio.SetMaxPayloadLength( MODEM_FSK, 255);//0 FSK
	}
	else
	{
		
	}
    
    Radio.Rx( 0 );
	phyMacStatus = PHY_STATUS_RX;
}

static void phy_switch_to_tx_mode()
{
    Radio.Standby();//切换成standBy模式
    HAL_Delay(1);
    
    Radio.SetChannel( tx_parameters.Frequency );
    
    if ( tx_parameters.modulation ==  MODULATION_LORA )
	{

		Radio.SetTxConfig( MODEM_LORA, tx_parameters.TxPower, 0, tx_parameters.LoRa_parameter.bandwidth,
							   tx_parameters.LoRa_parameter.data_rate, LORA_CODINGRATE,
							   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
							   true, 0, 0, LORA_IQ_INVERSION_ON, 3000000 );
        
        Radio.SetMaxPayloadLength( MODEM_LORA, 255);//1 LoRa
        
        SX1276Write( REG_LR_SYNCWORD, tx_parameters.LoRa_parameter.SYNC_WORD );//设置同步字
	}

	else if ( tx_parameters.modulation ==  MODULATION_FSK )
	{
		Radio.SetTxConfig( MODEM_FSK, tx_parameters.TxPower, tx_parameters.FSK_parameter.fskBandwidth, 0,
							  tx_parameters.FSK_parameter.fskDatarate, 0,
							  FSK_PREAMBLE_LENGTH, false,
							  true, 0, 0, 0, 3000000 );
                              
        Radio.SetMaxPayloadLength( MODEM_FSK, 255);//0 FSK
	}
	else
	{
		
	}
    
    phyMacStatus = PHY_STATUS_TX;
    
}

static void phyTxDone( void )
{
    if(LoRa_notification.TxDone != NULL)
        LoRa_notification.TxDone();
}

static void phyRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{    
    int8_t snr_cal = 0;
    
    if( snr & 0x80 ) // The SNR sign bit is 1
    {
        // Invert and divide by 4
        snr_cal = ( ( ~snr + 1 ) & 0xFF ) >> 2;
        snr_cal = -snr_cal;
    }
    else
    {
        // Divide by 4
        snr_cal = ( snr & 0xFF ) >> 2;
    }
    
    if(LoRa_notification.RxDone != NULL)
        LoRa_notification.RxDone(payload,size,rssi,snr_cal);

	Radio.Rx( 0 );
	phyMacStatus = PHY_STATUS_RX;
}

static void phyTxTimeout( void )
{
}

static void phyRxTimeout( void )
{
	Radio.Sleep( );

	if(phyMacStatus == PHY_STATUS_RX)
	{
        phy_switch_to_rx_mode();
        Radio.Rx( 0 );
		phyMacStatus = PHY_STATUS_RX;
	}
}

static void phyRxError( void )
{
	Radio.Sleep( );

//    phy_switch_to_rx_mode();
	Radio.Rx( 0 );
	phyMacStatus = PHY_STATUS_RX;
}

void interface_LoRaPhy_SendFrame( uint8_t* buffer, uint8_t len )
{	
//    uint8_t rand_value = 0;
    
    Radio.Standby();
    
//    //空中信道能量检测
//    while( Radio.IsChannelFree( MODEM_LORA, SX1276.Settings.Channel, -90 ) == false )
//    {
//        rand_value = randr(2,20);
//        
//        DelayMs(rand_value);
//        // Free channel found
//    }
    
    phy_switch_to_tx_mode();
    
//	if(phyMacStatus == PHY_STATUS_RX)
//	{
//		phyMacStatus = PHY_STATUS_TX;
//		Radio.Standby();//切换成standBy模式
//        HAL_Delay(1);//延时1ms
//	}
    
//    LOG_PRINTF("fre:%d\r\n",SX1276.Settings.Channel);
	
	Radio.Send(buffer, len);
}

void interface_LoRaPhy_start_rx(void)
{
    phy_switch_to_rx_mode();
}

void interface_LoRaPhy_sleep(void)
{
    Radio.Sleep();
}

void interface_LoRaPhy_register_notification(LoRa_notification_t lora_notification)
{
    LoRa_notification = lora_notification;
}

void interface_LoRaPhy_init(RF_Paramater_t* tx_rf_parameter,RF_Paramater_t* rx_rf_parameter)
{    
	RadioEvents.TxDone = phyTxDone;
	RadioEvents.RxDone = phyRxDone;
	RadioEvents.TxTimeout = phyTxTimeout;
	RadioEvents.RxTimeout = phyRxTimeout;
	RadioEvents.RxError = phyRxError;
	Radio.Init( &RadioEvents );
    
    tx_parameters = *tx_rf_parameter;
    rx_parameters = *rx_rf_parameter;
    
    //使用随机函数，将同时上电的设备的发包时间随机错开
    srand(Radio.Random());//设置随机种子
    
    phy_switch_to_rx_mode();
}
