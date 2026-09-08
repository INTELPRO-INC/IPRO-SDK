/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#ifndef __ATBM_ETF_H__
#define __ATBM_ETF_H__


//keep it off under normal conditions
#define CONFIG_ATBM_PRODUCT_TEST_USE_GOLDEN_LED (1)//atbm test not use uart

#define SIGMASTAR_PRODUCT_TEST_USE_FEATURE_ID		(1)


enum ATBM_TEST_TYPE{
	TXRX_TEST_REQ,
	TXRX_TEST_RSP,
	TXRX_TEST_RESULT,
};

enum ATBM_TEST_RESULT{
	TXRX_TEST_NONE,
	TXRX_TEST_PASS,
	TXRX_TEST_FAIL,
};


struct ATBM_TEST_IE {
	atbm_uint8 ie_id;//D11_WIFI_ELT_ID
	atbm_uint8 len;
	atbm_uint8 oui[3]; //ATBM_OUI
	atbm_uint8 oui_type; //WIFI_ATBM_IE_OUI_TYPE
	atbm_uint8 test_type;
	atbm_uint8 resverd;	
	atbm_uint32 featureid;
	int result[16];
}__attribute__ ((packed));

struct test_threshold{
	int freq_ppm;
	int txevm;//txevm filter
	int rxevm;//send to lmac,rxevm filter
	int txevmthreshold;//test threshold
	int rxevmthreshold;//test threshold
	int txpwrmax;
	int txpwrmin;
	int rxpwrmax;
	int rxpwrmin;
	atbm_uint32 featureid;
	int rssifilter;
	int cableloss;
};

/* OUI's */
#define WIFI_OUI			{0x00, 0x50, 0xF2}
#define WFA_OUI			 {0x50, 0x6F, 0x9A}
#define ATBM_OUI			{0x00, 0xAA, 0xBB}
#define WIFI_WPA_OUI_TYPE	       0x01
#define WIFI_WME_OUI_TYPE	       0x02
#define WIFI_WPS_OUI_TYPE	       0x04
#define WIFI_P2P_IE_OUI_TYPE	    0x09
#define WIFI_ATBM_IE_OUI_TYPE	    0x0a

#define D11_WIFI_ELT_ID		0xDD 

//DCXO register address
#define DCXO_TRIM_REG 0x16101410 //bit 5:0

/*
0_0000000 – 0x00	BPSK modulation, coding rate 1/2
0_0000001 – 0x01	QPSK modulation, coding rate 1/2
0_0000010 – 0x02	QPSK modulation, coding rate 3/4
0_0000011 – 0x03	16QAM modulation, coding rate 1/2
0_0000100 – 0x04	16QAM modulation, coding rate 3/4
0_0000101 – 0x05	64QAM modulation, coding rate 2/3
0_0000110 – 0x06	64QAM modulation, coding rate 3/4
0_0000111 – 0x07	64QAM modulation, coding rate 5/6
0_0001000 – 0x08	256QAM modulation, coding rate 3/4
0_0001001 – 0x09	256QAM modulation, coding rate 5/6
0_0001010 – 0x0a	1024QAM modulation, coding rate 3/4
0_0001011 – 0x0b	1024QAM modulation, coding rate 5/6

1001: 802.11ax HE MU PPDU
1000: 802.11ax HE TB PPDU
0111: 802.11ax HE ER SU PPDU
0110: 802.11ax HE SU PPDU
0101: 802.11n OFDM long (mixed-mode) preamble
0100: 802.11n OFDM short (greenfield-mode) preamble
0010: 802.11a OFDM preamble
0001: 802.11b DSSS long preamble
0000: 802.11b DSSS short preamble (note this is not supported for 

*/
typedef enum
{
    // 802.11 b
    _1Mbps_DSSS_                = 0x00,
    _2Mbps_DSSS_                = 0x01,
    _5_5Mbps_CCK_               = 0x02,
    _11Mbps_CCK_                = 0x03,

    // 802.11a Legacy modes
    _6Mbps_BPSK_Code1_2_        = 0x0B,
    _9Mbps_BPSK_Code3_4_        = 0x0F,
    _12Mbps_QPSK_Code1_2_       = 0x0A,
    _18Mbps_QPSK_Code3_4_       = 0x0E,
    _24Mbps_16QAM_Code1_2_      = 0x09,
    _36Mbps_16QAM_Code3_4_      = 0x0D,
    _48Mbps_64QAM_Code2_3_      = 0x08,
    _54Mbps_64QAM_Code3_4_      = 0x0C,

    // 802.11n Mixed/Greenfield Mode
    _6_5Mbps_BPSK_Code_1_2_     = 0x00,
    _13_5Mbps_QPSK_Code_1_2_    = 0x01,
    _19_5Mbps_QPSK_Code_3_4_    = 0x02,
    _26_Mbps_16QAM_Code_1_2_    = 0x03,
    _39_Mbps_16QAM_Code_3_4_    = 0x04,
    _52_Mbps_64QAM_Code_2_3_    = 0x05,
    _58_5Mbps_64QAM_Code_3_4_   = 0x06,
    _65_Mbps_64QAM_Code_5_6_    = 0x07,

	_xx_Mbps_256QAM_Code_3_4_ = 0x08,
	_xx_Mbps_256QAM_Code_5_6_ = 0x09,
	_xx_Mbps_1024QAM_Code_3_4_ = 0x0a,
	_xx_Mbps_1024QAM_Code_5_6_ = 0x0b,
	
	_6Mbps_MCS32_BPSK_Code1_2_	= 0x20

}WLAN_RATE_T;

typedef enum
{
    ATBM_WIFI_MODE_OFDM = 0,
    ATBM_WIFI_MODE_DSSS,

    ATBM_WIFI_MODE_MAX
}ATBMWiFiMode_e;


typedef enum
{
    ATBM_WIFI_BW_20M = 0,
    ATBM_WIFI_BW_40M,
    ATBM_WIFI_BW_80M,
    ATBM_WIFI_BW_20U,
    ATBM_WIFI_BW_20L,
    
    ATBM_WIFI_BW_RU242 = 2,
    ATBM_WIFI_BW_RU106 = 3,
    
    ATBM_WIFI_BW_MAX
}ATBMWiFiBandWidth_e;

typedef enum
{
    ATBM_WIFI_CH_OFFSET_0 = 0,
    ATBM_WIFI_CH_OFFSET_10U,
    ATBM_WIFI_CH_OFFSET_10L,
    
    ATBM_WIFI_CH_OFFSET_MAX
}ATBMWiFiChanOff_e;


typedef enum
{
    ATBM_WIFI_OFDM_MD_LM = 0,
    ATBM_WIFI_OFDM_MD_MM = 1,
    ATBM_WIFI_OFDM_MD_GF = 200,//not support in Cronus
    ATBM_WIFI_OFDM_MD_HE_SU = 2,
    ATBM_WIFI_OFDM_MD_HE_ER_SU = 3,
    ATBM_WIFI_OFDM_MD_HE_TB = 4,
    ATBM_WIFI_OFDM_MD_HE_MU = 5,
    ATBM_WIFI_OFDM_MD_VHT_SU = 6,
    ATBM_WIFI_OFDM_MD_VHT_MU = 7,
    ATBM_WIFI_OFDM_MD_VHT,
    ATBM_WIFI_OFDM_MD_MAX
}ATBMWiFiOFDMMode_e;

#define OFDM_MD_IS_HE(_OFDM_Mode) ((ATBM_WIFI_OFDM_MD_HE_SU == _OFDM_Mode)||(ATBM_WIFI_OFDM_MD_HE_ER_SU == _OFDM_Mode)||(ATBM_WIFI_OFDM_MD_HE_TB == _OFDM_Mode)||(ATBM_WIFI_OFDM_MD_HE_MU == _OFDM_Mode))
#define OFDM_MD_IS_VHT(_OFDM_Mode) ((ATBM_WIFI_OFDM_MD_VHT == _OFDM_Mode)||(ATBM_WIFI_OFDM_MD_VHT_SU == _OFDM_Mode)||(ATBM_WIFI_OFDM_MD_VHT_MU == _OFDM_Mode))

typedef enum
{
	/*DSSS*/
	ATBM_WIFI_RATE_1M = 0,
	ATBM_WIFI_RATE_2M= 0x01,
	ATBM_WIFI_RATE_5D5M = 0x02,
	ATBM_WIFI_RATE_11M= 0x03,

	/*OFDM: LM*/
/*
	ATBM_WIFI_RATE_6M = 6,//0x0B,
	ATBM_WIFI_RATE_9M = 9,//0x0F,
	ATBM_WIFI_RATE_12M= 12,//0x0A,
	ATBM_WIFI_RATE_18M= 18,//0x0E,
	ATBM_WIFI_RATE_24M= 24,//0x09,
	ATBM_WIFI_RATE_36M= 36,//0x0D,
	ATBM_WIFI_RATE_48M= 48,//0x08,
	ATBM_WIFI_RATE_54M= 54,//0x0C,
*/

	//OFDM: LM
	ATBM_WIFI_RATE_6M = 0,
	ATBM_WIFI_RATE_9M,
	ATBM_WIFI_RATE_12M,
	ATBM_WIFI_RATE_18M,
	ATBM_WIFI_RATE_24M,
	ATBM_WIFI_RATE_36M,
	ATBM_WIFI_RATE_48M,
	ATBM_WIFI_RATE_54M,
	
	/*OFDM: MM/GF */
	ATBM_WIFI_RATE_MCS0 = 0x00,
	ATBM_WIFI_RATE_MCS1 = 0x01,
	ATBM_WIFI_RATE_MCS2 = 0x02,
	ATBM_WIFI_RATE_MCS3 = 0x03,
	ATBM_WIFI_RATE_MCS4 = 0x04,
	ATBM_WIFI_RATE_MCS5 = 0x05,
	ATBM_WIFI_RATE_MCS6 = 0x06,
	ATBM_WIFI_RATE_MCS7 = 0x07,

/* Transmit Mode – 802.11ax HE Mode
0_0000000 – 0x00  BPSK modulation, coding rate 1/2        MCS0
0_0000001 – 0x01  QPSK modulation, coding rate 1/2        MCS1
0_0000010 – 0x02  QPSK modulation, coding rate 3/4        MCS2
0_0000011 – 0x03  16QAM modulation, coding rate 1/2       MCS3
0_0000100 – 0x04  16QAM modulation, coding rate 3/4       MCS4
0_0000101 – 0x05  64QAM modulation, coding rate 2/3       MCS5
0_0000110 – 0x06  64QAM modulation, coding rate 3/4       MCS6
0_0000111 – 0x07  64QAM modulation, coding rate 5/6       MCS7
0_0001000 – 0x08  256QAM modulation, coding rate 3/4      MCS8
0_0001001 – 0x09  256QAM modulation, coding rate 5/6      MCS9
0_0001010 – 0x0a  1024QAM modulation, coding rate 3/4     MCS10
0_0001011 – 0x0b  1024QAM modulation, coding rate 5/6     MCS11

*/
	ATBM_WIFI_RATE_MCS8 = 0x08,
	ATBM_WIFI_RATE_MCS9 = 0x09,
	ATBM_WIFI_RATE_MCS10 = 0x0a,
	ATBM_WIFI_RATE_MCS11 = 0x0b,
	
	ATBM_WIFI_RATE_MCS32 = 0x20,

    
	ATBM_WIFI_RATE_MAX = 0xff,
}ATBMWiFiRate_e;
/*
typedef enum
{
	//DSSS
	ATBM_WIFI_RATE_1M = 0,
	ATBM_WIFI_RATE_2M,
	ATBM_WIFI_RATE_5D5M,
    ATBM_WIFI_RATE_11M,

	//OFDM: LM
	ATBM_WIFI_RATE_6M = 0,
	ATBM_WIFI_RATE_9M,
	ATBM_WIFI_RATE_12M,
	ATBM_WIFI_RATE_18M,
	ATBM_WIFI_RATE_24M,
	ATBM_WIFI_RATE_36M,
	ATBM_WIFI_RATE_48M,
	ATBM_WIFI_RATE_54M,

	//OFDM: MM/GF 
	ATBM_WIFI_RATE_MCS0 = 0,
	ATBM_WIFI_RATE_MCS1,
	ATBM_WIFI_RATE_MCS2,
	ATBM_WIFI_RATE_MCS3,
	ATBM_WIFI_RATE_MCS4,
	ATBM_WIFI_RATE_MCS5,
	ATBM_WIFI_RATE_MCS6,
	ATBM_WIFI_RATE_MCS7,
	ATBM_WIFI_RATE_MCS32,
  
    ATBM_WIFI_RATE_MAX
}ATBMWiFiRate_e;
*/

typedef enum
{
    ATBM_WIFI_GI_MD_NORMAL = 0,
    ATBM_WIFI_GI_MD_SHORT,

    ATBM_WIFI_GILTF_0P8_1X = 0,
    ATBM_WIFI_GILTF_1P6_1X,
    ATBM_WIFI_GILTF_0P8_2X,
    ATBM_WIFI_GILTF_1P6_2X,
    ATBM_WIFI_GILTF_0P8_4X,
    ATBM_WIFI_GILTF_3P2_4X,
    
    ATBM_WIFI_GI_MD_MAX
}ATBMWiFiGIMode_e;

typedef enum
{
    ATBM_WIFI_PREAMBLE_LONG = 0,
    ATBM_WIFI_PREAMBLE_SHORT,
    
    ATBM_WIFI_PREAMBLE_MAX
}ATBMWiFiPreamble_e;

typedef enum
{
    ATBM_WIFI_TX_MODE_SINGLE = 0,
    ATBM_WIFI_TX_MODE_CONTINUE,
    
    ATBM_WIFI_TX_MODE_MAX
}ATBMWiFiTxMode_e;

/*parameters for PHY TX on UI*/
typedef struct
{
    unsigned int FreqMHz;
    unsigned int ChannelNum; /* 1~14 for standard channel, -1 means nonstandard channel and FreqMHz indicates frequency of the channel */
    unsigned int WiFiMode;
	unsigned int OFDMMode;
    unsigned int BW;    
    unsigned int ChOffset;
    unsigned int Rate;
    unsigned int GIMode;
	unsigned int PreambleMode;


	unsigned int PSDULen; /*PSDU length in byte*/
	unsigned int PacketInterval; /*packet interval in us*/
	unsigned int DigitalScaler;

	unsigned int PacketNum; /*packet number*/
	unsigned int DataRateMbps;
	
    unsigned int TxMode;
    unsigned int InfiniteLongPacket;//true: INF=1, send only one packet which is infinite long
    
    /*802.11ax params*/
    unsigned int MPDUNum;//the MPDU number of one AMPDU
    unsigned int MPDULen;//the length of MPDU, be multiple of 4

    
    unsigned int Smoothing;//
    unsigned int Sounding;//
    unsigned int Aggregation;//This field is used to indicate if the PSDU contains an A-MPDU
    unsigned int STBC;//0 or 1
    unsigned int LTFNum;// 1 LTF symbols; 2 LTF symbols; 4 LTF symbols; 6 LTF symbols; 8 LTF symbols;
    unsigned int BeamFormed;//
    unsigned int Doppler;//0 or 1
	unsigned int BurstLen; 
	unsigned int TxopDuration;
    unsigned int NoSigExtn;
    unsigned int ServiceField;//802.11b, tx vector0 [7:0]. This field specifies the service information transmitted in the frame header. 


    unsigned int TxPower;
    unsigned int TxStreams;
    unsigned int TxAntennas;//0: 1TX; 1:2TX;
    //unsigned int Vector1TxMode; //This field is used to indicate which of the supported transmit modes is selected and which preamble to use.
                                //depending on "WiFiMode" and "OFDMMode" 
    unsigned int TxAbsPower;
    unsigned int TxPowerModeSel;


    unsigned int StartingStsNum;//The sum of the number of space-time streams assigned to other users with lower space-time stream indices than this user.
    unsigned int HELTFMode;//0: HE single stream pilot HE-LTF mode; 1: HE masked HE-LTF sequence mode
    unsigned int HESigA2Reserved;//Reserved field setting for HE-SIG-A2 of HE TB.
    unsigned int SpatialReuse1,SpatialReuse2,SpatialReuse3,SpatialReuse4;

    unsigned int TriggerResponding;
    unsigned int TriggerMethod;
    unsigned int BSSColor;
    unsigned int UplinkFlag;
    unsigned int ScramblerValueEn;//Scrambler Initial Value enable
    unsigned int ScramblerValue;//Scrambler Initial Value
    unsigned int LdpcExtrSysm;//Indicates the presence of the LDPC extra symbol segment in an HE TB PPDU(0: not present ; 1:  present)

    unsigned int ReservedForMAC;//Used for MAC rate down algorithms. Another setting of Control4[15:0]

    unsigned int  CFO;//This field is a measure of the carrier frequency offset estimated during the reception of the frame. The format is signed S(13,20)
    unsigned int  PPM;//This field is a measure of the ppm observed at the antenna during the reception of the current data frame. The format is signed S(14,25)

    
    unsigned int DCM;//Indicates whether Dual Carrier Modulation is used for the Data field in HE PPDU
    unsigned int Coding;//OFDM coding (0: Convolutional coding, BCC?    1: Advanced (LDPC) coding)
    unsigned int Padding;//NomPacketPadding, 0: 0us, 1: 8us,2: 16us
/* AFactor 
HE-SIG-A2 subfield of HE-SIG-A field, B13–B14, Pre-FEC Padding Factor
Set to 0 to indicate a pre-FEC padding factor of 4.
Set to 1 to indicate a pre-FEC padding factor of 1.
Set to 2 to indicate a pre-FEC padding factor of 2.
Set to 3 to indicate a pre-FEC padding factor of 3.
*/
    unsigned int AFactor;//A-factor or pre-FEC padding factor of 802.11ax trigger-based PPDU
    //unsigned int PacketExtDura;//Packet Extension Duration, 0us,4us,8,us,12us,16us
    unsigned int PEDisambiguity; //PE disambiguity(0 or 1)
    unsigned int MidamblePeriod;//Midamble Period (no midamble, 1: period = 10 symbols, 2: period = 20 symbols)
    unsigned int BeamChange;//0 or 1
	unsigned int RuAllocation; /*params_cont_tx_control_vectore4, [15:8] ,Indicate the RU allocated for current transmission in HE TB PPDU*/
    
    
    //RF BW paramter
    unsigned int DefaultRFBW;
	unsigned int StationID0;
	
	
	//TPC flag
	unsigned int TPC;
	//config CaliEnable or not
	unsigned int ConfigCalibration;
	//lmac tx calibration 
	//bit[0:3]: BLE
	// bit[4:7]: dsss
	// 8~17: ofdm 20m
	// 18~27: ofdm 40m;
	unsigned int CaliEnable;
	unsigned int LowEnergy;
    unsigned int _20MOnly;
    unsigned int _11BOnlyMode;

//new params which will be used by ETF GUI when test Leto
    unsigned int BssBw; //bss bandwidth
    unsigned int PrimaryIndex; //primary channel index

    unsigned int TxAntSel; //0:ANT1; 1:ANT2; Only used when 1TX/1RX (TxAntennas is 0)

    //RX other paramters
    unsigned char VhtGrpId; //1~62
    unsigned char VhtUsrPos;//1~4
    unsigned char BfReportFbType;//params_bf_report_fb_type
    unsigned char TXBW20M_use_20UL;//reserved
	//20M precompensation sel
	int precompensation;
}ETF_PHY_TX_PARAM_T;

typedef ETF_PHY_TX_PARAM_T ETF_PHY_RX_PARAM_T;


typedef struct
{
	unsigned short          MsgLen;
	unsigned short          MsgId;
	ETF_PHY_TX_PARAM_T		TxConfig;
}ETF_HE_TX_CONFIG_REQ;

typedef struct
{
	unsigned short          MsgLen;
	unsigned short          MsgId;
	ETF_PHY_RX_PARAM_T		RxConfig;
}ETF_HE_RX_CONFIG_REQ;

/*
Register Name	                    Base	Width	Abs addr		
params_nt_contf	                    0080	31:0	ACD0080
params_cont_tx_rate	                0084	7:0	    ACD0084
params_cont_tx_length	            0088	18:0	ACD0088
params_cont_tx_control_vectore0	    008c	31:0	ACD008C
params_cont_tx_control_vectore1	    0090	31:0	ACD0090
params_cont_tx_control_vectore2	    0094	31:0	ACD0094
params_cont_tx_control_vectore3	    0098	31:0	ACD0098
params_cont_tx_control_vectore4	    009c	31:0	ACD009C
params_cont_tx_control_vectore5	    0100	31:0	ACD0100
//params_cont_tx_ampdu_num	        0104	16:0	ACD0104
params_cont_ifs_time    	        0104	20:0	ACD0104
params_cont_tx_mpdu_len	            0108	13:0	ACD0108
*/
#define  WIFI_AX_REG_NT_CONFIG          0xACD0080
#define  WIFI_AX_REG_TX_RATE            0xACD0084
#define  WIFI_AX_REG_TX_LEN             0xACD0088
#define  WIFI_AX_REG_TX_VECTOR0         0xACD008C
#define  WIFI_AX_REG_TX_VECTOR1         0xACD0090
#define  WIFI_AX_REG_TX_VECTOR2         0xACD0094
#define  WIFI_AX_REG_TX_VECTOR3         0xACD0098
#define  WIFI_AX_REG_TX_VECTOR4         0xACD009C
#define  WIFI_AX_REG_TX_VECTOR5         0xACD0100
//#define  WIFI_AX_REG_TX_MPDU_NUM        0xACD0104 //for AMPDU
#define  WIFI_AX_REG_TX_IFS_TIME        0xACD0104 
#define  WIFI_AX_REG_TX_MPDU_LEN        0xACD0108 //for AMPDU
#define  WIFI_AX_REG_SCRAMBLE_SEED       0xACD010C
//#define  WIFI_AX_REG_PRBS_SEED          0xACD0xxx


/* params_nt_contf
即NTCONTCONF寄存器，是主要的模式控制寄存器。包括是否是continuous模式，发送什么帧，IFS是多长等

params_nt_contf[0]          CONT        1：continuous模式    0：非continuous模式
params_nt_contf[1]          INF         1：无限帧长    0：有限帧长
params_nt_contf[15:3]      IFS          帧之间的间隔长度，指的是phy_read下降沿到phy_enable上升沿之间的cycle数
params_nt_contf[31:22]      NFRAMES      n：n个frame
*/
typedef struct
{
    unsigned int CONT : 1;
    unsigned int INF : 1;
    unsigned int PRBSGEN : 1;
    unsigned int IFS : 13;
    unsigned int NBITS : 6;
    unsigned int NFRAMES : 10;
    
}NetConfField_t;
typedef struct
{
    union
    {
        unsigned int Reg;
        NetConfField_t Bits;
    };
}NtContf_t;


typedef WLAN_RATE_T ContTxRate_e;

typedef struct
{
    unsigned int Smoothing : 1;
    unsigned int Sounding : 1;
    unsigned int Aggregation : 1;
    unsigned int STBC : 2;
    unsigned int reserved1 : 2;//bit5~bit6
    unsigned int NumOfLTF : 3; // 0: 1 LTF symbols     1:02     3:04    5:06    7:08,     TB only, else set to 0
    unsigned int Beamformed : 1;
    unsigned int Doppler : 1;
    unsigned int BurstLength : 12;
    unsigned int TxopDuration : 7;
    unsigned int NoSigExtn : 1;
    
}TxCtlVec0Field_t;
typedef struct
{
    unsigned int ServiceField : 8;
    unsigned int reserved : 24;
    
}TxCtlVec0Field11b_t; //for 802.11b
typedef struct
{
    union
    {
        unsigned int Reg;
        TxCtlVec0Field_t Bits;
        TxCtlVec0Field11b_t Bits11b;
    };
}TxCtlVec0_t;

typedef struct
{
    unsigned int TxPower : 4;
    unsigned int reserved : 2;
    unsigned int TxStreams : 2;
    unsigned int TxAntennas : 2;
/*
 1001: 802.11ax HE MU PPDU
 1000: 802.11ax HE TB PPDU
 0111: 802.11ax HE ER SU PPDU
 0110: 802.11ax HE SU PPDU
 0101: 802.11n OFDM long (mixed-mode) preamble
 0100: 802.11n OFDM short (greenfield-mode) preamble
 0010: 802.11a OFDM preamble
 0001: 802.11b DSSS long preamble
 0000: 802.11b DSSS short preamble (note this is not supported for 1MBit/s)
 */    
    unsigned int TxMode : 4;
    unsigned int ChBW : 3;//0:20M. Valid for 802.11a/b/n modems, ER SU is 242 tone; 1:40M. Valid for 802.11a/b/n modems, ER SU is 106-tone
    unsigned int ChOffset : 3;
    unsigned int TxAbsPower : 8;
    unsigned int TxPowerModeSel : 2;
    unsigned int BeamChange : 1;
   // unsigned int reserved2 : 1;//bit31
    unsigned int iTx : 1;//When Ntx=2, set iTx to 0 means select the first antenna, set iTx to 1 means select the second antenna, 
    
}TxCtlVec1Field_t;
typedef struct
{
    union
    {
        unsigned int Reg;
        TxCtlVec1Field_t Bits;
    };
}TxCtlVec1_t;


typedef struct
{
    unsigned int PEDisambiguity : 1;
    unsigned int StartingStsNum : 3;
    unsigned int HELTFMode : 1;
    unsigned int HESigA2Reserved : 9;    
    unsigned int PreFecPaddingFactor : 2;
    unsigned int SpatialReuse1 : 4;    
    unsigned int SpatialReuse2 : 4;    
    unsigned int SpatialReuse3 : 4;    
    unsigned int SpatialReuse4 : 4;    
    
}TxCtlVec2Field_t;
typedef struct
{
    union
    {
        unsigned int Reg;
        TxCtlVec2Field_t Bits;
    };
}TxCtlVec2_t;

typedef struct
{
    unsigned int TriggerResponding : 1;
    unsigned int TriggerMethod : 1;
    unsigned int NomPacketPadding : 2;
    unsigned int reserved : 1;//bit4
    unsigned int BSSColor : 6;
    unsigned int UplinkFlag : 1;
    unsigned int ScramblerInitialvalue : 7;
    unsigned int ScramblerInitialvalue_en : 1;
    unsigned int LdpcExtraSymbol : 1;
    unsigned int reserved2 : 11;//bit21~bit31
    
}TxCtlVec3Field_t;
typedef struct
{
    union
    {
        unsigned int Reg;
        TxCtlVec3Field_t Bits;
    };
}TxCtlVec3_t;

typedef struct
{
    unsigned int HELTFType : 2;
    unsigned int MidamblePeriod : 2;
    unsigned int Dcm : 1;
    unsigned int Coding : 1;
    unsigned int GI_Type : 2;
    unsigned int RUAllocation : 8;
    unsigned int ReservedForMAC : 16;    
    
}TxCtlVec4Field_t;
typedef struct
{
    union
    {
        unsigned int Reg;
        TxCtlVec4Field_t Bits;
    };
}TxCtlVec4_t;

typedef struct
{
    unsigned int CFO : 13; //This field is a measure of the carrier frequency offset estimated during the reception of the frame. The format is signed S(13,20)
    unsigned int reserved1 : 3;
    unsigned int PPM : 14; //This field is a measure of the ppm observed at the antenna during the reception of the current data frame. The format is signed S(14,25)
    unsigned int reserved2 : 2;
    
}TxCtlVec5Field_t;
typedef struct
{
    union
    {
        unsigned int Reg;
        TxCtlVec5Field_t Bits;
    };
}TxCtlVec5_t;

/*
Register Name	                    Base	Width	Abs addr		
params_nt_contf	                    0080	31:0	ACD0080
params_cont_tx_rate	                0084	7:0	    ACD0084
params_cont_tx_length	            0088	18:0	ACD0088
params_cont_tx_control_vectore0	    008c	31:0	ACD008C
params_cont_tx_control_vectore1	    0090	31:0	ACD0090
params_cont_tx_control_vectore2	    0094	31:0	ACD0094
params_cont_tx_control_vectore3	    0098	31:0	ACD0098
params_cont_tx_control_vectore4	    009c	31:0	ACD009C
params_cont_tx_control_vectore5	    0100	31:0	ACD0100
*/
typedef struct
{
    NtContf_t nt_contf;
    ContTxRate_e tx_rate;
    unsigned int tx_length;//Length of the PSDU in terms of bytes
    TxCtlVec0_t TxVector0;
    TxCtlVec1_t TxVector1;
    TxCtlVec2_t TxVector2;
    TxCtlVec3_t TxVector3;
    TxCtlVec4_t TxVector4;
    TxCtlVec5_t TxVector5;
    //unsigned int MPDUNum; //for AMPDU
    unsigned int TxIfsTime;//WIFI_AX_REG_TX_IFS_TIME
    unsigned int MPDULen; //for AMPDU
    
    unsigned int ScrambleSeed;

	//unsigned char TxAntSel; //0:RF1; 1:RF2;   params_tx_ant_sel       0B48        0   ACB8B48
    unsigned char PrimaryIndex;
    unsigned char BssBw; //0:20M; 1:40M; 2:80M;
    unsigned char FcBand; //0:2.4G; 1:5G;
	unsigned char ScrInitValChange; //scrambler Initial value change dynamic
	int precom;//20M precompensation sel
}ContTxParam_t;


typedef struct
{
    unsigned int FreqMHz;//channel center frequency in MHz

    unsigned int BandWidthMHz;
    //unsigned int PrimaryChUpper;//1:primary is upper; 0:primary is lower
    unsigned char PrimaryIndex;
    unsigned char BssBw; //0:20M; 1:40M; 2:80M;
    unsigned char FcBand; //0:2.4G; 1:5G;
    unsigned int StationID0;
    unsigned int StationID1;
    unsigned int StationID2;
    unsigned int StationID3;

    unsigned int NRx; //0: 1 Rx; 1:2 Rx;
    unsigned int RxAntSel; //0:RF1; 1:RF2;

    unsigned char VhtGrpId; //1~62
    unsigned char VhtUsrPos;//1~4

    unsigned char BfReportFbType;//params_bf_report_fb_type 4:3 AC9032C
}ContRxParam_t;

#define OCEANUS_5G_ALL_RATE_USE_20UL 	0

unsigned int HW_READ_REG(unsigned int addr);
void HW_WRITE_REG(unsigned int addr, unsigned int data);
unsigned int HW_READ_REG_BIT(unsigned int addr,int endbit,int startbit);
void HW_WRITE_REG_BIT(unsigned int addr,unsigned int endBit,unsigned int startBit,unsigned int data );

int atbm_etf_start_tx(int channel,int mode, int rateIdx,int  bw, int chOff, int ldpc, int packetLen, int precom);
int atbm_etf_stop_tx(void);
int atbm_etf_start_rx(int channel ,int bw, int chOff, int mode);
int atbm_etf_stop_rx(get_result_rx_data *rx_data);
void etf_set_deltagain(struct efuse_headr efuse);
extern int atbm_etf_set_powerTarget(int powerTar);
extern int atbm_etf_get_powerTarget(void);
int atbm_etf_test_is_start(void);
int atbm_etf_start_tx_single_tone(int channel);
atbm_void etf_v2_scan_end(struct atbmwifi_common *hw_priv, struct atbmwifi_vif *vif );

#endif  //__ATBM_ETF_H__
