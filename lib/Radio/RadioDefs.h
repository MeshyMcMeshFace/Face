#ifndef RADIODEFS_H
#define RADIODEFS_H

// Actual
#define RADIO_FREQUENCY     (0x1000)

// General
#define RADIO_TX_POWER      (0x1100)
#define RADIO_SYNC_WORD     (0x1200)
#define RADIO_PREAMBLE_LEN  (0x1201)
#define RADIO_CRC           (0x1300)

// KISS-Specific
#define RADIO_TX_DELAY      (0x1500)
#define RADIO_CSMA_P        (0x1501)
#define RADIO_CSMA_SLOT     (0x1502)
#define RADIO_TX_TAIL       (0x1503)
#define RADIO_DUPLEX        (0x1504)

// LORA-SPECIFIC
#define RADIO_BANDWIDTH     (0x2000)
#define RADIO_SF            (0x2001)
#define RADIO_CR            (0x2002)



#endif