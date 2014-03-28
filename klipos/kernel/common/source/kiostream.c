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

#include "../../../hw/include/libs-default.h"
#include "../../kernel-klipos.h"

#ifdef FIRMWARE_USE_KERNEL_FULL
#       include "../../full/include/kernel-private.h"
#endif

void initIOStream(KIOStream *stream, UInt8 *buffer, UInt32 size)
{
#ifdef FIRMWARE_USE_KERNEL_FULL
    stream->receiver = 0;
#endif
    stream->read = 0;
    stream->write = 0;
    stream->buffer = buffer;
    stream->size = size;
}

void writeByteToIOStream(KIOStream *stream, UInt8 data)
{
    stream->buffer[stream->write & (stream->size-1)] = data;
    stream->write++;
}

UInt32 readBufferFromIOStream(KIOStream *stream, UInt8 *buffer, UInt32 len)
{
    UInt32 counter, i, size, read;
    
    read = stream->read;
    size = stream->write;
    if (size == read)
    {
        return 0;
    }
    
    if ( (UInt32)(size-read) < len )
    {
        return (size-read);
    }
    
    for (i = read, counter = 0 ; i != size && counter < len ; counter++ )
    {
        buffer[counter] = stream->buffer[i & (stream->size-1)];
        i++;
    }
    stream->read = i;
    
    return counter;
}

Bool readByteFromIOStream(KIOStream *stream, UInt8 *data)
{
    if(stream->read!=stream->write)
    {
        *data = stream->buffer[stream->read & (stream->size-1)];
        stream->read++;
        return True;
    }

    return False;
}

Bool isDataAvailableFromIOStream(KIOStream *stream)
{
    if(stream->read!=stream->write)
    {
        return True;
    }
    
    return False;
}

#ifdef FIRMWARE_USE_KERNEL_FULL

void irqWakeUpTaskFromIOStream(KIOStream *stream)
{
    KThread * th = stream->receiver;
    
    if(th!=0)
    {
        // a task is waiting, wakeup this task !
        stream->receiver = 0;
        irqSetTaskAsReady(th);
    }
}

void waitDataFromIOStream(KIOStream *stream)
{
    if( stream->read==stream->write)
    {
        stream->receiver = (KThread *)currentTask;
        setTaskAsBlocked();
    }
}

#endif
