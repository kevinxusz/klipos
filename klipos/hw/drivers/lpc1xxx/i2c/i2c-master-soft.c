/*
 The MIT License (MIT)
 
 Copyright (c) 2013 Baptiste Burles, Christophe Casson, Sylvain Fay-Chatelard
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "../../../include/libs-klipos.h"

//-------------------------- configuration macro

#ifdef MCU_IS_LPC1311
#       define I2C_DIR         I2CSOFT_PORT->DIR     
#       define I2C_SET(pin)    SETBIT(I2CSOFT_PORT->DATA,pin);
#       define I2C_CLR(pin)    CLRBIT(I2CSOFT_PORT->DATA,pin);
#       define I2C_GET         I2CSOFT_PORT->DATA
#endif

#ifdef MCU_IS_LPC1315
#       define I2C_DIR         I2CSOFT_PORT->DIR[0]     
#       define I2C_SET(pin)    SETBIT(I2CSOFT_PORT->SET[0],pin);
#       define I2C_CLR(pin)    SETBIT(I2CSOFT_PORT->CLR[0],pin);
#       define I2C_GET         I2CSOFT_PORT->PIN[0]
#endif

#ifdef MCU_IS_LPC17XX
#       define I2C_DIR         I2CSOFT_PORT->FIODIR
#       define I2C_SET(pin)    SETBIT(I2CSOFT_PORT->FIOSET,pin);
#       define I2C_CLR(pin)    SETBIT(I2CSOFT_PORT->FIOCLR,pin);
#       define I2C_GET         I2CSOFT_PORT->FIOPIN
#endif


#define SCL_INPUT()     CLRBIT(I2C_DIR,SCL_PIN)
#define SCL_OUTPUT()    SETBIT(I2C_DIR,SCL_PIN)

#define SCL0()          SCL_OUTPUT(); I2C_CLR(SCL_PIN)
#define SCL1()          SCL_INPUT()

#define SDA_INPUT()     CLRBIT(I2C_DIR,SDA_PIN)
#define SDA_OUTPUT()    SETBIT(I2C_DIR,SDA_PIN)

#define SDA(bit)        if(bit==0){ SDA_OUTPUT(); I2C_CLR(SDA_PIN); }\
                        else { SDA_INPUT(); }

#define GET_SDA()       ((I2C_GET >> SDA_PIN) & 0x1)
#define GET_SCL()       ((I2C_GET >> SCL_PIN) & 0x1)


//-------------------------- private objects

static const I2cMaster i2cSoftMaster = 
{
    sendBufferToI2cSoft,
    getBufferFromI2cSoft
};

static LPC_GPIO_TypeDef *I2CSOFT_PORT;
static int SCL_PIN;
static int SDA_PIN;


//-------------------------- private functions

static void i2c_soft_start(void)
{
    SDA(0);
    SCL0();
    waitSomeTimeInUs(1);
}

static void i2c_soft_send_bit(uint8_t bit)
{
    // set bit on SDA
    // NOP
    // SCL = 1
    // wait SCL low
    // then SCL = 0

    SDA((bit&0x1));
    waitSomeTimeInUs(1);
    SCL1();
    waitSomeTimeInUs(2);
    
    SCL0();
    waitSomeTimeInUs(1);
}

static uint8_t i2c_soft_receive_bit(void)
{
    // SDA = 1
    // SCL = 1
    // wait SCL low
    // bit = SDA
    // SCL = 0
    
    uint8_t bit;
    
    SDA(1);
    waitSomeTimeInUs(1);
    
    SCL1();
    waitSomeTimeInUs(1);
    while( GET_SCL() == 0);
    
    bit = GET_SDA();
    
    SCL0();
    SDA_OUTPUT();
    waitSomeTimeInUs(1);
    
    return bit;
}

static void i2c_soft_stop(void)
{
    SCL1();
    waitSomeTimeInUs(2);
    SDA(1);
    waitSomeTimeInUs(1);
}

static uint8_t i2c_soft_send_byte(uint8_t byte)
{    
    // send byte :
    i2c_soft_send_bit(byte>>7);
    i2c_soft_send_bit(byte>>6);
    i2c_soft_send_bit(byte>>5);
    i2c_soft_send_bit(byte>>4);
    i2c_soft_send_bit(byte>>3);
    i2c_soft_send_bit(byte>>2);
    i2c_soft_send_bit(byte>>1);
    i2c_soft_send_bit(byte>>0);
    
    // receive ack:
    return i2c_soft_receive_bit();
}

static uint8_t i2c_soft_receive_byte(uint8_t ack)
{
    uint8_t bit;
    uint8_t byte;
    
    bit = i2c_soft_receive_bit();
    byte = bit << 7;
    
    bit = i2c_soft_receive_bit();
    byte |= bit << 6;
    
    bit = i2c_soft_receive_bit();
    byte |= bit << 5;
    
    bit = i2c_soft_receive_bit();
    byte |= bit << 4;
    
    bit = i2c_soft_receive_bit();
    byte |= bit << 3;
    
    bit = i2c_soft_receive_bit();
    byte |= bit << 2;
    
    bit = i2c_soft_receive_bit();
    byte |= bit << 1;
    
    bit = i2c_soft_receive_bit();
    byte |= bit;
    
    
    // send ack
    i2c_soft_send_bit(ack);
    
    return byte;
}

//-------------------------- public functions

const I2cMaster * initI2cSoft(LPC_GPIO_TypeDef *port, int sda, int scl)
{
    I2CSOFT_PORT = port;
    SDA_PIN = sda;
    SCL_PIN = scl;
        
    return &i2cSoftMaster;
}

uint32_t sendBufferToI2cSoft(uint8_t addr, uint8_t *buffer, uint32_t len)
{
    uint32_t i;
    
    i2c_soft_start();
    
    i2c_soft_send_byte(addr);    
    
    for(i=0;i<len;i++)
    {
        i2c_soft_send_byte(buffer[i]);
    }
 
    i2c_soft_stop();
    
    return 0;
}

uint32_t getBufferFromI2cSoft(uint8_t addr, uint8_t *buffer, uint32_t len)
{
    uint32_t i;
    
    i2c_soft_start();
    
    i2c_soft_send_byte(addr|1);
    
    for(i=0;i<len-1;i++)
    {
        buffer[i]=i2c_soft_receive_byte(0);
    }
    buffer[i]=i2c_soft_receive_byte(1);
    
    i2c_soft_stop();
    
    return 0;
}

