#include "KissRadio.h"
#include <string.h>

#define FEND  (0xC0)
#define FESC  (0xDB)
#define TFEND (0xDC)
#define TFESC (0xDD)

/* TransformToKiss()
 * Input is raw packet in this->buff
 * Output is escaped, kiss format packet suitable for host.
 */
void KissRadioClass::TransformToKiss(uint8_t *buffer, size_t *len)
{
    memset(this->buff,0,this->mtu);
    size_t p = 2;
    this->buff[0] = 0xC0;
    this->buff[1] = 0x00;
    for(size_t i = 0; i < this->buffLen;i++)
    {
        uint8_t ch = buffer[i];
        // escape things that need escaping.
        switch(ch)
        {
            case FEND: this->buff[p] = FESC; p++; this->buff[p] = TFEND; break;
            case FESC: this->buff[p] = FESC; p++; this->buff[p] = TFESC; break;
            default: this->buff[p] = ch; break;
        }
        p++;
    }
    this->buff[p] = 0xC0;
    this->buffLen = p +1;
}
/* TransformFromKiss()
 * Input is Kiss format, from host. 0xC0 $op ...data... 0xC0
 * Output is the original packet into this->buff
 * return true
 */
bool KissRadioClass::TransformFromKiss(uint8_t *buffer, size_t len)
{
    // clear out output buffer
    memset(this->buff,0,this->mtu);
    if(len < 3)
    {
        return false; // invalid length
    }
    if(buffer[0] != 0xC0 || buffer[len-1] != 0xC0)
    {
        return false; // invalid frame
    }
    switch(buffer[1])
    {
        case 0x00: // TX DATA PACKET
            {
                memset(buffer,0,chain->getMTU());
                size_t p = 0;
                for(size_t i=2;i<len-2;i++)
                {
                    uint8_t ch = buffer[i];
                    if(ch == FESC)
                    {
                        i++;
                        // error
                        if(i >= len-2)
                            return false;
                        // unescape things.
                        switch(buffer [i])
                        {
                            case TFEND: ch = FEND; break;
                            case TFESC: ch = FESC; break;
                            default:
                                return false; // error.
                        }
                        this->buff[p] = ch;
                    }
                    p++;
                }
                this->buffLen = p;
            }            
            break;
        case 0x01: // TX DELAY = N x 10ms 
            if(len == 4) {
                setValue(RADIO_TX_DELAY, buffer[2] * 10);
                return true;
            }
            return false;
        case 0x02: // PERSISTENCE = Data *256 -1 (CSMA P)
            if(len == 4) {
                setValue(RADIO_CSMA_P, buffer[2] * 10);
                return true;
            }
            return false;
        case 0x03: // SLOT TIME = N x 10ms
            if(len == 4) {
                setValue(RADIO_CSMA_SLOT, buffer[2] * 10);
                return true;
            }
            return false;
        case 0x04: // TX TAIL = N x 10ms
            if(len == 4) {
                setValue(RADIO_TX_TAIL, buffer[2] * 10);
                return true;
            }
            return false;
        case 0x05: // DUPLEX = 0 = half duplex else full duplex
            if(len == 4) {
                setValue(RADIO_DUPLEX, buffer[2] * 10);
                return true;
            }
            return false;
        case 0x06: // H/W PARAMETERS
            // TODO: fixup general solution.
            break;
        case 0xFF: // Exit KISS Mode.
            // TODO: decide how we handle 'exit kiss mode'.
            break;
    }
}