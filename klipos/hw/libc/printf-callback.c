/*
 The MIT License (MIT)
 
 Copyright (c) 2014 Baptiste Burles
 
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
#include "../include/libs-default.h"


void writeByteToPrintfStream(uint8_t c);

void defaultPrintfInterface(uint8_t c)
{
    c = 0;
}

int writeStringToPrintfStream(const char * str)
{
    int count = 0;
    while (*str)
    {
        writeByteToPrintfStream(*str++);
        count++;
    }
    
    return count;
}

static PrintfInterfaceCallback printfInterfaceCallback = &defaultPrintfInterface;


void setPrintfInterface(PrintfInterfaceCallback callback)
{
    printfInterfaceCallback = callback;
}

void writeByteToPrintfStream(uint8_t c)
{
    printfInterfaceCallback(c);
}
