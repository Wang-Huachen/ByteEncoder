/**
 * @file byteencoder.c
 * @author Wang-Huachen (oisun@qq.com)
 * @brief Simple byte framing encoding source file
 * @version 0.1
 * @date 2020-11-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <string.h>

#include "byteencoder.h"

#define BYTEENCODER_SOF   0x7D      // Start of Frame
#define BYTEENCODER_EOF   0x7E      // End of Frame
#define BYTEENCODER_ESC   0x7F      // Escape character

#define BYTEENCODER_SOF_INDEX   0x00    // Escape sequence number of SOF
#define BYTEENCODER_EOF_INDEX   0x01    // Escape sequence number of EOF
#define BYTEENCODER_ESC_INDEX   0x02    // Escape sequence number of ESC

static inline int isByteEncoderSpecial(byteEncoder encoder, unsigned char byte)
{
    return (byte == encoder->sof) || (byte == encoder->eof) || (byte == encoder->esc);
}

static inline void byteEncoderInputBuffer(byteEncoder encoder, unsigned char byte)
{
    if (encoder->bufferIndex >= encoder->bufferSize)
    {
        encoder->err = BYTEENCODER_ERR_BUF;
        return;
    }
    encoder->buffer[encoder->bufferIndex++] = byte;
}

static inline void byteDecoderInputBuffer(byteDecoder decoder, unsigned char byte)
{
    if (decoder->bufferIndex >= decoder->bufferSize)
    {
        decoder->err = BYTEENCODER_ERR_BUF;
        return;
    }
    decoder->buffer[decoder->bufferIndex++] = byte;
}

static inline int isByteDecoderSpecial(byteDecoder decoder, unsigned char byte)
{
    return (byte == decoder->sof) || (byte == decoder->eof) || (byte == decoder->esc);
}

#ifndef BYTEENCODER_USING_STATIC_MEMORY
byteEncoder byteEncoderCreate(size_t bufferSize)
{
    if (!bufferSize) return NULL;
    byteEncoder encoder = (byteEncoder)malloc(sizeof(struct byteEncoder_t) + sizeof(unsigned char)*bufferSize);
    if (!encoder) return NULL;

    encoder->sof = BYTEENCODER_SOF;
    encoder->eof = BYTEENCODER_EOF;
    encoder->esc = BYTEENCODER_ESC;

    encoder->sofIndex = BYTEENCODER_SOF_INDEX;
    encoder->eofIndex = BYTEENCODER_EOF_INDEX;
    encoder->escIndex = BYTEENCODER_ESC_INDEX;

    encoder->err = BYTEENCODER_ERR_OK;

    encoder->bufferSize = bufferSize;
    encoder->bufferIndex = 0;
    memset(encoder->buffer, 0, encoder->bufferSize);

    return encoder;
}

void byteEncoderDelete(byteEncoder encoder)
{
    free((void *)encoder);
    encoder = NULL;
    return;
}
#else
byteEncoder byteEncoderInit(byteEncoder encoder)
{
    encoder->sof = BYTEENCODER_SOF;
    encoder->eof = BYTEENCODER_EOF;
    encoder->esc = BYTEENCODER_ESC;

    encoder->sofIndex = BYTEENCODER_SOF_INDEX;
    encoder->eofIndex = BYTEENCODER_EOF_INDEX;
    encoder->escIndex = BYTEENCODER_ESC_INDEX;

    encoder->err = BYTEENCODER_ERR_OK;

    encoder->bufferSize = BYTEENCODER_STATIC_MEMORY_SIZE;
    encoder->bufferIndex = 0;
    memset(encoder->buffer, 0, encoder->bufferSize);
}
#endif

void byteEncoderReset(byteEncoder encoder)
{
    if (!encoder) return;

    encoder->err = BYTEENCODER_ERR_OK;
    encoder->bufferIndex = 0;
}

void byteEncoderExecute(byteEncoder encoder, unsigned char byte)
{
    if (!encoder) return;

    if (isByteEncoderSpecial(encoder, byte))
    {
        byteEncoderInputBuffer(encoder, encoder->esc);
        if (byte == encoder->sof) byteEncoderInputBuffer(encoder, encoder->sofIndex);
        else if (byte == encoder->eof) byteEncoderInputBuffer(encoder, encoder->eofIndex);
        else if (byte == encoder->esc) byteEncoderInputBuffer(encoder, encoder->escIndex);
    }
    else
    {
        byteEncoderInputBuffer(encoder, byte);
    }
}

void byteEncoderExecuteBlock(byteEncoder encoder, unsigned char *bytes, size_t size)
{
    for (size_t i = 0; i < size; i++)
        byteEncoderExecute(encoder, bytes[i]);
}

size_t byteEncoderGetResultSize(byteEncoder encoder)
{
    return encoder->bufferIndex + 2;       // SOF + Data + EOF
}

size_t byteEncoderGetResult(byteEncoder encoder, unsigned char *result, size_t size)
{
    size_t resultSize = byteEncoderGetResultSize(encoder);
    resultSize = (resultSize > size)?size:resultSize;

    if ((!result) || (!resultSize)) return 0;

    result[0] = encoder->sof;
    for (size_t i = 1; i < resultSize-1; i++)
        result[i] = encoder->buffer[i-1];
    result[resultSize-1] = encoder->eof;

    return resultSize;
}

#ifndef BYTEENCODER_USING_STATIC_MEMORY
byteDecoder byteDecoderCreate(size_t bufferSize)
{
    if (!bufferSize) return NULL;
    byteDecoder decoder = (byteDecoder)malloc(sizeof(struct byteDecoder_t) + sizeof(unsigned char)*bufferSize);
    if (!decoder) return NULL;

    decoder->sof = BYTEENCODER_SOF;
    decoder->eof = BYTEENCODER_EOF;
    decoder->esc = BYTEENCODER_ESC;

    decoder->sofIndex = BYTEENCODER_SOF_INDEX;
    decoder->eofIndex = BYTEENCODER_EOF_INDEX;
    decoder->escIndex = BYTEENCODER_ESC_INDEX;

    decoder->err = BYTEENCODER_ERR_OK;
    decoder->state = byteDecoderFindSOF;
    decoder->callback = NULL;

    decoder->bufferSize = bufferSize;
    decoder->bufferIndex = 0;
    memset(decoder->buffer, 0, decoder->bufferSize);

    return decoder;
}

void byteDecoderDelete(byteDecoder decoder)
{
    free((void *)decoder);
    decoder = NULL;
    return;
}
#else
byteDecoder byteDecoderInit(byteDecoder decoder)
{
    decoder->sof = BYTEENCODER_SOF;
    decoder->eof = BYTEENCODER_EOF;
    decoder->esc = BYTEENCODER_ESC;

    decoder->sofIndex = BYTEENCODER_SOF_INDEX;
    decoder->eofIndex = BYTEENCODER_EOF_INDEX;
    decoder->escIndex = BYTEENCODER_ESC_INDEX;

    decoder->err = BYTEENCODER_ERR_OK;
    decoder->state = byteDecoderFindSOF;
    decoder->callback = NULL;

    decoder->bufferSize = BYTEENCODER_STATIC_MEMORY_SIZE;
    decoder->bufferIndex = 0;
    memset(decoder->buffer, 0, decoder->bufferSize);
}
#endif

void byteDecoderReset(byteDecoder decoder)
{
    if (!decoder) return;

    decoder->err = BYTEENCODER_ERR_OK;
    decoder->state = byteDecoderFindSOF;
    decoder->bufferIndex = 0;
}

void byteDecoderSetCallback(byteDecoder decoder, void (* callback)(const unsigned char *, size_t))
{
    decoder->callback = callback;
}

void byteDecoderExecute(byteDecoder decoder, unsigned char byte)
{
    switch (decoder->state)
    {
    case byteDecoderFindSOF:
        if (byte == decoder->sof) decoder->state = byteDecoderDecodeNormData;
        break;

    case byteDecoderDecodeNormData:
        if (byte == decoder->eof)
        {
            if (decoder->callback)
                decoder->callback(decoder->buffer, decoder->bufferIndex);
            byteDecoderReset(decoder);
        }
        else if (byte == decoder->esc)
        {
            decoder->state = byteDecoderDecodeSpecData;
        }
        else
        {
            byteDecoderInputBuffer(decoder, byte);
        }
        break;

    case byteDecoderDecodeSpecData:
        if (byte == decoder->sofIndex) byteDecoderInputBuffer(decoder, decoder->sof);
        else if (byte == decoder->eofIndex) byteDecoderInputBuffer(decoder, decoder->eof);
        else if (byte == decoder->escIndex) byteDecoderInputBuffer(decoder, decoder->esc);
        else decoder->err = BYTEENCODER_ERR_ERR;
        decoder->state = byteDecoderDecodeNormData;
        break;
    
    default:
        break;
    }
}

void byteDecoderExecuteBlock(byteDecoder decoder, unsigned char *bytes, size_t size)
{
    for (size_t i = 0; i < size; i++)
        byteDecoderExecute(decoder, bytes[i]);
}