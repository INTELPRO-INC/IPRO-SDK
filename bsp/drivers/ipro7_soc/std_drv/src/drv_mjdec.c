/**
  ******************************************************************************
  * @file    drv_mjdec.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2025 INTELPRO </center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of IntelPro nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "ipro7.h"
#include "drv_mjdec.h"
#include "drv_mjcomb.h"
#include "drv_glb.h"
#include "compiler/common.h"

/** @addtogroup  Peripheral_Driver
 *  @{
 */

/** @addtogroup  MJDEC
 *  @{
 */

/** @defgroup  MJDEC_Private_Macros
 *  @{
 */

/*@} end of group MJDEC_Private_Macros */

/** @defgroup  MJDEC_Private_Types
 *  @{

 */
typedef struct {

    //uint8_t * streamWrPtr;
    //uint8_t * streamRdPtr;
    //int    streamEndflag;
    uint8_t *streamBufStartAddr;
    uint8_t *streamBufEndAddr;
    int streamBufSize;
    uint8_t *pBitStream;

    //int frameOffset;
    //Uint64 consumeByte;     //Abner++, due to JpuReadReg(MJPEG_GBU_TT_CNT_REG+4)<<32 in apuapi_altek.c

    //FrameBuffer * frameBufPool;
    //int numFrameBuffers;
    //int stride;
    //int rotationEnable;
    //int mirrorEnable;
    //int mirrorDirection;
    //int rotationAngle;
    //FrameBuffer rotatorOutput;
    //int rotatorStride;
    //int rotatorOutputValid;
    //int initialInfoObtained;
    //int minFrameBufferNum;
    //int streamEndian;
    //int frameEndian;
    //int chromaInterleave;

    int picWidth;
    int picHeight;
    int alignedWidth;
    int alignedHeight;

    int ecsPtr;
    //int pagePtr;
    //int wordPtr;
    //int bitPtr;
    int format;
    int rstIntval;

    int userHuffTab;

    int huffDcIdx;
    int huffAcIdx;
    int Qidx;

    uint8_t huffVal[4][162];
    uint8_t huffBits[4][256];
    uint8_t cInfoTab[4][6];
    uint8_t qMatTab[4][64];

    uint32_t huffMin[4][16];
    uint32_t huffMax[4][16];
    uint8_t  huffPtr[4][16];

    // partial
    int usePartial;
    int lineNum;
    int bufNum;


    int busReqNum;
    int compNum;
    int mcuBlockNum;
    int compInfo[3];

    //int frameIdx;

    //int bitEmpty;
    //int iHorScaleMode;
    //int iVerScaleMode;

    //GetBitContext gbc;

    uint8_t *checkedByteAddr;
    int     headerBytes;
} MJDEC_Header_Info;
/*@} end of group MJDEC_Private_Types */

/** @defgroup  MJDEC_Private_Variables
 *  @{
 */
#if 0   /*not use ==> use hal driver to handle IRQ*/
static intCallback_Type * mjdecIntCbfArra[MJDEC_INT_ALL] = {NULL};
#endif

/*@} end of group MJDEC_Private_Variables */

/** @defgroup  MJDEC_Global_Variables
 *  @{
 */

ATTR_PSRAM_SECTION MJDEC_Header_Info g_tHeaderInfo;

/*@} end of group MJDEC_Global_Variables */

/** @defgroup  MJDEC_Private_Fun_Declaration
 *  @{
 */

/*@} end of group MJDEC_Private_Fun_Declaration */

/** @defgroup  MJDEC_Private_Functions
 *  @{
 */

/*@} end of group MJDEC_Private_Functions */

/** @defgroup  MJDEC_Public_Functions
 *  @{
 */

typedef enum {
    FORMAT_420 = 0,
    FORMAT_422 = 1,
    FORMAT_224 = 2,
    FORMAT_444 = 3,
    FORMAT_400 = 4
} FrameFormat;


enum {
    SAMPLE_420 = 0xA,
    SAMPLE_H422 = 0x9,
    SAMPLE_V422 = 0x6,
    SAMPLE_444 = 0x5,
    SAMPLE_400 = 0x1
};

const uint8_t cDefHuffBits[4][16] =
{
    {    // DC index 0 (Luminance DC)
        0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01,
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {    // AC index 0 (Luminance AC)
        0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03,
            0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D
    },
    {    // DC index 1 (Chrominance DC)
        0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
            0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {    // AC index 1 (Chrominance AC)
        0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04,
            0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77
    }
};

const uint8_t cDefHuffVal[4][162] =
{
    {    // DC index 0 (Luminance DC)
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B
    },
    {    // AC index 0 (Luminance AC)
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
        0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
        0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08,
        0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16,
        0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
        0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6,
        0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
        0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4,
        0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
        0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,
        0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
        0xF9, 0xFA
    },
    {    // DC index 1 (Chrominance DC)
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B
    },
    {    // AC index 1 (Chrominance AC)
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
        0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
        0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
        0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34,
        0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
        0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96,
        0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
        0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4,
        0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
        0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2,
        0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
        0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9,
        0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
        0xF9, 0xFA
    }
};

const uint8_t zigzag[64] =
{ // zig-zag �Ƨ�
  0,  1,  5,  6, 14, 15, 27, 28,
  2,  4,  7, 13, 16, 26, 29, 42,
  3,  8, 12, 17, 25, 30, 41, 43,
  9, 11, 18, 24, 31, 40, 44, 53,
 10, 19, 23, 32, 39, 45, 52, 54,
 20, 22, 33, 38, 46, 51, 55, 60,
 21, 34, 37, 47, 50, 56, 59, 61,
 35, 36, 48, 49, 57, 58, 62, 63
};

enum {
    Marker            = 0xFF,
    FF_Marker         = 0x00,

    SOI_Marker        = 0xFFD8,            // Start of image
    EOI_Marker        = 0xFFD9,            // End of image

    JFIF_CODE         = 0xFFE0,            // Application
    EXIF_CODE         = 0xFFE1,

    DRI_Marker        = 0xFFDD,            // Define restart interval
    RST_Marker        = 0xD,               // 0xD0 ~0xD7

    DQT_Marker        = 0xFFDB,            // Define quantization table(s)
    DHT_Marker        = 0xFFC4,            // Define Huffman table(s)

    SOF_Marker        = 0xFFC0,            // Start of frame : Baseline DCT
    SOS_Marker        = 0xFFDA,            // Start of scan
};

#define DC_TABLE_INDEX0                 0
#define AC_TABLE_INDEX0                 1
#define DC_TABLE_INDEX1                 2
#define AC_TABLE_INDEX1                 3

#define Q_COMPONENT0                    0
#define Q_COMPONENT1                    0x40
#define Q_COMPONENT2                    0x80




int find_start_soi_code(MJDEC_Header_Info *a_ptHeaderInfo)
{
    int word;
    uint8_t *pucChkAddr = a_ptHeaderInfo->checkedByteAddr;

    while(1)
    {
        if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) <= 2)    /*remain bytes < 2*/
        {
            printf("hit end of stream\r\n");
            return -1;
        }

        word =  (*(pucChkAddr+0)<<8) |
                (*(pucChkAddr+1)<<0);

        if ((word > 0xFF00) && (word < 0xFFFF))
        {
            if (word != SOI_Marker)
                pucChkAddr += 1;
            break;
        }


        pucChkAddr += 1;
    }

    a_ptHeaderInfo->checkedByteAddr = pucChkAddr;

    return word;
}
int find_start_code(MJDEC_Header_Info *a_ptHeaderInfo)
{
    int word;
    uint8_t *pucChkAddr = a_ptHeaderInfo->checkedByteAddr;

    while(1)
    {
        if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) <= 2)    /*remain bytes < 2*/
        {
            printf("hit end of stream\n");
            return -1;
        }

        word =  (*(pucChkAddr+0)<<8) |
                (*(pucChkAddr+1)<<0);
        if ((word > 0xFF00) && (word < 0xFFFF))
            break;


        pucChkAddr += 1;
    }

    a_ptHeaderInfo->checkedByteAddr = pucChkAddr;

    return word;
}
int decode_app_header(MJDEC_Header_Info *a_ptHeaderInfo)
{
    int length;

    uint8_t *pucChkAddr = a_ptHeaderInfo->checkedByteAddr;

    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < 2)     // next 2 bytes availabe or not
        return -1;

    length =  (*(pucChkAddr+0)<<8) |
              (*(pucChkAddr+1)<<0);
    pucChkAddr += 2;
    length -= 2;

    while(length-- > 0)
    {
        if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < 1)   // next 1 byte availabe or not
            return -1;
        pucChkAddr += 1;
    }

    a_ptHeaderInfo->checkedByteAddr = pucChkAddr;

    return 1;
}
int decode_dri_header(MJDEC_Header_Info *a_ptHeaderInfo)
{
    uint8_t *pucChkAddr = a_ptHeaderInfo->checkedByteAddr;

    //Length, Lr
    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < 4)     // next 4 bytes availabe or not
        return -1;
    pucChkAddr += 2;

    a_ptHeaderInfo->rstIntval = (*(pucChkAddr+0)<<8) |
                                (*(pucChkAddr+1)<<0);

    pucChkAddr += 2;
    a_ptHeaderInfo->checkedByteAddr = pucChkAddr;

    return 1;
}
int decode_dqt_header(MJDEC_Header_Info *a_ptHeaderInfo)
{
    int Pq=0;
    int Tq=0;
    int i=0;
    uint8_t *pucChkAddr = a_ptHeaderInfo->checkedByteAddr;

    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < 2)     // next 2 bytes availabe or not
        return -1;

    // Lq, Length of DQT
    pucChkAddr += 2;

    do {

        if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < ((4+4+8*64)/8))     // next (4+4+8*64)/8 bytes availabe or not
            return -1;
        // Pq, Quantization Precision, 0: 8bit, 1: 16bit
        Pq = ((*pucChkAddr)&0xf0)>>4;
        // Tq, Quantization table destination identifier
        Tq = (*pucChkAddr)&0xf;
        pucChkAddr+=1;

        for (i=0; i<64; i++)
            a_ptHeaderInfo->qMatTab[Tq][i] = *pucChkAddr++;
    } while(*pucChkAddr != 0xFF);

    a_ptHeaderInfo->checkedByteAddr = pucChkAddr;

    if (Pq != 0) // not 8-bit
    {
        printf("decode_dqt_header => pq is not set to zero\r\n");
        return -1;
    }
    return 1;
}

int decode_dth_header(MJDEC_Header_Info *a_ptHeaderInfo)
{
    int Tc=0;
    int Th=0;
    int ThTc=0;
    int bitCnt=0;
    int i=0;
    uint8_t *pucChkAddr = a_ptHeaderInfo->checkedByteAddr;

    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < 2)     // next 2 bytes availabe or not
        return -1;

    // Length, Lh
    pucChkAddr += 2;

    do {
        if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < ((8 + 8*16)/8))     // next (8 + 8*16)/8 bytes availabe or not
            return -1;

        // Table class - DC, AC
        Tc = ((*pucChkAddr)&0xf0)>>4;
        // Table destination identifier
        Th = (*pucChkAddr)&0xf;
        pucChkAddr+=1;

        // DC_ID0 (0x00) -> 0
        // AC_ID0 (0x10) -> 1
        // DC_ID1 (0x01) -> 2
        // AC_ID1 (0x11) -> 3
        ThTc = ((Th&1)<<1) | (Tc&1);

        // Get Huff Bits list
        bitCnt = 0;
        for (i=0; i<16; i++)
        {
            a_ptHeaderInfo->huffBits[ThTc][i] = *pucChkAddr++;
            bitCnt += a_ptHeaderInfo->huffBits[ThTc][i];

            if (cDefHuffBits[ThTc][i] != a_ptHeaderInfo->huffBits[ThTc][i])
                a_ptHeaderInfo->userHuffTab = 1;
        }

        if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < (8*bitCnt)/8)     // next (8*bitCnt)/8 bytes availabe or not
            return -1;

        // Get Huff Val list
        for (i=0; i<bitCnt; i++)
        {
            a_ptHeaderInfo->huffVal[ThTc][i] = *pucChkAddr++;

            if (cDefHuffVal[ThTc][i] != a_ptHeaderInfo->huffVal[ThTc][i])
                a_ptHeaderInfo->userHuffTab = 1;
        }
    } while(*pucChkAddr != 0xFF);

    a_ptHeaderInfo->checkedByteAddr = pucChkAddr;

    return 1;
}
int decode_sof_header(MJDEC_Header_Info *a_ptHeaderInfo)
{
    int samplePrecision;
    int sampleFactor;
    int i;
    int Tqi=0;
    int compID=0;
    int hSampFact[3];
    int vSampFact[3];
    int picX=0, picY=0;
    int numComp=0;
    uint8_t *pucChkAddr = a_ptHeaderInfo->checkedByteAddr;

    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < ((16+8+16+16+8)/8))     // next (16+8+16+16+8)/8 bytes availabe or not
        return -1;

    // LF, Length of SOF
    pucChkAddr += 2;

    // Sample Precision: Baseline(8), P
    samplePrecision = *pucChkAddr++;

    if (samplePrecision != 8)
    {
        printf("Sample Precision is not 8\r\n");
        return -1;
    }

    picY = (*(pucChkAddr+0)<<8) |
           (*(pucChkAddr+1)<<0);
    pucChkAddr += 2;

    if (picY > MAX_MJPG_PIC_HEIGHT )
    {
        printf("Picture Vertical Size limits Maximum size\r\n");
        return -1;
    }

    picX = (*(pucChkAddr+0)<<8) |
           (*(pucChkAddr+1)<<0);
    pucChkAddr += 2;

    if (picX > MAX_MJPG_PIC_WIDTH)
    {
        printf("Picture Horizontal Size limits Maximum size\r\n");
        return -1;
    }

    //Number of Components in Frame: Nf
    numComp = *pucChkAddr++;
    if (numComp > 3)
    {
        printf("Picture Horizontal Size limits Maximum size\r\n");
    }

    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < ((numComp*(8+4+4+8))/8))     // next (numComp*(8+4+4+8))/8 bytes availabe or not
        return -1;

    for (i=0; i<numComp; i++)
    {
        // Component ID, Ci 0 ~ 255
        compID = *pucChkAddr++;
        // Horizontal Sampling Factor, Hi
        hSampFact[i] = ((*pucChkAddr)&0xf0)>>4;
        // Vertical Sampling Factor, Vi
        vSampFact[i] = (*pucChkAddr)&0xf;
        pucChkAddr+=1;

        // Quantization Table Selector, Tqi
        Tqi = *pucChkAddr++;

        a_ptHeaderInfo->cInfoTab[i][0] = compID;
        a_ptHeaderInfo->cInfoTab[i][1] = hSampFact[i];
        a_ptHeaderInfo->cInfoTab[i][2] = vSampFact[i];
        a_ptHeaderInfo->cInfoTab[i][3] = Tqi;
    }

    //if ( hSampFact[0]>2 || vSampFact[0]>2 || ( numComp == 3 && ( hSampFact[1]!=1 || hSampFact[2]!=1 || vSampFact[1]!=1 || vSampFact[2]!=1) ) )
        //printf("Not Supported Sampling Factor\r\n");

    if (numComp == 1)
        sampleFactor = SAMPLE_400;
    else
        sampleFactor = ((hSampFact[0]&3)<<2) | (vSampFact[0]&3);

    switch(sampleFactor) {
    case SAMPLE_420:
        a_ptHeaderInfo->format = FORMAT_420;
        break;
    case SAMPLE_H422:
        a_ptHeaderInfo->format = FORMAT_422;
        break;
    case SAMPLE_V422:
        a_ptHeaderInfo->format = FORMAT_224;
        break;
    case SAMPLE_444:
        a_ptHeaderInfo->format = FORMAT_444;
        break;
    default:    // 4:0:0
        a_ptHeaderInfo->format = FORMAT_400;
    }

    a_ptHeaderInfo->picWidth = picX;
    a_ptHeaderInfo->picHeight = picY;

    a_ptHeaderInfo->checkedByteAddr = pucChkAddr;

    return 1;
}

int decode_sos_header(MJDEC_Header_Info *a_ptHeaderInfo)
{
    int i=0, j=0;
    int len=0;
    int numComp=0;
    int compID=0;
    //int ecsPtr;
    int ss=0, se=0, ah=0, al=0;
    int dcHufTblIdx[3];
    int acHufTblIdx[3];
    uint8_t *pucChkAddr = a_ptHeaderInfo->checkedByteAddr;

    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < (16/8))     // next 16/8 bytes availabe or not
        return -1;

    // Length, Ls
    len = (*(pucChkAddr+0)<<8) |
          (*(pucChkAddr+1)<<0);
    pucChkAddr += 2;


    a_ptHeaderInfo->ecsPtr = (pucChkAddr - a_ptHeaderInfo->streamBufStartAddr) + (len - 2) ;
    a_ptHeaderInfo->headerBytes = a_ptHeaderInfo->ecsPtr;

    //ecsPtr = a_ptHeaderInfo->ecsPtr+a_ptHeaderInfo->frameOffset;

    ////the second case is that HOST fill the bistream data from begging(SOI).
    //a_ptHeaderInfo->pagePtr = ecsPtr/256;
    //a_ptHeaderInfo->wordPtr = (ecsPtr % 256) / 4;    // word unit
    //if (a_ptHeaderInfo->pagePtr & 1)
    //    a_ptHeaderInfo->wordPtr += 64;
    //if (a_ptHeaderInfo->wordPtr & 1)
    //    a_ptHeaderInfo->wordPtr -= 1; // to make even.
   //
    //a_ptHeaderInfo->bitPtr = (ecsPtr % 4) * 8;    // bit unit
    //if (((ecsPtr % 256) / 4) & 1)
    //    a_ptHeaderInfo->bitPtr += 32;

    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < (8/8))     // next 8/8 bytes availabe or not
        return -1;

    //Number of Components in Scan: Ns
    numComp = *pucChkAddr++;

    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < ((numComp*(8+4+4))/8))     // next (numComp*(8+4+4))/8 bytes availabe or not
        return -1;

    for (i=0; i<numComp; i++) {
        // Component ID, Csj 0 ~ 255
        compID = *pucChkAddr++;
        // dc entropy coding table selector, Tdj
        dcHufTblIdx[i] = ((*pucChkAddr)&0xf0)>>4;
        // ac entropy coding table selector, Taj
        acHufTblIdx[i] = (*pucChkAddr)&0xf;
        pucChkAddr+=1;


        for (j=0; j<numComp; j++)
        {
            if (compID == a_ptHeaderInfo->cInfoTab[j][0])
            {
                a_ptHeaderInfo->cInfoTab[j][4] = dcHufTblIdx[i];
                a_ptHeaderInfo->cInfoTab[j][5] = acHufTblIdx[i];
            }
        }
    }

    if ((a_ptHeaderInfo->streamBufEndAddr - pucChkAddr +1) < ((8+8+4+4)/8))     // next (8+8+4+4)/8 bytes availabe or not
        return -1;

    // Ss 0
    ss = *pucChkAddr++;
    // Se 3F
    se = *pucChkAddr++;
    // Ah 0
    ah = ((*pucChkAddr)&0xf0)>>4;
    // Al 0
    al = (*pucChkAddr)&0xf;
    pucChkAddr+=1;

    if ((ss != 0) || (se != 0x3F) || (ah != 0) || (al != 0))
    {
        //printf("The Jpeg Image must be another profile\r\n");
        return -1;
    }

    a_ptHeaderInfo->checkedByteAddr = pucChkAddr;

    return 1;
}

static void genDecHuffTab(MJDEC_Header_Info *a_ptHeaderInfo, int tabNum)
{
	uint8_t  *huffPtr, *huffBits;
	uint32_t *huffMax, *huffMin;

	int ptrCnt =0;
	int huffCode = 0;
	int zeroFlag = 0;
	int dataFlag = 0;
	int i;

	huffBits	= a_ptHeaderInfo->huffBits[tabNum];
	huffPtr		= a_ptHeaderInfo->huffPtr[tabNum];
	huffMax		= a_ptHeaderInfo->huffMax[tabNum];
	huffMin		= a_ptHeaderInfo->huffMin[tabNum];

	for (i=0; i<16; i++)
	{
		if (huffBits[i]) // if there is bit cnt value
		{
			huffPtr[i] = ptrCnt;
			ptrCnt += huffBits[i];
			huffMin[i] = huffCode;
			huffMax[i] = huffCode + (huffBits[i] - 1);
			dataFlag = 1;
			zeroFlag = 0;
		}
		else
		{
			huffPtr[i] = 0xFF;
			huffMin[i] = 0xFFFF;
			huffMax[i] = 0xFFFF;
			zeroFlag = 1;
		}

		if (dataFlag == 1)
		{
			if (zeroFlag == 1)
				huffCode <<= 1;
			else
				huffCode = (huffMax[i] + 1) << 1;
		}
	}

}


void fill_quantize_table(uint8_t *input_yy, uint8_t *input_uv)
{

    uint8_t i, j;
    uint16_t tmp1;
    uint16_t tmp2;
    unsigned char reverse_oder_yy[64];
    unsigned char reverse_oder_uv[64];

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            reverse_oder_yy[i*8+j] = input_yy[zigzag[i*8+j]];
            reverse_oder_uv[i*8+j] = input_uv[zigzag[i*8+j]];
        }
    }

#if 0
    /*fill q table for encoder => for quantization*/
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            tmp1 = 2048 / reverse_oder_yy[16 * j + i];
            tmp2 = 2048 / reverse_oder_yy[16 * j + i + 8];

            if (20480 / reverse_oder_yy[16 * j + i] % 10 > 4) {
                tmp1++;
            }

            if (20480 / reverse_oder_yy[16 * j + i + 8] % 10 > 4) {
                tmp2++;
            }

            writel(tmp1 | tmp2 << 16, MJPEG_BASE + MJPEG_Q_PARAM_00_OFFSET + (i * 4 + j) * 4);
        }
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            tmp1 = 2048 / reverse_oder_uv[16 * j + i];
            tmp2 = 2048 / reverse_oder_uv[16 * j + i + 8];

            if (20480 / reverse_oder_uv[16 * j + i] % 10 > 4) {
                tmp1++;
            }

            if (20480 / reverse_oder_uv[16 * j + i + 8] % 10 > 4) {
                tmp2++;
            }

            writel(tmp1 | tmp2 << 16, MJPEG_BASE + MJPEG_Q_PARAM_40_OFFSET + (i * 4 + j) * 4);
        }
    }
#else
    /*fill q table for decoder => for dequantization*/
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            tmp1 = reverse_oder_yy[16 * j + i];
            tmp2 = reverse_oder_yy[16 * j + i + 8];

            writel(tmp1 | tmp2 << 16, MJPEG_BASE + MJPEG_Q_PARAM_00_OFFSET + (i * 4 + j) * 4);
        }
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            tmp1 = reverse_oder_uv[16 * j + i];
            tmp2 = reverse_oder_uv[16 * j + i + 8];

            writel(tmp1 | tmp2 << 16, MJPEG_BASE + MJPEG_Q_PARAM_40_OFFSET + (i * 4 + j) * 4);
        }
    }
#endif
}


void fill_user_dth_table(MJDEC_Header_Info *a_ptHeaderInfo)
{
    uint32_t i, idx, cnt;
    uint32_t udWrRegAddr;
    uint32_t udTempValue;

    // DC_ID0 (0x00) -> 0 /*yy dc*/
    // AC_ID0 (0x10) -> 1 /*yy ac*/
    // DC_ID1 (0x01) -> 2 /*uv dc*/
    // AC_ID1 (0x11) -> 3 /*uv ac*/

    /*fill yy & uv DC len & DC value*/
    for(idx=0;idx<2;idx++)
    {
        if(idx==0)
            udWrRegAddr = MJCOMB_CSR_JDEC_YY_DC_CFG0_OFFSET;
        else
            udWrRegAddr = MJCOMB_CSR_JDEC_UV_DC_CFG0_OFFSET;

        /*fill DC bits*/
        cnt = 16/8;    /*use 8 elements per loop*/
        for(i=0;i<cnt; i++)
        {
            udTempValue = ((a_ptHeaderInfo->huffBits[idx*2][i*8+0] ) << (0*4))|
                          ((a_ptHeaderInfo->huffBits[idx*2][i*8+1] ) << (1*4))|
                          ((a_ptHeaderInfo->huffBits[idx*2][i*8+2] ) << (2*4))|
                          ((a_ptHeaderInfo->huffBits[idx*2][i*8+3] ) << (3*4))|
                          ((a_ptHeaderInfo->huffBits[idx*2][i*8+4] ) << (4*4))|
                          ((a_ptHeaderInfo->huffBits[idx*2][i*8+5] ) << (5*4))|
                          ((a_ptHeaderInfo->huffBits[idx*2][i*8+6] ) << (6*4))|
                          ((a_ptHeaderInfo->huffBits[idx*2][i*8+7] ) << (7*4));
            writel(udTempValue, MJPEG_BASE + udWrRegAddr);
            udWrRegAddr +=4;
        }

        /*fill DC value*/
        cnt = 12/8;    /*use 8 elements per loop*/
        for(i=0;i<cnt; i++)
        {
            udTempValue = ((a_ptHeaderInfo->huffVal[idx*2][i*8+0] ) << (0*4))|
                          ((a_ptHeaderInfo->huffVal[idx*2][i*8+1] ) << (1*4))|
                          ((a_ptHeaderInfo->huffVal[idx*2][i*8+2] ) << (2*4))|
                          ((a_ptHeaderInfo->huffVal[idx*2][i*8+3] ) << (3*4))|
                          ((a_ptHeaderInfo->huffVal[idx*2][i*8+4] ) << (4*4))|
                          ((a_ptHeaderInfo->huffVal[idx*2][i*8+5] ) << (5*4))|
                          ((a_ptHeaderInfo->huffVal[idx*2][i*8+6] ) << (6*4))|
                          ((a_ptHeaderInfo->huffVal[idx*2][i*8+7] ) << (7*4));
            writel(udTempValue, MJPEG_BASE + udWrRegAddr);
            udWrRegAddr +=4;
        }

        /*last 4 elements 0x08~0x0b of dc value*/
        udTempValue = ((a_ptHeaderInfo->huffVal[idx*2][i*8+0] ) << (0*4))|
                      ((a_ptHeaderInfo->huffVal[idx*2][i*8+1] ) << (1*4))|
                      ((a_ptHeaderInfo->huffVal[idx*2][i*8+2] ) << (2*4))|
                      ((a_ptHeaderInfo->huffVal[idx*2][i*8+3] ) << (3*4));
        writel(udTempValue, MJPEG_BASE + udWrRegAddr);
    }


    /*fill yy & uv AC len & AC value*/
    for(idx=0;idx<2;idx++)
    {
        if(idx==0)
            udWrRegAddr = MJCOMB_CSR_JDEC_YY_AC_CFG0_OFFSET;
        else
            udWrRegAddr = MJCOMB_CSR_JDEC_UV_AC_CFG0_OFFSET;

        /*fill AC bits*/
        cnt = 16/4;   /*use 4 elements per loop*/
        for(i=0;i<cnt; i++)
        {
            udTempValue = ((a_ptHeaderInfo->huffBits[idx*2+1][i*4+0] ) << (0*8))|
                          ((a_ptHeaderInfo->huffBits[idx*2+1][i*4+1] ) << (1*8))|
                          ((a_ptHeaderInfo->huffBits[idx*2+1][i*4+2] ) << (2*8))|
                          ((a_ptHeaderInfo->huffBits[idx*2+1][i*4+3] ) << (3*8));
            writel(udTempValue, MJPEG_BASE + udWrRegAddr);
            udWrRegAddr +=4;
        }

        /*fill AC value*/
        cnt = 162/4;   /*use 4 elements per loop*/
        for(i=0;i<cnt; i++)
        {
            udTempValue = ((a_ptHeaderInfo->huffVal[idx*2+1][i*4+0] ) << (0*8))|
                          ((a_ptHeaderInfo->huffVal[idx*2+1][i*4+1] ) << (1*8))|
                          ((a_ptHeaderInfo->huffVal[idx*2+1][i*4+2] ) << (2*8))|
                          ((a_ptHeaderInfo->huffVal[idx*2+1][i*4+3] ) << (3*8));
            writel(udTempValue, MJPEG_BASE + udWrRegAddr);
            udWrRegAddr +=4;
        }
        /*last 2 elements 0xa1~0xa2 of dc value*/
        udTempValue = ((a_ptHeaderInfo->huffVal[idx*2+1][i*4+0] ) << (0*8))|
                      ((a_ptHeaderInfo->huffVal[idx*2+1][i*4+1] ) << (1*8));
        writel(udTempValue, MJPEG_BASE + udWrRegAddr);
    }
}

int drv_mjdec_decode_header(uint8_t *a_pucHeaderAddr, int a_udBitStreamSize)
{

    unsigned int code;
    int ret;
    int i;
    int temp;
    //int wrOffset;
    uint8_t *b = a_pucHeaderAddr;
    int size = a_udBitStreamSize;
    uint8_t *pucChkAddr;


    g_tHeaderInfo.streamBufSize      = a_udBitStreamSize;
    g_tHeaderInfo.pBitStream         = a_pucHeaderAddr;
    g_tHeaderInfo.checkedByteAddr    = a_pucHeaderAddr;
    g_tHeaderInfo.streamBufStartAddr = a_pucHeaderAddr;
    g_tHeaderInfo.streamBufEndAddr   = a_pucHeaderAddr + a_udBitStreamSize;
    g_tHeaderInfo.ecsPtr             = 0;
    g_tHeaderInfo.headerBytes        = 0;

    ret = 1;
    //if (jpg->streamWrPtr == jpg->streamBufStartAddr)
    //{
    //    size = jpg->streamBufSize-jpg->frameOffset;
    //    wrOffset = jpg->streamBufSize;
    //}
    //else
    //{
    //    if ((jpg->streamWrPtr-jpg->streamBufStartAddr) < jpg->frameOffset)
    //        size = jpg->streamBufSize - jpg->frameOffset;
    //    else
    //        size = (jpg->streamWrPtr-jpg->streamBufStartAddr)-jpg->frameOffset;
    //    wrOffset = (jpg->streamWrPtr-jpg->streamBufStartAddr);
    //}

    if (!a_udBitStreamSize) {
        ret = -1;
        goto DONE_DEC_HEADER;
    }

    // find start code of next frame
    if (1)
    {
        int nextOffset = 0;
        int soiOffset = 0;

        //if (jpg->consumeByte != 0)    // meaning is frameIdx > 0
        //{
        //    nextOffset = jpg->consumeByte;
        //    if (nextOffset <= 0)
        //        nextOffset = 2;    //in order to consume start code.
        //}

        //consume to find the start code of next frame.
        b += nextOffset;

        if (b > g_tHeaderInfo.streamBufEndAddr)
        {
            ret = -1;
            goto DONE_DEC_HEADER;
        }

        size -= nextOffset;

        if (size < 0)
        {
            ret = -1;
            goto DONE_DEC_HEADER;
        }

        //init_get_bits(&jpg->gbc, b, size*8);
        for (;;)
        {
            code = find_start_soi_code(&g_tHeaderInfo);
            if (code == 0)
            {
                ret = -1;
                goto DONE_DEC_HEADER;
            }

            if (code == SOI_Marker)
                break;

        }

        soiOffset = g_tHeaderInfo.checkedByteAddr - g_tHeaderInfo.streamBufStartAddr;

        b += soiOffset;
        size -= soiOffset;
        //jpg->frameOffset += (soiOffset+ nextOffset);
    }


    //init_get_bits(&jpg->gbc, b, size*8);

    // Initialize component information table
    for (i=0; i<4; i++)
    {
        g_tHeaderInfo.cInfoTab[i][0] = 0;
        g_tHeaderInfo.cInfoTab[i][1] = 0;
        g_tHeaderInfo.cInfoTab[i][2] = 0;
        g_tHeaderInfo.cInfoTab[i][3] = 0;
        g_tHeaderInfo.cInfoTab[i][4] = 0;
        g_tHeaderInfo.cInfoTab[i][5] = 0;
    }

    for (;;)
    {
        if (find_start_code(&g_tHeaderInfo) == 0)
        {
            ret = -1;
            goto DONE_DEC_HEADER;
        }

        pucChkAddr = g_tHeaderInfo.checkedByteAddr;
        g_tHeaderInfo.checkedByteAddr += 2;

        code = ((*(pucChkAddr+0))<<8) |
               ((*(pucChkAddr+1))<<0);

        switch (code) {
        case SOI_Marker:
            break;
        case JFIF_CODE:
        case EXIF_CODE:
            if (!decode_app_header(&g_tHeaderInfo))
            {
                ret = -1;
                goto DONE_DEC_HEADER;
            }
            break;
        case DRI_Marker:
            if (!decode_dri_header(&g_tHeaderInfo))
            {
                ret = -1;
                goto DONE_DEC_HEADER;
            }
            break;
        case DQT_Marker:
            if (!decode_dqt_header(&g_tHeaderInfo))
            {
                ret = -1;
                goto DONE_DEC_HEADER;
            }
            break;
        case DHT_Marker:
            if (!decode_dth_header(&g_tHeaderInfo))
            {
                ret = -1;
                goto DONE_DEC_HEADER;
            }
            break;
        case SOF_Marker:
            if (!decode_sof_header(&g_tHeaderInfo))
            {
                ret = -1;
                goto DONE_DEC_HEADER;
            }
            break;
        case SOS_Marker:
            if (!decode_sos_header(&g_tHeaderInfo))
            {
                ret = -1;
                goto DONE_DEC_HEADER;
            }
            goto DONE_DEC_HEADER;
//            break; //remove warning 64: Unreachable statement.
        case EOI_Marker:
            goto DONE_DEC_HEADER;
        default:
            switch (code&0xFFF0)
            {
            case 0xFFE0:    // 0xFFEX
            case 0xFFF0:    // 0xFFFX
                if (g_tHeaderInfo.streamBufEndAddr <= g_tHeaderInfo.checkedByteAddr )
                {
                    {
                        ret = -1;
                        goto DONE_DEC_HEADER;
                    }
                }
                else
                {
                    if (!decode_app_header(&g_tHeaderInfo))
                    {
                        ret = -1;
                        goto DONE_DEC_HEADER;
                    }
                    break;
                }
            default:
                //printf("code = [%x]\r\n", code);
                return    0;
            }
            break;
        }
    }

DONE_DEC_HEADER:

    //if (ret == -1)
    //{
    //    if (wrOffset < jpg->frameOffset)
    //        return -2;
    //
    //    return -1;
    //}

    if (!g_tHeaderInfo.ecsPtr)  /*not find ecsPtr*/
        return 0;


    //if (wrOffset - (jpg->frameOffset+jpg->ecsPtr)  < JPU_GBU_SIZE) {
        //return -1;
    //}


    // Generate Huffman table information
    for(i=0; i<4; i++)
        genDecHuffTab(&g_tHeaderInfo, i);

    // Q Idx
    temp =             g_tHeaderInfo.cInfoTab[0][3];
    temp = temp << 1 | g_tHeaderInfo.cInfoTab[1][3];
    temp = temp << 1 | g_tHeaderInfo.cInfoTab[2][3];
    g_tHeaderInfo.Qidx = temp;


    // Huff Idx[DC, AC]
    temp =             g_tHeaderInfo.cInfoTab[0][4];
    temp = temp << 1 | g_tHeaderInfo.cInfoTab[1][4];
    temp = temp << 1 | g_tHeaderInfo.cInfoTab[2][4];
    g_tHeaderInfo.huffDcIdx = temp;

    temp =             g_tHeaderInfo.cInfoTab[0][5];
    temp = temp << 1 | g_tHeaderInfo.cInfoTab[1][5];
    temp = temp << 1 | g_tHeaderInfo.cInfoTab[2][5];
    g_tHeaderInfo.huffAcIdx = temp;


    switch(g_tHeaderInfo.format)
    {
    case FORMAT_420:
        g_tHeaderInfo.busReqNum = 2;
        g_tHeaderInfo.mcuBlockNum = 6;
        g_tHeaderInfo.compNum = 3;
        g_tHeaderInfo.compInfo[0] = 10;
        g_tHeaderInfo.compInfo[1] = 5;
        g_tHeaderInfo.compInfo[2] = 5;
        g_tHeaderInfo.alignedWidth = ((g_tHeaderInfo.picWidth+15)&~15);
        g_tHeaderInfo.alignedHeight = ((g_tHeaderInfo.picHeight+15)&~15);
        break;
    case FORMAT_422:
        g_tHeaderInfo.busReqNum = 3;
        g_tHeaderInfo.mcuBlockNum = 4;
        g_tHeaderInfo.compNum = 3;
        g_tHeaderInfo.compInfo[0] = 9;
        g_tHeaderInfo.compInfo[1] = 5;
        g_tHeaderInfo.compInfo[2] = 5;
        g_tHeaderInfo.alignedWidth = ((g_tHeaderInfo.picWidth+15)&~15);
        g_tHeaderInfo.alignedHeight = ((g_tHeaderInfo.picHeight+7)&~7);
        break;
    case FORMAT_224:
        g_tHeaderInfo.busReqNum = 3;
        g_tHeaderInfo.mcuBlockNum = 4;
        g_tHeaderInfo.compNum = 3;
        g_tHeaderInfo.compInfo[0] = 6;
        g_tHeaderInfo.compInfo[1] = 5;
        g_tHeaderInfo.compInfo[2] = 5;
        g_tHeaderInfo.alignedWidth = ((g_tHeaderInfo.picWidth+7)&~7);
        g_tHeaderInfo.alignedHeight = ((g_tHeaderInfo.picHeight+15)&~15);
        break;
    case FORMAT_444:
        g_tHeaderInfo.busReqNum = 4;
        g_tHeaderInfo.mcuBlockNum = 3;
        g_tHeaderInfo.compNum = 3;
        g_tHeaderInfo.compInfo[0] = 5;
        g_tHeaderInfo.compInfo[1] = 5;
        g_tHeaderInfo.compInfo[2] = 5;
        g_tHeaderInfo.alignedWidth = ((g_tHeaderInfo.picWidth+7)&~7);
        g_tHeaderInfo.alignedHeight = ((g_tHeaderInfo.picHeight+7)&~7);
        break;
    case FORMAT_400:
        g_tHeaderInfo.busReqNum = 4;
        g_tHeaderInfo.mcuBlockNum = 1;
        g_tHeaderInfo.compNum = 1;
        g_tHeaderInfo.compInfo[0] = 5;
        g_tHeaderInfo.compInfo[1] = 0;
        g_tHeaderInfo.compInfo[2] = 0;
        g_tHeaderInfo.alignedWidth = ((g_tHeaderInfo.picWidth+7)&~7);
        g_tHeaderInfo.alignedHeight = ((g_tHeaderInfo.picHeight+7)&~7);
        break;
    }

    return ret;
}


/****************************************************************************//**
 * @brief  Mjdec module init
 *
 * @param  cfg: Mjdec configuration structure pointer
 *
 * @return None
 *
*******************************************************************************/
int drv_mjdec_init(MJDEC_CFG_Type *cfg)
{
    uint32_t tmpVal;
    int      ret=0;
    //uint32_t q;


    if(g_nMJComboHWState != MJCOMBO_HW_STATE_UNINIT)
    {
        /*for JPEG combo HW not support using encode and decode at the same time*/
        if(!IS_MJCOMBO_HW_STATE_FOR_DEC(g_nMJComboHWState))
            return -1;
    }
    g_nMJComboHWState = MJCOMBO_HW_STATE_DEC_INIT;

    /* disable mjdec */
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_MJCOMB_MODE);   // MJPEG combo mode. 0: encoder, 1:decoder
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_MJCOMB_ENABLE); // disable decode
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);

    /*parse JPEG header*/
    if(cfg->headSkip == 1)
    {
        ret=drv_mjdec_decode_header((uint8_t *)cfg->bufferStream, (cfg->headByte == 0)? MAX_MJDEC_HEADER_BYTES:cfg->headByte);

        if( (g_tHeaderInfo.picWidth  != cfg->resolutionX) ||
            (g_tHeaderInfo.picHeight != cfg->resolutionY)   )
        {
            printf("[mjdec][ERR] Width/Height setup mis-match from JPEG header info!\r\n");
            ret = -1;
        }

        if(ret<0)
        {
            g_nMJComboHWState = MJCOMBO_HW_STATE_UNINIT;
            return ret;
        }
    }

    /*set Q table*/
    fill_quantize_table(&g_tHeaderInfo.qMatTab[0][0], &g_tHeaderInfo.qMatTab[1][0]);

    /*set User Huffman table*/
    if(g_tHeaderInfo.userHuffTab)
    {
        fill_user_dth_table(&g_tHeaderInfo);

        tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_2_OFFSET);
        tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_DEC_HUF_CAL_PULSE);   // calc. Huffman table
        writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_2_OFFSET);

        /*todo: wait  sts_dec_huf_cal_done*/
        //while( !(readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_2_OFFSET )&MJCOMB_CSR_STS_DEC_HUF_CAL_DONE_MSK));
    }


     //if(cfg->picQuality < 1) {
    //    q = 1;
    //} else if(cfg->picQuality > 75) {
    //    q = 100;
    //} else {
    //    q = cfg->picQuality;
    //}

    /* basic configure */
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_YUV_MODE,          cfg->yuv);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_W_XLEN,            cfg->burstWrite);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_ORDER_U_EVEN,      cfg->evenOrderEnable);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_LAST_HF_HBLK_DMY,  cfg->dmyVertical);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_LAST_HF_WBLK_DMY,  cfg->dmyHorizational);
    //tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_SWAP_MODE,         cfg->swapMode);   /*N/A for JPEG combo HW*/
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_0_OFFSET);
    //HW removed. //tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_DEC_LAST_HF_BLK_DMY,   cfg->dmyBlock);
    tmpVal = reg_set_bits(tmpVal, MJCOMB_CSR_REG_DEC_R_XLEN,            cfg->burstRead);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_0_OFFSET);


    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT, cfg->intCnt);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET);

    #if 0
        /* align buffer to 16 bytes boundary */
        writel((cfg->bufferFrameYY + 0xF) & (~0xF), MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_1_OFFSET);
        writel((cfg->bufferFrameUV + 0xF) & (~0xF), MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_2_OFFSET);
    #else
        /* align buffer to 8 bytes boundary */
        writel((cfg->bufferFrameYY + 0x7) & (~0x7), MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_1_OFFSET);
        writel((cfg->bufferFrameUV + 0x7) & (~0x7), MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_2_OFFSET);
    #endif
    
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_3_OFFSET);
    switch(cfg->yuv)
    {
        case MJDEC_YUV422_SP:
            tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_FRAME_WBLK, (cfg->resolutionX+15)>>4);
            tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_FRAME_HBLK, (cfg->resolutionY+ 7)>>3);
            break;
        case MJDEC_YUV420_SP:
            tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_FRAME_WBLK, (cfg->resolutionX+15)>>4);
            tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_FRAME_HBLK, (cfg->resolutionY+15)>>4);
            break;
        case MJDEC_YUV400:
            tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_FRAME_WBLK, (cfg->resolutionX+ 7)>>3);
            tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_FRAME_HBLK, (cfg->resolutionY+ 7)>>3);
            break;
        default:
            break;
    }
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_3_OFFSET);


    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_5_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_DEC_HDER_SKIP,  cfg->headSkip);
    tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_HEAD_BYTE,      g_tHeaderInfo.headerBytes);    /*use decoder header info*/
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_5_OFFSET);

    /* Clear interrupt */
    writel(MJCOMB_CSR_REG_DEC_INT_CLR_MSK, MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_2_OFFSET);

#if 0   /*not use ==> use hal driver to handle IRQ*/
    Interrupt_Handler_Register(MJDEC_IRQn, MJDEC_IRQHandler);
#endif

    return 0;
}

/****************************************************************************//**
 * @brief  Deinit mjdec module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
int drv_mjdec_deinit(void)
{
    int ret = 0;

    //GLB_AHB_DSP_Software_Reset(GLB_AHB_DSP_SW_SWRST_MJPEG_DEC);

    if(g_nMJComboHWState != MJCOMBO_HW_STATE_UNINIT)
    {
        if(IS_MJCOMBO_HW_STATE_FOR_DEC(g_nMJComboHWState))
            g_nMJComboHWState = MJCOMBO_HW_STATE_UNINIT;
        else
            ret = -1;   /*Error! Current MJCOMBO_HW_STATE is for encode.*/
    }

    return ret;

}

/****************************************************************************//**
 * @brief  Enable mjdec module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void drv_mjdec_enable(void)
{
    uint32_t tmpVal;

    /* Enable mjdec module */
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_MJCOMB_ENABLE); // enable decode
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);

}

/****************************************************************************//**
 * @brief  Disable mjdec module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void drv_mjdec_disable(void)
{
    uint32_t tmpVal;

    /* Disable mjdec module */
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_MJCOMB_ENABLE); // enable decode
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_0_OFFSET);
}

/****************************************************************************/ /**
 * @brief  Get one mjdec frame
 *
 * @param  info: Mjdec frame infomation pointer
 *
 * @return None
 *
*******************************************************************************/
void drv_mjdec_get_frame_info(MJDEC_Frame_Info *info)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET);
    info->validFrames  = reg_get_bits(tmpVal, MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT);
    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_4_OFFSET);
    info->curFrameAddr = reg_get_bits(tmpVal, MJCOMB_CSR_STS_DEC_JP_ADDR_0);

}

/****************************************************************************//**
 * @brief  Get available count of frames
 *
 * @param  None
 *
 * @return Frames count
 *
*******************************************************************************/
uint8_t drv_mjdec_get_frame_count(void)
{
    return (uint8_t)reg_get_bits(readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET), MJCOMB_CSR_STS_ENC_FRAME_VALID_CNT);
}

/****************************************************************************/ /**
 * @brief  Push one jpeg picture frame
 *
 * @param  bufferMjpeg: jpeg picture frame buffer pointer
 *
 * @return None
 *
*******************************************************************************/
Err_Type drv_mjdec_push_frame(uint32_t bufferJpeg)
{
    /* because of AXI 64bits, bufferJpeg address bit [0:2] must be 0 */
    if((bufferJpeg & 0x07) != 0)
    {
        return INVALID;
    }
    writel(bufferJpeg | MJCOMB_CSR_REG_DEC_JP_PUSH_MSK, MJPEG_BASE + MJCOMB_CSR_SHARE_CSR_4_OFFSET);
    return SUCCESS;
}

/****************************************************************************//**
 * @brief  Pop one mjdec frame
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void drv_mjdec_pop_frame(void)
{
    writel(MJCOMB_CSR_REG_DEC_YUV_FRAME_POP_MSK, MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_2_OFFSET);
}

/****************************************************************************//**
 * @brief  Set frame threshold to issue normal interrupt
 *
 * @param  count: Frame threshold
 *
 * @return None
 *
*******************************************************************************/
void drv_mjdec_set_frame_threshold(uint8_t count)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MJCOMB_CSR_REG_DEC_FRAME_CNT_TRGR_INT, count);
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET);

}

/****************************************************************************//**
 * @brief  Set Q value
 *
 * @param  q: Q value, must be in 1-75 or 100
 *
 * @return None
 *
*******************************************************************************/
void drv_mjdec_set_q_value(uint8_t q)
{
    /*N/A for JPEG combo HW*/
    //uint32_t tmpVal;
    //
    //if(q < 1) {
    //    q = 1;
    //} else if(q > 75) {
    //    q = 100;
    //}
    //tmpVal = readl(MJDEC_BASE, MJDEC_JDEC_CONTROL_1);
    //tmpVal = reg_set_bits(tmpVal, MJDEC_REG_Q_MODE, q);
    //writel(MJDEC_BASE, MJDEC_JDEC_CONTROL_1, tmpVal);
}

/****************************************************************************//**
 * @brief  MJDEC Enable Disable Interrupt
 *
 * @param  intType: MJDEC Interrupt Type
 * @param  intMask: Enable or Disable
 *
 * @return None
 *
*******************************************************************************/
void drv_mjdec_int_mask(MJDEC_INT_Type intType, Mask_Type intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_MJDEC_INT_TYPE(intType));
    CHECK_PARAM(IS_MASK_TYPE(intMask));

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET);
    switch(intType)
    {
        case MJDEC_INT_NORMAL:
            if(intMask == UNMASK){
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_NORMAL_EN);
            }else{
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_NORMAL_EN);
            }
            break;

        case MJDEC_INT_BACK_IDLE:
            if(intMask == UNMASK){
                /* Enable this interrupt */
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_IDLE_EN);
            }else{
                /* Disable this interrupt */
                tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_IDLE_EN);
            }
            break;

        case MJDEC_INT_ALL:
            if(intMask == UNMASK){
                /* Enable all interrupt */
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_NORMAL_EN);
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_IDLE_EN);
            }else{
                /* Disable all interrupt */
                tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_NORMAL_EN);
                tmpVal = reg_clr_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_IDLE_EN);
            }
            break;

        default:
            break;
    }
    writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET);
}

/****************************************************************************//**
 * @brief  MJDEC Interrupt Clear
 *
 * @param  intType: MJDEC Interrupt Type
 *
 * @return None
 *
*******************************************************************************/
void drv_mjdec_int_clr(MJDEC_INT_Type intType)
{
    #if 0
        uint32_t tmpVal;

        CHECK_PARAM(IS_MJDEC_INT_TYPE(intType));

        tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_2_OFFSET);
        switch(intType)
        {
            case MJDEC_INT_NORMAL:
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_CLR);
                break;
            case MJDEC_INT_BACK_IDLE:
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_CLR);
                break;
            case MJDEC_INT_ALL:
                tmpVal = reg_set_bit(tmpVal, MJCOMB_CSR_REG_DEC_INT_CLR);
            default:
                break;
        }
        writel(tmpVal, MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_2_OFFSET);
    #else
        writel(MJCOMB_CSR_REG_DEC_INT_CLR_MSK, MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_2_OFFSET);
    #endif
}

/****************************************************************************//**
 * @brief  Install mjdec interrupt callback function
 *
 * @param  intType: MJDEC interrupt type
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return None
 *
*******************************************************************************/
#if 0   /*not use ==> use hal driver to handle IRQ*/
void drv_mjdec_int_callback_install(MJDEC_INT_Type intType,intCallback_Type* cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_MJDEC_INT_TYPE(intType));

    mjdecIntCbfArra[intType] = cbFun;
}
#endif

/****************************************************************************//**
 * @brief  Mjdec interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#if 0   /*not use ==> use hal driver to handle IRQ*/
void drv_mjdec_irqhandler(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET);      /*get INT status*/
    drv_mjdec_int_clr(MJDEC_INT_ALL);                               /*clear pending INT status*/

    if( reg_get_bits(tmpVal, MJCOMB_CSR_STS_DEC_NORMAL_INT) ){
        if(mjdecIntCbfArra[MJDEC_INT_NORMAL] != NULL) {
            /* call the callback function */
            mjdecIntCbfArra[MJDEC_INT_NORMAL]();
        }
    }
    if( reg_get_bits(tmpVal, MJCOMB_CSR_STS_DEC_IDLE_INT) ){
        if(mjdecIntCbfArra[MJDEC_INT_BACK_IDLE] != NULL) {
            /* call the callback function */
            mjdecIntCbfArra[MJDEC_INT_BACK_IDLE]();
        }
    }
}
#endif

/****************************************************************************//**
 * @brief  Mjdec get interrupt status
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint32_t drv_mjdec_get_intstatus(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MJPEG_BASE + MJCOMB_CSR_JDEC_CSR_1_OFFSET);
    tmpVal &= 0x0030;

    return tmpVal;
}

/*@} end of group MJDEC_Public_Functions */

/*@} end of group MJDEC */

/*@} end of group IPRO7_Peripheral_Driver */
