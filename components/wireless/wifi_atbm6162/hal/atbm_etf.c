/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"
#include "atbm_etf.h"
#include "atbm_os_timer.h"

struct rxstatus{
	atbm_uint32 GainImb;
	atbm_uint32 PhaseImb;
	atbm_uint32 Cfo;
	atbm_uint32 evm;
	atbm_uint32  RSSI;
	atbm_uint32 probcnt;
};

struct rxstatus_signed{
	atbm_uint8 valid;
	atbm_int32 GainImb;
	atbm_int32 PhaseImb;
	atbm_int32 Cfo;
	atbm_int32 txevm;
	atbm_int32 evm;
	atbm_int32  RxRSSI;
	atbm_int32  TxRSSI;
	atbm_uint16 dcxo;
	atbm_int32 result;
};
static struct rxstatus_signed gRxs_s;
struct test_threshold gthreshold_param;

int g_Channel = 0;
static atbm_uint8 CodeStart = 0;
static atbm_uint8 CodeEnd = 0;

static atbm_uint8 ucWriteEfuseFlag = 0;

int Atbm_Test_Success = 0;

int atbm_test_rx_cnt = 0;
int txevm_total = 0;
int g_ProductTestGlobal = 0;
int g_EtfRxMode=0;
int g_DutyCycle = 1;//10%


#define DCXO_CODE_MINI		0//24//0
#define DCXO_CODE_MAX		127//38//63
#define TARGET_FREQOFFSET_HZ  (7000)

#define _32bitTo5bit(dg) (dg &= 0x1f)
#define _5bitTo32bit(delta_gain) ((delta_gain & 0x10)?(delta_gain |0xffffffe0):(delta_gain))
#define _32bitTo6bit(dg) (dg &= 0x3f)
#define _6bitTo32bit(delta_gain) ((delta_gain & 0x20)?(delta_gain |0xffffffc0):(delta_gain))

struct efuse_headr efuse_data_etf;


static atbm_uint8 ETF_bStartTx = 0;
static atbm_uint8 ETF_bStartRx = 0;
extern struct atbmwifi_common g_hw_prv;
extern atbm_int32 atbm_wifi_set_rate_txpower_mode(atbm_int32 txpower_idx);
extern int wsm_efuse_change_data_cmd(struct atbmwifi_common *hw_priv, const struct efuse_headr *arg,int if_id);
extern int wsm_start_tx_v2(struct atbmwifi_common *hw_priv, struct atbmwifi_vif *priv );
extern void frame_hexdump(char *prefix, atbm_uint8 *data, atbm_uint8 len);

int atbm_support_5G(struct atbmwifi_common *hw_priv)
{
	int support_5g = 0;
	switch(hw_priv->chip_id){
	case HW_CHIP_VERSION_Oceanus_FM:
	case HW_CHIP_VERSION_Coeus:
	case HW_CHIP_VERSION_OCEANUS_LITE:
		support_5g = 1;
		break;
	default:
		support_5g = 0;
		break;
	}

	return support_5g;
}

int atbm_etf_test_is_start(void)
{
	return (ETF_bStartTx+ETF_bStartRx);
}

unsigned int HW_READ_REG(unsigned int addr)
{
	unsigned int regdata=0;
	struct atbmwifi_common *hw_priv;

	hw_priv = &g_hw_prv;
	atbm_direct_read_reg_32(hw_priv,addr,&regdata);
	return regdata;
}

void HW_WRITE_REG(unsigned int addr, unsigned int data)
{
	struct atbmwifi_common *hw_priv;

	hw_priv = &g_hw_prv;
	atbm_direct_write_reg_32(hw_priv,addr,data);
}

unsigned int HW_READ_REG_BIT(unsigned int addr,int endbit,int startbit)
{	
	unsigned int regdata=0;
	unsigned int regmask=0;
	

	regdata = HW_READ_REG(addr);

	regmask = ~((1<<startbit) -1);
	regmask &= ((1<<endbit) -1)|(1<<endbit);
	regdata &= regmask;
	regdata >>=  startbit;
	return regdata;
}
void HW_WRITE_REG_BIT(unsigned int addr,unsigned int endBit,unsigned int startBit,unsigned int data )
{
	unsigned int	uiRegValue=0;
	unsigned int  regmask=0;

	uiRegValue=HW_READ_REG(addr);		
	regmask = ~((1<<startBit) -1);
	regmask &= ((1<<endBit) -1)|(1<<endBit);
	uiRegValue &= ~regmask;
	uiRegValue |= (data <<startBit)&regmask;
	HW_WRITE_REG(addr,uiRegValue);
}


//void HW_WRITE_REG_BIT(atbm_uint32 addr,u8 endBit,u8 startBit,atbm_uint32 data )
//void ATBMPhyRegBitsSet(unsigned int  u32address, unsigned int u32Value,unsigned char bitLen, unsigned char bitOffset)
#define ATBMPhyRegBitsSet(addr, data, bitLen, bitOffset)   (HW_WRITE_REG_BIT((addr),  (bitOffset) + (bitLen) - 1, (bitOffset), (data)))
//uint32 HW_READ_REG_BIT(uint32 addr,int endbit,int startbit)
//UINT32 HW_READ_BIT(uint32 addr,int startbit,int endbit)
#define HW_READ_BIT(addr, startbit, endbit)		(HW_READ_REG_BIT((addr), (endbit), (startbit)))
#define ATBMPhyRegI2CWrite(addr, data)			(HW_WRITE_REG((addr), (data)))
//void HW_WRITE_REG_BIT(atbm_uint32 addr,u8 endBit,u8 startBit,atbm_uint32 data )
//void HW_WRITE_BIT(uint32 addr,uint32 data,int startbit,int endbit)
#define HW_WRITE_BIT(addr, data, startbit, endbit)   (HW_WRITE_REG_BIT((addr), (endbit), (startbit), (data)))

//get current paramters from registers
void ETF_PHY_Cont_Tx_Param_Get(ContTxParam_t *pContTxParam)
{
    if(NULL == pContTxParam)
    {
        return;
    }
    
    pContTxParam->nt_contf.Reg 	= HW_READ_REG(WIFI_AX_REG_NT_CONFIG);
    pContTxParam->tx_rate 	  	= (ContTxRate_e)HW_READ_REG(WIFI_AX_REG_TX_RATE);
    pContTxParam->tx_length 	= HW_READ_REG(WIFI_AX_REG_TX_LEN);
    pContTxParam->TxVector0.Reg = HW_READ_REG(WIFI_AX_REG_TX_VECTOR0);
    pContTxParam->TxVector1.Reg = HW_READ_REG(WIFI_AX_REG_TX_VECTOR1);
    pContTxParam->TxVector2.Reg = HW_READ_REG(WIFI_AX_REG_TX_VECTOR2);
    pContTxParam->TxVector3.Reg = HW_READ_REG(WIFI_AX_REG_TX_VECTOR3);
    pContTxParam->TxVector4.Reg = HW_READ_REG(WIFI_AX_REG_TX_VECTOR4);
    pContTxParam->TxVector5.Reg = HW_READ_REG(WIFI_AX_REG_TX_VECTOR5);
    //ATBMPhyRegI2CRead(WIFI_AX_REG_TX_MPDU_NUM, &pContTxParam->MPDUNum);
    pContTxParam->TxIfsTime 	= HW_READ_REG(WIFI_AX_REG_TX_IFS_TIME);
    pContTxParam->ScrambleSeed	= HW_READ_REG(WIFI_AX_REG_SCRAMBLE_SEED);
    
}
//set all paramters except "pContTxParam->nt_contf"
void ETF_PHY_Cont_Tx_Param_Set(ContTxParam_t *pContTxParam)
{
	
    HW_WRITE_REG(WIFI_AX_REG_TX_RATE, 	 (unsigned int )pContTxParam->tx_rate);
    HW_WRITE_REG(WIFI_AX_REG_TX_LEN, 	 pContTxParam->tx_length);
    HW_WRITE_REG(WIFI_AX_REG_TX_VECTOR0, pContTxParam->TxVector0.Reg);
    HW_WRITE_REG(WIFI_AX_REG_TX_VECTOR1, pContTxParam->TxVector1.Reg);
    HW_WRITE_REG(WIFI_AX_REG_TX_VECTOR2, pContTxParam->TxVector2.Reg);
    HW_WRITE_REG(WIFI_AX_REG_TX_VECTOR3, pContTxParam->TxVector3.Reg);
    HW_WRITE_REG(WIFI_AX_REG_TX_VECTOR4, pContTxParam->TxVector4.Reg);
    HW_WRITE_REG(WIFI_AX_REG_TX_VECTOR5, pContTxParam->TxVector5.Reg);
    HW_WRITE_REG(WIFI_AX_REG_TX_IFS_TIME, pContTxParam->TxIfsTime);
	
	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
    	HW_WRITE_REG(WIFI_AX_REG_SCRAMBLE_SEED, pContTxParam->ScrambleSeed);
	else
	{
		//HW_WRITE_REG(WIFI_AX_REG_NTTMP_CNT_RST_CONFIG, 0x0);
		//HW_WRITE_REG_BIT(WIFI_AX_REG_POST_FEC_PADDING_CONFIG, 0, 0, 0x0);

		//HW_WRITE_REG_BIT(0x0AC9007C, 5, 5, 0x1);//params_rx_phy_reg_params_sel  for sounding
		//HW_WRITE_REG_BIT(0x0AC9007C, 6, 6, 0x1);//params_sounding_ndp_en
		//HW_WRITE_REG_BIT(0x0AC9032C, 29, 29, 0x0);//params_rx_phy_reg_params_sel

	    HW_WRITE_REG_BIT(0x0ACB8004,1,0, pContTxParam->BssBw); //Bss_bw: ACB8004 [1:0] 0:20M; 1:40M; 2:80M
	    HW_WRITE_REG_BIT(0x0ACB8900,2,1, pContTxParam->PrimaryIndex); //Primary_index: ACB8900 [2:1]

	    //HW_WRITE_REG_BIT(PHY_BBDIG_RICTRL,2,2, pContTxParam->FcBand); //bandsel
	}
}

void ETF_PHY_Cont_Tx_Enable(ContTxParam_t *pContTxParam,ATBM_BOOL TxEnable)
{
    NtContf_t NtContf = {{0}};
    if(NULL == pContTxParam)
    {
        NtContf.Reg = HW_READ_REG(WIFI_AX_REG_NT_CONFIG);
    }
    else
    {
        NtContf = pContTxParam->nt_contf;
    }
    if(TxEnable)
    {
        NtContf.Bits.CONT = 1;
    }
    else
    {
        NtContf.Bits.CONT = 0;
    }
    HW_WRITE_REG(WIFI_AX_REG_NT_CONFIG, NtContf.Reg);
}
//start tx (set only "pContTxParam->nt_contf")
void ETF_PHY_Cont_Tx_Start(ContTxParam_t *pContTxParam)
{
    ETF_PHY_Cont_Tx_Enable(pContTxParam,ATBM_TRUE);
}

//stop tx (set only "pContTxParam->nt_contf")
void ETF_PHY_Cont_Tx_Stop(ContTxParam_t *pContTxParam)
{
    ETF_PHY_Cont_Tx_Enable(pContTxParam,ATBM_FALSE);
}

static WLAN_RATE_T atbm_ETF_ATBMPhyRateMapping(ETF_PHY_TX_PARAM_T *pTxParam)
{
	WLAN_RATE_T eRate = _6Mbps_BPSK_Code1_2_;
	 pTxParam->DataRateMbps = 0;
	 if(ATBM_WIFI_MODE_DSSS == pTxParam->WiFiMode)
	 {
		 switch(pTxParam->Rate) //int type, the value differentiated by WiFiMode(DSS,LM,MM,GF)
		 {
			 case ATBM_WIFI_RATE_1M:
				 eRate = _1Mbps_DSSS_;
				 pTxParam->DataRateMbps = 1;
				 break;
			 case ATBM_WIFI_RATE_2M:
				 eRate = _2Mbps_DSSS_;
				 pTxParam->DataRateMbps = 2;
				 break;
			 case ATBM_WIFI_RATE_5D5M:
				 eRate = _5_5Mbps_CCK_;
				 pTxParam->DataRateMbps = 5.5;
				 break;
			 case ATBM_WIFI_RATE_11M:
				 eRate = _11Mbps_CCK_;
				 pTxParam->DataRateMbps = 11;
				 break;
			 default:
				 eRate = _1Mbps_DSSS_;
				 pTxParam->DataRateMbps = 1;
				 break;
		 }
	 }
	 else if((ATBM_WIFI_MODE_OFDM == pTxParam->WiFiMode)&&(ATBM_WIFI_OFDM_MD_LM == pTxParam->OFDMMode)) //legacy mode
	 {
		 switch(pTxParam->Rate)
		 {
			 case ATBM_WIFI_RATE_6M:
				 eRate = _6Mbps_BPSK_Code1_2_;
				 pTxParam->DataRateMbps = 6;
				 break;
			 case ATBM_WIFI_RATE_9M:
				 eRate = _9Mbps_BPSK_Code3_4_;
				 pTxParam->DataRateMbps = 9;
				 break;
			 case ATBM_WIFI_RATE_12M:
				 eRate = _12Mbps_QPSK_Code1_2_;
				 pTxParam->DataRateMbps = 12;
				 break;
			 case ATBM_WIFI_RATE_18M:
				 eRate = _18Mbps_QPSK_Code3_4_;
				 pTxParam->DataRateMbps = 18;
				 break;
			 case ATBM_WIFI_RATE_24M:
				 eRate = _24Mbps_16QAM_Code1_2_;
				 pTxParam->DataRateMbps = 24;
				 break;
			 case ATBM_WIFI_RATE_36M:
				 eRate = _36Mbps_16QAM_Code3_4_;
				 pTxParam->DataRateMbps = 36;
				 break;
			 case ATBM_WIFI_RATE_48M:
				 eRate = _48Mbps_64QAM_Code2_3_;
				 pTxParam->DataRateMbps = 48;
				 break;
			 case ATBM_WIFI_RATE_54M:
				 eRate = _54Mbps_64QAM_Code3_4_;
				 pTxParam->DataRateMbps = 54;
				 break;
			 default:
				 eRate = _6Mbps_BPSK_Code1_2_;
				 pTxParam->DataRateMbps = 6;
				 break;
		 }
	 }
	 else if((ATBM_WIFI_MODE_OFDM == pTxParam->WiFiMode)  
			 &&((ATBM_WIFI_OFDM_MD_MM == pTxParam->OFDMMode)||(ATBM_WIFI_OFDM_MD_GF == pTxParam->OFDMMode))) //MM,GF mode
	 {
	 
		 if(pTxParam->BW == ATBM_WIFI_BW_40M)
		 {
			 if(pTxParam->GIMode == ATBM_WIFI_GI_MD_NORMAL)
			 {
				 switch(pTxParam->Rate)
				 {
				 case ATBM_WIFI_RATE_MCS0:
					 eRate = _6_5Mbps_BPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 13.5;
					 break;
				 case ATBM_WIFI_RATE_MCS1:
					 eRate = _13_5Mbps_QPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 27;
					 break;
				 case ATBM_WIFI_RATE_MCS2:
					 eRate = _19_5Mbps_QPSK_Code_3_4_;
					 pTxParam->DataRateMbps = 40.5;
					 break;
				 case ATBM_WIFI_RATE_MCS3:
					 eRate = _26_Mbps_16QAM_Code_1_2_;
					 pTxParam->DataRateMbps = 54;
					 break;
				 case ATBM_WIFI_RATE_MCS4:
					 eRate = _39_Mbps_16QAM_Code_3_4_;
					 pTxParam->DataRateMbps = 81;
					 break;
				 case ATBM_WIFI_RATE_MCS5:
					 eRate = _52_Mbps_64QAM_Code_2_3_;
					 pTxParam->DataRateMbps = 108;
					 break;
				 case ATBM_WIFI_RATE_MCS6:
					 eRate = _58_5Mbps_64QAM_Code_3_4_;
					 pTxParam->DataRateMbps = 121.5;
					 break;
				 case ATBM_WIFI_RATE_MCS7:
					 eRate = _65_Mbps_64QAM_Code_5_6_;
					 pTxParam->DataRateMbps = 135;
					 break;
				 case ATBM_WIFI_RATE_MCS32:
					 eRate = _6Mbps_MCS32_BPSK_Code1_2_; /*Driver need add MCS32 */
					 pTxParam->DataRateMbps = 6;
					 break;
				 default:
					 eRate = _6_5Mbps_BPSK_Code_1_2_;
					 pTxParam->DataRateMbps =13.5;
					 break;
				 }
			 }
			 if(pTxParam->GIMode == ATBM_WIFI_GI_MD_SHORT)
			 {
		 /*
		 0 BPSK 1/2 1 108 6 108 54	 13.5 15.0
		 1 QPSK 1/2 2 108 6 216 108  27.0 30.0
		 2 QPSK 3/4 2 108 6 216 162  40.5 45.0
		 3 16-QAM 1/2 4 108 6 432 216 54.0 60.0
		 4 16-QAM 3/4 4 108 6 432 324 81.0 90.0
		 5 64-QAM 2/3 6 108 6 648 432 108.0 120.0
		 6 64-QAM 3/4 6 108 6 648 486 121.5 135.0
		 7 64-QAM 5/6 6 108 6 648 540 135.0 150.0
		 */
				 switch(pTxParam->Rate)
				 {
				 case ATBM_WIFI_RATE_MCS0:
					 eRate = _6_5Mbps_BPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 15;
					 break;
				 case ATBM_WIFI_RATE_MCS1:
					 eRate = _13_5Mbps_QPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 30;
					 break;
				 case ATBM_WIFI_RATE_MCS2:
					 eRate = _19_5Mbps_QPSK_Code_3_4_;
					 pTxParam->DataRateMbps = 45;
					 break;
				 case ATBM_WIFI_RATE_MCS3:
					 eRate = _26_Mbps_16QAM_Code_1_2_;
					 pTxParam->DataRateMbps = 60;
					 break;
				 case ATBM_WIFI_RATE_MCS4:
					 eRate = _39_Mbps_16QAM_Code_3_4_;
					 pTxParam->DataRateMbps = 90;
					 break;
				 case ATBM_WIFI_RATE_MCS5:
					 eRate = _52_Mbps_64QAM_Code_2_3_;
					 pTxParam->DataRateMbps = 120;
					 break;
				 case ATBM_WIFI_RATE_MCS6:
					 eRate = _58_5Mbps_64QAM_Code_3_4_;
					 pTxParam->DataRateMbps =135;
					 break;
				 case ATBM_WIFI_RATE_MCS7:
					 eRate = _65_Mbps_64QAM_Code_5_6_;
					 pTxParam->DataRateMbps = 150;
					 break;
				 case ATBM_WIFI_RATE_MCS32:
					 eRate = _6Mbps_MCS32_BPSK_Code1_2_; /*Driver need add MCS32 */
					 pTxParam->DataRateMbps = 6.7;
					 break;
				 default:
					 eRate = _6_5Mbps_BPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 15;
					 break;
				 }
			 } //short GI
	
	
		 }
		 else
		 {
			 if(pTxParam->GIMode == ATBM_WIFI_GI_MD_NORMAL)
			 {
				 switch(pTxParam->Rate)
				 {
				 case ATBM_WIFI_RATE_MCS0:
					 eRate = _6_5Mbps_BPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 6.5;
					 break;
				 case ATBM_WIFI_RATE_MCS1:
					 eRate = _13_5Mbps_QPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 13.5;
					 break;
				 case ATBM_WIFI_RATE_MCS2:
					 eRate = _19_5Mbps_QPSK_Code_3_4_;
					 pTxParam->DataRateMbps = 19.5;
					 break;
				 case ATBM_WIFI_RATE_MCS3:
					 eRate = _26_Mbps_16QAM_Code_1_2_;
					 pTxParam->DataRateMbps = 26;
					 break;
				 case ATBM_WIFI_RATE_MCS4:
					 eRate = _39_Mbps_16QAM_Code_3_4_;
					 pTxParam->DataRateMbps = 39;
					 break;
				 case ATBM_WIFI_RATE_MCS5:
					 eRate = _52_Mbps_64QAM_Code_2_3_;
					 pTxParam->DataRateMbps = 52;
					 break;
				 case ATBM_WIFI_RATE_MCS6:
					 eRate = _58_5Mbps_64QAM_Code_3_4_;
					 pTxParam->DataRateMbps = 58.5;
					 break;
				 case ATBM_WIFI_RATE_MCS7:
					 eRate = _65_Mbps_64QAM_Code_5_6_;
					 pTxParam->DataRateMbps = 65;
					 break;
				 case ATBM_WIFI_RATE_MCS32:
					 eRate = _6Mbps_MCS32_BPSK_Code1_2_; /*Driver need add MCS32 */
					 pTxParam->DataRateMbps = 6;
					 break;
				 default:
					 eRate = _6_5Mbps_BPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 6.5;
					 break;
				 }
			 }
			 if(pTxParam->GIMode == ATBM_WIFI_GI_MD_SHORT)
			 {
				 switch(pTxParam->Rate)
				 {
				 case ATBM_WIFI_RATE_MCS0:
					 eRate = _6_5Mbps_BPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 7.2;
					 break;
				 case ATBM_WIFI_RATE_MCS1:
					 eRate = _13_5Mbps_QPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 14.4;
					 break;
				 case ATBM_WIFI_RATE_MCS2:
					 eRate = _19_5Mbps_QPSK_Code_3_4_;
					 pTxParam->DataRateMbps = 21.7;
					 break;
				 case ATBM_WIFI_RATE_MCS3:
					 eRate = _26_Mbps_16QAM_Code_1_2_;
					 pTxParam->DataRateMbps = 28.9;
					 break;
				 case ATBM_WIFI_RATE_MCS4:
					 eRate = _39_Mbps_16QAM_Code_3_4_;
					 pTxParam->DataRateMbps = 43.3;
					 break;
				 case ATBM_WIFI_RATE_MCS5:
					 eRate = _52_Mbps_64QAM_Code_2_3_;
					 pTxParam->DataRateMbps = 57.8;
					 break;
				 case ATBM_WIFI_RATE_MCS6:
					 eRate = _58_5Mbps_64QAM_Code_3_4_;
					 pTxParam->DataRateMbps =65;
					 break;
				 case ATBM_WIFI_RATE_MCS7:
					 eRate = _65_Mbps_64QAM_Code_5_6_;
					 pTxParam->DataRateMbps = 72.2;
					 break;
				 case ATBM_WIFI_RATE_MCS32:
					 eRate = _6Mbps_MCS32_BPSK_Code1_2_; /*Driver need add MCS32 */
					 pTxParam->DataRateMbps = 6.7;
					 break;
				 default:
					 eRate = _6_5Mbps_BPSK_Code_1_2_;
					 pTxParam->DataRateMbps = 6.5;
					 break;
				 }
			 } //short GI
		 }	 //20M
	 }
	 else if((ATBM_WIFI_MODE_OFDM == pTxParam->WiFiMode) && OFDM_MD_IS_HE(pTxParam->OFDMMode))
	 {
		 switch(pTxParam->Rate)
		 {
		 case ATBM_WIFI_RATE_MCS0:
			 eRate = _6_5Mbps_BPSK_Code_1_2_;
			 pTxParam->DataRateMbps = 6.5;
			 break;
		 case ATBM_WIFI_RATE_MCS1:
			 eRate = _13_5Mbps_QPSK_Code_1_2_;
			 pTxParam->DataRateMbps = 13.5;
			 break;
		 case ATBM_WIFI_RATE_MCS2:
			 eRate = _19_5Mbps_QPSK_Code_3_4_;
			 pTxParam->DataRateMbps = 19.5;
			 break;
		 case ATBM_WIFI_RATE_MCS3:
			 eRate = _26_Mbps_16QAM_Code_1_2_;
			 pTxParam->DataRateMbps = 26;
			 break;
		 case ATBM_WIFI_RATE_MCS4:
			 eRate = _39_Mbps_16QAM_Code_3_4_;
			 pTxParam->DataRateMbps = 39;
			 break;
		 case ATBM_WIFI_RATE_MCS5:
			 eRate = _52_Mbps_64QAM_Code_2_3_;
			 pTxParam->DataRateMbps = 52;
			 break;
		 case ATBM_WIFI_RATE_MCS6:
			 eRate = _58_5Mbps_64QAM_Code_3_4_;
			 pTxParam->DataRateMbps = 58.5;
			 break;
		 case ATBM_WIFI_RATE_MCS7:
			 eRate = _65_Mbps_64QAM_Code_5_6_;
			 pTxParam->DataRateMbps = 65;
			 break;
	
		 case ATBM_WIFI_RATE_MCS8:
			 eRate = _xx_Mbps_256QAM_Code_3_4_;
			 pTxParam->DataRateMbps = 65; //need check 
			 break;
		 case ATBM_WIFI_RATE_MCS9:
			 eRate = _xx_Mbps_256QAM_Code_5_6_;
			 pTxParam->DataRateMbps = 65; //need check 
			 break;
		 case ATBM_WIFI_RATE_MCS10:
			 eRate = _xx_Mbps_1024QAM_Code_3_4_;
			 pTxParam->DataRateMbps = 65; //need check 
			 break;
		 case ATBM_WIFI_RATE_MCS11:
			 eRate = _xx_Mbps_1024QAM_Code_5_6_;
			 pTxParam->DataRateMbps = 65; //need check 
			 break;
	
		 default:
			 eRate = _6_5Mbps_BPSK_Code_1_2_;
			 pTxParam->DataRateMbps = 6.5;
			 break;
		 }
	 }
	 else if((ATBM_WIFI_MODE_OFDM == pTxParam->WiFiMode) && OFDM_MD_IS_VHT(pTxParam->OFDMMode))
	 {
		 switch(pTxParam->Rate)
		 {
		 case ATBM_WIFI_RATE_MCS0:
			 eRate = _6_5Mbps_BPSK_Code_1_2_;
			 pTxParam->DataRateMbps = 6.5;
			 break;
		 case ATBM_WIFI_RATE_MCS1:
			 eRate = _13_5Mbps_QPSK_Code_1_2_;
			 pTxParam->DataRateMbps = 13.5;
			 break;
		 case ATBM_WIFI_RATE_MCS2:
			 eRate = _19_5Mbps_QPSK_Code_3_4_;
			 pTxParam->DataRateMbps = 19.5;
			 break;
		 case ATBM_WIFI_RATE_MCS3:
			 eRate = _26_Mbps_16QAM_Code_1_2_;
			 pTxParam->DataRateMbps = 26;
			 break;
		 case ATBM_WIFI_RATE_MCS4:
			 eRate = _39_Mbps_16QAM_Code_3_4_;
			 pTxParam->DataRateMbps = 39;
			 break;
		 case ATBM_WIFI_RATE_MCS5:
			 eRate = _52_Mbps_64QAM_Code_2_3_;
			 pTxParam->DataRateMbps = 52;
			 break;
		 case ATBM_WIFI_RATE_MCS6:
			 eRate = _58_5Mbps_64QAM_Code_3_4_;
			 pTxParam->DataRateMbps = 58.5;
			 break;
		 case ATBM_WIFI_RATE_MCS7:
			 eRate = _65_Mbps_64QAM_Code_5_6_;
			 pTxParam->DataRateMbps = 65;
			 break;
		 case ATBM_WIFI_RATE_MCS8:
			 eRate = _xx_Mbps_256QAM_Code_3_4_;
			 pTxParam->DataRateMbps = 65; //need check 
			 break;
		 case ATBM_WIFI_RATE_MCS9:
			 eRate = _xx_Mbps_256QAM_Code_5_6_;
			 pTxParam->DataRateMbps = 65; //need check 
			 break;
		 default:
			 eRate = _65_Mbps_64QAM_Code_5_6_;
			 pTxParam->DataRateMbps = 65;
			 break;
		 }
	 }
	 return eRate;

}

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
unsigned int atbm_ETF_TxVector1TxModeGet(ATBMWiFiMode_e WiFiMode,ATBMWiFiOFDMMode_e OFDMMode,ATBMWiFiPreamble_e PreambleMode)
{
    unsigned int Vector1TxMode = 0;
    if(ATBM_WIFI_MODE_DSSS == WiFiMode)
    {
        if(ATBM_WIFI_PREAMBLE_SHORT == PreambleMode)
        {
            Vector1TxMode = 0x00;
        }
        else
        {
            Vector1TxMode = 0x01;
        }
    }
    else
    {//OFDM
        switch(OFDMMode)
        {
            case ATBM_WIFI_OFDM_MD_LM:
                Vector1TxMode = 0x02;
                break;
            case ATBM_WIFI_OFDM_MD_GF:
                Vector1TxMode = 0x04;
                break;
            case ATBM_WIFI_OFDM_MD_MM:
                Vector1TxMode = 0x05;
                break;
            case ATBM_WIFI_OFDM_MD_HE_SU:
                Vector1TxMode = 0x06;
                break;
            case ATBM_WIFI_OFDM_MD_HE_ER_SU:
                Vector1TxMode = 0x07;
                break;
            case ATBM_WIFI_OFDM_MD_HE_TB:
                Vector1TxMode = 0x08;
                break;
            case ATBM_WIFI_OFDM_MD_HE_MU:
                Vector1TxMode = 0x09;
                break;
			case ATBM_WIFI_OFDM_MD_VHT:
            case ATBM_WIFI_OFDM_MD_VHT_SU:
            case ATBM_WIFI_OFDM_MD_VHT_MU:
                Vector1TxMode = 0x0A;
                break;
            default:
                Vector1TxMode = 0x06;
                break;
        }
    }
    return Vector1TxMode;
}

/*
0: 1x HE-LTF
1: 2x HE-LTF
2: 4x HE-LTF
*/
unsigned int atbm_ETF_TxVector4LtfTypeGet(ETF_PHY_TX_PARAM_T *pTxParam)
{
    unsigned int LtfType = 0;
    if((ATBM_WIFI_MODE_OFDM == pTxParam->WiFiMode)&&OFDM_MD_IS_HE(pTxParam->OFDMMode))
    {//OFDM
        switch(pTxParam->GIMode)
        {
            case ATBM_WIFI_GILTF_0P8_1X:
            case ATBM_WIFI_GILTF_1P6_1X:
                LtfType = 0x00;
                break;
            case ATBM_WIFI_GILTF_0P8_2X:
            case ATBM_WIFI_GILTF_1P6_2X:
                LtfType = 0x01;
                break;
            case ATBM_WIFI_GILTF_0P8_4X:
            case ATBM_WIFI_GILTF_3P2_4X:
                LtfType = 0x02;
                break;
            default:
                LtfType = 0x00;
                break;
        }
    }
    return LtfType;
}

/*
0: 0.4us.  not supported in HE PPDU.
1: 0.8us
2: 1.6us 
3: 3.2us
*/
unsigned int atbm_ETF_TxVector4GiTypeGet(ETF_PHY_TX_PARAM_T *pTxParam)
{
     unsigned int GiType = 0;
    if(ATBM_WIFI_MODE_OFDM == pTxParam->WiFiMode)
    {//OFDM
        if(OFDM_MD_IS_HE(pTxParam->OFDMMode))
        {
            switch(pTxParam->GIMode)
            {
                case ATBM_WIFI_GILTF_0P8_1X:
                case ATBM_WIFI_GILTF_0P8_2X:
                case ATBM_WIFI_GILTF_0P8_4X:
                    GiType = 0x01;
                    break;
                case ATBM_WIFI_GILTF_1P6_1X:
                case ATBM_WIFI_GILTF_1P6_2X:
                    GiType = 0x02;
                    break;
                case ATBM_WIFI_GILTF_3P2_4X:
                    GiType = 0x03;
                    break;
                default:
                    GiType = 0x00;
                    break;
            }
        }
        else if((OFDM_MD_IS_VHT(pTxParam->OFDMMode)) || (ATBM_WIFI_OFDM_MD_MM == pTxParam->OFDMMode)||(ATBM_WIFI_OFDM_MD_GF == pTxParam->OFDMMode))
        {
            switch(pTxParam->GIMode)
            {
                case ATBM_WIFI_GI_MD_SHORT:
                    GiType = 0x00;
                    break;
                case ATBM_WIFI_GI_MD_NORMAL:
                    GiType = 0x01;
                    break;
                default:
                    GiType = 0x00;
                    break;
            }
        }
		else if(ATBM_WIFI_OFDM_MD_LM == pTxParam->OFDMMode)
		{
			GiType = 0x01;//msut be 1 (0.8us) for LM
		}
    }

    return GiType;
}

void ETF_PHY_Cont_Tx_Param_Parse(ETF_PHY_TX_PARAM_T *pTxParam,ContTxParam_t *pContTxParam)
{
    //pContTxParam->nt_contf.Bits.CONT = 1;
    pContTxParam->nt_contf.Bits.INF = (pTxParam->InfiniteLongPacket)?(1):(0);
    pContTxParam->nt_contf.Bits.IFS = pTxParam->PacketInterval*160; //old FPGA code with "*160" (from us to colock cyles)
    pContTxParam->nt_contf.Bits.NFRAMES = pTxParam->PacketNum;
    
    pContTxParam->TxIfsTime = pTxParam->PacketInterval*160;
	if(pContTxParam->TxIfsTime >= 0x1FFFFF)
		pContTxParam->TxIfsTime = 0x1FFFFF;
		

    pContTxParam->tx_rate = atbm_ETF_ATBMPhyRateMapping(pTxParam);
    pContTxParam->tx_length = pTxParam->PSDULen;


    /* TxVector0 */
    if(ATBM_WIFI_MODE_DSSS == pTxParam->WiFiMode)
    {//802.11b
        pContTxParam->TxVector0.Bits11b.ServiceField = pTxParam->ServiceField;
    }
    else
    {
        pContTxParam->TxVector0.Bits.Smoothing = pTxParam->Smoothing;
        pContTxParam->TxVector0.Bits.Sounding = pTxParam->Sounding;
        pContTxParam->TxVector0.Bits.Aggregation = pTxParam->Aggregation;
        pContTxParam->TxVector0.Bits.STBC = pTxParam->STBC;
        pContTxParam->TxVector0.Bits.Beamformed = pTxParam->BeamFormed;
        pContTxParam->TxVector0.Bits.Doppler = pTxParam->Doppler;
        pContTxParam->TxVector0.Bits.BurstLength = pTxParam->BurstLen;
		if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
		{
			if(pTxParam->TxopDuration>=512)
	        {
	            pContTxParam->TxVector0.Bits.TxopDuration = (((pTxParam->TxopDuration-512)/128)<<1)|0x01;
	        }
	        else
	        {
	            pContTxParam->TxVector0.Bits.TxopDuration = ((pTxParam->TxopDuration/8)<<1);
	        }
		}
        else
        	pContTxParam->TxVector0.Bits.TxopDuration = 0x7F;
        //pContTxParam->TxVector0.Bits.TxopDuration = pTxParam->TxopDuration;
        pContTxParam->TxVector0.Bits.NoSigExtn = pTxParam->NoSigExtn;
    }
    /*    0: 1 LTF symbols; 1: 2LTF symbols;  3: 4LTF symbols;   5: 6LTF symbols;    7: 8 LTF symbols    */
    if((ATBM_WIFI_MODE_OFDM == pTxParam->WiFiMode)&&(ATBM_WIFI_OFDM_MD_HE_TB == pTxParam->OFDMMode))
    {
        pContTxParam->TxVector0.Bits.NumOfLTF = pTxParam->LTFNum - 1;
    }
    else
    {
        pContTxParam->TxVector0.Bits.NumOfLTF = 0;
    }
    
    
    /* TxVector1 */
    pContTxParam->TxVector1.Bits.TxPower = pTxParam->TxPower;
    pContTxParam->TxVector1.Bits.TxStreams = pTxParam->TxStreams;
    pContTxParam->TxVector1.Bits.TxAntennas = pTxParam->TxAntennas;
    pContTxParam->TxVector1.Bits.TxMode = atbm_ETF_TxVector1TxModeGet(pTxParam->WiFiMode,pTxParam->OFDMMode,pTxParam->PreambleMode);
    if((ATBM_WIFI_MODE_OFDM == pTxParam->WiFiMode)&&(ATBM_WIFI_OFDM_MD_HE_ER_SU == pTxParam->OFDMMode))
    {
        pContTxParam->TxVector1.Bits.ChBW = 0;//(ATBM_WIFI_BW_RU242==pTxParam->BW)?(0):(1) , only support RU242
    }
    else if(ATBM_WIFI_BW_20M == pTxParam->BW)
    {
        pContTxParam->TxVector1.Bits.ChBW = 0;
    }
    else if(ATBM_WIFI_BW_40M == pTxParam->BW)
    {
        pContTxParam->TxVector1.Bits.ChBW = 1;
    }
    else/* if(ATBM_WIFI_BW_80M == pTxParam->BW)*/
    {
        pContTxParam->TxVector1.Bits.ChBW = 2;
    }
	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
	{
		pContTxParam->TxVector1.Bits.ChOffset = pTxParam->ChOffset;
		pContTxParam->TxVector1.Bits.BeamChange = pTxParam->BeamChange; 
	}
	else
	{
		pContTxParam->TxVector1.Bits.ChOffset = 0;
		pContTxParam->TxVector1.Bits.BeamChange = 1;//pTxParam->BeamChange; Must be 1 in Leto
	}
    
    pContTxParam->TxVector1.Bits.TxAbsPower = pTxParam->TxAbsPower;
    pContTxParam->TxVector1.Bits.TxPowerModeSel = pTxParam->TxPowerModeSel;
    pContTxParam->TxVector1.Bits.iTx = pTxParam->TxAntSel;

    
    /* TxVector2 */
    pContTxParam->TxVector2.Bits.PEDisambiguity = pTxParam->PEDisambiguity;
    pContTxParam->TxVector2.Bits.StartingStsNum = pTxParam->StartingStsNum;
    pContTxParam->TxVector2.Bits.HELTFMode = pTxParam->HELTFMode;
    pContTxParam->TxVector2.Bits.HESigA2Reserved = pTxParam->HESigA2Reserved;
    pContTxParam->TxVector2.Bits.PreFecPaddingFactor = pTxParam->AFactor;
    pContTxParam->TxVector2.Bits.SpatialReuse1 = pTxParam->SpatialReuse1;
    pContTxParam->TxVector2.Bits.SpatialReuse2 = pTxParam->SpatialReuse2;
    pContTxParam->TxVector2.Bits.SpatialReuse3 = pTxParam->SpatialReuse3;
    pContTxParam->TxVector2.Bits.SpatialReuse4 = pTxParam->SpatialReuse4;
    
    /* TxVector3 */
    pContTxParam->TxVector3.Bits.TriggerResponding = pTxParam->TriggerResponding;
    pContTxParam->TxVector3.Bits.TriggerMethod = pTxParam->TriggerMethod;
    pContTxParam->TxVector3.Bits.NomPacketPadding = pTxParam->Padding;
    pContTxParam->TxVector3.Bits.BSSColor = pTxParam->BSSColor;
    pContTxParam->TxVector3.Bits.UplinkFlag = pTxParam->UplinkFlag;
    pContTxParam->TxVector3.Bits.ScramblerInitialvalue = pTxParam->ScramblerValue;
    pContTxParam->TxVector3.Bits.ScramblerInitialvalue_en = pTxParam->ScramblerValueEn;
    pContTxParam->TxVector3.Bits.LdpcExtraSymbol = pTxParam->LdpcExtrSysm;
    
    /* TxVector4 */
    pContTxParam->TxVector4.Bits.HELTFType = atbm_ETF_TxVector4LtfTypeGet(pTxParam);
    pContTxParam->TxVector4.Bits.MidamblePeriod = pTxParam->MidamblePeriod;
    pContTxParam->TxVector4.Bits.Dcm = pTxParam->DCM;
    pContTxParam->TxVector4.Bits.Coding = pTxParam->Coding;
    pContTxParam->TxVector4.Bits.GI_Type = atbm_ETF_TxVector4GiTypeGet(pTxParam);
    //Ru_allocation include ru_allocation[0] and ru_allocation[7:1] (ru_allocation_code)
    pContTxParam->TxVector4.Bits.RUAllocation = pTxParam->RuAllocation;
    pContTxParam->TxVector4.Bits.ReservedForMAC = pTxParam->ReservedForMAC;
    
    /* TxVector5 */
    pContTxParam->TxVector5.Bits.CFO = pTxParam->CFO;
    pContTxParam->TxVector5.Bits.PPM = pTxParam->PPM;

    //AMPDU: MPDU length and number
    //pContTxParam->MPDUNum = pTxParam->MPDUNum;
#if 0 //no need to set mpdu_len in new fpga
    pContTxParam->MPDULen = pTxParam->MPDULen;
#endif

    pContTxParam->ScrambleSeed = pTxParam->ScramblerValue;
    //pContTxParam->TxAntSel = pTxParam->TxAntSel;
    pContTxParam->BssBw = (unsigned char)pTxParam->BssBw;
    pContTxParam->PrimaryIndex = (unsigned char)pTxParam->PrimaryIndex;
    pContTxParam->FcBand = (pTxParam->FreqMHz > 4000)?(1):(0);
    
    pContTxParam->ScrInitValChange = (pTxParam->ScramblerValueEn>>1)&0x01;

	pContTxParam->precom = pTxParam->precompensation;//only channel1 20M used


}


typedef struct
{
    unsigned int Addr;
    unsigned int Val;
    unsigned int Msb;
    unsigned int Lsb;
}RegSetting_t;
static RegSetting_t NTPCOMPRegsBW40M[]=
{
    {0x0ACA010C, 0x00, 19, 19},//params_prc_bypass

    {0x0ACA2000, 0xA6, 7, 0},
    {0x0ACA2004, 0xA6, 7, 0},
    {0x0ACA2008, 0x9A, 7, 0},
    {0x0ACA200C, 0x78, 7, 0},
    {0x0ACA2010, 0x65, 7, 0},
    {0x0ACA2014, 0x59, 7, 0},
    {0x0ACA2018, 0x53, 7, 0},
    {0x0ACA201C, 0x4F, 7, 0},
    {0x0ACA2020, 0x4E, 7, 0},
    {0x0ACA2024, 0x4D, 7, 0},
    {0x0ACA2028, 0x4D, 7, 0},
    {0x0ACA202C, 0x4C, 7, 0},
    {0x0ACA2030, 0x4A, 7, 0},
    {0x0ACA2034, 0x48, 7, 0},
    {0x0ACA2038, 0x45, 7, 0},
    {0x0ACA203C, 0x43, 7, 0},
    {0x0ACA2040, 0x41, 7, 0},
    {0x0ACA2044, 0x40, 7, 0},
    {0x0ACA2048, 0x3F, 7, 0},
    {0x0ACA204C, 0x3E, 7, 0},
    {0x0ACA2050, 0x3D, 7, 0},
    {0x0ACA2054, 0x3C, 7, 0},
    {0x0ACA2058, 0x3B, 7, 0},
    {0x0ACA205C, 0x39, 7, 0},
    {0x0ACA2060, 0x38, 7, 0},
    {0x0ACA2064, 0x37, 7, 0},
    {0x0ACA2068, 0x35, 7, 0},
    {0x0ACA206C, 0x35, 7, 0},
    {0x0ACA2070, 0x34, 7, 0},
    {0x0ACA2074, 0x34, 7, 0},
    {0x0ACA2078, 0x34, 7, 0},
    {0x0ACA207C, 0x33, 7, 0},
    {0x0ACA2080, 0x32, 7, 0},
    {0x0ACA2084, 0x31, 7, 0},
    {0x0ACA2088, 0x31, 7, 0},
    {0x0ACA208C, 0x30, 7, 0},
    {0x0ACA2090, 0x30, 7, 0},
    {0x0ACA2094, 0x30, 7, 0},
    {0x0ACA2098, 0x30, 7, 0},
    {0x0ACA209C, 0x30, 7, 0},
    {0x0ACA20A0, 0x2F, 7, 0},
    {0x0ACA20A4, 0x2F, 7, 0},
    {0x0ACA20A8, 0x2F, 7, 0},
    {0x0ACA20AC, 0x2F, 7, 0},
    {0x0ACA20B0, 0x2F, 7, 0},
    {0x0ACA20B4, 0x2F, 7, 0},
    {0x0ACA20B8, 0x2F, 7, 0},
    {0x0ACA20BC, 0x2F, 7, 0},
    {0x0ACA20C0, 0x30, 7, 0},
    {0x0ACA20C4, 0x30, 7, 0},
    {0x0ACA20C8, 0x30, 7, 0},
    {0x0ACA20CC, 0x30, 7, 0},
    {0x0ACA20D0, 0x30, 7, 0},
    {0x0ACA20D4, 0x30, 7, 0},
    {0x0ACA20D8, 0x31, 7, 0},
    {0x0ACA20DC, 0x31, 7, 0},
    {0x0ACA20E0, 0x31, 7, 0},
    {0x0ACA20E4, 0x32, 7, 0},
    {0x0ACA20E8, 0x32, 7, 0},
    {0x0ACA20EC, 0x32, 7, 0},
    {0x0ACA20F0, 0x32, 7, 0},
    {0x0ACA20F4, 0x32, 7, 0},
    {0x0ACA20F8, 0x32, 7, 0},
    {0x0ACA20FC, 0x32, 7, 0},
    {0x0ACA2100, 0x32, 7, 0},
    {0x0ACA2104, 0x32, 7, 0},
    {0x0ACA2108, 0x32, 7, 0},
    {0x0ACA210C, 0x32, 7, 0},
    {0x0ACA2110, 0x32, 7, 0},
    {0x0ACA2114, 0x32, 7, 0},
    {0x0ACA2118, 0x31, 7, 0},
    {0x0ACA211C, 0x31, 7, 0},
    {0x0ACA2120, 0x31, 7, 0},
    {0x0ACA2124, 0x30, 7, 0},
    {0x0ACA2128, 0x30, 7, 0},
    {0x0ACA212C, 0x30, 7, 0},
    {0x0ACA2130, 0x30, 7, 0},
    {0x0ACA2134, 0x30, 7, 0},
    {0x0ACA2138, 0x30, 7, 0},
    {0x0ACA213C, 0x2F, 7, 0},
    {0x0ACA2140, 0x2F, 7, 0},
    {0x0ACA2144, 0x2F, 7, 0},
    {0x0ACA2148, 0x2F, 7, 0},
    {0x0ACA214C, 0x2F, 7, 0},
    {0x0ACA2150, 0x2F, 7, 0},
    {0x0ACA2154, 0x2F, 7, 0},
    {0x0ACA2158, 0x2F, 7, 0},
    {0x0ACA215C, 0x30, 7, 0},
    {0x0ACA2160, 0x30, 7, 0},
    {0x0ACA2164, 0x30, 7, 0},
    {0x0ACA2168, 0x30, 7, 0},
    {0x0ACA216C, 0x30, 7, 0},
    {0x0ACA2170, 0x31, 7, 0},
    {0x0ACA2174, 0x31, 7, 0},
    {0x0ACA2178, 0x32, 7, 0},
    {0x0ACA217C, 0x33, 7, 0},
    {0x0ACA2180, 0x34, 7, 0},
    {0x0ACA2184, 0x34, 7, 0},
    {0x0ACA2188, 0x34, 7, 0},
    {0x0ACA218C, 0x35, 7, 0},
    {0x0ACA2190, 0x35, 7, 0},
    {0x0ACA2194, 0x37, 7, 0},
    {0x0ACA2198, 0x38, 7, 0},
    {0x0ACA219C, 0x39, 7, 0},
    {0x0ACA21A0, 0x3B, 7, 0},
    {0x0ACA21A4, 0x3C, 7, 0},
    {0x0ACA21A8, 0x3D, 7, 0},
    {0x0ACA21AC, 0x3E, 7, 0},
    {0x0ACA21B0, 0x3F, 7, 0},
    {0x0ACA21B4, 0x40, 7, 0},
    {0x0ACA21B8, 0x41, 7, 0},
    {0x0ACA21BC, 0x43, 7, 0},
    {0x0ACA21C0, 0x45, 7, 0},
    {0x0ACA21C4, 0x48, 7, 0},
    {0x0ACA21C8, 0x4A, 7, 0},
    {0x0ACA21CC, 0x4C, 7, 0},
    {0x0ACA21D0, 0x4D, 7, 0},
    {0x0ACA21D4, 0x4D, 7, 0},
    {0x0ACA21D8, 0x4E, 7, 0},
    {0x0ACA21DC, 0x4F, 7, 0},
    {0x0ACA21E0, 0x53, 7, 0},
    {0x0ACA21E4, 0x59, 7, 0},
    {0x0ACA21E8, 0x65, 7, 0},
    {0x0ACA21EC, 0x78, 7, 0},
    {0x0ACA21F0, 0x9A, 7, 0},
    {0x0ACA21F4, 0xA6, 7, 0},
    {0x0ACA21F8, 0xA6, 7, 0},
};

#define NTP_COMP_REG_BW40_NUM sizeof(NTPCOMPRegsBW40M)/sizeof(NTPCOMPRegsBW40M[0])

static RegSetting_t NTPCOMPRegsBW20M[]=
{
    {0x0ACA010C, 0x00, 19, 19},//params_prc_bypass

   	{0x0ACA21FC, 0x96, 7, 0},
    {0x0ACA2200, 0x93, 7, 0},
    {0x0ACA2204, 0x94, 7, 0},
    {0x0ACA2208, 0x8D, 7, 0},
    {0x0ACA220C, 0x8F, 7, 0},
    {0x0ACA2210, 0x8C, 7, 0},
    {0x0ACA2214, 0x87, 7, 0},
    {0x0ACA2218, 0x89, 7, 0},
    {0x0ACA221C, 0x86, 7, 0},
    {0x0ACA2220, 0x81, 7, 0},
    {0x0ACA2224, 0x87, 7, 0},
    {0x0ACA2228, 0x80, 7, 0},
    {0x0ACA222C, 0x7F, 7, 0},
    {0x0ACA2230, 0x82, 7, 0},
    {0x0ACA2234, 0x82, 7, 0},
    {0x0ACA2238, 0x7F, 7, 0},
    {0x0ACA223C, 0x7E, 7, 0},
    {0x0ACA2240, 0x81, 7, 0},
    {0x0ACA2244, 0x7D, 7, 0},
    {0x0ACA2248, 0x7E, 7, 0},
    {0x0ACA224C, 0x7F, 7, 0},
    {0x0ACA2250, 0x7E, 7, 0},
    {0x0ACA2254, 0x82, 7, 0},
    {0x0ACA2258, 0x81, 7, 0},
    {0x0ACA225C, 0x81, 7, 0},
    {0x0ACA2260, 0x7F, 7, 0},
    {0x0ACA2264, 0x7F, 7, 0},
    {0x0ACA2268, 0x81, 7, 0},
    {0x0ACA226C, 0x83, 7, 0},
    {0x0ACA2270, 0x82, 7, 0},
    {0x0ACA2274, 0x7D, 7, 0},
    {0x0ACA2278, 0x81, 7, 0},
    {0x0ACA227C, 0x85, 7, 0},
    {0x0ACA2280, 0x80, 7, 0},
    {0x0ACA2284, 0x84, 7, 0},
    {0x0ACA2288, 0x82, 7, 0},
    {0x0ACA228C, 0x81, 7, 0},
    {0x0ACA2290, 0x7E, 7, 0},
    {0x0ACA2294, 0x82, 7, 0},
    {0x0ACA2298, 0x7F, 7, 0},
    {0x0ACA229C, 0x7C, 7, 0},
    {0x0ACA22A0, 0x82, 7, 0},
    {0x0ACA22A4, 0x82, 7, 0},
    {0x0ACA22A8, 0x7F, 7, 0},
    {0x0ACA22AC, 0x80, 7, 0},
    {0x0ACA22B0, 0x80, 7, 0},
    {0x0ACA22B4, 0x81, 7, 0},
    {0x0ACA22B8, 0x80, 7, 0},
    {0x0ACA22BC, 0x80, 7, 0},
    {0x0ACA22C0, 0x82, 7, 0},
    {0x0ACA22C4, 0x85, 7, 0},
    {0x0ACA22C8, 0x82, 7, 0},
    {0x0ACA22CC, 0x83, 7, 0},
    {0x0ACA22D0, 0x86, 7, 0},
    {0x0ACA22D4, 0x82, 7, 0},
    {0x0ACA22D8, 0x8A, 7, 0},
    {0x0ACA22DC, 0x89, 7, 0},
    {0x0ACA22E0, 0x84, 7, 0},
    {0x0ACA22E4, 0x8D, 7, 0},
    {0x0ACA22E8, 0x8F, 7, 0},
    {0x0ACA22EC, 0x8C, 7, 0},
    {0x0ACA22F0, 0x92, 7, 0},
    {0x0ACA22F4, 0x9A, 7, 0},

};
//spectrum arc:attenuate 3dB at 8M 
static RegSetting_t NTPCOMPRegsBW20M_arc8M[]=
{
	{0x0ACA010C, 0x00, 19, 19},//params_prc_bypass

	{0x0ACA21FC, 0x5B, 7, 0},
	{0x0ACA2200, 0x5D, 7, 0},
	{0x0ACA2204, 0x60, 7, 0},
	{0x0ACA2208, 0x62, 7, 0},
	{0x0ACA220C, 0x64, 7, 0},
	{0x0ACA2210, 0x66, 7, 0},
	{0x0ACA2214, 0x68, 7, 0},
	{0x0ACA2218, 0x6B, 7, 0},
	{0x0ACA221C, 0x6D, 7, 0},
	{0x0ACA2220, 0x6F, 7, 0},
	{0x0ACA2224, 0x72, 7, 0},
	{0x0ACA2228, 0x75, 7, 0},
	{0x0ACA222C, 0x77, 7, 0},
	{0x0ACA2230, 0x79, 7, 0},
	{0x0ACA2234, 0x7C, 7, 0},
	{0x0ACA2238, 0x7F, 7, 0},
	{0x0ACA223C, 0x81, 7, 0},
	{0x0ACA2240, 0x84, 7, 0},
	{0x0ACA2244, 0x86, 7, 0},
	{0x0ACA2248, 0x89, 7, 0},
	{0x0ACA224C, 0x8B, 7, 0},
	{0x0ACA2250, 0x8D, 7, 0},
	{0x0ACA2254, 0x90, 7, 0},
	{0x0ACA2258, 0x91, 7, 0},
	{0x0ACA225C, 0x93, 7, 0},
	{0x0ACA2260, 0x95, 7, 0},
	{0x0ACA2264, 0x96, 7, 0},
	{0x0ACA2268, 0x98, 7, 0},
	{0x0ACA226C, 0x98, 7, 0},
	{0x0ACA2270, 0x99, 7, 0},
	{0x0ACA2274, 0x99, 7, 0},
	{0x0ACA2278, 0x9A, 7, 0},
	{0x0ACA227C, 0x99, 7, 0},
	{0x0ACA2280, 0x9A, 7, 0},
	{0x0ACA2284, 0x98, 7, 0},
	{0x0ACA2288, 0x98, 7, 0},
	{0x0ACA228C, 0x97, 7, 0},
	{0x0ACA2290, 0x95, 7, 0},
	{0x0ACA2294, 0x94, 7, 0},
	{0x0ACA2298, 0x92, 7, 0},
	{0x0ACA229C, 0x90, 7, 0},
	{0x0ACA22A0, 0x8E, 7, 0},
	{0x0ACA22A4, 0x8B, 7, 0},
	{0x0ACA22A8, 0x89, 7, 0},
	{0x0ACA22AC, 0x87, 7, 0},
	{0x0ACA22B0, 0x84, 7, 0},
	{0x0ACA22B4, 0x82, 7, 0},
	{0x0ACA22B8, 0x7F, 7, 0},
	{0x0ACA22BC, 0x7D, 7, 0},
	{0x0ACA22C0, 0x7A, 7, 0},
	{0x0ACA22C4, 0x77, 7, 0},
	{0x0ACA22C8, 0x75, 7, 0},
	{0x0ACA22CC, 0x73, 7, 0},
	{0x0ACA22D0, 0x70, 7, 0},
	{0x0ACA22D4, 0x6D, 7, 0},
	{0x0ACA22D8, 0x6B, 7, 0},
	{0x0ACA22DC, 0x69, 7, 0},
	{0x0ACA22E0, 0x67, 7, 0},
	{0x0ACA22E4, 0x65, 7, 0},
	{0x0ACA22E8, 0x62, 7, 0},
	{0x0ACA22EC, 0x61, 7, 0},
	{0x0ACA22F0, 0x5E, 7, 0},
	{0x0ACA22F4, 0x5C, 7, 0},

};

//spectrum arc:attenuate 3dB at 9M 
static RegSetting_t NTPCOMPRegsBW20M_arc9M[]=
{
	{0x0ACA010C, 0x00, 19, 19},//params_prc_bypass

	{0x0ACA21FC, 0x66, 7, 0},
	{0x0ACA2200, 0x67, 7, 0},
	{0x0ACA2204, 0x69, 7, 0},
	{0x0ACA2208, 0x6A, 7, 0},
	{0x0ACA220C, 0x6C, 7, 0},
	{0x0ACA2210, 0x6E, 7, 0},
	{0x0ACA2214, 0x70, 7, 0},
	{0x0ACA2218, 0x72, 7, 0},
	{0x0ACA221C, 0x74, 7, 0},
	{0x0ACA2220, 0x75, 7, 0},
	{0x0ACA2224, 0x78, 7, 0},
	{0x0ACA2228, 0x7A, 7, 0},
	{0x0ACA222C, 0x7C, 7, 0},
	{0x0ACA2230, 0x7E, 7, 0},
	{0x0ACA2234, 0x80, 7, 0},
	{0x0ACA2238, 0x83, 7, 0},
	{0x0ACA223C, 0x85, 7, 0},
	{0x0ACA2240, 0x87, 7, 0},
	{0x0ACA2244, 0x89, 7, 0},
	{0x0ACA2248, 0x8B, 7, 0},
	{0x0ACA224C, 0x8D, 7, 0},
	{0x0ACA2250, 0x8F, 7, 0},
	{0x0ACA2254, 0x91, 7, 0},
	{0x0ACA2258, 0x92, 7, 0},
	{0x0ACA225C, 0x94, 7, 0},
	{0x0ACA2260, 0x95, 7, 0},
	{0x0ACA2264, 0x97, 7, 0},
	{0x0ACA2268, 0x98, 7, 0},
	{0x0ACA226C, 0x99, 7, 0},
	{0x0ACA2270, 0x99, 7, 0},
	{0x0ACA2274, 0x99, 7, 0},
	{0x0ACA2278, 0x9A, 7, 0},
	{0x0ACA227C, 0x99, 7, 0},
	{0x0ACA2280, 0x9A, 7, 0},
	{0x0ACA2284, 0x98, 7, 0},
	{0x0ACA2288, 0x98, 7, 0},
	{0x0ACA228C, 0x97, 7, 0},
	{0x0ACA2290, 0x96, 7, 0},
	{0x0ACA2294, 0x94, 7, 0},
	{0x0ACA2298, 0x93, 7, 0},
	{0x0ACA229C, 0x91, 7, 0},
	{0x0ACA22A0, 0x8F, 7, 0},
	{0x0ACA22A4, 0x8D, 7, 0},
	{0x0ACA22A8, 0x8B, 7, 0},
	{0x0ACA22AC, 0x89, 7, 0},
	{0x0ACA22B0, 0x87, 7, 0},
	{0x0ACA22B4, 0x85, 7, 0},
	{0x0ACA22B8, 0x83, 7, 0},
	{0x0ACA22BC, 0x81, 7, 0},
	{0x0ACA22C0, 0x7E, 7, 0},
	{0x0ACA22C4, 0x7C, 7, 0},
	{0x0ACA22C8, 0x7A, 7, 0},
	{0x0ACA22CC, 0x78, 7, 0},
	{0x0ACA22D0, 0x76, 7, 0},
	{0x0ACA22D4, 0x74, 7, 0},
	{0x0ACA22D8, 0x72, 7, 0},
	{0x0ACA22DC, 0x71, 7, 0},
	{0x0ACA22E0, 0x6F, 7, 0},
	{0x0ACA22E4, 0x6D, 7, 0},
	{0x0ACA22E8, 0x6B, 7, 0},
	{0x0ACA22EC, 0x6A, 7, 0},
	{0x0ACA22F0, 0x68, 7, 0},
	{0x0ACA22F4, 0x66, 7, 0},

};

#define NTP_COMP_REG_BW20_NUM sizeof(NTPCOMPRegsBW20M)/sizeof(NTPCOMPRegsBW20M[0])

static RegSetting_t NTPCOMPRegsBW20M_U10[]=
{
    {0x0ACA010C, 0x00, 19, 19},//params_prc_bypass

    {0x0ACA21FC, 0x4B, 7, 0},
    {0x0ACA2200, 0x4B, 7, 0},
    {0x0ACA2204, 0x4B, 7, 0},
    {0x0ACA2208, 0x4A, 7, 0},
    {0x0ACA220C, 0x4A, 7, 0},
    {0x0ACA2210, 0x4A, 7, 0},
    {0x0ACA2214, 0x49, 7, 0},
    {0x0ACA2218, 0x49, 7, 0},
    {0x0ACA221C, 0x49, 7, 0},
    {0x0ACA2220, 0x49, 7, 0},
    {0x0ACA2224, 0x48, 7, 0},
    {0x0ACA2228, 0x48, 7, 0},
    {0x0ACA222C, 0x47, 7, 0},
    {0x0ACA2230, 0x48, 7, 0},
    {0x0ACA2234, 0x47, 7, 0},
    {0x0ACA2238, 0x47, 7, 0},
    {0x0ACA223C, 0x47, 7, 0},
    {0x0ACA2240, 0x46, 7, 0},
    {0x0ACA2244, 0x47, 7, 0},
    {0x0ACA2248, 0x46, 7, 0},
    {0x0ACA224C, 0x46, 7, 0},
    {0x0ACA2250, 0x47, 7, 0},
    {0x0ACA2254, 0x47, 7, 0},
    {0x0ACA2258, 0x47, 7, 0},
    {0x0ACA225C, 0x47, 7, 0},
    {0x0ACA2260, 0x48, 7, 0},
    {0x0ACA2264, 0x48, 7, 0},
    {0x0ACA2268, 0x49, 7, 0},
    {0x0ACA226C, 0x49, 7, 0},
    {0x0ACA2270, 0x4A, 7, 0},
    {0x0ACA2274, 0x4B, 7, 0},
    {0x0ACA2278, 0x4C, 7, 0},
    {0x0ACA227C, 0x4C, 7, 0},
    {0x0ACA2280, 0x4D, 7, 0},
    {0x0ACA2284, 0x4F, 7, 0},
    {0x0ACA2288, 0x50, 7, 0},
    {0x0ACA228C, 0x51, 7, 0},
    {0x0ACA2290, 0x52, 7, 0},
    {0x0ACA2294, 0x54, 7, 0},
    {0x0ACA2298, 0x56, 7, 0},
    {0x0ACA229C, 0x57, 7, 0},
    {0x0ACA22A0, 0x59, 7, 0},
    {0x0ACA22A4, 0x5B, 7, 0},
    {0x0ACA22A8, 0x5D, 7, 0},
    {0x0ACA22AC, 0x5F, 7, 0},
    {0x0ACA22B0, 0x61, 7, 0},
    {0x0ACA22B4, 0x63, 7, 0},
    {0x0ACA22B8, 0x65, 7, 0},
    {0x0ACA22BC, 0x68, 7, 0},
    {0x0ACA22C0, 0x6A, 7, 0},
    {0x0ACA22C4, 0x6D, 7, 0},
    {0x0ACA22C8, 0x70, 7, 0},
    {0x0ACA22CC, 0x73, 7, 0},
    {0x0ACA22D0, 0x76, 7, 0},
    {0x0ACA22D4, 0x79, 7, 0},
    {0x0ACA22D8, 0x7D, 7, 0},
    {0x0ACA22DC, 0x80, 7, 0},
    {0x0ACA22E0, 0x84, 7, 0},
    {0x0ACA22E4, 0x88, 7, 0},
    {0x0ACA22E8, 0x8D, 7, 0},
    {0x0ACA22EC, 0x92, 7, 0},
    {0x0ACA22F0, 0x97, 7, 0},
    {0x0ACA22F4, 0x9A, 7, 0},

};

#define NTP_COMP_REG_BW20_U10_NUM sizeof(NTPCOMPRegsBW20M_U10)/sizeof(NTPCOMPRegsBW20M_U10[0])

static RegSetting_t NTPCOMPRegsBW20M_L10[]=
{
    {0x0ACA010C, 0x00, 19, 19},//params_prc_bypass

    {0x0ACA21FC, 0x9A, 7, 0},
    {0x0ACA2200, 0x97, 7, 0},
    {0x0ACA2204, 0x92, 7, 0},
    {0x0ACA2208, 0x8D, 7, 0},
    {0x0ACA220C, 0x88, 7, 0},
    {0x0ACA2210, 0x84, 7, 0},
    {0x0ACA2214, 0x80, 7, 0},
    {0x0ACA2218, 0x7D, 7, 0},
    {0x0ACA221C, 0x79, 7, 0},
    {0x0ACA2220, 0x76, 7, 0},
    {0x0ACA2224, 0x73, 7, 0},
    {0x0ACA2228, 0x70, 7, 0},
    {0x0ACA222C, 0x6D, 7, 0},
    {0x0ACA2230, 0x6A, 7, 0},
    {0x0ACA2234, 0x68, 7, 0},
    {0x0ACA2238, 0x65, 7, 0},
    {0x0ACA223C, 0x63, 7, 0},
    {0x0ACA2240, 0x61, 7, 0},
    {0x0ACA2244, 0x5F, 7, 0},
    {0x0ACA2248, 0x5D, 7, 0},
    {0x0ACA224C, 0x5B, 7, 0},
    {0x0ACA2250, 0x59, 7, 0},
    {0x0ACA2254, 0x57, 7, 0},
    {0x0ACA2258, 0x56, 7, 0},
    {0x0ACA225C, 0x54, 7, 0},
    {0x0ACA2260, 0x52, 7, 0},
    {0x0ACA2264, 0x51, 7, 0},
    {0x0ACA2268, 0x50, 7, 0},
    {0x0ACA226C, 0x4F, 7, 0},
    {0x0ACA2270, 0x4D, 7, 0},
    {0x0ACA2274, 0x4C, 7, 0},
    {0x0ACA2278, 0x4C, 7, 0},
    {0x0ACA227C, 0x4B, 7, 0},
    {0x0ACA2280, 0x4A, 7, 0},
    {0x0ACA2284, 0x49, 7, 0},
    {0x0ACA2288, 0x49, 7, 0},
    {0x0ACA228C, 0x48, 7, 0},
    {0x0ACA2290, 0x48, 7, 0},
    {0x0ACA2294, 0x47, 7, 0},
    {0x0ACA2298, 0x47, 7, 0},
    {0x0ACA229C, 0x47, 7, 0},
    {0x0ACA22A0, 0x47, 7, 0},
    {0x0ACA22A4, 0x46, 7, 0},
    {0x0ACA22A8, 0x46, 7, 0},
    {0x0ACA22AC, 0x47, 7, 0},
    {0x0ACA22B0, 0x46, 7, 0},
    {0x0ACA22B4, 0x47, 7, 0},
    {0x0ACA22B8, 0x47, 7, 0},
    {0x0ACA22BC, 0x47, 7, 0},
    {0x0ACA22C0, 0x48, 7, 0},
    {0x0ACA22C4, 0x47, 7, 0},
    {0x0ACA22C8, 0x48, 7, 0},
    {0x0ACA22CC, 0x48, 7, 0},
    {0x0ACA22D0, 0x49, 7, 0},
    {0x0ACA22D4, 0x49, 7, 0},
    {0x0ACA22D8, 0x49, 7, 0},
    {0x0ACA22DC, 0x49, 7, 0},
    {0x0ACA22E0, 0x4A, 7, 0},
    {0x0ACA22E4, 0x4A, 7, 0},
    {0x0ACA22E8, 0x4A, 7, 0},
    {0x0ACA22EC, 0x4B, 7, 0},
    {0x0ACA22F0, 0x4B, 7, 0},
    {0x0ACA22F4, 0x4B, 7, 0},
};

#define NTP_COMP_REG_BW20_L10_NUM sizeof(NTPCOMPRegsBW20M_L10)/sizeof(NTPCOMPRegsBW20M_L10[0])

void wifiax_ntp_comp_setting(ContTxParam_t *pContTxParam)
{
    unsigned int i = 0;
    unsigned int RegNum = 0;
    RegSetting_t *pReg = NTPCOMPRegsBW20M;
//	unsigned int addr = 0;
//	int reg_idx = 0;

	if(atbm_hw_priv_chip_id() != HW_CHIP_VERSION_Cronus)
		return;

    if(NULL == pContTxParam)
    {
        return;
    }

    //0ACB8900  40  [12:4]  //params_tx_digital_scale
    ATBMPhyRegBitsSet(0x0ACB8900, 0x40, 9, 4);

#if 0
    ACA0104[9:0]  21c // NtScale_512
    ACA0104[29:20] 21c // NtScale_128
    
   
    ACA0104[19:10] 175 // NtScale_256
    ACA0110[9:0] 175 // NtScale_64
   
    ACA0104[19:10] 104 // NtScale_256
    ACA0110[9:0] 104 // NtScale_64
#endif

    //only valid for 40M
    ATBMPhyRegBitsSet(0x0ACA0104, 0x21C, 10, 0);// NtScale_512
    ATBMPhyRegBitsSet(0x0ACA0104, 0x21C, 10, 20);// NtScale_128

    //precomp
    if(1 == pContTxParam->TxVector1.Bits.ChBW)
    {//Bandwidth 40M
        RegNum = NTP_COMP_REG_BW40_NUM;
        pReg = NTPCOMPRegsBW40M;
    }
    else if(0 == pContTxParam->TxVector1.Bits.ChBW)
    {//Bandwidth 20M
        if(0 == pContTxParam->TxVector1.Bits.ChOffset)
        {//ZERO
            RegNum = NTP_COMP_REG_BW20_NUM;
			if(pContTxParam->precom == 0)
            	pReg = NTPCOMPRegsBW20M;
			else if(pContTxParam->precom == 1)
				pReg = NTPCOMPRegsBW20M_arc8M;
			else if(pContTxParam->precom == 2)
				pReg = NTPCOMPRegsBW20M_arc9M;
            ATBMPhyRegBitsSet(0x0ACA0104, 0x104, 10, 10);// NtScale_256
            ATBMPhyRegBitsSet(0x0ACA0110, 0x104, 10, 0);// NtScale_64
        }
        else if(1 == pContTxParam->TxVector1.Bits.ChOffset)
        {//U10
            RegNum = NTP_COMP_REG_BW20_U10_NUM;
            pReg = NTPCOMPRegsBW20M_U10;
            ATBMPhyRegBitsSet(0x0ACA0104, 0x168, 10, 10);// NtScale_256
            ATBMPhyRegBitsSet(0x0ACA0110, 0x168, 10, 0);// NtScale_64
        }
        else
        {//L10
            RegNum = NTP_COMP_REG_BW20_L10_NUM;
            pReg = NTPCOMPRegsBW20M_L10;
            ATBMPhyRegBitsSet(0x0ACA0104, 0x168, 10, 10);// NtScale_256
            ATBMPhyRegBitsSet(0x0ACA0110, 0x168, 10, 0);// NtScale_64
        }
    }
/*
	if(pContTxParam->precom == 3){
		RegNum = NTP_COMP_REG_BW40_NUM;
        pReg = NTPCOMPRegsBW20M_SRRC;
	
		//SRRC 20M && /FCC/CE high chan
		ATBMPhyRegBitsSet(0x0ACA0104, 0x380, 10, 0);// NtScale_512
        ATBMPhyRegBitsSet(0x0ACA0110, 0x380, 10, 20);// NtScale_128
	}
	else if(pContTxParam->precom == 4){
		RegNum = NTP_COMP_REG_BW40_NUM;
        pReg = NTPCOMPRegsBW40M_SRRC;
		//SRRC 40M && /FCC/CE high chan
		ATBMPhyRegBitsSet(0x0ACA0104, 0x250, 10, 0);// NtScale_512
        ATBMPhyRegBitsSet(0x0ACA0110, 0x250, 10, 20);// NtScale_128
	}
	else if(pContTxParam->precom == 6){
		RegNum = NTP_COMP_REG_BW20_NUM;
        pReg = NTPCOMPRegsBW20M_FCC_CE_Low;
		//20M /FCC/CE low chan
		ATBMPhyRegBitsSet(0x0ACA0104, 0x250, 10, 0);// NtScale_512
        ATBMPhyRegBitsSet(0x0ACA0110, 0x250, 10, 20);// NtScale_128
	}
	else if(pContTxParam->precom == 7){
		RegNum = NTP_COMP_REG_BW40_NUM;
        pReg = NTPCOMPRegsBW40M_FCC_CE_Low;
		//40M /FCC/CE low chan
		ATBMPhyRegBitsSet(0x0ACA0104, 0x250, 10, 0);// NtScale_512
        ATBMPhyRegBitsSet(0x0ACA0110, 0x250, 10, 20);// NtScale_128
	}
	*/
	for(i=0;i<RegNum;i++)
    {
        ATBMPhyRegBitsSet(pReg->Addr, pReg->Val, (pReg->Msb + 1 - pReg->Lsb), pReg->Lsb);
        pReg++;
    }
	
}


//set tx parameters, tx action not starting
void ETF_PHY_TxParamSet(ContTxParam_t *pContTxParam)
{
    ETF_PHY_Cont_Tx_Param_Set(pContTxParam);
    //NTPComp
    wifiax_ntp_comp_setting(pContTxParam);
}

static RegSetting_t BASIC_REG_SCRIPT_TX[]=
{
	{0xACD0080, 0x0   ,0,0}, 
	{0xACB8900, 0x1   ,3,3},
	{0xACB8900, 0x0   ,2,2},	
	{0xACB8908, 0x801 ,21,10},
	{0xACB892C, 0x801 ,11,0},	
	{0xACB892C, 0x801 ,23,12},
	{0xACB8930, 0x801 ,11,0},	
	{0xACB8998, 0x0   ,1,0},	
	{0xACB8A00, 0xc0  ,11,0},	
	{0xACB8A00, 0xc0  ,23,12},
	{0xACB8A04, 0x0   ,11,0},	
	{0xACB8A04, 0x0   ,23,12},
	{0xACB8A08, 0x0   ,11,0},	
	{0xACB8A08, 0x0   ,23,12},
	{0xACB8A0C, 0x1   ,2,0},	
	{0xACB8A0C, 0x1   ,5,3},	
	{0xACB8A10, 0x1   ,2,0},	
	{0xACB8A10, 0x1   ,5,3},	
	{0xACB8AB8, 0x20  ,6,0},	
	{0xACB8AB8, 0x20  ,13,7},	
	{0xACB8AD0, 0x0   ,13,0},	
	{0xACB8ACC, 0x0   ,12,0},	
	{0xACB8A14, 0x1   ,0,0},	
	{0xAC98040, 0x0   ,31,0},
	{0xac980cc, 0x1   ,0,0},
};

#define BASIC_REG_SCRIPT_TX_NUM sizeof(BASIC_REG_SCRIPT_TX)/sizeof(BASIC_REG_SCRIPT_TX[0])

////load Tx basic reigster setting, mem etc
void ETF_PHY_Load_Basic_Reg_TxSetting(void)
{	
    RegSetting_t *pReg = NULL;
	int i;
    unsigned int RegNum = 0;
	
    pReg = BASIC_REG_SCRIPT_TX;
    RegNum = BASIC_REG_SCRIPT_TX_NUM;
	
    for(i=0;i<RegNum;i++)
    {
        ATBMPhyRegBitsSet(pReg->Addr, pReg->Val, (pReg->Msb + 1 - pReg->Lsb), pReg->Lsb);
        pReg++;
    }
}

void ETF_PHY_Chip_Reset(void)
{
	ATBMPhyRegI2CWrite(0xACD0008, 0x04);
	ATBMPhyRegI2CWrite(0xACD0008, 0x07);
}


//stop rx
void ETF_PHY_Cont_Rx_Stop(ContRxParam_t *pRxParam)
{
    ATBMPhyRegBitsSet(0x0ACD0000, 0, 1, 16);//EnableRx        0000        16  ACD0000
}

unsigned int ETF_PHY_ChannelCenterFrequencyGet(unsigned int ChannelNo)
{
	unsigned int ChFreqMHz = 0;
    if((ChannelNo >= 1)&&(ChannelNo <= 13))
    {
        ChFreqMHz = 2412 + 5*(ChannelNo-1);
    }
    else if(ChannelNo < 1)
    {
        ChFreqMHz = 2412;
    }
    else if(ChannelNo > 13)
    {
        ChFreqMHz = 2484;
    }
	return ChFreqMHz;
}

void atbm_ETF_PHY_Cont_Rx_Param_Parse(ETF_PHY_RX_PARAM_T *pRxParam,ContRxParam_t *pContRxParam)
{  
    //UI parameters parse
    switch(pRxParam->BssBw)
    {
        case ATBM_WIFI_BW_20M:
            pContRxParam->BssBw = 0;
			pContRxParam->BandWidthMHz = 20;
            break;
        case ATBM_WIFI_BW_40M:
            pContRxParam->BssBw = 1;
			pContRxParam->BandWidthMHz = 40;
            break;
        case ATBM_WIFI_BW_80M:
            pContRxParam->BssBw = 2;
            break;
        default:
            pContRxParam->BssBw = 0;
            break;
    }
    pContRxParam->FreqMHz = pRxParam->FreqMHz;
    pContRxParam->BssBw = pRxParam->BssBw;
    pContRxParam->FcBand = (pRxParam->FreqMHz > 4000)?(1):(0);

    pContRxParam->StationID0 = pRxParam->StationID0;
    pContRxParam->StationID1 = 0xFFF;
    pContRxParam->StationID2 = 0xFFF;
    pContRxParam->StationID3 = 0xFFF;

    pContRxParam->NRx = pRxParam->TxAntennas;
    pContRxParam->RxAntSel = pRxParam->TxAntSel;
    pContRxParam->VhtGrpId = pRxParam->VhtGrpId;
    pContRxParam->VhtUsrPos = pRxParam->VhtUsrPos;
    pContRxParam->BfReportFbType = pRxParam->BfReportFbType;
    pContRxParam->PrimaryIndex = pRxParam->PrimaryIndex;

    //pContRxParam->FreqMHz = atbm_ETF_PHY_ChannelCenterFrequencyGet(pRxParam->ChannelNum);
}


static RegSetting_t RxRegsBW20M[]=
{
    {0x0ACB89A0, 0x3FD,    9,  0},//rx precompen filter
    {0x0ACB89A0,    0x4,  19, 10},
    {0x0ACB89A0,    0x8,  29, 20},
    {0x0ACB89A4,  0x3EE,   9,  0},
    {0x0ACB89A4,  0x3CC,  19, 10},
    {0x0ACB89A4,  0x179,  29, 20},
};
#define RX_REG_BW20_NUM sizeof(RxRegsBW20M)/sizeof(RxRegsBW20M[0])

static RegSetting_t RxRegsBW40M[]=
{
    {0x0ACB89A0, 0x3FA,    9,  0},//rx precompen filter
    {0x0ACB89A0,    0xB,  19, 10},
    {0x0ACB89A0,    0xF,  29, 20},
    {0x0ACB89A4,  0x3DF,   9,  0},
    {0x0ACB89A4,  0x39F,  19, 10},
    {0x0ACB89A4,  0x1DE,  29, 20},
};
#define RX_REG_BW40_NUM sizeof(RxRegsBW40M)/sizeof(RxRegsBW40M[0])

//rx counter reset
void wifiax_cont_rx_cnt_rst(void)
{
    ATBMPhyRegBitsSet(0x0AC88334, 1, 1,  8);//params_rst_rnn_evm      334     8   AC88334 0       REG_CFG ro_evm reset
    ATBMPhyRegBitsSet(0x0AC88334, 0, 1,  8);
    
    ATBMPhyRegBitsSet(0x0AC882F4, 1, 1,  2); //ch_len_small_flag_stat_clr
    ATBMPhyRegBitsSet(0x0AC882F4, 0, 1, 2);

    ATBMPhyRegBitsSet(0x0ACD0020, 1, 1,  0); //params_rst_frame_num        0020        0   ACD0020 0
    ATBMPhyRegBitsSet(0x0ACD0020, 0, 1, 0);

    ATBMPhyRegBitsSet(0x0AC90298, 1, 1,  0); //params_rst_ro_sounding_counter      0298        0:0 AC90298
    ATBMPhyRegBitsSet(0x0AC90298, 0, 1, 0);
}

void atbm_ETF_wifiax_cont_rx_cnt_rst(void)
{
    HW_WRITE_REG_BIT(0x0AC88058,19,19, 1);//params_rst_rnn_evm
    HW_WRITE_REG_BIT(0x0AC88058,19,19, 0);

    HW_WRITE_REG_BIT(0x0AC88058,20,20, 1); //ch_len_small_flag_stat_clr
    HW_WRITE_REG_BIT(0x0AC88058,20,20, 0);

    HW_WRITE_REG_BIT(0x0ACD0020,0,0, 1); //params_rst_frame_num
    HW_WRITE_REG_BIT(0x0ACD0020,0,0, 0);

    HW_WRITE_REG_BIT(0x0AC90044,16,16, 1); //params_rst_ro_sounding_counter
    HW_WRITE_REG_BIT(0x0AC90044,16,16, 0);

    //    params_rst_violation_unsupport_cnt 
    //    params_rst_sig_cnt 
    HW_WRITE_REG_BIT(0x0AC90308,1,0, 0x03);
    HW_WRITE_REG_BIT(0x0AC90308,1,0, 0x00);

    //params_rst_power_up_and_carrier_lost_num 
    HW_WRITE_REG_BIT(0x0AC80C1C,15,15, 0x01);
    HW_WRITE_REG_BIT(0x0AC80C1C,15,15, 0x00);

    HW_WRITE_REG_BIT(0x0A50067C,1,1, 0x01); //clear mac cnt;
    HW_WRITE_REG_BIT(0x0A50067C,1,1, 0x00);

    HW_WRITE_REG_BIT(0x0AC8005C,13,13, 0x01); //sync_locked_num_clr(RDT)
    HW_WRITE_REG_BIT(0x0AC8005C,13,13, 0x00);

    HW_WRITE_REG_BIT(0x0AC8005C,10,10, 0x01); //frame_det_num_clr
    HW_WRITE_REG_BIT(0x0AC8005C,10,10, 0x00);

    HW_WRITE_REG_BIT(0x0AC8005C,9,9, 0x01); //params_det_ok_siir_failed_num_clr
    HW_WRITE_REG_BIT(0x0AC8005C,9,9, 0x00);

    HW_WRITE_REG_BIT(0x0AC90040,21,21, 0x01); //params_ldpc_ro_clr
    HW_WRITE_REG_BIT(0x0AC90040,21,21, 0x00);

    HW_WRITE_REG_BIT(0x0AC8005C,11,11, 0x01); //params_sync_buf_ovfl_cnt_clr
    HW_WRITE_REG_BIT(0x0AC8005C,11,11, 0x00);
}


//start rx
void atbm_ETF_PHY_Cont_Rx_Start(ContRxParam_t *pRxParam)
{
    unsigned char VhtGrpId = pRxParam->VhtGrpId;
    unsigned char VhtUsrPos = pRxParam->VhtUsrPos;


    //rx counter reset
    atbm_ETF_wifiax_cont_rx_cnt_rst();

	//params_clk_cont_force_on		ACD0158		15
	HW_WRITE_REG_BIT(0x0ACD0158, 15, 15, 0x1);

    HW_WRITE_REG_BIT(0x0ACB8004,1,0, pRxParam->BssBw); //Bss_bw: ACB8004 [1:0] 0:20M; 1:40M; 2:80M
    HW_WRITE_REG_BIT(0x0ACB8900,2,1,pRxParam->PrimaryIndex); //Primary_index: ACB8900 [2:1]

    /*
    bbdig_apb_ram_710       ACBDB18     15:0            {params_local_sta_id_vec_2[3:0], params_local_sta_id_vec_1[11:0]}
    bbdig_apb_ram_711       ACBDB1C     15:0            {params_local_sta_id_vec_3[7:0], params_local_sta_id_vec_2[11:4]}
    bbdig_apb_ram_712       ACBDB20     15:0            {params_local_sta_id_vec_4[11:0], params_local_sta_id_vec_3[11:8]}
    */    
    HW_WRITE_REG_BIT(0x0ACBDB18,15,0, ((pRxParam->StationID1&0x0F)<<12)|(pRxParam->StationID0&0xFFF));
    HW_WRITE_REG_BIT(0x0ACBDB1C,15,0, ((pRxParam->StationID2&0xFF)<<8)|((pRxParam->StationID1&0xFF0)>>4));
    HW_WRITE_REG_BIT(0x0ACBDB20,15,4, ((pRxParam->StationID3&0xFFF)<<4)|((pRxParam->StationID2&0xF00)>>8));
    /*
    bbdig_apb_ram_713       ACBDB24     15:0            {params_local_sta_id_vec_backup_2[3:0], params_local_sta_id_vec_backup_1[11:0]}
    bbdig_apb_ram_714       ACBDB28     15:0            {params_local_sta_id_vec_backup_3[7:0], params_local_sta_id_vec_backup_2[11:4]}
    bbdig_apb_ram_715       ACBDB2C     15:0            {params_local_sta_id_vec_backup_4[11:0], params_local_sta_id_vec_backup_3[11:8]}
    */
    HW_WRITE_REG_BIT(0x0ACBDB24,15,0, ((pRxParam->StationID1&0x0F)<<12)|(pRxParam->StationID0&0xFFF));
    HW_WRITE_REG_BIT(0x0ACBDB28,15,0, ((pRxParam->StationID2&0xFF)<<8)|((pRxParam->StationID1&0xFF0)>>4));
    HW_WRITE_REG_BIT(0x0ACBDB2C,15,4, ((pRxParam->StationID3&0xFFF)<<4)|((pRxParam->StationID2&0xF00)>>8));


    //HW_WRITE_REG_BIT(0x0AC88240,8,8, 0);//ac88240 0 [8:8]  //mld_enable
    HW_WRITE_REG_BIT(0x0ACB8B48,1,1, pRxParam->NRx); //N_rx

    //HW_WRITE_REG_BIT(0x0ACB8B48,2,2, pRxParam->RxAntSel); //rx_ant_sel acb8b48 [2:2]

    /*
    bbdig_apb_ram_716   　 ACBDB30 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_717   　 ACBDB34 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_718   　 ACBDB38 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_719   　 ACBDB3C 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_720   　 ACBDB40 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_721   　 ACBDB44 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_722   　 ACBDB48 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_723   　 ACBDB4C 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_724   　 ACBDB50 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_725   　 ACBDB54 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_726   　 ACBDB58 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_727   　 ACBDB5C 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_728   　 ACBDB60 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_729   　 ACBDB64 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_730   　 ACBDB68 　 15:0    　 　 {4'b0, vht_sig_a_read[2:0], vht_sig_a_read[2:0] ,vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    bbdig_apb_ram_731   　 ACBDB6C 　 15:0    　 　 {10'b0, vht_sig_a_read[2:0],vht_sig_a_read[2:0]}
    */
    if(((VhtGrpId >= 1)&&(VhtGrpId <= 62))
        &&((VhtUsrPos >= 1)&&(VhtUsrPos <= 4)))
    {        
        unsigned int VhtUsrPosAddr = 0x0ACBDB30 + ((VhtGrpId-1)>>2)*4;
        unsigned int AddrBitStart = ((VhtGrpId-1)%4)*3;
        HW_WRITE_REG_BIT(VhtUsrPosAddr, AddrBitStart+2, AddrBitStart, (VhtUsrPos - 1)|(1<<2)); //params_vht_user_pos_in_group_id_x
    }

    HW_WRITE_REG_BIT(0x0AC9032C,4,3, pRxParam->BfReportFbType);//params_bf_report_fb_type 4:3 AC9032C

}

void atbm_cronus_ETF_PHY_Cont_Rx_Start(ContRxParam_t *pRxParam)
{
    unsigned int i = 0;
    unsigned int RegNum = 0;
    RegSetting_t *pReg = NULL;

    //RF register setting
    //if(pRxParam->RFRegNeedSet)
    //{
    //   wifiax_rx_rf_set(pRxParam);
    //}

    //rx counter reset
    wifiax_cont_rx_cnt_rst();

    //带宽寄存器：ACB8004 [0]。   1： 40M， 0： 20M
    //params_primary_channel_is_upper_flag      094C        0   ACB894C 1   1: primary is upper; 0: primayr is lower;
    if(20 == pRxParam->BandWidthMHz)
    {
        ATBMPhyRegBitsSet(0x0ACB8004, 0, 1, 0); //bandwidth 20MHz
        RegNum = RX_REG_BW20_NUM;
        pReg = RxRegsBW20M;
    }
    else
    {
        ATBMPhyRegBitsSet(0x0ACB8004, 1, 1, 0); //bandwidth 40MHz
        RegNum = RX_REG_BW40_NUM;
        pReg = RxRegsBW40M;
    }
    for(i=0;i<RegNum;i++)
    {
        ATBMPhyRegBitsSet(pReg->Addr, pReg->Val, (pReg->Msb + 1 - pReg->Lsb), pReg->Lsb);
        pReg++;
    }

    if(1 == pRxParam->PrimaryIndex)
    {
        ATBMPhyRegBitsSet(0x0ACB894C, 1, 1, 0); //U10, primary is upper
    }
    else
    {
        ATBMPhyRegBitsSet(0x0ACB894C, 0, 1, 0); //L10, primayr is lower
    }
    
    ATBMPhyRegBitsSet(0x0AC901B8, pRxParam->StationID0, 12, 0); //StationID0
    ATBMPhyRegBitsSet(0x0AC901BC, pRxParam->StationID1, 12, 0); //StationID1
    ATBMPhyRegBitsSet(0x0AC901C0, pRxParam->StationID2, 12, 0); //StationID2
    ATBMPhyRegBitsSet(0x0AC901C4, pRxParam->StationID3, 12, 0); //StationID3

    //params_fs_over_fc_clock_lock, AC88230 [11:0], fs_over_fc_clock_lock = dec2hex(round[2^18*(20*10^6)/FreqHz])
    //ATBMPhyRegBitsSet(0x0AC88230, pRxParam->FreqMHz, 12, 0); //params_fs_over_fc_clock_lock
}


static RegSetting_t BASIC_REG_SCRIPT_RX[]=
{
	{0xAC80800	,0x25BC60  ,31, 0},
	{0xAC80804	,0x25BC60  ,31, 0},
	{0xAC80808	,0x25BC60  ,31, 0},
	{0xAC8080C	,0x25BC60  ,31, 0},
	{0xAC80810	,0x25BC60  ,31, 0},
	{0xAC80814	,0x25BC60  ,31, 0},
	{0xAC80818	,0x25BC60  ,31, 0},
	{0xAC8081C	,0x25BA64  ,31, 0},
	{0xAC80820	,0x25B868  ,31, 0},
	{0xAC80824	,0x25B66C  ,31, 0},
	{0xAC80828	,0x25B46D  ,31, 0},
	{0xAC8082C	,0x25B26E  ,31, 0},
	{0xAC80830	,0x25B07C  ,31, 0},
	{0xAC80834	,0x25AE7D  ,31, 0},
	{0xAC80838	,0x25AC7E  ,31, 0},
	{0xAC8083C	,0x25AA70  ,31, 0},
	{0xAC80840	,0x25A840  ,31, 0},
	{0xAC80844	,0x25A644  ,31, 0},
	{0xAC80848	,0x25A448  ,31, 0},
	{0xAC8084C	,0x25224C  ,31, 0},
	{0xAC80850	,0x24204D  ,31, 0},
	{0xAC80854	,0x231E4E  ,31, 0},
	{0xAC80858	,0x221C5C  ,31, 0},
	{0xAC8085C	,0x211A20  ,31, 0},
	{0xAC80860	,0x201824  ,31, 0},
	{0xAC80864	,0x1F1628  ,31, 0},
	{0xAC80868	,0x1E142C  ,31, 0},
	{0xAC8086C	,0x1D122D  ,31, 0},
	{0xAC80870	,0x1C102E  ,31, 0},
	{0xAC80874	,0x1B0E3C  ,31, 0},
	{0xAC80878	,0x1A0C3D  ,31, 0},
	{0xAC8087C	,0x190A3E  ,31, 0},
	{0xAC80880	,0x180830  ,31, 0},
	{0xAC80884	,0x170631  ,31, 0},
	{0xAC80888	,0x160432  ,31, 0},
	{0xAC8088C	,0x150234  ,31, 0},
	{0xAC80890	,0x140035  ,31, 0},
	{0xAC80894	,0x137E36  ,31, 0},
	{0xAC80898	,0x127C38  ,31, 0},
	{0xAC8089C	,0x117A39  ,31, 0},
	
	{0xAC80444	,0x9a		,9 ,0 },  //one over r
	{0xACB8004	,0x20024002 ,31 ,1 },
	{0xACD0060	,0x2		  ,31 ,0 },
	{0xACD0000	,0x00010000 ,31 ,0 },
	{0xAC90164	,0xFFFFFFFF  ,31 ,0 },
	{0xACB8904	,0xFA,	  31 ,0 },
	{0xAC88320	,0x00000010, 31 ,0 },
	{0xaca805c	,0x4309261A ,31 ,0 },//dsss sync reg
	{0xACA803C	,0x77671F7B ,31 ,0 },// BRAGCT2
	{0xACA8574	,0x090a0f0f ,31 ,0 },// BRDLYSCCA
	{0xACB0078	,0x480035	,31 ,0 },// BRCALC2
	{0xACA8540	,0x0000017f ,31 ,0 },// BRCCACALC
	{0xACA8048	,0x342D6F0	,31 ,0 },// BRAGCT1
	{0xACA8040	,0x77bbd	,31 ,0 },// BRCONF
	{0xac80ca0	,0xf ,7 ,4 },  // power up enable
	{0xAC880AC	,0xc ,5 ,0 }, // params_ch_len_th1_high_snr 00AC	5 ,0	AC880AC 6
	{0xAC880AC	,0xc ,11 ,6 }, // ch_len_th1_low_snr 
	{0xAC880AC	,0xc ,17 ,12 }, // params_ch_len_th2_high_snr	00AC	17 ,12	AC880AC 6
	{0xAC880AC	,0xc ,23 ,18 }, // ch_len_th2_low_snr 
	{0xAC880B0	,0xc ,17 ,12 }, // params_ch_len_th_high_snr	00B0	17 ,12	AC880B0 6
	{0xAC880B0	,0xc ,23 ,18 }, // params_ch_len_th_low_snr 
	{0xAC880C4	,0xc  ,5 ,0 }, //params_ch_len_th1_high_snr_2x
	{0xAC880C4	,0xc  ,11 ,6  }, // params_ch_len_th1_high_snr_4x
	{0xAC880C4	,0xc  ,17 ,12 }, // params_ch_len_th2_high_snr_2x
	{0xAC880C4	,0xc  ,23 ,18 }, // params_ch_len_th2_high_snr_4x
	{0xAC880C8	,0x1e ,5 ,0 },	 //params_ch_len_th3_high_snr_2x
	{0xAC880C8	,0x1e ,11 ,6 }, // params_ch_len_th3_high_snr_4x
	{0xAC880C8	,0xc  ,17 ,12 }, // params_ch_len_th_high_snr_2x 
	{0xAC880C8	,0xc  ,23 ,18 }, // params_ch_len_th_high_snr_4x 
	{0xac80154	 ,0x1c1c4F66   ,29 ,0 },
	{0xac8020c	 ,0x1a		   ,5 ,0 },
	{0xac8020c	 ,0x1a		   ,11 ,6 },
	{0xAC88234 ,0x6 ,3 ,0 }, //thr_bpsk_data 
	{0xac80d40 ,0x6 ,24 ,21 },
	{0xac80444 ,0x1d ,19 ,14 },
	{0xac80464 ,0x0 ,17 ,12 },
	{0xac80434 ,0x0 ,1 ,1 },
	{0xAC80D08 ,0x3F ,11 ,0 }, //params_cca_fsm_watchdog_en  AC80D08  5 ,0
	{0xac80ca0 ,0x1 ,25 ,25 }, // 0 , use sync end of ltf to decide frame end;
	{0xac80ca0 ,0x1 ,0 ,0 },  //  double check suc continue to run dsss;
	{0xAC80CCC ,0xfe ,9 ,0 },  //frame_end_cnt_for_rx_phy_ready_thr1/2/3/4, hex2dec('108')/20=13.2us, filter delay 1.1us, so rx delay 14.3us
	{0xAC80D00 ,0xfe ,9 ,0 },
	{0xAC80D00 ,0xfe ,25 ,16 },
	{0xAC80D04 ,0xfe ,9 ,0 },
	{0xac80cc4 ,0x6  ,23 ,16 }, // 40M_no_initial_cnt
	{0xAC901CC ,0xd  ,22 ,17 }, //ht_min_length
	{0xac901cc ,0x7f ,16 ,9 }, //he_duration_limit, the minimum he data bits
	{0xAC80450	,0x80  ,21 ,14 },	// sat_thr, 8*16+2=130, U(8,-2) 				
	{0xAC80454	,0x10	,5 ,0 },   // n_above_thr_high_20M			
	{0xAC80454	,0x10	,11 ,6 },	// n_above_thr_high_40M 
	{0xAC80454	,0x28	,17 ,12 },	// n_above_thr_low_20M		
	{0xAC80454	,0x28	,23 ,18 },	// n_above_thr_low_40M		
	{0xAC80458	,0xd8  ,7 ,0 },   // pwr_sat_high_thr_dB, 12*4+3 = 51dB 		
	{0xAC80458	,0xd0  ,15 ,8 },	// pwr_sat_low_thr_dB, 12*4=48dB		
	{0xAC80458	,0xcc  ,23 ,16 }, // pwr_sat_not_sat_thr_dB, 11*4 + 1 = 45dB
	{0xAC8045C	,0x1   ,1 ,0 },   // num_sat_buffer 					
	{0xAC8045C	,0x6   ,5 ,2 },   // n_sat_thr_20M						
	{0xAC8045C	,0xb   ,9 ,6 },   // n_sat_thr_40M						
	{0xAC8045C	,0x0   ,13 ,10 },	//continuous_not_sat_points
	{0xAC804BC ,0x0   ,0 ,0 }, // aagc_sat_condition_method
	{0xAC80D48 ,0x8 ,8 ,0 }, //iq_enable_cnt_mode_enable = 0, iq_enable_delay_cnt = 8;
	{0xacb8b34 ,0x1 ,0 ,0 }, // aagc init gain enable
	{0xAC880bc ,0x3c ,31 ,25 },  //aagc high gain value 
	{0xAC901CC ,0x0 ,29 ,29 }, //sig_b_over_16 unsupported
	{0xAC88230 ,0x875 ,11 ,0 }, //fs_over_fc =	dec2hex(round(20e6/2.422e9*2^18)), 2.422e9 Hz is channle 3 frequency;
	{0xAC80D18 ,0x140000 ,21 ,0 }, // cca watch dog cnt , dsss_rx_processing thr,  hex2dec('140000')/20e3=65.5ms, dsss max long packets is 64ms
};

#define BASIC_REG_SCRIPT_RX_NUM sizeof(BASIC_REG_SCRIPT_RX)/sizeof(BASIC_REG_SCRIPT_RX[0])


////load Tx basic reigster setting, mem etc
void ETF_PHY_Load_Basic_Reg_RxSetting(void)
{	
    RegSetting_t *pReg = NULL;
	int i;
    unsigned int RegNum = 0;
	
    pReg = BASIC_REG_SCRIPT_RX;
    RegNum = BASIC_REG_SCRIPT_RX_NUM;
	
    for(i=0;i<RegNum;i++)
    {
        ATBMPhyRegBitsSet(pReg->Addr, pReg->Val, (pReg->Msb + 1 - pReg->Lsb), pReg->Lsb);
        pReg++;
    }
}
ContTxParam_t ContTxParam;

void ETF_PHY_Start_Tx_Step1(ETF_HE_TX_CONFIG_REQ* pItem)
{
	ETF_HE_TX_CONFIG_REQ *pEtfHeTxConfigReq = (ETF_HE_TX_CONFIG_REQ *)pItem;
	ETF_PHY_TX_PARAM_T *pContTxMsgParam= &pEtfHeTxConfigReq->TxConfig;
	
	memset(&ContTxParam, 0, sizeof(ContTxParam_t));
	
	//ETF_PHY_Chip_Reset(); 
	//get current parameter setting from registers
	//ETF_PHY_Cont_Tx_Param_Get(&ContTxParam);
	//stop tx
	ETF_PHY_Cont_Tx_Stop(&ContTxParam);
	//translate user parameter(ETF_PHY_TX_PARAM_T) to driver parameter(ContTxParam_t)
	ETF_PHY_Cont_Tx_Param_Parse(pContTxMsgParam,&ContTxParam);
	//set all paramters except "ContTxParam.nt_contf"
	ETF_PHY_TxParamSet(&ContTxParam);
}
void ETF_PHY_Start_Tx_Step2(ETF_HE_TX_CONFIG_REQ* pItem)
{
	//load register script
	//ETF_PHY_Load_Basic_Reg_TxSetting();

	ETF_PHY_Cont_Tx_Start(&ContTxParam);
	HW_WRITE_REG_BIT(0x0ACD010C, 7, 7, 0x1);
}
void ETF_PHY_Stop_Tx(ETF_HE_TX_CONFIG_REQ* pItem)
{
	//stop tx
	HW_WRITE_REG_BIT(0x0ACD0008, 0, 0, 0x0);
	atbm_SleepMs(100);
	HW_WRITE_REG_BIT(0x0ACD0008, 0, 0, 0x1);
	ETF_PHY_Cont_Tx_Stop(NULL);
	
}
void ETF_PHY_Start_Rx(ETF_HE_RX_CONFIG_REQ* pItem)
{
#if 1
	ETF_HE_RX_CONFIG_REQ *pEtfHeRxConfigReq = (ETF_HE_RX_CONFIG_REQ *)pItem;
	ETF_PHY_RX_PARAM_T *pContRxMsgParam= &pEtfHeRxConfigReq->RxConfig;
	ContRxParam_t ContRxParam;

	//atbm_ETF_PHY_Chip_Reset(); 
	//stop rx
	//atbm_ETF_PHY_Cont_Rx_Stop(NULL);

	atbm_ETF_PHY_Cont_Rx_Param_Parse(pContRxMsgParam,&ContRxParam);
	//start rx
	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
		atbm_cronus_ETF_PHY_Cont_Rx_Start(&ContRxParam);
	else
		atbm_ETF_PHY_Cont_Rx_Start(&ContRxParam);

	//load register script
	//atbm_ETF_PHY_Load_Basic_Reg_RxSetting();
	ATBMPhyRegBitsSet(0x0ACD0000, 1, 1, 16);//EnableRx        0000        16  ACD0000
#else
	ETF_HE_RX_CONFIG_REQ *pEtfHeRxConfigReq = (ETF_HE_RX_CONFIG_REQ *)pItem;
	ETF_PHY_RX_PARAM_T *pContRxMsgParam= &pEtfHeRxConfigReq->RxConfig;
	ContRxParam_t ContRxParam;

	//ETF_PHY_Chip_Reset(); 
	//stop rx
	//ETF_PHY_Cont_Rx_Stop(NULL);

	ETF_PHY_Cont_Rx_Param_Parse(pContRxMsgParam,&ContRxParam);
	//start rx
	ETF_PHY_Cont_Rx_Start(&ContRxParam);

	//load register script
	//ETF_PHY_Load_Basic_Reg_RxSetting();
	ATBMPhyRegBitsSet(0x0ACD0000, 1, 1, 16);//EnableRx        0000        16  ACD0000
#endif	
}
void ETF_PHY_Stop_Rx(ETF_HE_RX_CONFIG_REQ* pItem)
{
	//stop rx
	ETF_PHY_Cont_Rx_Stop(NULL);
}
void ETF_PHY_TxRxParamInit(ETF_HE_TX_CONFIG_REQ* pItem)
{
	ETF_PHY_TX_PARAM_T *pEtfPhyTxParam = &pItem->TxConfig;
	
	pEtfPhyTxParam->FreqMHz=2422;				
	pEtfPhyTxParam->ChannelNum=7;
	pEtfPhyTxParam->WiFiMode=0;
	pEtfPhyTxParam->OFDMMode=2;
	pEtfPhyTxParam->BW=0;
	pEtfPhyTxParam->BssBw = 0;
	pEtfPhyTxParam->ChOffset=0;
	pEtfPhyTxParam->Rate=0;
	pEtfPhyTxParam->GIMode=0;
	pEtfPhyTxParam->PreambleMode=0;
	pEtfPhyTxParam->PSDULen=1024;
	pEtfPhyTxParam->PacketInterval=16;
	pEtfPhyTxParam->DigitalScaler=1;
	pEtfPhyTxParam->PacketNum=0;
	pEtfPhyTxParam->DataRateMbps=0;
	pEtfPhyTxParam->TxMode=0;
	pEtfPhyTxParam->InfiniteLongPacket=0;
	pEtfPhyTxParam->MPDUNum=1;
	pEtfPhyTxParam->MPDULen=0;
	pEtfPhyTxParam->Smoothing=0;
	pEtfPhyTxParam->Sounding=1;
	pEtfPhyTxParam->Aggregation=0;
	pEtfPhyTxParam->STBC=0;
	pEtfPhyTxParam->LTFNum=1;
	pEtfPhyTxParam->BeamFormed=0;
	pEtfPhyTxParam->Doppler=0;
	pEtfPhyTxParam->BurstLen=4090;
	pEtfPhyTxParam->TxopDuration=0;
	pEtfPhyTxParam->NoSigExtn=1;
	pEtfPhyTxParam->ServiceField=0;
	pEtfPhyTxParam->TxPower=0;
	pEtfPhyTxParam->TxStreams=0;
	pEtfPhyTxParam->TxAntennas=0;
	pEtfPhyTxParam->TxAbsPower=0;
	pEtfPhyTxParam->TxPowerModeSel=0;
	pEtfPhyTxParam->StartingStsNum=0;
	pEtfPhyTxParam->HELTFMode=0;
	pEtfPhyTxParam->HESigA2Reserved=0x3;
	pEtfPhyTxParam->SpatialReuse1=0;
	pEtfPhyTxParam->SpatialReuse2=0;
	pEtfPhyTxParam->SpatialReuse3=0;
	pEtfPhyTxParam->SpatialReuse4=0;
	pEtfPhyTxParam->TriggerResponding=0;
	pEtfPhyTxParam->TriggerMethod=0;
	pEtfPhyTxParam->BSSColor=0;
	pEtfPhyTxParam->UplinkFlag=1;
	pEtfPhyTxParam->ScramblerValueEn=0;
	pEtfPhyTxParam->ScramblerValue=0;
	pEtfPhyTxParam->LdpcExtrSysm=0;
	pEtfPhyTxParam->ReservedForMAC=0;
	pEtfPhyTxParam->CFO=0;
	pEtfPhyTxParam->PPM=0;
	pEtfPhyTxParam->DCM=0;
	pEtfPhyTxParam->Coding=0;
	pEtfPhyTxParam->Padding=0;
	pEtfPhyTxParam->AFactor=0;
	pEtfPhyTxParam->PEDisambiguity=0;
	pEtfPhyTxParam->MidamblePeriod=0;
	pEtfPhyTxParam->BeamChange=0;
	pEtfPhyTxParam->RuAllocation=0;
	pEtfPhyTxParam->DefaultRFBW=0;
	pEtfPhyTxParam->StationID0=1440;
}



int atbm_set_channel(struct atbmwifi_common *hw_priv, atbm_uint8 flag)
{
	int ret = -1;
	//set channel
	struct wsm_set_chantype arg = {
			.band = 0,			//0:2.4G,1:5G
			.flag = flag,		//no use
			.channelNumber = hw_priv->etf_channel, // channel number
			.channelType =  hw_priv->etf_channel_type,	// channel type
			};

	ret = wsm_set_chantype_func(hw_priv,&arg,0);
	return ret;
}



#define PRINT_VARIABLE(var)  {wifi_printk(WIFI_DBG_ERROR, "=%d\n", var);}
void ETFTxConfigShow(ETF_PHY_TX_PARAM_T *pEtfPhyTxParam)
{
	PRINT_VARIABLE(pEtfPhyTxParam->FreqMHz				);
	PRINT_VARIABLE(pEtfPhyTxParam->ChannelNum           );
	PRINT_VARIABLE(pEtfPhyTxParam->WiFiMode             );
	PRINT_VARIABLE(pEtfPhyTxParam->OFDMMode             );
	PRINT_VARIABLE(pEtfPhyTxParam->BW                   );
	PRINT_VARIABLE(pEtfPhyTxParam->ChOffset             );
	PRINT_VARIABLE(pEtfPhyTxParam->Rate                 );
	PRINT_VARIABLE(pEtfPhyTxParam->GIMode               );
	PRINT_VARIABLE(pEtfPhyTxParam->PreambleMode         );
	PRINT_VARIABLE(pEtfPhyTxParam->PSDULen              );
	PRINT_VARIABLE(pEtfPhyTxParam->PacketInterval       );
	PRINT_VARIABLE(pEtfPhyTxParam->DigitalScaler        );
	PRINT_VARIABLE(pEtfPhyTxParam->PacketNum            );
	PRINT_VARIABLE(pEtfPhyTxParam->DataRateMbps         );
	PRINT_VARIABLE(pEtfPhyTxParam->TxMode               );
	PRINT_VARIABLE(pEtfPhyTxParam->InfiniteLongPacket   );
	PRINT_VARIABLE(pEtfPhyTxParam->MPDUNum              );
	PRINT_VARIABLE(pEtfPhyTxParam->MPDULen              );
	PRINT_VARIABLE(pEtfPhyTxParam->Smoothing            );
	PRINT_VARIABLE(pEtfPhyTxParam->Sounding             );
	PRINT_VARIABLE(pEtfPhyTxParam->Aggregation          );
	PRINT_VARIABLE(pEtfPhyTxParam->STBC                 );
	PRINT_VARIABLE(pEtfPhyTxParam->LTFNum               );
	PRINT_VARIABLE(pEtfPhyTxParam->BeamFormed           );
	PRINT_VARIABLE(pEtfPhyTxParam->Doppler              );
	PRINT_VARIABLE(pEtfPhyTxParam->BurstLen             );
	PRINT_VARIABLE(pEtfPhyTxParam->TxopDuration         );
	PRINT_VARIABLE(pEtfPhyTxParam->NoSigExtn            );
	PRINT_VARIABLE(pEtfPhyTxParam->ServiceField         );
	PRINT_VARIABLE(pEtfPhyTxParam->TxPower              );
	PRINT_VARIABLE(pEtfPhyTxParam->TxStreams            );
	PRINT_VARIABLE(pEtfPhyTxParam->TxAntennas           );
	PRINT_VARIABLE(pEtfPhyTxParam->TxAbsPower           );
	PRINT_VARIABLE(pEtfPhyTxParam->TxPowerModeSel       );
	PRINT_VARIABLE(pEtfPhyTxParam->StartingStsNum       );
	PRINT_VARIABLE(pEtfPhyTxParam->HELTFMode            );
	PRINT_VARIABLE(pEtfPhyTxParam->HESigA2Reserved      );
	PRINT_VARIABLE(pEtfPhyTxParam->SpatialReuse1        );
	PRINT_VARIABLE(pEtfPhyTxParam->SpatialReuse2        );
	PRINT_VARIABLE(pEtfPhyTxParam->SpatialReuse3        );
	PRINT_VARIABLE(pEtfPhyTxParam->SpatialReuse4        );
	PRINT_VARIABLE(pEtfPhyTxParam->TriggerResponding    );
	PRINT_VARIABLE(pEtfPhyTxParam->TriggerMethod        );
	PRINT_VARIABLE(pEtfPhyTxParam->BSSColor             );
	PRINT_VARIABLE(pEtfPhyTxParam->UplinkFlag           );
	PRINT_VARIABLE(pEtfPhyTxParam->ScramblerValueEn     );
	PRINT_VARIABLE(pEtfPhyTxParam->ScramblerValue       );
	PRINT_VARIABLE(pEtfPhyTxParam->LdpcExtrSysm         );
	PRINT_VARIABLE(pEtfPhyTxParam->ReservedForMAC       );
	PRINT_VARIABLE(pEtfPhyTxParam->CFO                  );
	PRINT_VARIABLE(pEtfPhyTxParam->PPM                  );
	PRINT_VARIABLE(pEtfPhyTxParam->DCM                  );
	PRINT_VARIABLE(pEtfPhyTxParam->Coding               );
	PRINT_VARIABLE(pEtfPhyTxParam->Padding              );
	PRINT_VARIABLE(pEtfPhyTxParam->AFactor              );
	PRINT_VARIABLE(pEtfPhyTxParam->PEDisambiguity       );
	PRINT_VARIABLE(pEtfPhyTxParam->MidamblePeriod       );
	PRINT_VARIABLE(pEtfPhyTxParam->BeamChange           );
	PRINT_VARIABLE(pEtfPhyTxParam->RuAllocation         );
	PRINT_VARIABLE(pEtfPhyTxParam->DefaultRFBW          );
	PRINT_VARIABLE(pEtfPhyTxParam->StationID0           );
}

int WiFiMode;
int OFDMMode;
int ChBW;
int RateIndex;
int g_PacketInterval = 16;
void atbm_etf_tx_rx_param_config(ETF_PHY_TX_PARAM_T *pParamTxRx, int channel, int mode, int rate, int chBW, int chOff, int ldpc)
{

	atbm_uint32 ChipId = 0;

	ChipId = HW_READ_REG_BIT(0x0ACC017C, 31, 0);
	pParamTxRx->ChannelNum = channel;
	pParamTxRx->PrimaryIndex = 0;
	//0:DSSS,else OFDM
	pParamTxRx->WiFiMode = mode>0?ATBM_WIFI_MODE_OFDM:ATBM_WIFI_MODE_DSSS;
	switch(mode){
	case 0:
		break;
	case 1://11g
		pParamTxRx->OFDMMode = ATBM_WIFI_OFDM_MD_LM;
		break;
	case 2://11n
		pParamTxRx->OFDMMode = ATBM_WIFI_OFDM_MD_MM;
		break;
	case 3://11ax HE-SU
		pParamTxRx->OFDMMode = ATBM_WIFI_OFDM_MD_HE_SU;
		pParamTxRx->GIMode = ATBM_WIFI_GILTF_0P8_2X;
		break;
	case 4://11ac VHT
		pParamTxRx->OFDMMode = ATBM_WIFI_OFDM_MD_VHT;
		pParamTxRx->GIMode = ATBM_WIFI_GI_MD_NORMAL;
		break;
	case 5://11ax HE_ER_SU
		pParamTxRx->OFDMMode = ATBM_WIFI_OFDM_MD_HE_ER_SU;
		pParamTxRx->GIMode = ATBM_WIFI_GILTF_0P8_2X;
		break;
	default:
		pParamTxRx->OFDMMode = ATBM_WIFI_OFDM_MD_MM;
		break;
	}
	pParamTxRx->BW = chBW;//0:20M;1:40M;
	pParamTxRx->BssBw = chBW;
	pParamTxRx->ChOffset = chOff;//0:ZERO;1:10U;2:10L;
	pParamTxRx->Rate = rate;		
	if((ldpc) || (rate >= 10))
		pParamTxRx->Coding = 1;//HE-SU MCS11 coding: 0:BCC;1:LDPC

	if((chBW == ATBM_WIFI_BW_20M) && (chOff != ATBM_WIFI_CH_OFFSET_0))
		pParamTxRx->BssBw = ATBM_WIFI_BW_40M;

	if((ChipId == HW_CHIP_VERSION_Oceanus_FM) || (ChipId == HW_CHIP_VERSION_Cronus_Lite_FM))
	{	
		pParamTxRx->HESigA2Reserved = 0x1F;
		pParamTxRx->ScramblerValueEn = 0x02;
		pParamTxRx->ScramblerValue = 0x01;
	}
	else
		pParamTxRx->HESigA2Reserved = 0x03;

	if(pParamTxRx->OFDMMode == ATBM_WIFI_OFDM_MD_HE_SU)
		pParamTxRx->Padding = 2;//16us
		
}

void RxResetCount(void)
{
	// TODO: 鍦ㄦ娣诲姞鎺т欢閫氱煡澶勭悊绋嬪簭浠ｇ爜	
	HW_WRITE_REG_BIT(0x0AC88334, 8, 8, 1);//params_rst_rnn_evm      334     8   AC88334 0       REG_CFG ro_evm reset
	HW_WRITE_REG_BIT(0x0AC88334, 8, 8, 0);

	HW_WRITE_REG_BIT(0x0AC882F4, 2, 2,  1); //ch_len_small_flag_stat_clr
	HW_WRITE_REG_BIT(0x0AC882F4, 2, 2, 0);

	HW_WRITE_REG_BIT(0x0ACD0020, 0, 0, 1); //params_rst_frame_num        0020        0   ACD0020 0
	HW_WRITE_REG_BIT(0x0ACD0020, 0, 0, 0);

	HW_WRITE_REG_BIT(0x0AC90298, 0, 0,  1); //params_rst_ro_sounding_counter      0298        0:0 AC90298
	HW_WRITE_REG_BIT(0x0AC90298, 0, 0, 0);
}
extern atbm_uint32 chipversion;
extern atbm_uint8 ETF_bStartTx;
extern atbm_uint8 ETF_bStartRx;
extern char ch_and_type[20];


extern atbm_uint8 CodeStart;
extern atbm_uint8 CodeEnd;
extern atbm_uint8 ucWriteEfuseFlag;
extern int Atbm_Test_Success;
extern int atbm_test_rx_cnt;
extern int txevm_total;
atbm_uint8 Rf_Channel_Table[]=
{	
	2412 ,//ch1
	2417 ,//ch2
	2422 ,//ch3
	2427 ,//ch4
	2432 ,//ch5
	2437 ,//ch6
	2442 ,//ch7
	2447 ,//ch8
	2452 ,//ch9
	2457 ,//ch10
	2462 ,//ch11
	2467 ,//ch12
	2472 ,//ch13
	2484  //ch14
};

void atbm_wlan_get_freq_value(int chan_number, atbm_uint32 *centrefreq)
{
//	int index = 0;

	if ((chan_number > 0) && (chan_number <= 14))
	{
		*centrefreq = Rf_Channel_Table[chan_number - 1];
	}
	else if ((chan_number > 22) && (chan_number <= 35))
	{
		//*centrefreq = Rf_Special_Channel_Table[chan_number - 23];
	}
	else
	{
		if(chan_number <= 165)
		{//ch36 (5180) ~ ch165 (5825)
			*centrefreq = 5180 + (chan_number-36)*5; //(iChnlID-36)/4*20
		}
	}
}

/*
from caiwei:
表中是不同模式下mcs7/8/9筛选到的种子，其他模式及mcs使用默认种子0x6c(108)

表中为10进制数
模式			20M		40M
Ax mcs7			41		108
Ax mcs8			120		108
Ax mcs9			120		117
11n mcs7		7		108
Ac mcs7			108		108
Ac Mcs8			45		21
Ac Mcs9			-		108
*/
/*
	@bw:1:40M;else 20M
	@rateIndex:[0,41]
*/
void ETFTxConfigSetScrambleSeed(struct atbmwifi_common *hw_priv, int bw, int mode, int rateIndex, atbm_uint8 bRevert)
{
	atbm_uint32 scramble = 0x6C;//默认扰码种子

	if(!((hw_priv->chip_id == HW_CHIP_VERSION_Cronus_Lite_FM) || (hw_priv->chip_id == HW_CHIP_VERSION_Oceanus_FM)))
		return;


	if(bRevert)
	{
		//停止发包后应该恢复到默认扰码种子
		HW_WRITE_REG_BIT(0x0ACD010C, 6, 0, scramble);
	}
	else
	{
		//Oceanus/Cronuslite 不同速率和带宽使用指定的扰码种子进行发包
		if(bw == 1)
		{
			if((mode == 3) && (rateIndex == 9))
				scramble = 0x75;
			else if((mode == 4) && (rateIndex == 8))
				scramble = 0x15;
		}
		else
		{
			if((mode == 2) && (rateIndex == 7))
				scramble = 0x7;
			else if((mode == 3) && (rateIndex == 7))
				scramble = 0x29;
			else if((mode == 3) && ((rateIndex == 8) || (rateIndex == 9)))
				scramble = 0x78;
			else if((mode == 4) && (rateIndex == 8))
				scramble = 0x23;
		}
		HW_WRITE_REG_BIT(0x0ACD010C, 6, 0, scramble);
	}

}


void SingleToneDisable(void)
{
	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
	{
		//force tx disable
		HW_WRITE_REG_BIT(0xACB8004, 21, 20, 0); 
		HW_WRITE_REG_BIT(0xACB8004, 16, 16, 0); 
		HW_WRITE_REG_BIT(0xACB8004, 10, 10, 0);
		HW_WRITE_REG_BIT(0xACB8900, 3, 3, 0);
	}
	else
	{
		HW_WRITE_REG_BIT(0xACB8004, 22, 21, 0); //Tx RF enable control selection
		HW_WRITE_REG_BIT(0xACB8004, 17, 17, 0); //Tx RF enable 
		HW_WRITE_REG_BIT(0xACB8004, 11, 11, 0); //Tx Force On mode  

		HW_WRITE_REG_BIT(0xACB8998, 1, 0, 0);   //params_operative_mode
	}
	 
}



int MacStringToHex(char *mac, atbm_uint8  *umac)
{
	int i = 0, j = 0;
	unsigned char d = 0;
	char ch = 0,buffer[12] = {0};

	if(mac)
		memcpy(buffer, mac, strlen(mac));

    for (i=0;i<12;i++)
    {
        ch = buffer[i];

        if (ch >= '0' && ch <= '9')
        {
            d = (d<<4) | (ch - '0');
        }
        else if (ch >= 'a' && ch <= 'f')
        {
            d = (d<<4) | (ch - 'a' + 10);
        }
        else if (ch >= 'A' && ch <= 'F')
        {
            d = (d<<4) | (ch - 'A' + 10);
        }
		if((i%2 == 1)){
			umac[j++] = d;
			d = 0;
		}
    }

    return 0;
}


/*
	start_rx <channel>,<bw>,<chOff>
	channel: [1,14]
	bw: 0:20M;1:40M;
	chOff: 0:zero;1:Upper;2:Lower
	tips:20M chOff = 0;40M chOff = 1 or chOff = 2;
*/

int atbm_etf_start_rx(int channel ,int bw, int chOff, int mode)
{
	int ret = -1;
	atbm_uint8 ucDbgPrintOpenFlag = 1;
	struct atbmwifi_vif *priv;
	struct atbmwifi_common *hw_priv;
	atbm_uint8 if_id=0;
	ETF_HE_RX_CONFIG_REQ  EtfConfig;


	if(ETF_bStartTx || ETF_bStartRx){
		if(ETF_bStartRx){
			atbm_etf_stop_rx(NULL);
			atbm_SleepMs(500);
		}else{
			wifi_printk(WIFI_DBG_ERROR, "Error! already start_rx, please stop_tx first!\n");
			return -1;
		}
	}
	
	g_EtfRxMode=mode;
	/*
	*parase channel
	*/
	if((channel < 0) || (channel > 165)){
		wifi_printk(WIFI_DBG_ERROR, "atbm_internal_start_rx : channel[%d] err ! \n",channel);
		return 0;
	}
	/*
	*parase bw:0:20M;1:40M;2:RU242;3:RU106;
	*/
	if((bw < 0) || (bw > 1)){
		wifi_printk(WIFI_DBG_ERROR, "atbm_internal_start_rx : bw[%d] err ! \n",bw);
		return 0;
	}
	/*
	*parase chOff:0:zero;1:10U;2:10L;
	*/
	if((chOff < 0) || (chOff > 2)){
		wifi_printk(WIFI_DBG_ERROR, "atbm_internal_start_rx : chOff[%d] err ! \n",chOff);
		return 0;
	}

	if((mode < 0) || (mode > 1)){
		wifi_printk(WIFI_DBG_ERROR, "atbm_internal_start_rx : mode[%d] err ! \n",mode);
		return 0;
	}

	if((bw == 0) && (chOff != 0))
	{
		wifi_printk(WIFI_DBG_ERROR, "Invalid chOffset!chOff[0]\n");
		return 0;
	}

	if((bw == 1) && (chOff == 0))
	{
		wifi_printk(WIFI_DBG_ERROR, "Invalid chOffset!chOff[1,2]\n");
		return 0;
	}
	
	atbm_for_each_vif(&g_hw_prv,priv,if_id){
		if(priv == ATBM_NULL){
			//wifi_printk(WIFI_ALWAYS,"priv==NULL\n");
			continue;
		}
		hw_priv=_atbmwifi_vifpriv_to_hwpriv(priv);
		if(priv->iftype != ATBM_NL80211_IFTYPE_STATION){
			wifi_printk(WIFI_DBG_ERROR, "(tx)iftype is not station mode, try next!\n");
			continue;
		}		

		//bw: 0:20M;1:40M_10U;2:40M_10L
		hw_priv->etf_channel = channel;
		//lmac:0:zero;1:invalid;2:10U;3:10L
		//hamc:0:zero;1:10U;2:10L
		hw_priv->etf_channel_type = chOff?(chOff+1):(chOff);

		hw_priv->etf_channel_type |= (0xa << 16);//use rx flag start rx status timer in lmac
		if(mode == 0)
			hw_priv->etf_channel_type |= (0xb << 24);//use rx flag start rx counter
		else if(mode == 1)
			hw_priv->etf_channel_type |= (0xc << 24);//use rx flag start rx counter
			
		ret = atbm_set_channel(hw_priv, ATBM_BIT(WSM_SET_CHANTYPE_FLAGS__ETF_TEST_START));//set channel and start TPC
		if(ret != 0)
		{
			//up(&hw_priv->scan.lock);
			wifi_printk(WIFI_DBG_ERROR, "atbm_set_channel err,ret:%d\n", ret);
			//goto exit;
		}
		ret=wsm_write_mib(hw_priv, WSM_MIB_ID_DBG_PRINT_TO_HOST,
			&ucDbgPrintOpenFlag, sizeof(ucDbgPrintOpenFlag), priv->if_id);	
		break;
	}
	
	ETF_bStartRx = 1;
		
	ETF_PHY_TxRxParamInit((ETF_HE_TX_CONFIG_REQ*)&EtfConfig);
	EtfConfig.RxConfig.ChannelNum = channel;
	EtfConfig.RxConfig.BW = bw;
	EtfConfig.RxConfig.BssBw = bw;
	EtfConfig.RxConfig.ChOffset = chOff;
	EtfConfig.RxConfig.PrimaryIndex = (chOff==1)?1:0;
	atbm_wlan_get_freq_value(EtfConfig.RxConfig.ChannelNum, &EtfConfig.RxConfig.FreqMHz);
	ETF_PHY_Start_Rx(&EtfConfig);
	
	return ret;
}
int atbm_etf_stop_rx(get_result_rx_data *rx_data)
{
	int i = 0;
	int ret = 0;
	atbm_uint8 ucDbgPrintOpenFlag = 0;
	struct atbmwifi_vif *vif;
	struct atbmwifi_common *hw_priv=&g_hw_prv;
	atbm_int8 rssi_reg = 0;
	if((0 == ETF_bStartRx)){
		wifi_printk(WIFI_DBG_ERROR,"please start start_rx first,then stop_rx\n");
		return -1;
	}

	//close lmac print
	atbm_for_each_vif(hw_priv,vif,i){
		if (vif != NULL)
		{
			wsm_write_mib(hw_priv, WSM_MIB_ID_DBG_PRINT_TO_HOST,
				&ucDbgPrintOpenFlag, sizeof(ucDbgPrintOpenFlag), vif->if_id);
			break;
		}
	}
	hw_priv->etf_channel_type = 0;
	atbm_for_each_vif(hw_priv,vif,i){
			if((vif != NULL)){
				ret = atbm_set_channel(hw_priv, 0);//clear lmac etf flag and cancel rx status timer
				if(ret != 0)
				{
					wifi_printk(WIFI_DBG_ERROR,"atbm_set_channel err,ret:%d\n", ret);
					//goto exit;
				}
				break;
			}
		}

	
	
	ETF_bStartRx = 0;

	{
		atbm_uint32 rxTotal = 0;
		atbm_uint32 rxSuccess = 0;
		atbm_uint32 rxError = 0;

		if(g_EtfRxMode == 0)
		{
			//ofdm
			rxTotal = HW_READ_REG(0x0ACD002C);
			rxError = HW_READ_REG(0x0ACD0024);
		}
		else if(g_EtfRxMode == 1)
		{
			//dsss
			rxTotal = HW_READ_REG(0x0ACD012C);
			rxError = HW_READ_REG(0x0ACD0130);
		}
		else
		{
			rxTotal = HW_READ_REG(0x0ACD002C) + HW_READ_REG(0x0ACD012C);
			rxError = HW_READ_REG(0x0ACD0024) + HW_READ_REG(0x0ACD0130);
		}


		rxSuccess = rxTotal - rxError;
		rssi_reg = HW_READ_REG_BIT(0x0AC80C30,18,11);
		if(rx_data){
			rx_data->rxError = rxError;
			rx_data->rxSuccess = rxSuccess;
			rx_data->rssi = rssi_reg;
		}
		wifi_printk(WIFI_ALWAYS,"\n%s rxSuc:%d,Err:%d,rssi:%d\n",g_EtfRxMode ? "DSSS":"OFDM",rxSuccess,  rxError,rssi_reg);
	}
	RxResetCount();//clear rx statstics
	ETF_PHY_Stop_Rx(NULL);

	return ret;
}

int atbm_get_packet_length(int mode, int rateIndex, int bw)
{
	int packetLen = 0;
#if 1
	//保证每个包至少16个symbol
	atbm_uint32 packetLen_DSSS[] = {16,28,80,156};
	atbm_uint32 packetLen_OFDM_LM[] = {188,284,380,568,756,1136,1512,1700};
	atbm_uint32 packetLen_OFDM_MM[] = {204,408,616,820,1228,1640,1844,2064};
	atbm_uint32 packetLen_OFDM_VHT[] = {204,408,616,820,1228,1640,1844,2048,2456};
	atbm_uint32 packetLen_OFDM_HE[] = {220,440,660,876,1316,1756,1976,2192,2632,2924,3292,3656};
	atbm_uint32 packetLen_OFDM_MM_40M[] = {424,852,1276,1700,2552,3404,3828,4252};
	atbm_uint32 packetLen_OFDM_VHT_40M[] = {440,876,1316,1756,2632,3512,3948,4388,5264,5852,6580,7312};
	atbm_uint32 packetLen_OFDM_HE_40M[] = {424,852,1276,1700,2552,3404,3828,4252,5104,5672};

#else
	atbm_uint32 packetLen_DSSS[] = {12,24,60,124};
	atbm_uint32 packetLen_OFDM_LM[] = {52,76,104,156,204,308,408,460};
	atbm_uint32 packetLen_OFDM_MM[] = {44,88,128,172,256,340,380,424};
	atbm_uint32 packetLen_OFDM_VHT[] = {40,80,120,156,236,312,392,392,468};
	atbm_uint32 packetLen_OFDM_HE[] = {28,60,88,120,180,240,268,300,360,400,452,500};
	atbm_uint32 packetLen_OFDM_MM_40M[] = {84,176,256,340,508,676,760,848};
	atbm_uint32 packetLen_OFDM_VHT_40M[] = {84,164,244,324,488,648,728,812,972,1080};
	atbm_uint32 packetLen_OFDM_HE_40M[] = {60,120,180,240,360,480,540,600,720,800,904,1004};
#endif
	switch(mode){
	case 0://DSSS
		packetLen = packetLen_DSSS[rateIndex];
		break;
	case 1://LM
		packetLen = packetLen_OFDM_LM[rateIndex];
		break;
	case 2://MM
		if(bw)
			packetLen = packetLen_OFDM_MM_40M[rateIndex];
		else
			packetLen = packetLen_OFDM_MM[rateIndex];
		break;
	case 4://VHT
		if(bw)
			packetLen = packetLen_OFDM_VHT_40M[rateIndex];
		else
			packetLen = packetLen_OFDM_VHT[rateIndex];
		break;
	case 3://HE
	case 5://HE
		if(bw)
			packetLen = packetLen_OFDM_HE_40M[rateIndex];
		else
			packetLen = packetLen_OFDM_HE[rateIndex];
		break;
	default:
		packetLen = 40;
		break;
	}
	//atbm_printk_always("mode=%d, bw=%d, rateIndex=%d, packetLen=%d\n", mode, bw, rateIndex, packetLen);
	return packetLen;
}


int atbm_get_packet_interval_us(int mode, int rateIndex, int bw) {
	int packet_interval_us = 0;

	atbm_uint32 interval_us_DSSS[] = {1152,1008,1047,1021};
	atbm_uint32 interval_us_OFDM_LM[] = {2256,2272,2280,2272,2268,2272,2268,2266};
	atbm_uint32 interval_us_OFDM_MM[] = {2259,2176,2274,2270,2267,2270,2269,2286};
	atbm_uint32 interval_us_OFDM_VHT[] = {2259,2259,2274,2270,2267,2270,2269,2268,2267};
	atbm_uint32 interval_us_OFDM_HE[] = {2397,2389,2386,2377,2378,2379,2379,2376,2377,2376,2377,2376};
	atbm_uint32 interval_us_OFDM_MM_40M[] = {2348,2272,2355,2353,2355,2356,2355,2354};
	atbm_uint32 interval_us_OFDM_VHT_40M[] = {2261,2272,2268,2266,2268,2269,2277,2267,2268,2268};
	atbm_uint32 interval_us_OFDM_HE_40M[] = {2389,2377,2378,2379,2377,2378,2376,2377,2376,2377,2376,2376};

	switch(mode){
	case 0://DSSS
		packet_interval_us = interval_us_DSSS[rateIndex];
		break;
	case 1://LM
		packet_interval_us = interval_us_OFDM_LM[rateIndex];
		break;
	case 2://MM
		if(bw)
			packet_interval_us = interval_us_OFDM_MM_40M[rateIndex];
		else
			packet_interval_us = interval_us_OFDM_MM[rateIndex];
		break;
	case 4://VHT
		if(bw)
			packet_interval_us = interval_us_OFDM_VHT_40M[rateIndex];
		else
			packet_interval_us = interval_us_OFDM_VHT[rateIndex];
		break;
	case 3://HE
	case 5://HE
		if(bw)
			packet_interval_us = interval_us_OFDM_HE_40M[rateIndex];
		else
			packet_interval_us = interval_us_OFDM_HE[rateIndex];
		break;
	default:
		packet_interval_us = 620;
		break;
	}
	//atbm_printk_always("mode=%d, bw=%d, rateIndex=%d, packet_interval_us=%d\n", mode, bw, rateIndex, packet_interval_us);
	return packet_interval_us;
}

/*
start_tx chan,mode,rate,bw,chOff,ldpc,len
interface：wlan0 or p2p0 
channel: [1,14]
mode: 0:11b;1:11g:2:11n:3:11ax HE-SU;4:HE-ER_SU
rate:
	11b: [0,1,2,3] --> [1M, 2M, 5.5M, 11M]
	11g: [0,1,2,3,4,5,6,7] --> [6M, 9M, 12M, 18M, 24M, 36M, 48M, 54M]
	11n: [0,1,2,3,4,5,6,7] --> [mcs0, ..... mcs7]
	11ax: [0,11] --> [mcs0, mcs11]
	tips:HE-ER_SU only supported mcs0~mcs2
bw: 0:20M;1:40M;
chOff: 0:zero;
ldpc:0:BBC;1:LDPC
	tips:HE 20M mcs10/mcs11,40M mcs0-mcs11 ldpc must be 1
len      ：packet len
*/
//atbm_uint32 start_tx_sleep_time = 0;
int atbm_etf_start_tx(int channel,int mode, int rateIdx,int  bw, int chOff, int ldpc, int packetLen, int precom)
{
	int ret = -1;
	struct atbmwifi_vif *priv;
	struct atbmwifi_common *hw_priv;
	int if_id = 0;
	int rateMin = 0;
	int rateMax = 0;
	int pkt_len = 0;
	int precom_real = 0;
	ETF_HE_TX_CONFIG_REQ  EtfConfig = {0};
	atbm_uint8 is_5g_flag = 0;
	atbm_uint8 ch_id_mod8 = 0xff;
	int flag20UL = 0;
	int dutyCycle_x[10] = {1,2,4,6,9,13,20,36,81,890};
	
	atbm_for_each_vif(&g_hw_prv,priv,if_id){		
		if(priv == ATBM_NULL){
			//wifi_printk(WIFI_ALWAYS,"priv==NULL\n");
			continue;
		}
		hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
		if(priv->iftype != ATBM_NL80211_IFTYPE_STATION){
			wifi_printk(WIFI_DBG_ERROR, "(tx)iftype is not station mode, try next!\n");
			continue;
		}

		if(ETF_bStartTx || ETF_bStartRx){
			if(ETF_bStartTx){
				atbm_etf_stop_tx();
				atbm_SleepMs(500);
			}else{
				wifi_printk(WIFI_DBG_ERROR,"Error! already start_tx, please stop_rx first!\n");
				return 0;
			}
		}

		/*
		*parase channel
		*/
		if((channel < 0) || (channel > 165)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : channel[%d] err\n",channel);
			return 0;
		}

		if ((channel >= 36) &&  (channel <= 165))//  36 is 5180M
			is_5g_flag = 1;
		else
			is_5g_flag = 0;
	
		/*
		*parase mode
		*/
		if((mode < 0) || (mode > 5)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : mode[%d] err\n",mode);
			return 0;
		}
		/*
		*parase rateIdx
		*/
		switch(mode){
			case 0://DSSS
				rateMin = 0;
				rateMax = 3;
				break;
			case 1://LM
			case 2://MM
				rateMin = 0;
				rateMax = 7;
				break;
			case 3://HE-SU
				rateMin = 0;
				rateMax = 11;
				break;
			case 4://VHT
				rateMin = 0;
				rateMax = (bw)?9:8;//20M:mcs0~mcs8;40M:mcs0~mcs9
				break;
			case 5://HE-ER_SU
				rateMin = 0;
				rateMax = 2;//only support mcs0~mcs2
				break;
			default:
				break;
		};
		if((rateIdx < rateMin) || (rateIdx > rateMax)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : rateIdx[%d] err,rateMin[%d],rateMax[%d]\n",rateIdx,rateMin,rateMax);
			return 0;
		}
		/*
		*parase bw,0:20M,1:40M;
		*/
		if((bw < 0) || (bw > 1)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : bw[%d] err\n",bw);
			return 0;
		}
		//HE_ER_SU only support 20M
		if((mode == 5) && (bw == 1)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : bw[%d] mode[%d]err\n",bw,mode);
			return 0;
		}

		/*
		*parase chOff,0:ZERO
		*/
		if((bw == 1) && (chOff != 0)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : chOff[%d] err\n",chOff);
			return 0;
		}

		/*
		*parase LDPC,0:BBC;1:LDPC
		*/
		if((ldpc < 0) || (ldpc > 1)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : ldpc[%d] err\n",ldpc);
			return 0;
		}
		//HE 40M must use LDPC
		if((mode == 3) && (bw) && (ldpc == 0)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : ldpc[%d],bw[%d],mode[%d] err\n",ldpc,bw,mode);
			return 0;
		}

		/*
		*parase packetLen
		*/
		pkt_len = atbm_get_packet_length(mode, rateIdx, bw);
		if((pkt_len < 0)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : packetLen[%d] err\n",pkt_len);
			return 0;
		}
		if((precom < 0) || (precom > 10)){
			wifi_printk(WIFI_ALWAYS,"atbm_internal_start_tx : precom[%d] err\n",precom);
			return -1;
		}

		if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
		{
			switch(precom){
			case 1://FCC
			case 2:
				precom_real = precom;
				if(bw != 0)
					precom_real = 0;	
				break;
			case 3://SRRC 10dBm 
			case 6://SRRC 3dBm
				precom_real = (precom==3)?9:10;	
				break;
			case 4://FCC
			case 5://CE
				if(mode == 0)//DSSS
					precom_real = 8;
				else{//OFDM
					if(bw == 0){ 
						if(channel==11){
							//bw = 1;
							precom_real = 3; 
						}
						else
							precom_real = 6;
					}
					else{
						precom_real = (channel==9)?4:7;			
					}
				}
				break;
			default:
				precom_real = 0;
				break;
			}
		}
		else
		{
			precom_real = !!precom;
		}

		wifi_printk(WIFI_ALWAYS, "ATBM_NL80211_CHAN_HT40PLUS:%d\n", ATBM_NL80211_CHAN_HT40PLUS);

		if(pkt_len == 0){
		//Prevent USB from being unplugged suddenly in product testing
		//11b 100% duty cycle
			pkt_len = 1024;
		}
		
		ETF_PHY_TxRxParamInit(&EtfConfig);
		
		atbm_etf_tx_rx_param_config(&EtfConfig.TxConfig, channel, mode, rateIdx, bw, chOff, ldpc);
		EtfConfig.TxConfig.PSDULen = pkt_len;
		EtfConfig.TxConfig.precompensation = precom_real;
		if((precom_real) && (g_DutyCycle == 1))
		{
			EtfConfig.TxConfig.PSDULen = 1024;//(packetLen<1024)?1024:packetLen;
			if(EtfConfig.TxConfig.WiFiMode == ATBM_WIFI_MODE_DSSS)
				EtfConfig.TxConfig.PacketInterval = 3;//intervale must >= 3
			else
				EtfConfig.TxConfig.PacketInterval = 6;//srrc/fcc 100% DUTY CYCLE
		}
		else
			EtfConfig.TxConfig.PacketInterval = atbm_get_packet_interval_us(mode, rateIdx, bw)/dutyCycle_x[g_DutyCycle-1];
		hw_priv->etf_channel = channel;
		if(bw == 1)
		{
			//hw_priv->etf_channel = channel + 2;
			hw_priv->etf_channel_type = ATBM_NL80211_CHAN_HT40MINUS;
		}
		else
		{
			//lmac:0:zero;1:invalid;2:10U;3:10L
			//hamc:0:zero;1:10U;2:10L
			hw_priv->etf_channel_type = chOff?(chOff+1):(chOff);
		}

		if(atbm_support_5G(hw_priv) && (is_5g_flag) && ((bw == 0)))
		{
			/*
				11g:6M/9M/12M/18M/24M，11n/ax/ac MCS0~MCS3 使用20M
				其他速率使用20UL
			*/

#if (OCEANUS_5G_ALL_RATE_USE_20UL ==  0)
			if(((mode == 1) && (rateIdx > 4)) || ((mode >= 2) && (rateIdx > 3)))
#endif
			{
				EtfConfig.TxConfig.BssBw = 1;
				ch_id_mod8 = (channel & 0x7);
				if((ch_id_mod8 == 4) || (ch_id_mod8 == 5))
				{
					EtfConfig.TxConfig.PrimaryIndex = 0;//ACB8900 [2:1]
					hw_priv->etf_channel_type = 3;
				}
				else
				{
					EtfConfig.TxConfig.PrimaryIndex = 1;//ACB8900 [2:1]
					hw_priv->etf_channel_type = 2;
				}
				flag20UL = 1;
				atbm_for_each_vif(&g_hw_prv,priv,if_id){
						ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TX_TXBW20M_USE_20UL,&flag20UL, sizeof(flag20UL), priv->if_id);
						break;
				}
			}
		}

		atbm_for_each_vif(&g_hw_prv,priv,if_id){
			if(precom_real < 9)
			{
				//1: lmac config rfpll reg 0x0acc0288 and reduce 1dB
				//5: lmac config DSSS digital gain=1.5dB && ppagain=4dB
				//8: lmac config DSSS digital gain=9dB && ppagain=2.5dB	
				if((precom_real == 1) ||(precom_real == 2))
					ret = wsm_write_mib(hw_priv, WSM_MIB_ID_SET_PRE_COMPENSATION,&precom_real, sizeof(precom_real), priv->if_id);
			}
			ret = atbm_set_channel(hw_priv, ATBM_BIT(WSM_SET_CHANTYPE_FLAGS__ETF_TEST_START));//set channel and start TPC
			if(ret != 0)
			{
				//up(&hw_priv->scan.lock);
				wifi_printk(WIFI_DBG_ERROR, "atbm_set_channel err,ret:%d\n", ret);
				//goto exit;
			}
			break;
		}
		
		if((precom_real >= 3)&&(precom_real <= 10))//FCC/CE/SRRC
		{
			ret = wsm_write_mib(hw_priv, WSM_MIB_ID_SET_PRE_COMPENSATION,&precom, sizeof(precom), 0);
		}

		g_Channel = channel;
		ETF_bStartTx = 1;
		atbm_wlan_get_freq_value(EtfConfig.TxConfig.ChannelNum, &EtfConfig.TxConfig.FreqMHz);
		//atbm_ETF_TxConfigShow(&EtfConfig.TxConfig);

		ETFTxConfigSetScrambleSeed(hw_priv, bw, mode, rateIdx, 0);
		//ETFTxConfigShow(&EtfConfig.TxConfig);
		
		ETF_PHY_Start_Tx_Step1(&EtfConfig);
		ETF_PHY_Start_Tx_Step2(&EtfConfig);

		atbm_SleepMs(3000);
		HW_WRITE_REG_BIT(0x0ACD0008, 0, 0, 0x0);
		atbm_SleepMs(100);
		HW_WRITE_REG_BIT(0x0ACD0008, 0, 0, 0x1);
		
		WiFiMode = EtfConfig.TxConfig.WiFiMode;
		OFDMMode = EtfConfig.TxConfig.OFDMMode;
		ChBW = EtfConfig.TxConfig.BW;
		RateIndex = EtfConfig.TxConfig.Rate;
		
		ret = 0;
		break;
	}

	
	return ret;
}

int atbm_etf_stop_tx(void)
{
	int ret = -1;
	struct atbmwifi_vif *priv;
	struct atbmwifi_common *hw_priv;
	atbm_uint8 if_id;
	
	atbm_for_each_vif(&g_hw_prv,priv,if_id){
		if(priv == ATBM_NULL){
			//wifi_printk(WIFI_ALWAYS,"priv==NULL\n");
			continue;
		}
		hw_priv=_atbmwifi_vifpriv_to_hwpriv(priv);
		if(priv->iftype != ATBM_NL80211_IFTYPE_STATION){
			wifi_printk(WIFI_DBG_ERROR, "(tx)iftype is not station mode, try next!\n");
			continue;
		}

		hw_priv->etf_channel = 7;
		hw_priv->etf_channel_type = 0;

		ret = atbm_set_channel(hw_priv, 0);//set channel and start TPC
		if(ret != 0)
		{
			wifi_printk(WIFI_DBG_ERROR, "atbm_set_channel err,ret:%d\n", ret);
		}
	//	g_PacketInterval = 16;//default 16us
		ETFTxConfigSetScrambleSeed(hw_priv, 0, 0, 0, 1);
		if(ETF_bStartTx == 1)
		{
			ETF_PHY_Stop_Tx(NULL);
			wifi_printk(WIFI_ALWAYS, "stop_tx\n");
		}
	   	else if(ETF_bStartTx == 2)
		{
			SingleToneDisable();
			wifi_printk(WIFI_ALWAYS, "stop singletone\n");
		}

		ETF_bStartTx = 0;
		g_Channel = 0;
		ret = 0;
		break;
	}

	return ret;
}

void atbm_etf_set_packet_interval(int interval)
{
	//int interval = 0;

	if(interval < 0)
		interval = 16;
	g_PacketInterval = interval;
}


typedef struct Power_LUT_bit_s{
	atbm_uint32 addr;
	atbm_uint8 endBit;
	atbm_uint8 startBit;
}Power_LUT_bit;
#define PHY_RFIP_INVALID 0xffffffff
#define OCEANUS_TXPOW_LUT_DIGGAIN 38
/*The sequence of 'OceanusPower_LUT_Baisc_Addr_Table' must be the same with 'OCEANUS_TXPOW_LUT_ITERM_e' */
const Power_LUT_bit OceanusPower_LUT_Baisc_Addr_Table[] = {
    {0xACBD000,15,14}, //0 :txabb_degr[1:0]
    {0xACBD000,13,13}, //1 :txabb_input_res_tieh
    {0xACBD000,12,9}, //2 :txabb_gcw[3:0]
    {0xACBD000,8,7}, //3 :txabb_ibleed_bias_adj[1:0]
    {0xACBD000,6,1}, //4 :txabb_ibleed_ctrl[5:0]
    {0xACBD06C,15,10}, //5 :txabb_lpf_ccal[5:0]
    {0xACBD06C,9,7}, //6 :txabb_lpf2_tune[2:0]
    {0xACBD06C,6,5}, //7 :txabb_bandwidth_sel[1:0]
    {0xACBD06C,4,4}, //8 :txdac_gain
    {0xACBD06C,3,0}, //9 :txdac_iref_trim[3:0]
    {0xACBD288,10,9}, //10 :txrf_rf_casc_ctrl[1:0]
    {0xACBD0D8,15,15}, //11 :txpa_adptbias_en_LB
    {0xACBD0D8,14,10}, //12 :txpa_adptbias_tune_LB[4:0]
    {0xACBD0D8,9,9}, //13 :txpa_bulk_hr_en_LB
    {0xACBD0D8,8,6}, //14 :txpa_drv_ics_tune_LB[2:0]
    {0xACBD0D8,5,3}, //15 :txpa_drv_vcg_tune_LB[2:0]
    {0xACBD0D8,2,0}, //16 :txpa_pow_ics_tune_LB[2:0]
    {0xACBD144,15,13}, //17 :txpa_pow_vcg_tune_LB[2:0]
    {0xACBD144,12,12}, //18 :txpa_bias_opa_en_LB
    {0xACBD144,11,8}, //19 :txpa_vpmos_tune_LB[3:0]
    {0xACBD144,7,4}, //20 :txpa_vphc_tune_LB[3:0]
    {0xACBD144,3,2}, //21 :txpa_drv_bypass_en_LB[1:0]
    {0xACBD144,1,1}, //22 :txpa_drv_ics_lp_en_LB
    {0xACBD144,0,0}, //23 :txpa_pow_ics_lp_en_LB
    {0xACBD1B0,15,15}, //24 :txpa_adptbias_en
    {0xACBD1B0,14,10}, //25 :txpa_adptbias_tune[4:0]
    {0xACBD1B0,9,9}, //26 :txpa_bulk_hr_en
    {0xACBD1B0,8,5}, //27 :txpa_drv_ics_tune1[3:0]
    {0xACBD1B0,4,1}, //28 :txpa_drv_ics_tune2[3:0]
    {0xACBD21C,15,13}, //29 :txpa_drv_vcg_tune[2:0]
    {0xACBD21C,12,9}, //30 :txpa_pow_ics_tune1[3:0]
    {0xACBD21C,8,5}, //31 :txpa_pow_ics_tune2[3:0]
    {0xACBD21C,4,1}, //32 :txpa_pow_ics_tune3[3:0]
    {0xACBD288,15,13}, //33 :txpa_pow_vcg_tune[2:0]
    {0xACBD288,12,11}, //34 :txpa_drv_bypass_en[1:0]
    {0xACBD2F4,15,12}, //35 :txpa_vpmos_tune[3:0]
    {0xACBD360,15,12}, //36 :txpa_vphc_tune[3:0]
    {0xACBD3CC,15,8}, //37 :txrf_mixer_gain[7:0]
    {0xACBD288,8,0}, //38 :DigGain (dB)
    {0xACBD2F4,11,3}, //39 :txEPSILON[8:0]
    {0xACBD360,11,2}, //40 :txPHI[9:0]
    {0xACBD438,15,4}, //41 :txDIGOFFSETI[11:0]
    {0xACBD4A4,15,4}, //42 :txDIGOFFSETQ[11:0]

	{PHY_RFIP_INVALID,	31, 0},
};

/*
'PowerIndex' is absolute index of all tx power LUTs, BLE: 0~3, DSSS: 4~5, OFDM 20M: 6~15, OFDM 40M: 16~25
*/
void  atbm_ETF_OceanusChip_PHY_RF_TX_Force_Set(atbm_uint8 SrcPowerIndex, ATBM_BOOL TxForceEn)
{
	atbm_uint32 regval = 0;
	atbm_uint32 SrcOffset = 0;
	atbm_uint32 SrcAddr = 0;
	atbm_uint32 DstOffset = 0;
	atbm_uint32 DstAddr = 0;
	atbm_uint32 idx = 0;
    atbm_uint8 ForcePowerIndex = 0x1A;
    atbm_uint32 params_tx_force_en = 0;
    Power_LUT_bit *pTxPowLutBaseAddr = (Power_LUT_bit *)OceanusPower_LUT_Baisc_Addr_Table;

    HW_WRITE_REG_BIT(0xACB8904,5,1,ForcePowerIndex);//params_tx_force_idx     ACB8904 U(5,0)  5:1

    params_tx_force_en = HW_READ_REG_BIT(0xACB8904,0,0); //params_tx_force_en

    if(TxForceEn || (1 == params_tx_force_en))
    {
        HW_WRITE_REG_BIT(0xACB8904,0,0,0x00); //params_tx_force_en
    }

	SrcOffset = (SrcPowerIndex<<2); //PowerIndex is absolute index of all tx power LUTs
    DstOffset = (ForcePowerIndex<<2);

    if(DstOffset != SrcOffset)
    {
    	while(pTxPowLutBaseAddr->addr != PHY_RFIP_INVALID)
    	{
    	    SrcAddr = pTxPowLutBaseAddr->addr + SrcOffset;
            DstAddr = pTxPowLutBaseAddr->addr + DstOffset;
            regval = HW_READ_REG_BIT(SrcAddr,pTxPowLutBaseAddr->endBit,pTxPowLutBaseAddr->startBit);
    		//int gain = N_BIT_TO_SIGNED_32BIT(regval, 8);
    		if(OCEANUS_TXPOW_LUT_DIGGAIN == idx)
    		{
    			regval = 0x71;//pow(10.0,(gain/4/20.0))*pow(2.0,6);//dB to linear
    		}

    		//CS_PRINT("0x%x[%d:%d]=%x  --> ", SrcAddr,pTxPowLutBaseAddr->endBit,pTxPowLutBaseAddr->startBit, regval);

    		//CS_PRINT("0x%x[%d:%d]\n", DstAddr,pTxPowLutBaseAddr->endBit,pTxPowLutBaseAddr->startBit);

    		HW_WRITE_REG_BIT(DstAddr,pTxPowLutBaseAddr->endBit,pTxPowLutBaseAddr->startBit,regval);
    		pTxPowLutBaseAddr++;
    		idx++;
    	}
    }

    if(TxForceEn || (1 == params_tx_force_en))
    {
        HW_WRITE_REG_BIT(0xACB8904,0,0,0x01); //params_tx_force_en
    }
}

Power_LUT_bit Power_LUT_force_Addr[] = {		
	{0xACB8910,  6, 0}, 	//txPAPOWERTRIM 
	{0xACB8900,  12, 4},	//DigGain (dB)	
	{0xACB8910,  24, 20},	//txpa_adptbias_tune	
	{0xACB8910,  19, 19},	//txpa_adptbias_en	
	{0xACB8910,  18, 16},  //txPAPOWICSTUNE 
	{0xACB8ADC,  3, 2}, //	txpa_driver_cell_select 
	{0xACB8910,  15, 10},  //txPAVPMOSTUNE	
	{0xACB8900,  22, 13},  //PpaGain	
	{0xACB8910,  9, 7}, //	txPADRIVERBIASCURRENTTRIM	
	{0xACB8910,  25,25},		// txpa_bulk_hr_en	
	{0xACB8910,  31, 29},  //txpa_drv_vcg_tune		
	{0xACB8910,  28, 26},  //txpa_pow_vcg_tune	
	{0xACB8ADC,  13, 10},  //txpa_power_dynamic_bias	
	{0xACB8ADC,  9, 6}, //	txpa_driver_dynamic_bias	
	{0xACB8ADC,  5,5},		// txpa_power_dynamic_bias_en	
	{0xACB8ADC,  4,4},		// txpa_driver_dynamic_bias_en	
	{0xACB8ADC,  1,1},		// txpa_driver_lowr 
	{0xACB8ADC,  0,0},		// txpa_mixer_lowr	
	{0xACB8914,  0,0},		// txdac_gain	
	{0xACB8914,  11, 7},	//txabb_lpf_ccal	
	{0xACB8914,  12,12},		// txabb_bandwidth_sel	
	{0xACB8914,  19, 13},  //txpa_drvstg_pup	
	{0xACB8914,  6, 1}, //	txabb_ibleed_ctrl	
	{0xACB890c,  25, 24},  //rf_reserved_00 (txpa_drv_bypass_en)	
	{0xACB8904,  21, 12},  //txEPSILON	
	{0xACB8908,  9, 0}, //	txPHI	
	{0xACB890c,  11, 0},	//txDIGOFFSETI	
	{0xACB890c,  23, 12},  //txDIGOFFSETQ	
	{PHY_RFIP_INVALID,	31, 0},
};
Power_LUT_bit Power_LUT_Basic_Addr[] = {
	{0xACBD000, 25, 19},//txPAPOWERTRIM
	{0xACBD000, 17, 11},//DigGain
	{0xACBD000, 10, 6},//txpa_adptbias_tune
	{0xACBD000, 5, 5},//txpa_adptbias_en
	{0xACBD000, 4, 2},//txPAPOWICSTUNE
	{0xACBD000, 1, 0},//txpa_driver_cell_select
	{0xACBD070, 25, 20},//txPAVPMOSTUNE
	{0xACBD070, 19, 10},//PpaGain
	{0xACBD070, 9, 7},//txPADRIVERBIASCURRENTTRIM
	{0xACBD070, 6, 6},//txpa_bulk_hr_en
	{0xACBD070, 5, 3},//txpa_drv_vcg_tune
	{0xACBD070, 2, 0},//txpa_pow_vcg_tune
	{0xACBD0E0, 25, 22},//txpa_power_dynamic_bias
	{0xACBD0E0, 21, 18},//txpa_driver_dynamic_bias
	{0xACBD0E0, 17, 17},//txpa_power_dynamic_bias_en
	{0xACBD0E0, 16, 16},//txpa_driver_dynamic_bias_en
	{0xACBD0E0, 15, 15},//txpa_driver_lowr
	{0xACBD0E0, 14, 14},//txpa_mixer_lowr
	{0xACBD0E0, 13, 13},//txdac_gain
	{0xACBD0E0, 12, 8},//txabb_lpf_ccal
	{0xACBD0E0, 7, 7},//txabb_bandwidth_sel
	{0xACBD0E0, 6, 0},//txpa_drvstg_pup
	{0xACBD150, 25, 20},//txabb_ibleed_ctrl
	{0xACBD1C0, 25, 24},//rf_reserved_00
	{0xACBD150, 19, 10},//txEPSILON
	{0xACBD150, 9, 0},//txPHI
	{0xACBD1C0, 23, 12},//txDIGOFFSETI
	{0xACBD1C0, 11, 0},//txDIGOFFSETQ
	{PHY_RFIP_INVALID,	31, 0},
};


void  atbm_cronus_PHY_RF_TX_Cal_Force_Set(atbm_uint8 PowerIndex, Power_LUT_bit *pforcetable, Power_LUT_bit *pMemoryGainTable)
{
	atbm_uint32 regval = 0;
	atbm_uint32 AddrOffset = 0;
	atbm_uint32 idx = 0;
	AddrOffset = (PowerIndex<<2);


	while(pMemoryGainTable->addr != PHY_RFIP_INVALID)
	{
		regval = HW_READ_REG_BIT(pMemoryGainTable->addr+AddrOffset,pMemoryGainTable->endBit,pMemoryGainTable->startBit);			
		
		if(idx == 1)
		{
			regval = 0x65;//pow(10.0,(regval/4/20.0))*pow(2.0,6);//dB to linear
		}

		HW_WRITE_REG_BIT(pforcetable->addr,pforcetable->endBit,pforcetable->startBit,regval);
		pMemoryGainTable++;
		pforcetable++;
		idx++;
	}
}


void SingleToneEnable(void)
{
	//0 不发单频； 1 发tone1； 2 发tone2； 3 发tone1 + tone2
	HW_WRITE_REG_BIT(0xACB8998, 1, 0, 1);   //params_operative_mode         


	//HW_WRITE_REG_BIT(0xACB899C, 6, 0, 0xa);  //f_leakage                                       
	//HW_WRITE_REG_BIT(0xACB899C, 13, 7, 0x10); //f_tone                                          
	//HW_WRITE_REG_BIT(0xACB899C, 20, 14, 4); //f_image     
	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
	{
		HW_WRITE_REG_BIT(0xACB8A00, 11, 0,  0); //tone1STEP, value*312.5k = fc                   
		HW_WRITE_REG_BIT(0xACB8A00, 23, 12, 0); //tone2STEP, value*312.5k = fc                    
		HW_WRITE_REG_BIT(0xACB8A04, 11, 0,  0); //tone1DELTAI,                                    
		HW_WRITE_REG_BIT(0xACB8A04, 23, 12, 0); //tone1DELTAQ,                                   
		HW_WRITE_REG_BIT(0xACB8A08, 11, 0,  0); //tone2DELTAI,                                    
		HW_WRITE_REG_BIT(0xACB8A08, 23, 12, 0); //tone2DELTAQ,                                   
		HW_WRITE_REG_BIT(0xACB8A0C, 2, 0, 2); //tone1AMPI                                       
		HW_WRITE_REG_BIT(0xACB8A0C, 5, 3, 2); //tone1AMPQ                                       
		HW_WRITE_REG_BIT(0xACB8A10, 2, 0, 2); //tone2AMPI                                       
		HW_WRITE_REG_BIT(0xACB8A10, 5, 3, 2); //tone2AMPQ   

		atbm_cronus_PHY_RF_TX_Cal_Force_Set(15, Power_LUT_force_Addr, Power_LUT_Basic_Addr);

		// force tx enable 
		HW_WRITE_REG_BIT(0xACB8004, 21, 20, 1);	//Tx RF enable control selection
		HW_WRITE_REG_BIT(0xACB8004, 16, 16, 1);	//Tx RF enable 
		HW_WRITE_REG_BIT(0xACB8004, 10, 10, 1); //Tx Force On mode    	
		HW_WRITE_REG_BIT(0xACB8900, 3, 3, 1); 	//params_tx_force_en
	}
	else
	{
		HW_WRITE_REG_BIT(0xACB8A00, 11, 0,  0); //tone1STEP, value*312.5k = fc                   
		HW_WRITE_REG_BIT(0xACB8A00, 23, 12, 0); //tone2STEP, value*312.5k = fc    

		HW_WRITE_REG_BIT(0xACB8A04, 11, 0,  0); //tone1DELTAI,                                    
		HW_WRITE_REG_BIT(0xACB8A08, 11, 0,  0); //tone2DELTAI,   
		HW_WRITE_REG_BIT(0xACB8A0C, 2, 0, 2); //tone1AMPI
		HW_WRITE_REG_BIT(0xACB8A10, 2, 0, 2); //tone2AMPI


		atbm_ETF_OceanusChip_PHY_RF_TX_Force_Set(13,1);//10dBm
	/***********************************************************************
	ApbSel_TxRfEnable(ACB8004 [22:21]) set to be 0x02 to send Tone.
	If ApbSel_TxRfEnable(ACB8004 [22:21]) set to be 0x01 for sending Tone, 
	params_TxRfEnable_use_old_design(ACB866C [18]) need to be 1.
	************************************************************************/
		HW_WRITE_REG_BIT(0xACB8004, 22, 21, 2); //Tx RF enable control selection
		HW_WRITE_REG_BIT(0xACB8004, 17, 17, 1); //Tx RF enable 
		HW_WRITE_REG_BIT(0xACB8004, 11, 11, 1); //Tx Force On mode  
	}
	    
}



int atbm_etf_start_tx_single_tone(int channel)
{
	int ret = -1;
	struct atbmwifi_vif *priv;
	struct atbmwifi_common *hw_priv;
	int if_id;
	//int channel = 0;
	int mode = 2;
	int rateIdx = 7;
	int bw = 0;
	int chOff = 0;
	int ldpc = 0;

	ETF_HE_TX_CONFIG_REQ  EtfConfig;
	
	atbm_for_each_vif(&g_hw_prv,priv,if_id){		
		if(priv == ATBM_NULL){
			//wifi_printk(WIFI_ALWAYS,"priv==NULL\n");
			continue;
		}
		hw_priv = _atbmwifi_vifpriv_to_hwpriv(priv);
		if(priv->iftype != ATBM_NL80211_IFTYPE_STATION){
			wifi_printk(WIFI_DBG_ERROR, "(tx)iftype is not station mode, try next!\n");
			continue;
		}

		if(ETF_bStartTx || ETF_bStartRx){
			wifi_printk(WIFI_DBG_ERROR, "Error! already ETF_bStartRx/ETF_bStartTx, please stop first!\n");
			return 0;
		}

		/*
		*parase channel
		*/
		if((channel < 0) || (channel > 165)){
			wifi_printk(WIFI_DBG_ERROR,"atbm_internal_start_tx : channel[%d] err\n",channel);
			return 0;
		}
		/*
		*parase mode
		*/
		

		wifi_printk(WIFI_ALWAYS, "ATBM_NL80211_CHAN_HT40PLUS:%d\n", ATBM_NL80211_CHAN_HT40PLUS);

		ETF_PHY_TxRxParamInit(&EtfConfig);
		
		atbm_etf_tx_rx_param_config(&EtfConfig.TxConfig, channel, mode, rateIdx, bw, chOff, ldpc);
		EtfConfig.TxConfig.PSDULen = 1024;
		
		hw_priv->etf_channel = channel;
		hw_priv->etf_channel_type = 0;
		
		ret = atbm_set_channel(hw_priv, ATBM_BIT(WSM_SET_CHANTYPE_FLAGS__ETF_TEST_START));//set channel and start TPC
		if(ret != 0)
		{
			//up(&hw_priv->scan.lock);
			wifi_printk(WIFI_ALWAYS, "atbm_set_channel err,ret:%d\n", ret);
			//goto exit;
		}

		
		ETF_bStartTx = 2;

		//ETFTxConfigShow(&EtfConfig.TxConfig);
		
		ETF_PHY_Start_Tx_Step1(&EtfConfig);
		ETF_PHY_Start_Tx_Step2(&EtfConfig);

		ret = atbm_set_channel(hw_priv, 0);//set channel and start TPC
		if(ret != 0)
		{
			//up(&hw_priv->scan.lock);
			wifi_printk(WIFI_ALWAYS, "atbm_set_channel err,ret:%d\n", ret);
			//goto exit;
		}

		ETF_PHY_Stop_Tx(NULL);

		SingleToneEnable();
	
		
		ret = 0;
		break;
	}

	
	return ret;
}

atbm_uint32 MyRand(void)
{
	atbm_uint32 random_num = 0;
	atbm_uint32 randseed = 0;

	randseed = atbm_GetOsTimeMs();
	random_num = randseed * 1103515245 + 12345;
	return ((random_num/65536)%32768);
}


int atbm_etf_PT_Test_start(atbm_uint8 if_id,/*atbm_int32 targetFreq, atbm_int32 rssiFilter, atbm_int32 evmFilter, atbm_int32 cableLoss, */atbm_int32 isWriteEfuse)
{
	struct atbmwifi_vif *priv;
	struct atbmwifi_common *hw_priv;
	atbm_for_each_vif(&g_hw_prv,priv,if_id){
		if(priv == ATBM_NULL){
			//wifi_printk(WIFI_ALWAYS,"priv==NULL\n");
			continue;
		}
		hw_priv=_atbmwifi_vifpriv_to_hwpriv(priv);
		atbm_memset(&gRxs_s, 0, sizeof(struct rxstatus_signed));
		atbm_memset(&gthreshold_param, 0, sizeof(struct test_threshold));

		hw_priv->etf_channel = 7;
		hw_priv->etf_channel_type = 0;
		hw_priv->etf_rate = WSM_TRANSMIT_RATE_HT_65;
		hw_priv->etf_len = 1000; 
		hw_priv->etf_greedfiled = 0;
		priv->scan.in_progress = 0;
		ucWriteEfuseFlag = isWriteEfuse;

		gthreshold_param.featureid = MyRand();
		gthreshold_param.freq_ppm = TARGET_FREQOFFSET_HZ;
		gthreshold_param.rssifilter = -100;
		gthreshold_param.rxevm = 400;
		gthreshold_param.txevm = 400;
		gthreshold_param.txevmthreshold = 400;
		gthreshold_param.rxevmthreshold = 400;
		gthreshold_param.cableloss = 30*4;

		wifi_printk(WIFI_DBG_ERROR, "featureid:%d\n", gthreshold_param.featureid);
		wifi_printk(WIFI_DBG_ERROR, "Freq:%d,txEvm:%d,rxEvm:%d,txevmthreshold:%d,rxevmthreshold:%d,Txpwrmax:%d,Txpwrmin:%d,Rxpwrmax:%d,Rxpwrmin:%d,rssifilter:%d,cableloss:%d\n",
			gthreshold_param.freq_ppm,gthreshold_param.txevm,gthreshold_param.rxevm,gthreshold_param.txevmthreshold,gthreshold_param.rxevmthreshold,
			gthreshold_param.txpwrmax,gthreshold_param.txpwrmin,gthreshold_param.rxpwrmax,
			gthreshold_param.rxpwrmin,gthreshold_param.rssifilter,gthreshold_param.cableloss);

		if(ETF_bStartTx || ETF_bStartRx){
			wifi_printk(WIFI_DBG_ERROR, "Error! already ETF_bStartRx/ETF_bStartTx, please stop first!\n");
			return 0;
		}
		
		g_ProductTestGlobal = 1; //product test flag

		CodeStart = DCXO_CODE_MINI;
		CodeEnd = DCXO_CODE_MAX;
		atbm_test_rx_cnt = 0;
		
		ETF_bStartTx = 1;
		wsm_start_tx_v2(hw_priv,priv);
		break;
	}
	return 0;
}

void etf_set_deltagain(struct efuse_headr efuse)
{
	int delta_gain = 0;

	delta_gain = efuse.delta_gain1==16?0:efuse.delta_gain1;
	delta_gain = _5bitTo32bit(delta_gain);
	HW_WRITE_REG_BIT(0xACB8B28, 6, 0, delta_gain);
	delta_gain = efuse.delta_gain2==16?0:efuse.delta_gain2;
	delta_gain = _5bitTo32bit(delta_gain);
	HW_WRITE_REG_BIT(0xACB8B28, 13, 7, delta_gain);
	delta_gain = efuse.delta_gain3==16?0:efuse.delta_gain3;
	delta_gain = _5bitTo32bit(delta_gain);
	HW_WRITE_REG_BIT(0xACB8B28, 20, 14, delta_gain);
}
void atbm_set_reg_deltagain(struct efuse_headr *efuse)
{
	int delta_gain = 0;
	atbm_uint32 ChipId = 0;
	
	if(efuse == NULL){
		wifi_printk(WIFI_ALWAYS,"atbm_get_reg_deltagain : param is NULL");
		return;
	}


	if ((g_Channel > 0) && (g_Channel <= 4))
	{
		delta_gain = efuse->delta_gain1;
	}else if ((g_Channel >= 5) && (g_Channel <= 9))
	{
		delta_gain = efuse->delta_gain2;
	}else if ((g_Channel >= 10) && (g_Channel <= 14))
	{
		delta_gain = efuse->delta_gain3;
	}

	ChipId = HW_READ_REG_BIT(0x0ACC017C, 31, 0);

	if(atbm_support_5G(&g_hw_prv))
	{
		delta_gain = delta_gain==32?0:delta_gain;
		delta_gain = _6bitTo32bit(delta_gain);
	}
	else
		
	{
		delta_gain = delta_gain==16?0:delta_gain;
		delta_gain = _5bitTo32bit(delta_gain);
	}
	
	HW_WRITE_REG_BIT(0xACB8B28, 6, 0, delta_gain);

	


}

void get_reg_deltagain(struct efuse_headr *efuse)
{
	int delta_gain = 0;
	atbm_uint32 ChipId = 0;
	
	if(efuse == NULL){
		wifi_printk(WIFI_ALWAYS,"atbm_get_reg_deltagain : param is NULL");
		return;
	}
	
	delta_gain = HW_READ_REG_BIT(0xACB8B28, 6, 0);
	
	ChipId = HW_READ_REG_BIT(0x0ACC017C, 31, 0);
	if(atbm_support_5G(&g_hw_prv))
		_32bitTo6bit(delta_gain);
	else
		
		_32bitTo5bit(delta_gain);
	efuse->delta_gain1 = delta_gain;
	
}


int DCXOCodeWrite(struct atbmwifi_common *hw_priv,atbm_uint8 data)
{
	atbm_uint32 uiRegData;


	atbm_direct_read_reg_32(hw_priv,DCXO_TRIM_REG,&uiRegData);

	//hw_priv->sbus_ops->sbus_read_sync(hw_priv->sbus_priv,DCXO_TRIM_REG,&uiRegData,4);
	uiRegData &= ~0x00007F;

	uiRegData |= (data&0x7F);
	
	//hw_priv->sbus_ops->sbus_write_sync(hw_priv->sbus_priv,DCXO_TRIM_REG,&uiRegData,4);
	atbm_direct_write_reg_32(hw_priv,DCXO_TRIM_REG,uiRegData);

	return 0;
}

atbm_uint8 DCXOCodeRead(struct atbmwifi_common *hw_priv)
{	
	atbm_uint32 uiRegData;
	atbm_uint8 dcxo;
	atbm_direct_read_reg_32(hw_priv,DCXO_TRIM_REG,&uiRegData);

	//hw_priv->sbus_ops->sbus_read_sync(hw_priv->sbus_priv,DCXO_TRIM_REG,&uiRegData,4);	
	dcxo = uiRegData&0x7f;
	
	return dcxo;
}


#define N_BIT_TO_SIGNED_32BIT(v,n)	(atbm_int32)(((v) & ATBM_BIT(n-1))?((v)|0xffffffff<<n):(v))



void etf_rx_status_get(struct atbmwifi_common *hw_priv)
{
	int i = 0;
	struct rxstatus rxs; 
	char *extra = NULL;
	struct atbmwifi_vif *vif;
	
	if(!(extra = (char *)atbm_kmalloc(sizeof(struct rxstatus), GFP_KERNEL)))
		return;

	atbm_for_each_vif(hw_priv,vif,i){
		if (vif != NULL)
		{
			/*WARN_ON*/(wsm_read_mib(hw_priv, WSM_MIB_ID_GET_ETF_RX_STATS,
				extra, sizeof(struct rxstatus), vif->if_id));
			break;
		}
	}
	atbm_memcpy(&rxs, extra, sizeof(struct rxstatus));
	
	atbm_kfree(extra);

	if(rxs.probcnt == 0)
		return;
	
	gRxs_s.evm				= rxs.evm/rxs.probcnt;
	gRxs_s.RxRSSI			= (atbm_int16)N_BIT_TO_SIGNED_32BIT(rxs.RSSI, 8)*4;
	gRxs_s.RxRSSI += gthreshold_param.cableloss;

	return;

}

int Test_FreqOffset_v2(struct atbmwifi_common *hw_priv, atbm_uint32 *dcxo, int *pfreqErrorHz)
{
	atbm_uint8 CodeValue,CodeValuebak;
	int b_fail =1;
	int freqErrorHz = 0;
	int targetFreqOffset = TARGET_FREQOFFSET_HZ;

	//if(gthreshold_param.freq_ppm != 0)
		//targetFreqOffset = gthreshold_param.freq_ppm;
		
	CodeValue = DCXOCodeRead(hw_priv);	
	DCXOCodeWrite(hw_priv,CodeValue);	


	wifi_printk(WIFI_DBG_ERROR,"CodeValue default:%d\n",CodeValue);

	
	CodeValuebak = CodeValue;

	freqErrorHz = gRxs_s.Cfo;

	if (freqErrorHz > targetFreqOffset)
	{
		CodeStart = CodeValue;
		CodeValue += (CodeEnd - CodeStart)/2;
		CodeStart = CodeValuebak;

		wifi_printk(WIFI_DBG_ERROR, "freqErrorHz[%d] > targetFreqOffset[%d],CodeValue[%d] ,CodeStart[%d], CodeEnd[%d] . \n",
			freqErrorHz,targetFreqOffset,	CodeValue, CodeStart ,CodeEnd );
		
		DCXOCodeWrite(hw_priv,CodeValue);

		b_fail = 1;
		if (CodeValue >= 126)
		{
			b_fail = 2;
		}
		if (CodeValue >= 0xff)
		{
			b_fail = 2;
		}
	}
	else if ((int)freqErrorHz < -targetFreqOffset)
	{
		CodeEnd = CodeValue;
		CodeValue -= (CodeEnd - CodeStart)/2;
		CodeEnd = CodeValuebak;

		wifi_printk(WIFI_DBG_ERROR, "freqErrorHz[%d] < targetFreqOffset[%d],CodeValue[%d] ,CodeStart[%d], CodeEnd[%d] . \n",
			freqErrorHz,targetFreqOffset,	CodeValue, CodeStart ,CodeEnd );
		DCXOCodeWrite(hw_priv,CodeValue);

		b_fail = 1;
		
		if (CodeValue <= 2)
		{
			b_fail = 3;
		}
		if (0x01 == CodeEnd)
		{
			b_fail = 3;
		}
	}
	else
	{
		wifi_printk(WIFI_DBG_ERROR, "[dcxo PASS]freqErrorKHz[%d] CodeValue[%d]!\n",freqErrorHz/1000,CodeValue);
		b_fail = 0;
		*dcxo = CodeValue;
		*pfreqErrorHz = freqErrorHz;
	}

	
	return b_fail;

}

static int atbm_freqoffset_save_efuse(struct atbmwifi_common *hw_priv,struct rxstatus_signed rxs_s,atbm_uint32 dcxo)
{
	int ret = 0;
	int iResult=0;
	//struct atbm_vif *vif;
	struct efuse_headr efuse_d,efuse_bak;

	atbm_memset(&efuse_d,0,sizeof(struct efuse_headr));
	atbm_memset(&efuse_bak,0,sizeof(struct efuse_headr));

	

	//tmp = DCXOCodeRead(hw_priv);printk("tmp %d\n"tmp);	
	if(ucWriteEfuseFlag)
	{
		wifi_printk(WIFI_DBG_ERROR, "ucWriteEfuseFlag :%d\n",ucWriteEfuseFlag);
		wsm_get_efuse_data(hw_priv,(void *)&efuse_d,sizeof(struct efuse_headr));
		
		if(efuse_d.version == 0)
		{
			//The first time efuse is written,all the data should be written, 
			//The production test only modifies part of the value, so efuse cannot be written.
			iResult = -3;
			goto FEEQ_ERR;
		}

		if(efuse_d.dcxo_trim == dcxo) // old dcxo equal new dcxo, no need to write efuse.
		{
			wifi_printk(WIFI_DBG_ERROR, " old dcxo equal new dcxo, no need to write efuse.\n");
			iResult = 0;
			goto FEEQ_ERR;
		}
		efuse_d.dcxo_trim = dcxo;
		/*
		*LMC_STATUS_CODE__EFUSE_VERSION_CHANGE	failed because efuse version change  
		*LMC_STATUS_CODE__EFUSE_FIRST_WRITE, 		failed because efuse by first write   
		*LMC_STATUS_CODE__EFUSE_PARSE_FAILED,		failed because efuse data wrong, cannot be parase
		*LMC_STATUS_CODE__EFUSE_FULL,				failed because efuse have be writen full
		*/
		ret = wsm_efuse_change_data_cmd(hw_priv, &efuse_d,0);
		if (ret == LMC_STATUS_CODE__EFUSE_FIRST_WRITE)
		{
			iResult = -3;
		}else if (ret == LMC_STATUS_CODE__EFUSE_PARSE_FAILED)
		{
			iResult = -4;
		}else if (ret == LMC_STATUS_CODE__EFUSE_FULL)
		{
			iResult = -5;
		}else if (ret == LMC_STATUS_CODE__EFUSE_VERSION_CHANGE)
		{
			iResult = -6;
		}else
		{
			iResult = 0;
		}
		
		//frame_hexdump("efuse_d", (atbm_uint8 *)&efuse_d, sizeof(struct efuse_headr));
		wsm_get_efuse_data(hw_priv,(void *)&efuse_bak, sizeof(struct efuse_headr));
		//frame_hexdump("efuse_bak", (atbm_uint8 *)&efuse_bak, sizeof(struct efuse_headr));
		
		if(atbm_memcmp((void *)&efuse_bak,(void *)&efuse_d, sizeof(struct efuse_headr)) !=0)
		{
			iResult = -2;
		}else
		{
			iResult = 0;
		}
		
	}

	
FEEQ_ERR:	
	
	/*sprintf(buff, "cfo:%d,evm:%d,gainImb:%d, phaseImb:%d,dcxo:%d,result:%d (0:OK; -1:FreqOffset Error; -2:efuse hard error;"
		" -3:efuse no written; -4:efuse anaysis failed; -5:efuse full; -6:efuse version change)",
	rxs_s.Cfo,
	rxs_s.evm,
	rxs_s.GainImb,
	rxs_s.PhaseImb,
	dcxo,
	iResult
	);*/

	//if((ret = copy_to_user(wrqu->data.pointer, buff, strlen(buff))) != 0){
	//	return -ATBM_EINVAL;
	//}

	return iResult;
}

/**************************************************************************
**
** NAME         LMC_FM_GetATBMIe
**
** PARAMETERS:  pElements  -> Pointer to the Ie list
**              Length     -> Size of the Ie List
**              
** RETURNS:     Pointer to element if found or 0 otherwise.
**
** DESCRIPTION  Searches for ATBM test element  from a given IE list.
** 
**************************************************************************/
atbm_uint8* LMC_FM_GetATBMIe(atbm_uint8 *pElements,atbm_uint16 Length)
{
  atbm_uint8     ATBMIeOui[3]   = ATBM_OUI	;
  
  struct ATBM_TEST_IE  *Atbm_Ie;
	//dump_mem(pElements,Length);

   if(Length > sizeof(struct ATBM_TEST_IE)){
		pElements += Length-sizeof(struct ATBM_TEST_IE);
		Atbm_Ie =(struct ATBM_TEST_IE  *) pElements;
		
		/*wifi_printk(WIFI_DBG_ERROR, "Atbm_Ie->oui_type %x,Atbm_Ie->oui %x %x,size %x\n",
			Atbm_Ie->oui_type,
			Atbm_Ie->oui[2],
			ATBMIeOui[2],
			sizeof(struct ATBM_TEST_IE));
		
		dump_mem(pElements,16);*/

		 if(pElements[0]== D11_WIFI_ELT_ID){
			 if((memcmp(Atbm_Ie->oui,ATBMIeOui,3)==0)&&
			 	(Atbm_Ie->oui_type== WIFI_ATBM_IE_OUI_TYPE) ){
				return pElements;
			}
		 }
   }

  return (atbm_uint8 *)NULL  ;
}//end LMC_FM_GetP2PIe()


void etf_v2_scan_end(struct atbmwifi_common *hw_priv, struct atbmwifi_vif *vif )
{
	int result = 0;//(0:OK; -1:FreqOffset Error; -2:Write efuse Failed;-3:efuse not write;-4:rx fail)
	atbm_uint32 dcxo = 0;
	int freqErrorHz;
	int ErrCode = -1;

	etf_rx_status_get(hw_priv);
	atbm_SleepMs(10);

	if(atbm_test_rx_cnt <= 5){
		memset(&gRxs_s, 0, sizeof(struct rxstatus_signed));
#if CONFIG_ATBM_PRODUCT_TEST_USE_GOLDEN_LED
		if((Atbm_Test_Success == 1) || (Atbm_Test_Success == -1)){
			gRxs_s.valid = 1;	
			Atbm_Test_Success = 0;
			atbm_test_rx_cnt = 0;
			txevm_total = 0;
			ETF_bStartTx = 0;
			return;
		}
#endif
		wifi_printk(WIFI_DBG_ERROR, "etf rx data[%d] less than 5 packet\n",atbm_test_rx_cnt);
		gRxs_s.result = -7;		

		gRxs_s.dcxo = dcxo;
		gRxs_s.valid = 1;	
		atbm_test_rx_cnt = 0;
		txevm_total = 0;
		ETF_bStartTx = 0;
		goto end;
	}
	
	gRxs_s.TxRSSI += gthreshold_param.cableloss;
	gRxs_s.txevm = txevm_total/atbm_test_rx_cnt;
	
	wifi_printk(WIFI_DBG_ERROR, "Average: Cfo:%d,TxRSSI:%d,RxRSSI:%d,txevm:%d,rxevm:%d\n",	
	gRxs_s.Cfo,
	gRxs_s.TxRSSI,
	gRxs_s.RxRSSI,
	gRxs_s.txevm,
	gRxs_s.evm
	);
	
#if 0//CONFIG_ATBM_PRODUCT_TEST_NO_UART
	int efuse_remainbit = 0;

	efuse_remainbit = wsm_get_efuse_status(hw_priv, vif);
	printk("efuse remain bit:%d\n", efuse_remainbit);

	if(efuse_remainbit < 8)
	{		
		printk("##efuse is full,do not calibrte FreqOffset\n##");
		dcxo = efuse_data_etf.dcxo_trim;
		if(gthreshold_param.freq_ppm != 0)
		{
			if((gRxs_s.Cfo > -gthreshold_param.freq_ppm) &&
				(gRxs_s.Cfo < gthreshold_param.freq_ppm))
			{
				printk("#1#cur cfo:%d, targetFreqOffset:%d\n",
					gRxs_s.Cfo, gthreshold_param.freq_ppm);
				goto success;
			}
			else
			{
				printk("#1#cur cfo:%d, targetFreqOffset:%d\n",
					gRxs_s.Cfo, gthreshold_param.freq_ppm);
				goto Error;
			}
		}
		else
		{
			if((gRxs_s.Cfo > -TARGET_FREQOFFSET_HZ) &&
				(gRxs_s.Cfo < TARGET_FREQOFFSET_HZ))
			{
				printk("#2#cur cfo:%d, targetFreqOffset:%d\n",
					gRxs_s.Cfo, TARGET_FREQOFFSET_HZ);
				goto success;
			}
			else
			{
				printk("#2#cur cfo:%d, targetFreqOffset:%d\n",
					gRxs_s.Cfo, TARGET_FREQOFFSET_HZ);
				goto Error;
			}
		}
	}
#endif
	if(gthreshold_param.freq_ppm != 0)
		result = Test_FreqOffset_v2(hw_priv,&dcxo,&freqErrorHz);
	else
	{
		dcxo = efuse_data_etf.dcxo_trim;
		wifi_printk(WIFI_DBG_ERROR, "Not need to Calibrate FreqOffset\n");
		result = 0;
		goto success;
	}
	
	if(result == 1)
	{
		//start next scan
		wifi_printk(WIFI_DBG_ERROR, "start next scan\n");

		//mutex_lock(&hw_priv->conf_mutex);
		//wsm_stop_tx(hw_priv);
		//mutex_unlock(&hw_priv->conf_mutex);

		atbm_SleepMs(100);
		txevm_total = 0;
		atbm_test_rx_cnt = 0;
		wsm_start_tx_v2(hw_priv,vif);
	}
	else  if(result == 0)  //etf dcxo success
	{
success:
		//if((ErrCode = etf_v2_compare_test_result()) != 0)
			//goto Error;
		wifi_printk(WIFI_DBG_ERROR, "etf test success \n");
		gRxs_s.result = atbm_freqoffset_save_efuse(hw_priv,gRxs_s,dcxo);

		gRxs_s.dcxo = dcxo;
		gRxs_s.valid = 1;
		//del_timer_sync(&hw_priv->etf_expire_timer);
#if CONFIG_ATBM_PRODUCT_TEST_USE_GOLDEN_LED
		Atbm_Test_Success = 1;
		//wsm_send_result(hw_priv,vif);
		wsm_start_tx_v2(hw_priv,vif);
#endif
		
	}else
	{
		gRxs_s.result = -1;

		gRxs_s.result = ErrCode;
		gRxs_s.dcxo = dcxo;
		gRxs_s.valid = 1;
		wifi_printk(WIFI_DBG_ERROR, "etf test Fail \n");
		//del_timer_sync(&hw_priv->etf_expire_timer);
#if CONFIG_ATBM_PRODUCT_TEST_USE_GOLDEN_LED
		Atbm_Test_Success = -1;
		//wsm_send_result(hw_priv,vif);
		wsm_start_tx_v2(hw_priv,vif);
#endif

	}
end:
	g_ProductTestGlobal = 0;
	atbm_test_rx_cnt = 0;
	ETF_bStartTx = 0;
}

void etf_v2_scan_rx(struct atbmwifi_common *hw_priv,struct atbm_buff *skb,atbm_uint8 rssi )
{

	atbm_int32 Cfo;
	atbm_int32  RSSI;
	atbm_int32 tmp;
	atbm_int16 txevm;
	struct ATBM_TEST_IE  *Atbm_Ie = NULL;
	atbm_uint8 *data = (atbm_uint8 *)skb->abuf + offsetof(struct atbmwifi_ieee80211_mgmt, u.probe_resp.variable);
	int len = skb->dlen - offsetof(struct atbmwifi_ieee80211_mgmt, u.probe_resp.variable);
	Atbm_Ie = (struct ATBM_TEST_IE  *)LMC_FM_GetATBMIe(data,len);
	
	if((Atbm_Ie) && (Atbm_Ie->featureid == gthreshold_param.featureid))
	{
		tmp				= Atbm_Ie->result[1];
		tmp				= (atbm_int32)N_BIT_TO_SIGNED_32BIT(tmp, 16);
		if(Atbm_Ie->resverd & ATBM_BIT(0))
			Cfo = (int)(((tmp*12207)/160));//6431 as golden
		else
			Cfo = (int)(((tmp*12207)/10));	//6421 as golden 
		 
		txevm				= (atbm_int16)N_BIT_TO_SIGNED_32BIT(Atbm_Ie->result[2], 16);
		RSSI			= (atbm_int16)N_BIT_TO_SIGNED_32BIT(Atbm_Ie->result[3], 10);
		
		if( RSSI < gthreshold_param.rssifilter)
		{
			wifi_printk(WIFI_DBG_ERROR, "[%d]: Cfo:%d,TxRSSI:%d, rx dump packet,throw......\n",
			atbm_test_rx_cnt,	
			Cfo,
			RSSI
			);
			return;
		}

		if(txevm < gthreshold_param.txevm)
		{
			if(atbm_test_rx_cnt == 0)
			{		
				gRxs_s.Cfo = Cfo;
				//gRxs_s.evm = evm;
				gRxs_s.TxRSSI = RSSI;
			}else
			{

				gRxs_s.Cfo = (gRxs_s.Cfo*3 + Cfo )/4;
				//gRxs_s.evm = evm;
				gRxs_s.TxRSSI = RSSI;
				//gRxs_s.TxRSSI = (gRxs_s.TxRSSI*3*10 + RSSI*10 +5)/40;

			}

			wifi_printk(WIFI_DBG_ERROR, "[%d]: Cfo1:%d, Cfo:%d,TxRSSI:%d,txevm:%d\n",
			atbm_test_rx_cnt,
			tmp,
			Cfo,
			RSSI,txevm
			);

			//printk("etf_v2_scan_rx %d,cnt %d,[0x%x,0x%x,0x%x,0x%x,0x%x]\n",Atbm_Ie->test_type,atbm_test_rx_cnt,
			//	Atbm_Ie->result[0],Atbm_Ie->result[1],Atbm_Ie->result[2],Atbm_Ie->result[3],Atbm_Ie->result[3]);
			txevm_total += txevm;
			atbm_test_rx_cnt++;
		}
		
	}
	else
	{
		wifi_printk(WIFI_DBG_ERROR,"<<< Not Found atbm_ie >>>\n");
	}

}


int Get_MCS_LUT_Offset_Index(int WiFiMode,int OFDMMode,int ChBW,int RateIndex)
{
	int offsetIndex = 0;
/*
	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
		offsetIndex = atbm_cronus_Get_MCS_LUT_Offset_Index(WiFiMode, OFDMMode, ChBW, RateIndex);
	else
		offsetIndex = atbm_oceanus_Get_MCS_LUT_Offset_Index(WiFiMode, OFDMMode, ChBW, RateIndex);
*/
	return offsetIndex;
}



/***********************************************************
* Get register address of MCS LUT by the index of control on Tx UI
*
***********************************************************/
unsigned int Get_MCS_LUT_Addr_ByTxUICtrlIndex(int WiFiModeCtrlIndex,int OfdmModeCtrlIndex,int ChBwCtrlIndex,int RateCtrlIndex)
{
	int offsetIndex = 0;
	atbm_uint32 mcsLUTAddr = 0x0ACBD510;//0xACBE010;//mcs LUT base addr

	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
		mcsLUTAddr = 0xACBE010;//cronus mcs LUT base addr

	offsetIndex = Get_MCS_LUT_Offset_Index(WiFiModeCtrlIndex,OfdmModeCtrlIndex,ChBwCtrlIndex,RateCtrlIndex);
	mcsLUTAddr += (offsetIndex << 2);
	return mcsLUTAddr;
}
unsigned int Get_MCS_LUT_Addr(int wifi_mode,int ofdm_mode, int ch_bw, int rate_index)
{
    return Get_MCS_LUT_Addr_ByTxUICtrlIndex(wifi_mode,ofdm_mode,ch_bw,rate_index);
}

void set_power_by_mode(int wifi_mode, int ofdm_mode, int bw, int rateIndex, int delfault_power, int power_delta, int powerTarFlag)
{
	atbm_uint32 mcsLUTAddr = 0;
	int power_current = 0;
	int power_tar = 0;
	int index = 0;

	mcsLUTAddr = Get_MCS_LUT_Addr(wifi_mode, ofdm_mode, bw, rateIndex);

	if(0 == powerTarFlag)
	{
		power_current = delfault_power;//HW_READ_REG_BIT(mcsLUTAddr, 7, 0);//read current power
		power_tar = power_current + power_delta;
	}
	else
		power_tar = power_delta;
	//atbm_printk_always("0x%x:%x\n", mcsLUTAddr, power_tar);
	
	//atbm_printk_always("mcsLUTAddr:0x%x:%d,%d,%d\n", mcsLUTAddr, delfault_power>>2, power_delta>>2, power_tar>>2);

	if(power_tar > 23*4)
		power_tar = 23*4;

	HW_WRITE_REG_BIT(mcsLUTAddr, 7, 0, (atbm_uint32)power_tar);//function register
}
void set_power_by_bandwidth(int bw, atbm_uint32 addrOffset, int delfault_power, int power_delta)
{
	int i = 0;
	atbm_uint32 basicAddr = 0;
	atbm_uint32 mcsLUTAddr = 0;
	int power_current = 0;
	int power_tar = 0;

	if(bw == 0)
		basicAddr = 0xACBE010;//20M
	else
		basicAddr = 0xACBE0A0;//40M

	//for(i=0;i<36;i++)
	{
		mcsLUTAddr = basicAddr + addrOffset;
			
		power_current = delfault_power;//HW_READ_REG_BIT(mcsLUTAddr, 7, 0);//read current power
		power_tar = power_current + power_delta;

		if(power_tar > 23*4)
			power_tar = 23*4;

		//atbm_printk_always("mcsLUTAddr:0x%x:%d,%d,%d\n", mcsLUTAddr, delfault_power>>2, power_delta>>2, power_tar>>2);
		HW_WRITE_REG_BIT(mcsLUTAddr, 7, 0, (atbm_uint32)power_tar);//function register
	}
	
}

void get_power_by_bandwidth(int bw)
{
	int i = 0;
	atbm_uint32 basicAddr = 0;
	atbm_uint32 mcsLUTAddr = 0;
	int power_tar = 0;

	if(bw == 0)
		basicAddr = 0xACBE010;//20M
	else
		basicAddr = 0xACBE0A0;//40M

	for(i=0;i<36;i++)
	{
		mcsLUTAddr = basicAddr + (i << 2);
			
		power_tar = HW_READ_REG_BIT(mcsLUTAddr, 7, 0);//read current power
		wifi_printk(WIFI_DBG_ERROR,"0x%x:%x\n", mcsLUTAddr, power_tar);
	}
	
}

/*
iwpriv wlan0 common set_power_target,<powertar>
powertar:dBm
*/
int atbm_etf_set_powerTarget(int powerTar)
{
	char *ptr = NULL;
	char const *pos = NULL;
	char const *pos_end = NULL;
	int ret = 0;
	int i   = 0;
	//int powerTar = 0;
	int len = 0;
	int flag = 0;
	int neg = 0;
	atbm_uint32 mcsLUTAddr = 0;

	if(powerTar > 230)
	{
		wifi_printk(WIFI_DBG_ERROR,"Invalid Param\n");
		return 0;
	}

	powerTar = (powerTar * 4)/10;
	
	
	wifi_printk(WIFI_DBG_ERROR,"powerTar:%d,0x%x\n", powerTar, powerTar);

	mcsLUTAddr = Get_MCS_LUT_Addr(WiFiMode, OFDMMode, ChBW, RateIndex);

	if(atbm_hw_priv_chip_id() == HW_CHIP_VERSION_Cronus)
		HW_WRITE_REG_BIT(mcsLUTAddr, 7, 0, (atbm_uint32)powerTar);//function register
	else
		HW_WRITE_REG_BIT(mcsLUTAddr, 15, 8, (atbm_uint32)powerTar);//function register

	//HW_WRITE_REG_BIT(0x0AC80CF8, 15, 8, (atbm_uint32)powerTar);//force register
	

	return ret;
}
//iwpriv wlan0 common get_power_target
int atbm_etf_get_powerTarget()
{
	char *ptr = NULL;
	int ret = 0;
	int powerTar = 0;
	atbm_uint32 mcsLUTAddr = 0;
	int if_id;
	

	mcsLUTAddr = Get_MCS_LUT_Addr(WiFiMode, OFDMMode, ChBW, RateIndex);
		
	powerTar = HW_READ_REG_BIT(mcsLUTAddr, 7, 0);//function register

	powerTar = (powerTar * 10)/4;

	wifi_printk(WIFI_DBG_ERROR,"powerTar:%d/10\n", powerTar);

	return ret;
}


