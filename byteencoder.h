/**
 * @file byteencoder.h
 * @author Wang-Huachen (oisun@qq.com)
 * @brief Simple byte framing encoding header file
 * @version 0.1
 * @date 2020-11-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef BYTEENCODER_H
#define BYTEENCODER_H

#include <stdlib.h>

#ifdef __cplusplus 
extern "C" {
#endif

// #define BYTEENCODER_USING_STATIC_MEMORY

#ifdef BYTEENCODER_USING_STATIC_MEMORY
#define BYTEENCODER_STATIC_MEMORY_SIZE   1000
#endif

#define BYTEENCODER_ERR_OK    (0)       // No error
#define BYTEENCODER_ERR_ERR   (-1)      // Normal error
#define BYTEENCODER_ERR_BUF   (-2)      // Insufficient buffer

struct byteEncoder_t
{
    unsigned char sof;
    unsigned char eof;
    unsigned char esc;

    unsigned char sofIndex;
    unsigned char eofIndex;
    unsigned char escIndex;

    int err;

    size_t bufferIndex;
    size_t bufferSize;
#ifndef BYTEENCODER_USING_STATIC_MEMORY
    unsigned char buffer[0];
#else
    unsigned char buffer[BYTEENCODER_STATIC_MEMORY_SIZE];
#endif
};

enum byteDecoderState
{
    byteDecoderFindSOF = 0,
    byteDecoderDecodeNormData,
    byteDecoderDecodeSpecData,
};

struct byteDecoder_t
{
    unsigned char sof;
    unsigned char eof;
    unsigned char esc;

    unsigned char sofIndex;
    unsigned char eofIndex;
    unsigned char escIndex;

    int err;
    enum byteDecoderState state;
    void (* callback)(const unsigned char *result, size_t size);

    size_t bufferIndex;
    size_t bufferSize;
#ifdef BYTEENCODER_USING_STATIC_MEMORY
    unsigned char buffer[BYTEENCODER_STATIC_MEMORY_SIZE];
#else
    unsigned char buffer[0];
#endif
};

typedef struct byteEncoder_t *byteEncoder;
typedef struct byteDecoder_t *byteDecoder;

// Encoder
#ifndef BYTEENCODER_USING_STATIC_MEMORY
byteEncoder byteEncoderCreate(size_t bufferSize);
void byteEncoderDelete(byteEncoder encoder);
#else
byteEncoder byteEncoderInit(byteEncoder encoder);
#endif
void byteEncoderReset(byteEncoder encoder);
void byteEncoderExecute(byteEncoder encoder, unsigned char byte);
void byteEncoderExecuteBlock(byteEncoder encoder, unsigned char *bytes, size_t size);
size_t byteEncoderGetResultSize(byteEncoder encoder);
size_t byteEncoderGetResult(byteEncoder encoder, unsigned char *result, size_t size);

// Decoder
#ifndef BYTEENCODER_USING_STATIC_MEMORY
byteDecoder byteDecoderCreate(size_t bufferSize);
void byteDecoderDelete(byteDecoder decoder);
#else
byteDecoder byteDecoderInit(byteDecoder decoder);
#endif
void byteDecoderReset(byteDecoder decoder);
void byteDecoderSetCallback(byteDecoder decoder, void (* callback)(const unsigned char *, size_t));
void byteDecoderExecute(byteDecoder decoder, unsigned char byte);
void byteDecoderExecuteBlock(byteDecoder decoder, unsigned char *bytes, size_t size);

#ifdef __cplusplus 
}
#endif

#endif
