#include "kiss.h"
#include <LoRa.h>

#define SET_STATUS(t) { snprintf(this->status,KISS_STATUS_LEN,t); this->status_changed = true; }
#define SET_STATUSD(t,d) { snprintf(this->status,KISS_STATUS_LEN,"%s: %d",t,d); this->status_changed = true; }

int KissClass::begin( 
                size_t mtu,
                uint8_t frameEnd,
                uint8_t frameEscape,
                uint8_t frameEndTransposed,
                uint8_t frameEscapeTransposed)
{
    this->mtu = mtu;
    this->frameEnd = frameEnd;
    this->frameEscape = frameEscape;
    this->frameEndTransposed = frameEndTransposed;
    this->frameEscapeTransposed = frameEscapeTransposed;

    this->inp = this->allocBuffer(this->mtu);
    if(!this->inp) {
        return 0;
    }

    this->out = this->allocBuffer(this->mtu);
    if(!this->out) {
        this->freeBuffer(this->inp);
        return 0;
    }

    this->tmp = this->allocBuffer(this->mtu);
    if(!this->tmp) {
        this->freeBuffer(this->inp);
        this->freeBuffer(this->out);
        return 0;
    }

    SET_STATUS("KISS START");
    
    return 1; // all okay.
}

void KissClass::freeBuffer(kiss_buff_t *buff)
{
    if(!buff)
        return;
    if(buff->data)
    {
        free(buff->data);
    }
    free(buff);
}

kiss_buff_t *KissClass::allocBuffer(int size)
{
    int len = (size * 2) + 3;
    kiss_buff_t *buff = (kiss_buff_t *)malloc(sizeof(kiss_buff_t));

    if(!buff)
        return nullptr;
    
    buff->data = (uint8_t *)malloc(size*2);

    if(!buff->data) {
        free(buff);
        return nullptr;
    }

    this->resetBuffer(buff);
    return buff;
}

void KissClass::end()
{
    if(this->inp)
        this->freeBuffer(inp);

    if(this->out)
        this->freeBuffer(out);

    if(this->tmp)
        this->freeBuffer(tmp);
    
    this->inp = nullptr;
    this->out = nullptr;
    this->tmp = nullptr;

    this->mtu = 0;

    SET_STATUS("KISS ENDED");
}

int KissClass::resetBuffer(kiss_buff_t *buff)
{
    memset(buff->data,0,mtu*2);
    buff->len = 0;
    buff->ready = false;
    return 0;
}

size_t KissClass::getMtu()
{
    return this->mtu;
}

int KissClass::transformToKiss()
{
    this->inp->data[0] = frameEnd;
    this->inp->data[1] = 0;
    int p=2;
    for(int i=0;i<  this->tmp->len; i++)
    {
        uint8_t ch = this->tmp->data[i];
        if(ch == frameEnd)
        {
            this->inp->data[p] = this->frameEscape;
            p++;
            this->inp->data[p] = this->frameEndTransposed;
        } else if(ch == frameEscape) {
            this->inp->data[p] = this->frameEscape;
            p++;
            this->inp->data[p] = this->frameEscapeTransposed;
        } else {
            this->inp->data[p] = ch;
        }
        p++;
    }
    this->inp->data[p] = frameEnd;
    this->inp->len = p;
    this->inp->ready = true;
    return p;
}

bool KissClass::isOkay()
{
    return (this->tmp != nullptr);
}

int KissClass::available()
{
    // have we got a buffer?
    if(this->inp->ready)
    {
        return this->inp->len;
    }
    // check that the radio has a packet or not
    int len = LoRa.available();
    if(!len)
        return 0;
    // we don't have anything in the buffer, but there's something in the radio
    // let's retrieve it.
    // if too long, eat it.
    if( len >= mtu )
    {
        for(int i=0;i < len; i++)
            radio->read();
        return 0;
    }
    // copy radio buffer so we can transform it
    tmp->len = LoRa.readBytes(tmp->data,len);
    SET_STATUSD("RX",len);
    return this->transformToKiss();
}
int KissClass::read()
{
    int r = this->peek();
    this->inp->p++;
    if(r == -1)
        this->resetBuffer(this->inp);
    return r;
}
int KissClass::peek()
{
    if(this->inp->ready && this->inp->p < this->inp->len)
        return this->inp->data[this->inp->p];
    this->inp->ready = false;
    return -1;
}

// From Print
size_t KissClass::write(uint8_t ch)
{
    // check first char is a frameEnd char.
    if(this->out->len == 0)
    {
        if(ch != this->frameEnd)
            return 0; // not written
        this->out->data[this->out->len] = ch;
        this->out->len ++;
        return 1;
    }
    if(this->out->len == 1)
    {
        if((ch >=0x00 && ch <= 0x06) || ch == 0xFF)
        {
            this->out->data[this->out->len] = ch;
            this->out->len ++;
            return 1;
        }
        this->resetBuffer(this->out);
        return 0;
    }
    // inp->len > 1
    if(this->out->len == 2 && ch == this->frameEnd)
    {
        // empty packet is an error.
        this->resetBuffer(this->out);
        return 0;
    }
    if(ch == this->frameEnd)
    {
        processPacket();
    }
    if(ch == this->frameEscape)
    {
        this->out->ready = true;
        return 1;
    }
    if(this->out->ready == true)
    {
        this->out->ready = false;
        if(ch == this->frameEndTransposed)
        {    
            ch = this->frameEnd;
        }
        else if(ch == this->frameEscapeTransposed)
        {
            ch = this->frameEscape;
        } else {
            // escape without valid char is an error.
            this->resetBuffer(this->out);
            return 0;
        }
    }
    this->out->data[this->out->len] = ch;
    this->out->len ++;
    return 1;
}
size_t KissClass::write(const uint8_t *buffer, size_t size)
{
    size_t a;
    for(int i=0;i< size;i++)
    {
        a += this->write(buffer[i]);
    }
    return a;
}

void KissClass::processPacket()
{
    // at this point the out buffer containes
    // 0xC0 CMD DATA
    // bail out of KISS mode if in error.
    if(this->out->len <2)
    {
        this->end();
        SET_STATUS("KISS FATAL 01");
        return;
    }
    switch(this->out->data[1])
    {
        case 0x00: // Data packet
            SET_STATUSD("KISS TX",this->out->len-2);
            LoRa.beginPacket();
            LoRa.write(this->out->data+2, this->out->len);
            LoRa.endPacket();
            resetBuffer(this->out);
            break;
        case 0x01: // TX Delay - number of ms before transmitting data
            // TODO: actually set number of preamble chars
            SET_STATUSD("TXDELAY P",10*(int)this->out->data[2]);
            break;
        case 0x02: // P - The persistence parameter. Persistence=Data*256-1. Used for CSMA.
            // TODO: read up on this.
            SET_STATUSD("CSMA P",10*(int)this->out->data[2]);

            break;
        case 0x03: // Slot time in 10 ms units. Used for CSMA.
            // TODO: read up on this.
            SET_STATUSD("SLOT",10*(int)this->out->data[2]);
            break;
        case 0x04: // The length of time to keep the transmitter keyed after sending the data (in 10 ms units).
            // TODO: can we set post-data chars?
            SET_STATUSD("TXTAIL",10*(int)this->out->data[2]);
            break;
        case 0x05: // 0 = half duplex; 1 = full duplex
            // TODO: this doesn't make sense in a LoRa system
            if(this->out->data[2]) {
                SET_STATUS("FULL DUPLEX");
            }
            else {
                SET_STATUS("HALF DUPLEX");
            }
            break;
        case 0x06: // set h/w parameters
            // TODO: what are common settings here?
            // I want to be able to set SF/CF4/BW/TXPower etc.
            SET_STATUS("HW PARAM?");
            break;
        case 0xFF: // Exit KISS mode.
            SET_STATUS("KISS EXIT");
            this->end();
            break;

    }
}

char *KissClass::getStatus()
{
    this->status_changed = false;
    return status;
}

bool KissClass::hasStatusChanged()
{
    return this->status_changed;
}

KissClass Kiss;