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


static uint32_t memoryModuleNumber;
static uint32_t memoryModuleIndex;
static MemoryModule * memoryModules;

MemProtocolSendCallback sendFromModuleToMaster;


void initMemoryModule(uint32_t number, MemProtocolSendCallback func)
{
    memoryModuleIndex = 0;
    memoryModuleNumber = number;
    memoryModules = malloc( sizeof(MemoryModule) * number);
    sendFromModuleToMaster = func;
}

void addMemoryModule(uint16_t virtualAddr, uint8_t * objectAddr, uint32_t objectSize, MemProtocolUpdateCallback update )
{
    memoryModules[memoryModuleIndex].id = virtualAddr;
    memoryModules[memoryModuleIndex].size = objectSize;
    memoryModules[memoryModuleIndex].ptr = objectAddr;
    memoryModules[memoryModuleIndex].update = update;
    
    memoryModuleIndex++;
}

MemoryModule * getMemoryModuleFromAddress(uint16_t address)
{
    uint32_t i;
    MemoryModule * mem;

    for(i=0; i<memoryModuleNumber; i++)
    {
        mem = &memoryModules[i];
        if( address >= mem->id && address <= mem->id + mem->size)
        {
            return mem;
        }
    }

    return 0;
}


void updateMemoryModule(uint8_t *buffer, uint8_t size)
{
    uint32_t i ;
    MemoryModule * mem;
    uint8_t* ptr;
    uint32_t offset;
    uint16_t virtualAddr;

    virtualAddr = ((buffer)[0]<<8)|((buffer)[1]);

    mem = getMemoryModuleFromAddress(virtualAddr);

//    printf("virtualAddr 0x%x size %d\r\n", virtualAddr, size);
    
    if ( mem == 0 )
    {
        return;
    }

    offset = virtualAddr - mem->id ;
    ptr     = mem->ptr + offset;

//    printf("Addr 0x%x FoundId 0x%x Off %d MemSize %d Size %d\r\n", virtualAddr, mem->id, offset, mem->size, size );

    if ( size == 2)
    {
        // If we have just 2 bytes of data, then it's a write order: 
        // Copy data from memory module to the master:
        sendFromModuleToMaster(virtualAddr, ptr, mem->size - offset);
    }
    else
    {    
        
        // Copy data to memory
        for (i=0; i < (uint32_t)(size-2) && i < mem->size ;i++)
        {
            ptr[i] = buffer[i+2];
        }

        // Update the memory module
        if (mem->update!=0)
        {
            mem->update();
        }
    }

}
