#ifndef WISAFE2_PROTOCOL_H
#define WISAFE2_PROTOCOL_H

#include <stdint.h>

// Packet structure for silence button messages
typedef struct {
    uint8_t cmd;
    uint8_t device_id;
    uint8_t device_type;
    uint8_t _unknown;
    uint8_t stop;
} pkt_tx_silence_button_t;

// Packet structure for emergency button messages
typedef struct {
    uint8_t cmd;
    uint8_t device_id;
    uint8_t device_type;
    uint8_t _unknown;
    uint8_t stop;
} pkt_tx_emergency_button_t;

// Macros (adjust these values as needed for your protocol)
#define SPI_TX_SILENCE_BUTTON    0x01  
#define SPI_TX_EMERGENCY_BUTTON  0x02  
#define SPI_STOP_WORD            0xFF  

// Remove the DEVICE_ID definition here to avoid conflict
// #ifndef DEVICE_ID
// #define DEVICE_ID 1  
// #endif

#endif // WISAFE2_PROTOCOL_H
