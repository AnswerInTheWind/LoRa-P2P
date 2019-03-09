/*
    2018 Grope

Description: LoRa PHY interface
License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Derek Chen
*/

#ifndef __LORAPHY_INTERFACE_H__
#define __LORAPHY_INTERFACE_H__


#define LORA_CODINGRATE                             1  // [1: 4/5,]
																											//  2: 4/6																										//  3: 4/7,
																											//  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8 // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5 // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        true     
    
#define FSK_PREAMBLE_LENGTH                         5         // Same for Tx and Rx

typedef enum
{
    PHY_STATUS_IDEL = 0,
    PHY_STATUS_TX,
    PHY_STATUS_RX,
    PHY_STATUS_TX_TIMEOUT,
    PHY_STATUS_RX_TIMEOUT,
    PHY_STATUS_RX_ERROR,
}LoRaPhy_status_t;

typedef enum
{
    MODULATION_LORA,
    MODULATION_FSK,
}modulation_t;

typedef enum
{
    LORA_125KHZ_BANDWIDTH = 0,
    LORA_250KHZ_BANDWIDTH,
    LORA_500KHZ_BANDWIDTH,
}LoRa_bandwidth_t;

typedef enum
{
    LoRa_SF6 = 6,
    LoRa_SF7 = 7,
    LoRa_SF8 = 8,
    LoRa_SF9 = 9,
    LoRa_SF10 = 10,
    LoRa_SF11 = 11,
    LoRa_SF12 = 12,
}LoRa_SF_t;

typedef struct LoRa_parameter
{
    LoRa_SF_t data_rate;//速率
    LoRa_bandwidth_t bandwidth;//带宽
    uint8_t SYNC_WORD;
}LoRa_parameter_t; 

typedef struct FSK_parameter
{
    uint32_t fskFdev;
	uint32_t fskDatarate;
	uint32_t fskBandwidth;
	uint32_t fskAfcBandwidth;
}FSK_parameter_t;

typedef struct rf_parameter
{
    modulation_t modulation;
    uint32_t Frequency;//频率
    uint8_t TxPower;//[0-20]
    LoRa_parameter_t LoRa_parameter;
    FSK_parameter_t FSK_parameter;
}RF_Paramater_t;

typedef struct
{
    void (*TxDone)(void);
    void (*RxDone)(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
    void (*RxTimeout)(void);
    void (*RxError)(void);
}LoRa_notification_t;


extern void interface_LoRaPhy_init(RF_Paramater_t* tx_rf_parameter,RF_Paramater_t* rx_rf_parameter);
extern void interface_LoRaPhy_register_notification(LoRa_notification_t lora_notification);
extern void interface_LoRaPhy_SendFrame( uint8_t* buffer, uint8_t len );
extern void interface_LoRaPhy_start_rx(void);
extern void interface_LoRaPhy_sleep(void);



#endif
