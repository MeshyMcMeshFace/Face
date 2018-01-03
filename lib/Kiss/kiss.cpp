#include <kiss.h>


int KissClass::begin( 
                size_t mtu,
                uint8_t dataTag,
                uint8_t frameEnd,
                uint8_t frameEscape,
                uint8_t frameBeginTransposed,
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

    snprintf(this->status,"KISS START")
    return 1; // all okay.
}

void KissClass::freeBuffer(kiss_buff_t *buff)
{
    if(!buff)
        return;
    if(buff->data)
    {
        free(buff->data)
    }
    free(buff);
}

kiss_buff_t *KissClass::allocBuffer(int size)
{
    int len = (size * 2) + 3;
    kiss_buff_t *buff = malloc(sizeof(kiss_buff_t));

    if(!buff)
        return nullptr;
    
    buff->data = malloc(size*2);

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

    snprintf(this->status,"KISS ENDED")
}

int KissClass::resetBuffer(kiss_buff_t *buff)
{
    memset(buff->data,0,mtu*2);
    buff->len = 0;
    buff->has = false;
    return 0;
}

size_t KissClass::getMtu()
{
    return this->mtu;
}

int KissClass::transformToKiss()
{
    this->inp->data[0] = frameEnd;
    this->inp->data[1] = dataTag;
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
    if(!radio->available)
    {
        return 0;
    }
    // we don't have anything in the buffer, but there's something in the radio
    // let's retrieve it.
    int len = radio->available();
    // if too long, eat it.
    if( len >= mtu )
    {
        for(int i=0;i < len; i++)
            radio->read();
        return 0;
    }
    // copy radio buffer so we can transform it
    tmp->len = radio->readBuffer(tmp->data,len);
    snprintf(this->status,32,"RX %d Bytes",len);
    return transformToKiss();
}
int KissClass::read()
{
    int r = this->peek();
    this->inp->p++;
    if(r == -1)
        resetBuffer(this->inp);
    return r;
}
int KissClass::peek()
{
    if(this->inp->ready && this->inp->p < this->inp->len)
        return this->inp->data[this->inp->p];
    this->read = false;
    return -1;
}
bool KissClass::flush()
{
    return radio->flush();
}

    // From Print
size_t KissClass::write(uint8_t ch)
{
    // check first char is a frameEnd char.
    if(this->out->len == 0)
    {
        if(ch != this->frameEnd)
            return 0; // not written
        this->out->data[p] = ch;
        this->out->len ++;
        return 1;
    }
    if(this->out->len == 1)
    {
        if((ch >=0x00 && ch <= 0x06) || ch == 0xFF)
        {
            this->out->data[p] = ch;
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
        this->out-ready = false;
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

void KissClass::processPacket()
{
    // at this point the out buffer containes
    // 0xC0 CMD DATA
    // bail out of KISS mode if in error.
    if(this->out->len <2)
    {
        this->end();
        snprintf(this->status,32,"KISS FATAL 01");
        return;
    }
    switch(this->out->data[1])
    {
        case 0x00: // Data packet
            snprintf(this->status,32,"KISS TX %d",this->out->len-2);
            LoRa.beginPacket();
            LoRa.writeBuffer(this->out->data+2, this->out->len);
            Lora.endPacket();
            resetBuffer(this->out);
            break;
        case 0x01: // TX Delay - number of ms before transmitting data
            // TODO: actually set number of preamble chars
            snprintf(this->status,32,"TXDELAY P %d",10*(int)this->out->data[2]);
            break;
        case 0x02: // P - The persistence parameter. Persistence=Data*256-1. Used for CSMA.
            // TODO: read up on this.
            snprintf(this->status,32,"CSMA P %d",10*(int)this->out->data[2]);

            break;
        case 0x03: // Slot time in 10 ms units. Used for CSMA.
            // TODO: read up on this.
            snprintf(this->status,32,"SLOT %d",10*(int)this->out->data[2]);
            break;
        case 0x04: // The length of time to keep the transmitter keyed after sending the data (in 10 ms units).
            // TODO: can we set post-data chars?
            snprintf(this->status,32,"TXTAIL %d",10*(int)this->out->data[2]);
            break;
        case 0x05: // 0 = half duplex; 1 = full duplex
            // TODO: this doesn't make sense in a LoRa system
            if(this->out->data[2])
                snprint(this->status,32,"FULL DUPLEX");
            else
                snprintf(this->status,32,"HALF DUPLEX");
            break;
        case 0x06: // set h/w parameters
            // TODO: what are common settings here?
            // I want to be able to set SF/CF4/BW/TXPower etc.
            snprintf(this->status,32,"HW PARAM?");
            break;
        case 0xFF: // Exit KISS mode.
            snprintf(this->status,32,"KISS EXIT");
            this->end();
            break;

    }
}

char *KissClass::getStatus()
{
    return status;
}
