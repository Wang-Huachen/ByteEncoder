#include <stdio.h>

#include "byteencoder.h"

static unsigned char test0Input[] = {0x55, 0x66, 0x88, 0x99};
static unsigned char test1Input[] = {0x55, 0x7d, 0x88, 0x99};
static unsigned char test2Input[] = {0x55, 0x7f, 0x00, 0x99};

void decoderCallback(const unsigned char *result, size_t size)
{
    printf("Decode:\n");
    for (size_t i = 0; i < size; i++)
        printf("0x%02X ", result[i]);
    printf("\n");
}

int main()
{
#ifndef BYTEENCODER_USING_STATIC_MEMORY
    // encoder
    byteEncoder encoder = byteEncoderCreate(200);

    byteEncoderReset(encoder);
    byteEncoderExecuteBlock(encoder, test0Input, sizeof(test0Input));
    size_t resultSize0 = byteEncoderGetResultSize(encoder);
    unsigned char res0[resultSize0];
    resultSize0 = byteEncoderGetResult(encoder, res0, resultSize0);
    printf("res0:\n");
    for (size_t i = 0; i < resultSize0; i++)
        printf("0x%02X ", res0[i]);
    printf("\n");

    byteEncoderReset(encoder);
    byteEncoderExecuteBlock(encoder, test1Input, sizeof(test1Input));
    size_t resultSize1 = byteEncoderGetResultSize(encoder);
    unsigned char res1[resultSize1];
    resultSize1 = byteEncoderGetResult(encoder, res1, resultSize1);
    printf("res1:\n");
    for (size_t i = 0; i < resultSize1; i++)
        printf("0x%02X ", res1[i]);
    printf("\n");

    byteEncoderReset(encoder);
    byteEncoderExecuteBlock(encoder, test2Input, sizeof(test2Input));
    size_t resultSize2 = byteEncoderGetResultSize(encoder);
    unsigned char res2[resultSize2];
    resultSize2 = byteEncoderGetResult(encoder, res2, resultSize2);
    printf("res2:\n");
    for (size_t i = 0; i < resultSize2; i++)
        printf("0x%02X ", res2[i]);
    printf("\n");

    byteEncoderDelete(encoder);

    // decoder
    byteDecoder decoder = byteDecoderCreate(200);
    byteDecoderSetCallback(decoder, decoderCallback);

    byteDecoderExecuteBlock(decoder, res0, resultSize0);
    byteDecoderExecuteBlock(decoder, res1, resultSize1);
    byteDecoderExecuteBlock(decoder, res2, resultSize2);

    byteDecoderDelete(decoder);
#else
    // encoder
    struct byteEncoder_t encoder;
    byteEncoderInit(&encoder);

    byteEncoderReset(&encoder);
    byteEncoderExecuteBlock(&encoder, test0Input, sizeof(test0Input));
    size_t resultSize0 = byteEncoderGetResultSize(&encoder);
    unsigned char res0[resultSize0];
    resultSize0 = byteEncoderGetResult(&encoder, res0, resultSize0);
    printf("res0:\n");
    for (size_t i = 0; i < resultSize0; i++)
        printf("0x%02X ", res0[i]);
    printf("\n");

    byteEncoderReset(&encoder);
    byteEncoderExecuteBlock(&encoder, test1Input, sizeof(test1Input));
    size_t resultSize1 = byteEncoderGetResultSize(&encoder);
    unsigned char res1[resultSize1];
    resultSize1 = byteEncoderGetResult(&encoder, res1, resultSize1);
    printf("res1:\n");
    for (size_t i = 0; i < resultSize1; i++)
        printf("0x%02X ", res1[i]);
    printf("\n");

    byteEncoderReset(&encoder);
    byteEncoderExecuteBlock(&encoder, test2Input, sizeof(test2Input));
    size_t resultSize2 = byteEncoderGetResultSize(&encoder);
    unsigned char res2[resultSize2];
    resultSize2 = byteEncoderGetResult(&encoder, res2, resultSize2);
    printf("res2:\n");
    for (size_t i = 0; i < resultSize2; i++)
        printf("0x%02X ", res2[i]);
    printf("\n");

    // decoder
    struct byteDecoder_t decoder;
    byteDecoderInit(&decoder);
    byteDecoderSetCallback(&decoder, decoderCallback);

    byteDecoderExecuteBlock(&decoder, res0, resultSize0);
    byteDecoderExecuteBlock(&decoder, res1, resultSize1);
    byteDecoderExecuteBlock(&decoder, res2, resultSize2);

#endif
    return 0;
}