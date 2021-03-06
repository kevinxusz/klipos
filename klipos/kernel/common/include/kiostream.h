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
#ifndef KLIPOS_KERNEL_IOSTREAM_H
#define KLIPOS_KERNEL_IOSTREAM_H

/** @brief Input/Output stream object
 */
typedef struct _kernel_io_stream_
{
#ifdef FIRMWARE_USE_KERNEL_FULL
    KThread *       receiver;   /**< receiver thread */
#endif
    
    uint8_t *         buffer;     /**< input/output buffer */
    uint16_t          read;       /**< read index */
    uint16_t          write;      /**< write index */
    uint16_t          size;       /**< buffer size */
} KIOStream;


/** @brief Create a new i/o stream.
 *
 * @param stream    stream to be initialized, you have to allocate this pointer !
 * @param buffer    stream's buffer, you have to allocate this pointer !
 * @param size      Size of buffer, MUST BE 8, 16, 32, 64 etc...
 */
extern void initIOStream(KIOStream *stream, uint8_t *buffer, uint32_t size);

/** @brief Write a byte to a stream.
 *
 * @param stream    i/o stream object.
 * @param data      Byte to write.
 */
extern void writeByteToIOStream(KIOStream *stream, uint8_t data);

/** @brief Read a buffer of the specified lenght from the stream.
 *
 * If stream is empty:
 *  --> return 0 
 * 
 * If stream have some data available but not the requested lenght
 *  --> return the number of byte currently stored in the i/o stream
 *  --> BUT the data are not written into the buffer !
 * 
 * Else, write into the buffer the requested lenght's byte and return the lenght.
 * 
 * @param stream        i/o stream object
 * @param buffer        buffer where store the byte read from the stream
 * @param len           number of bytes you want from the stream
 * @return 0 if no bytes available, else number of bytes available in the stream
 */
extern uint32_t readBufferFromIOStream(KIOStream *stream, uint8_t *buffer, uint32_t len);

/** @brief Get one byte from a stream.
 *
 * @param stream    i/o stream object.
 * @param data      Pointer to a byte where write the read's byte.
 * @return false if no byte, else true.
 */
extern bool readByteFromIOStream(KIOStream *stream, uint8_t *data);


/** @brief Get byte number available from stream
 * 
 * @param stream    i/o stream object 
 * @return true if data available, else false.
 */
extern uint32_t getByteAvailableFromIOStream(KIOStream *stream);


/** @brief Wake up a task inside an IRQ.
 *
 * @warning Call this function only in an IRQ !
 *
 * @param stream    i/o stream object.
 */
#ifdef FIRMWARE_USE_KERNEL_FULL
        extern void irqWakeUpTaskFromIOStream(KIOStream *stream);
#else
#       define irqWakeUpTaskFromIOStream(stream)
#endif

/** @brief Wait data from a stream.
 *
 * @param stream    i/o stream object.
 */
#ifdef FIRMWARE_USE_KERNEL_FULL
        extern void waitDataFromIOStream(KIOStream *stream);
#else
#       define waitDataFromIOStream(stream)
#endif
        

#endif
