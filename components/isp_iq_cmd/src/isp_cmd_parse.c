/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <math.h>


#include "img_utils.h"
#include "img_proc.h"
#include "isp_cmd_parse.h"
#include "img_param.h"
#include "img_param_parse.h"


#define AWB_CAL_WB_INFO_NUM     (21)
#define AWB_CAL_AWB_NUM         (88)
#define CCM_CAL_REG_NUM     (CCM_COEF_NUM + 1)
#define GAMMA_CAL_REG_NUM   (GAMMA_COEF_NUM + 1)

/*static ipro_isp_ae_grid_sum_t ae_grid_sum = {0};
static ipro_isp_ae_hist_y_t ae_y_sum = {0};
static ipro_isp_ae_hist_bayer_t ae_bayer_sum = {0};
static ipro_isp_awb3_stats_hist_t awb_stats_hist = {0};
static ipro_isp_wdr_curve_t wdr_curve = {0};

static isp_awb3_sum_t awb3_sum = {0};*/
//static xmedia_isp_awb_statistics_cfg awb_stats = {0};
//static xmedia_isp_ae_statistics_cfg ae_stats = {0};
//static ISP_Gamma_Curve_Type gammaCfg;

typedef enum {
    ISP_PARSE_READ = 0,
    ISP_PARSE_WRITE,
    ISP_PARSE_DUMP,
    ISP_PARSE_LOAD,
} ISP_PARSE_TYPE;
    
typedef enum {
    REG_HW = 0,
    REG_FW,
    REG_SW,
    REG_CAL,
} REG_TYPE;
    
int isIntger(char* str, int* value)
{
    int val = 0;
    if (str != NULL) {
        val = atoi(str);
        if ((val != 0) || (str[0] == '0')) {
            *value = val;
            return 1;
        }
    }
    return 0;
}

int isFloat(char* str, float* value)
{
    float val = 0;
    if (str != NULL) {
        val = atof(str);
        if ((val != 0) || (str[0] == '0')) {
            *value = val;
            return 1;
        }
    }
    return 0;
}

int isIntgerArray(char* str, int* array, int *ret_arr_num)
{
    int val = 0, arr_num = 0;
    const char delim[2] = ",";
    char *token;

    if (strstr(str, "[") != NULL) str++;
    token = strtok(str, delim);
    while (token != NULL) {
        if (strstr(token, "]") != NULL) {
            token[strlen(token)-1] = '\0';
        }
        val = atoi(token);
        if ((val != 0) || (token[0] == '0')) {
            *array++ = val;
            arr_num++;
            token = strtok(NULL, delim);
        } else { // '\0'
            *ret_arr_num = arr_num;
            return MIN(1, arr_num);
        }
    }
    *ret_arr_num = arr_num;
    return MIN(1, arr_num);

}

int checkRegIdx(char* reg, char* reg_name, int min_idx, int max_idx, int* idx_out)
{
    int idx = 0;
    char *token;

    *idx_out = idx;
    if (strstr(reg, reg_name) != NULL) {
        token = strtok(reg, reg_name);
        if(token == NULL) return -1;
        idx = atoi(token);
    } else return 0;//not match

    if ((idx < min_idx) || (idx > max_idx)) return -1;//out of range

    *idx_out = idx;
    return 1;
}

int checkRegIdx_Idx(char* reg, char* reg_name, int min_idx, int max_idx, int* idx_out, int min_idx_2, int max_idx_2, int* idx_out_2)
{
    int idx = 0, idx_2 = 0, cnt = 0;
    char *token;

    *idx_out = idx;
    *idx_out_2 = idx_2;

    if (strstr(reg, reg_name) != NULL) {
        token = strtok(reg, reg_name);
        while (token != NULL) {
            if (cnt == 0) idx = atoi(token);
            else if (cnt == 1) idx_2 = atoi(token);
            //printf("%s\n", token);
            token = strtok(NULL, reg_name);
            cnt++;
        }
    } else return 0;//not match

    if ((idx < min_idx) || (idx > max_idx)) return -1;//out of range
    if ((idx_2 < min_idx_2) || (idx_2 > max_idx_2)) return -1;//out of range

    *idx_out = idx;
    *idx_out_2 = idx_2;
    return 1;
}

int parseToolReg(char *reg_in, char *reg_out)
{
    char *reg_ptr;

    reg_ptr = strstr(reg_in, "HW_");
    if (reg_ptr != NULL) {
        sprintf(reg_out, (reg_ptr + 3));
        return REG_HW;
    }

    reg_ptr = strstr(reg_in, "SW_");
    if (reg_ptr != NULL) {
        sprintf(reg_out, (reg_ptr + 3));
        return REG_SW;
    }

    reg_ptr = strstr(reg_in, "FW_");
    if (reg_ptr != NULL) {
        sprintf(reg_out, (reg_ptr + 3));
        return REG_FW;
    }

    reg_ptr = strstr(reg_in, "CAL");
    if (reg_ptr != NULL) {
        sprintf(reg_out, (reg_ptr + 3));
        return REG_CAL;
    }

    sprintf(reg_out, reg_in);
    return REG_SW;
}

int parseNRTypeReg(char *reg_in, char *reg_out)
{
    char *reg_ptr;

    reg_ptr = strstr(reg_in, "y_");
    if (reg_ptr != NULL) {
        sprintf(reg_out, (reg_ptr + 2));
        return REG_HW;
    }

    reg_ptr = strstr(reg_in, "c_");
    if (reg_ptr != NULL) {
        sprintf(reg_out, (reg_ptr + 2));
        return REG_SW;
    }


    sprintf(reg_out, reg_in);
    return REG_SW;
}

void strCatVal(char* ret_string, int value)
{
    char value_str[16] = {'\0'};

    sprintf(value_str, "%d,", value);
    strcat(ret_string, value_str);

}

void strCatVal_u64(char* ret_string, uint64_t value)
{
    char value_str[32] = {'\0'};

    sprintf(value_str, "%lu,", (unsigned long)value);
    strcat(ret_string, value_str);

}

void strCatVal_hex(char* ret_string, int value)
{
    char value_str[16] = {'\0'};

    sprintf(value_str, "0x%08x,", value);
    strcat(ret_string, value_str);

}

enum numeric_short_options {
    SENSOR_INPUT = 1000,
    MODULE_STATE,
    /*GET_AE_SUM,
    Get_AE_HIST_SUM_Y,
    Get_AE_HIST_SUM_BAYER,

    GET_AWB3_CONF,
    GET_AWB3_SUM,*/

    AE_AUTO,
    AE_REG,
    //AE_ARRAY,
    //AE_INFO,
    //AWB_INFO,
    AWB_AUTO,
    /*AWB_STATS,
    AWB_STATS_CONF,*/
    AWB_CAL,
    AWB_REG,
    //AWB_ARRAY,

    GAMMA_CURVE,
    GAMMA_REG,
    CT_REG,
    CT_COEF,
    TM_AUTO,
    TM_REG,
    TM_CURVE,
    DEFOG_AUTO,
    DEFOG_REG,
    CS_AUTO,
    CS_REG,
    YUVCTRL_AUTO,
    YUVCTRL_REG,
    YUVADJ_REG,
    CS_COEF,
    SYS_REG,
};


void isp_cmd2(int argc, char **argv)
{
    int ch;
    char module[32] = {'\0'};
    char argv_reg[32] = {'\0'};
    char argv_val[512] = {'\0'};
    char ret_string[512] = {'\0'};
    
    static struct option long_options[] =
    {
        {"sensor-input",  no_argument,       NULL, SENSOR_INPUT},
        {"module-state",  required_argument, NULL, MODULE_STATE},
        {"gamma-curve",   required_argument, NULL, GAMMA_CURVE},
        {"gamma-reg",     required_argument, NULL, GAMMA_REG},
        {"ae-reg",        required_argument, NULL, AE_REG},
        {"ae-auto",       required_argument, NULL, AE_AUTO},
        {"awb-reg",       required_argument, NULL, AWB_REG},
        {"awb-auto",      required_argument, NULL, AWB_AUTO},
        {"awb-cal",       required_argument, NULL, AWB_CAL},
        {"ct-reg",        required_argument, NULL, CT_REG},
        {"ct-coef",       required_argument, NULL, CT_COEF},
        {"tm-auto",       required_argument, NULL, TM_AUTO},
        {"tm-reg",        required_argument, NULL, TM_REG},
        {"tm-curve",      required_argument, NULL, TM_CURVE},
        {"defog-auto",    required_argument, NULL, DEFOG_AUTO},
        {"defog-reg",     required_argument, NULL, DEFOG_REG},
        {"cs-auto",       required_argument, NULL, CS_AUTO},
        {"cs-reg",        required_argument, NULL, CS_REG},
        {"yuvctrl-auto",  required_argument, NULL, YUVCTRL_AUTO},
        {"yuvctrl-reg",   required_argument, NULL, YUVCTRL_REG},
        {"yuvadj-reg",    required_argument, NULL, YUVADJ_REG},
        {"cs-coef",       required_argument, NULL, CS_COEF},
        {"sys-reg",       required_argument, NULL, SYS_REG},
        {0, 0, 0, 0}
    };

    if (argc <= 1) {
        printf("please type isp module\r\n");
        return;
    }

    optind = 0;
    opterr = 0;
    while ((ch = getopt_long (argc, argv, "", long_options, NULL)) != -1) {
        switch (ch)
        {
        case SENSOR_INPUT: {
            printf("SENSOR_INPUT \r\n");
            break;
        }
        case MODULE_STATE:
        case GAMMA_CURVE:
        case GAMMA_REG:
        case AE_AUTO:
        case AE_REG:
        case AWB_AUTO:
        case AWB_REG:
        case AWB_CAL:
        case CT_REG:
        case CT_COEF:
        case TM_AUTO:
        case TM_REG:
        case DEFOG_AUTO:
        case DEFOG_REG:
        case CS_AUTO:
        case CS_REG:
        case CS_COEF:
        case YUVCTRL_AUTO:
        case YUVCTRL_REG:
        case YUVADJ_REG:
        case SYS_REG:{
            const char *format = "--%s";
            int payload_len_in = 0;
            int ret = -1;

            memset(module, '\0', sizeof(char) * 32);
            memset(argv_reg, '\0', sizeof(char) * 32);
            memset(argv_val, '\0', sizeof(char) * 512);
            memset(ret_string, '\0', sizeof(char) * 512);

            if(sscanf(argv[optind - 2], format, module) != 1) goto FAIL;
            sprintf(argv_reg, "%s", argv[optind - 1]);
            sprintf(argv_val, "%s", argv[optind]);
            payload_len_in = strlen(argv_val);

//            aos_cli_printf("cli avgv  module %s, argv_reg %s, argv_val %s\r\n", argv[optind - 2], argv[optind - 1], argv[optind]);
//            aos_cli_printf("cli module %s, argv_reg %s, argv_val %s\r\n", module, argv_reg, argv_val);

            ret = parse_isp_cmd(module, argv_reg, argv_val, payload_len_in, ret_string);
            if (ret == -1) goto FAIL;
            else printf("[OK] %s\r\n", ret_string);
            break;
        }
        default:
           goto FAIL;
       }
    }
    printf("[OK] \r\n");
    return;


FAIL:
    printf("[NG] \r\n");
    return;
}

static int ISP_Module_State_Parse(int type, char* reg, int value, char* ret_string)
{

    int val_get = 0;
    Fun_Type enable = (Fun_Type)value;

    if (strcmp(reg, "ae") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_states(ISP_ADJ_AE, enable);
        val_get = img_param_sw_get_states(ISP_ADJ_AE);
    } else if (strcmp(reg, "awb") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_states(ISP_ADJ_AWB, enable);
        val_get = img_param_sw_get_states(ISP_ADJ_AWB);
    } else if (strcmp(reg, "ct") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_states(ISP_ADJ_CT, enable);
        val_get = img_param_sw_get_states(ISP_ADJ_CT);
    } else if (strcmp(reg, "gamma") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_states(ISP_ADJ_GAMMA, enable);
        val_get = img_param_sw_get_states(ISP_ADJ_GAMMA);
    } else if (strcmp(reg, "tm") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_states(ISP_ADJ_TM, enable);
        val_get = img_param_sw_get_states(ISP_ADJ_TM);
    } else if (strcmp(reg, "defog") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_states(ISP_ADJ_DEFOG, enable);
        val_get = img_param_sw_get_states(ISP_ADJ_DEFOG);
    } else if (strcmp(reg, "cs") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_states(ISP_ADJ_CS, enable);
        val_get = img_param_sw_get_states(ISP_ADJ_CS);
    } else if (strcmp(reg, "yuvctrl") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_states(ISP_ADJ_YUVCTRL, enable);
        val_get = img_param_sw_get_states(ISP_ADJ_YUVCTRL);
    } else if (strcmp(reg, "yuvadj") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_states(ISP_ADJ_YUVADJ, enable);
        val_get = img_param_sw_get_states(ISP_ADJ_YUVADJ);
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        return 0;
    }
    sprintf(ret_string, "[OK] module_%s: %d", reg, val_get);

    return 0;
}


static int ISP_AE_Metering_Tbl_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0, idx = AE_METERING_CUSTOM;
    ae_metering_tbl_t *cfg = {0};

    img_param_sw_get_ae_metering_tbl(idx, &cfg);
    if (type == ISP_PARSE_WRITE) {
        for (i = 0; i < AE_STATS_GRID_NUM; i++) {
            cfg->coef[i] = value[i];
        }
        img_param_sw_set_ae_metering_tbl(cfg);
        //imgproc_check_2A_module(ISP_ADJ_AE);
        return 0;
    }

    sprintf(ret_string, "[OK] AE_%s_%d: ", reg, idx);
    for (i = 0; i < AE_STATS_GRID_NUM; i++) {
        strCatVal(ret_string, cfg->coef[i]);
    }

    return 0;
}
#if 0

static int ISP_AE_Agc_DB_Tbl_Parse(int type, char* reg, int *value, char* ret_string)
{
#if 0
    int i = 0;
    ae_agc_db_tbl *ptr;

    if (type == ISP_PARSE_WRITE) {
        return -1;
    }

    img_param_get_ae_agc_db_tbl(&ptr);
    sprintf(ret_string, "[OK] AE_%s: ", reg);
    for (i = 0; i < MAX_ENTRY_NUM_AE_AGC_DB_TBL; i++) {
        strCatVal(ret_string, ptr->agc_db[i]);
    }
#endif
    return 0;
}
#endif

static int ISP_AE_Auto_Tbl_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    ae_sub_attr *cfg = {0};

    if (type == ISP_PARSE_WRITE) {
        for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
            img_param_sw_get_ae(i, &cfg);
            if (strcmp(reg, "ae_target") == 0) cfg->ae_target = value[i];
            else return -1;

            img_param_sw_set_ae(i, cfg);
        }
        //imgproc_check_2A_module(ISP_ADJ_AE);
        return 0;
    }

    sprintf(ret_string, "[OK] auto AE_%s: ", reg);
    for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
        img_param_sw_get_ae(i, &cfg);
        if (strcmp(reg, "ae_target") == 0) strCatVal(ret_string, cfg->ae_target);
        else return -1;
    }

    return 0;
}

int ISP_AE_Auto_Parse(int type, char* reg, int *value, char* ret_string)
{
    int ret = 0;

    if (strstr(reg, "metering_tbl") != NULL) {
        ret = ISP_AE_Metering_Tbl_Parse(type, reg, value, ret_string);
     } else {
        ret = ISP_AE_Auto_Tbl_Parse(type, reg, value, ret_string);
    }

    return ret;
}


static int ISP_AE_SW_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;
    //float val_float_get = 0;
    //ae_info_t *ae_info = {0};
    //ISP_AE_INFO ae_info_luma = {0};
    //float gain_f = 0.0;
    //ISP_AE_HIGHLIGHT_WEIGHT_CONF *highlight_weight_cfg = {0};
    //IPRO_EXPO_TIME expo_time = EXPO_TIME_1_25_SEC;

    //ae_slow_shutter_conf_t *slow_shutter_conf = {0};
    ae_param_t *param = {0};

    //MM_ISP_GetAeInfo(&ae_info_luma);
    //ae_get_info(&ae_info);

    img_param_sw_get_ae_manual(&param);
    /*if (strcmp(reg, "version") == 0) {
        if (type == ISP_PARSE_WRITE) return -1;
        ae_get_algo_version(&val_get);
    } else if (strcmp(reg, "register") == 0) {
        if (type == ISP_PARSE_WRITE) {
            xmedia_ae_register(pipe, value);
            return 0;
        }
        val_get = imgproc_get_ae_algo();
    } else if (strcmp(reg, "unregister") == 0) {
        if (type == ISP_PARSE_READ) {
            xmedia_ae_unregister(pipe);
            val_get = imgproc_get_ae_algo();
        } else return -1;
    } else*/ if (strcmp(reg, "manual_mode") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_ae_manual_mode(value);
        val_get = img_param_sw_get_ae_manual_mode();
    } else if (strcmp(reg, "exp_time") == 0) {
        if (type == ISP_PARSE_WRITE) param->expo_time = value;
        val_get = param->expo_time;
            } else if (strcmp(reg, "gain") == 0) {
        if (type == ISP_PARSE_WRITE) param->gain = value;
        val_get = param->gain;
    //} else if (strcmp(reg, "speed") == 0) {
    //    if (type == ISP_PARSE_WRITE) attr.auto_attr.speed = value;
    //    val_get = attr.auto_attr.speed;
    //} else if (strcmp(reg, "ae_target") == 0) {
    //    if (type == ISP_PARSE_WRITE) return -1;
    //    ae_get_luma_target(&val_get);
    //} else if (strcmp(reg, "hist_mode") == 0) {
    //    if (type == ISP_PARSE_WRITE) ipro_set_AE_hist_mode((AE_HIST_MODE_TYPE_E)value);
    //    ipro_get_AE_hist_mode((AE_HIST_MODE_TYPE_E *)&val_get);

#if 0
    } else if (strcmp(reg, "expo_time") == 0) {
        if (type == ISP_PARSE_WRITE) ae_set_sensor_exposure(EXPO_TIME_TO_INT(value));// expo: 1/value
        val_get = ae_info->expo_time;
    } else if (strcmp(reg, "gain") == 0) {
        if (type == ISP_PARSE_WRITE) {
            value = gain_f * 1536;
            ae_set_sensor_gain(value);
        }
        val_float_get = (float)ae_info->iso / 1536;
    } else if (strcmp(reg, "iso") == 0) {
        if (type == ISP_PARSE_WRITE) ae_set_sensor_gain(value);
        val_get = ae_info->iso;
    } else if (strcmp(reg, "sys_gain") == 0) {
        if (type == ISP_PARSE_WRITE) {
            IPRO_EXPO_TIME expo_time_t;
            IPRO_GAIN_DB gain_t, gain_db;
            ae_calc_expo_and_gain(value, &expo_time_t, &gain_t);
            printf("value %d, expo_time_t %d, gain_t   %d\r\n",value, expo_time_t, gain_t);
            ae_set_sensor_exposure(expo_time_t);
            gain_f = GAIN_DB_INT_TO_FLOAT(gain_t);
            ae_set_sensor_gain(gain_f);
            printf("expo_time_t %d, gain_t   %d,  gain_f %f\r\n",expo_time_t, gain_t, gain_f);
        }
        #if 0
        ae_get_sensor_exposure(&expo_time);
        ae_get_sensor_gain(&gain_f);
        IPRO_GAIN_DB gain = GAIN_DB_FLOAT_TO_INT(gain_f);
        gen_sys_gain_by_expo_gain(expo_time, gain);
        #else
        val_get = ae_info->sys_gain;
        #endif
    } else if (strcmp(reg, "ISO") == 0) {
        if (type == ISP_PARSE_WRITE) {
            if(value == 100) { // 12 db
                expo_time = EXPO_TIME_1_25_SEC;
                gain_f = 0.0;
            } else if(value == 200) { // 18 db
                expo_time = EXPO_TIME_1_25_SEC;
                gain_f = 6.0;
            } else if(value == 400) { // 24 db
                expo_time = EXPO_TIME_1_25_SEC;
                gain_f = 12.0;
            } else if(value == 800) { // 30 db
                expo_time = EXPO_TIME_1_25_SEC;
                gain_f = 18.0;
            } else if(value == 1600) { // 36 db
                expo_time = EXPO_TIME_1_25_SEC;
                gain_f = 24.0;
            } else if(value == 3200) { // 42 db
                expo_time = EXPO_TIME_1_25_SEC;
                gain_f = 30.0;
            } else if(value == 6400) { // 48 db
                expo_time = EXPO_TIME_1_25_SEC;
                gain_f = 36.0;
            } else if(value == 12800) { // 54 db
                expo_time = EXPO_TIME_1_25_SEC;
                gain_f = 42.0;
            }
            ae_info_luma.expo_time = expo_time;
            ae_info_luma.gain = gain_f;
            MM_ISP_SetAeInfo(&ae_info_luma);
        }
        gain_f = GAIN_DB_INT_TO_FLOAT(ae_info_luma.gain);
#ifdef CONFIG_SINGLE_PRECISION_FLOAT
        val_get = (int)(powf(2.0f, gain_f / 6.0f) * 100.0f);
#else
        val_get = (int)(pow(2, gain_f / 6) * 100);
#endif

    } else if (strcmp(reg, "force_refresh") == 0) {
            if (type == ISP_PARSE_WRITE) ae_force_refresh(value);
            else return 0;
    } else if (strstr(reg, "slow_shutter_") != NULL) {
        img_param_get_ae_slow_shutter_conf(&slow_shutter_conf);
        if (strcmp(reg, "slow_shutter_en") == 0) {
            if (type == ISP_PARSE_WRITE) return -1;
            val_get = slow_shutter_conf->enable;
        } else if (strcmp(reg, "slow_shutter_fps") == 0) {
            if (type == ISP_PARSE_WRITE) return -1;
            val_get = slow_shutter_conf->fps;
        }
    } else if (strcmp(reg, "highlight_weight_hist_sum") == 0) {
        if (type == ISP_PARSE_WRITE) return -1;
        val_get = ae_info->highlight_weight.hist_sum;
    } else if (strcmp(reg, "highlight_weight_idx") == 0) {
        if (type == ISP_PARSE_WRITE) return -1;
        val_get = ae_info->highlight_weight.idx;

    } else if (strcmp(reg, "hist_weight_mode") == 0) {
        if (type == ISP_PARSE_WRITE) ae_set_hist_weight_mode(value);
        ae_get_hist_weight_mode(&val_get);
    } else if (strcmp(reg, "hist_state") == 0) {
        if (type == ISP_PARSE_WRITE) ipro_set_AE_hist_state(value);
        ipro_get_AE_hist_state(&val_get);
#endif 
    } else if (strcmp(reg, "metering_mode") == 0) {
        if (type == ISP_PARSE_WRITE) {
            img_param_sw_set_ae_metering_mode((AE_METERING)value);
            //imgproc_check_2A_module(ISP_ADJ_AE);
        }
        img_param_sw_get_ae_metering_mode((AE_METERING *)&val_get);
#if 0
    } else if (strcmp(reg, "mode") == 0) {
        if (type == ISP_PARSE_WRITE) ae_set_mode((AE_MODE)value);
        ae_get_mode((AE_MODE *)&val_get);
    } else if (strcmp(reg, "linearity_mode") == 0) {
        if (type == ISP_PARSE_WRITE) ae_set_linearity(value);
        ae_get_linearity(&val_get);
    } else if (strcmp(reg, "highlight_weight_en") == 0){
        if (type == ISP_PARSE_WRITE) {
            img_param_sw_set_ae_highlight_weight_en(value);
            imgproc_check_2A_module(ISP_ADJ_AE);
            return 0;
        }
        img_param_sw_get_ae_highlight_weight_en(&val_get);
    } else if (strstr(reg, "highlight_weight_") != NULL) {
        img_param_sw_get_ae_highlight_weight_conf(&highlight_weight_cfg);
        if (type == ISP_PARSE_WRITE) {
            if (strcmp(reg, "highlight_weight_thr_0") == 0) highlight_weight_cfg->thr[0] = value;
            else if (strcmp(reg, "highlight_weight_thr_1") == 0) highlight_weight_cfg->thr[1] = value;
            else if (strcmp(reg, "highlight_weight_thr_2") == 0) highlight_weight_cfg->thr[2] = value;
            else if (strcmp(reg, "highlight_weight_thr_3") == 0) highlight_weight_cfg->thr[3] = value;
            else if (strcmp(reg, "highlight_weight_roi_start") == 0) highlight_weight_cfg->roi_start = value;
            else if (strcmp(reg, "highlight_weight_roi_end") == 0) highlight_weight_cfg->roi_end = value;
            else return -1;
            img_param_sw_set_ae_highlight_weight_conf(highlight_weight_cfg);
            imgproc_check_2A_module(ISP_ADJ_AE);
            return 0;
        }
        if (strcmp(reg, "highlight_weight_thr_0") == 0) val_get = highlight_weight_cfg->thr[0];
        else if (strcmp(reg, "highlight_weight_thr_1") == 0) val_get = highlight_weight_cfg->thr[1];
        else if (strcmp(reg, "highlight_weight_thr_2") == 0) val_get = highlight_weight_cfg->thr[2];
        else if (strcmp(reg, "highlight_weight_thr_3") == 0) val_get = highlight_weight_cfg->thr[3];
        else if (strcmp(reg, "highlight_weight_roi_start") == 0) val_get = highlight_weight_cfg->roi_start;
        else if (strcmp(reg, "highlight_weight_roi_end") == 0) val_get = highlight_weight_cfg->roi_end;
        else return -1;
#endif
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        //xmedia_ae_set_exposure_attr(pipe, &attr);
        img_param_sw_set_ae_manual(param);
        return 0;
    }
    sprintf(ret_string, "[OK] AE_%s: %d", reg, val_get);

    return 0;
}

static int ISP_AE_Parse(int reg_type, int type, char* reg, int value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
        //ret = ISP_AE_HW_Parse(type, reg, value, ret_string);
	} else if (reg_type == REG_FW) {
        //ret = ISP_AE_FW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_AE_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}
#if 0
static int ISP_AE_Stats_Hist_Y_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    AE_HIST_MODE_TYPE_E mode;

    ipro_get_AE_hist_mode(&mode);
    if(mode == AE_HIST_MODE_BAYER) {
        aos_cli_printf("[NG]Please check hist mode, it's not AE_HIST_MODE_Y.\r\n");
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        return -1;
    }

    ipro_get_AE_hist_sum_y(&ae_y_sum);
    sprintf(ret_string, "[OK] AE_Hist_Y: ");
    for (i = 0; i < ISP_AE_HIST_Y_BIN_NUM; i++) {
        strCatVal(ret_string, ae_y_sum.y[i]);
    }

    return 0;
}

static int ISP_AE_Stats_Hist_Bayer_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0, idx = 0, idx_get = 0;
    AE_HIST_MODE_TYPE_E mode;

    if(checkRegIdx(reg, "hist_bayer_", 0, XMEDIA_ISP_BAYER_PATTERN_NUM - 1, &idx_get) == 1)  idx = idx_get;

    ipro_get_AE_hist_mode(&mode);
    if(mode == AE_HIST_MODE_Y) {
        aos_cli_printf("[NG]Please check hist mode, it's not AE_HIST_MODE_BAYER.\r\n");
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        return -1;
    }

    ipro_get_AE_hist_sum_bayer(&ae_bayer_sum);
    sprintf(ret_string, "[OK] AE_%s: ", reg);
    if (idx == 0)      for (i = 0; i < ISP_AE_HIST_BAYER_BIN_NUM; i++) strCatVal(ret_string, ae_bayer_sum.r[i]);
    else if (idx == 1) for (i = 0; i < ISP_AE_HIST_BAYER_BIN_NUM; i++) strCatVal(ret_string, ae_bayer_sum.gr[i]);
    else if (idx == 2) for (i = 0; i < ISP_AE_HIST_BAYER_BIN_NUM; i++) strCatVal(ret_string, ae_bayer_sum.gb[i]);
    else if (idx == 3) for (i = 0; i < ISP_AE_HIST_BAYER_BIN_NUM; i++) strCatVal(ret_string, ae_bayer_sum.b[i]);

    return 0;
}

static int ISP_AE_Stats_Hist_Roi_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    uint16_t x_min = 0, x_max = 0, y_min = 0, y_max = 0;

    if (type == ISP_PARSE_WRITE) {
        x_min = value[0];
        x_max = value[1];
        y_min = value[2];
        y_max = value[3];
        ipro_set_AE_hist_roi(x_min, x_max, y_min, y_max);
        return -1;
    }

    ipro_get_AE_hist_roi(&x_min, &x_max, &y_min, &y_max);
    sprintf(ret_string, "[OK] AE_Hist_Roi: ");
    strCatVal(ret_string, x_min);
    strCatVal(ret_string, x_max);
    strCatVal(ret_string, y_min);
    strCatVal(ret_string, y_max);

    return 0;
}

static int ISP_AE_Stats_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    int sum_val = 0;

    if (type == ISP_PARSE_WRITE) {
        return -1;
    }

    ipro_get_AE_stats(&ae_grid_sum);
    sprintf(ret_string, "[OK] AE_%s: ", reg);
    for (i = 0; i < AE_STATS_GRID_NUM; i++) {
        if (strcmp(reg, "stats_r") == 0) sum_val = ae_grid_sum.r_sum[i];
        else if (strcmp(reg, "stats_g") == 0) sum_val = ae_grid_sum.g_sum[i];
        else if (strcmp(reg, "stats_b") == 0) sum_val = ae_grid_sum.b_sum[i];
        else if (strcmp(reg, "stats_y") == 0) sum_val = ae_grid_sum.y_sum[i];
        strCatVal(ret_string, sum_val);
    }

    return 0;
}



int ISP_AE_Array_Parse(int type, char* reg, int *value, char* ret_string)
{
    if (strstr(reg, "hist_y") != NULL) {
        ISP_AE_Stats_Hist_Y_Parse(type, reg, value, ret_string);
    } else if (strstr(reg, "hist_bayer") != NULL) {
        ISP_AE_Stats_Hist_Bayer_Parse(type, reg, value, ret_string);
    } else if (strstr(reg, "hist_roi") != NULL) {
        ISP_AE_Stats_Hist_Roi_Parse(type, reg, value, ret_string);
    } else if (strstr(reg, "stats_") != NULL) {
        ISP_AE_Stats_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return 0;
}

int ISP_AE_Info_Parse(int type, char* reg, int value, char* ret_string)
{
    int64_t val_get = 0;
    xmedia_u32 pipe = 0;
    xmedia_ae_exp_info exp_info;

    if (type == ISP_PARSE_WRITE) return 0;

    xmedia_ae_query_exposure_info(pipe, &exp_info);
    if (strcmp(reg, "exp_time") == 0) {
        val_get = exp_info.exp_time;
    } else if (strcmp(reg, "gain") == 0) {
        val_get = exp_info.gain;
    } else if (strcmp(reg, "avg_luma") == 0) {
        val_get = exp_info.avg_luma;
    } else {
        return -1;
    }

    sprintf(ret_string, "[OK] Info AE_%s: %ld", reg, (long)val_get);

    return 0;
}
#endif

#if 0
int ISP_AWB_Info_Parse(int type, char* reg, int value, char* ret_string)
{
    int64_t val_get = 0;
    xmedia_u32 pipe = 0;
    xmedia_awb_info wb_info = {0};
    int idx = 0, idx_get = 0;

    if (type == ISP_PARSE_WRITE) return 0;

    xmedia_awb_query_info(pipe, &wb_info);
    if (strcmp(reg, "rgain") == 0) {
        val_get = wb_info.rgain;
    } else if (strcmp(reg, "grgain") == 0) {
        val_get = wb_info.grgain;
    } else if (strcmp(reg, "gbgain") == 0) {
        val_get = wb_info.gbgain;
    } else if (strcmp(reg, "bgain") == 0) {
        val_get = wb_info.bgain;
    } else if (strcmp(reg, "ct") == 0) {
        val_get = wb_info.ct;
    } else if (strcmp(reg, "sat") == 0) {
        val_get = wb_info.saturation;
    } else if (strcmp(reg, "ccm") == 0) {
        val_get =  wb_info.ccm[0];
    } else {
        return -1;
    }

    if (strcmp(reg, "ccm") == 0) {
        sprintf(ret_string, "[OK] Stats AWB_%s:", reg);
        for (int i = 0; i < XMEDIA_ISP_CCM_MATRIX_SIZE; i++) strCatVal(ret_string, wb_info.ccm[i]);
    } else sprintf(ret_string, "[OK] Stats AWB_%s: %ld", reg, (long)val_get);
        
    return 0;
}

static int ISP_AWB3_Stats_Conf_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0, j = 0;
    int val_get = 0;
    isp_awb3_stats_conf_t awb_stats_conf;

    ISP_Get_AWB3_Stats_Conf(&awb_stats_conf);
    /*if (strcmp(reg, "stat_en") == 0) {
        if (type == ISP_PARSE_WRITE) ISP_Set_AWB3_Hist_State(value[0]);
        ISP_Get_AWB3_Hist_State(&val_get);
    } else*/ if (strcmp(reg, "stat_x_min") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.stat_x_min = value[0];
        val_get = awb_stats_conf.stat_x_min;
    } else if (strcmp(reg, "stat_x_max") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.stat_x_max = value[0];
        val_get = awb_stats_conf.stat_x_max;
    } else if (strcmp(reg, "stat_y_min") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.stat_y_min = value[0];
        val_get = awb_stats_conf.stat_y_min;
    } else if (strcmp(reg, "stat_y_max") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.stat_y_max= value[0];
        val_get = awb_stats_conf.stat_y_max;
    } else if (strcmp(reg, "r_ratio_ofst") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.r_ratio_ofst = value[0];
        val_get = awb_stats_conf.r_ratio_ofst;
    } else if (strcmp(reg, "b_ratio_ofst") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.b_ratio_ofst = value[0];
        val_get = awb_stats_conf.b_ratio_ofst;
    } else if (strcmp(reg, "r_ratio_mult") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.r_ratio_mult = value[0];
        val_get = awb_stats_conf.r_ratio_mult;
    } else if (strcmp(reg, "b_ratio_mult") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.b_ratio_mult = value[0];
        val_get = awb_stats_conf.b_ratio_mult;
    } else if (strcmp(reg, "ratio_sum_min") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.ratio_sum_min = value[0];
        val_get = awb_stats_conf.ratio_sum_min;
    } else if (strcmp(reg, "ratio_sum_max") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.ratio_sum_max = value[0];
        val_get = awb_stats_conf.ratio_sum_max;
    } else if (strcmp(reg, "g_divisor") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.g_divisor = value[0];
        val_get = awb_stats_conf.g_divisor;
    } else if (strcmp(reg, "weight_ratio_en") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.weight_ratio_en = value[0];
        val_get = awb_stats_conf.weight_ratio_en;
    } else if (strcmp(reg, "weight_ratio_gain") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.weight_ratio_gain = value[0];
        val_get = awb_stats_conf.weight_ratio_gain;
    } else if (strcmp(reg, "y_r_coeff") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.y_r_coeff = value[0];
        val_get = awb_stats_conf.y_r_coeff;
    } else if (strcmp(reg, "y_g_coeff") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.y_g_coeff = value[0];
        val_get = awb_stats_conf.y_g_coeff;
    } else if (strcmp(reg, "y_b_coeff") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.y_b_coeff = value[0];
        val_get = awb_stats_conf.y_b_coeff;
    } else if (strcmp(reg, "weight_y_use_g") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.weight_y_use_g = value[0];
        val_get = awb_stats_conf.weight_y_use_g;
    } else if (strcmp(reg, "weight_y_en") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.weight_y_en = value[0];
        val_get = awb_stats_conf.weight_y_en;
    } else if (strcmp(reg, "weight_y_gain") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.weight_y_gain = value[0];
        val_get = awb_stats_conf.weight_y_gain;
    } else if (strcmp(reg, "count_zero_weight") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.count_zero_weight = value[0];
        val_get = awb_stats_conf.count_zero_weight;
    #if 0//NONE_LOWCOST
    } else if (strcmp(reg, "hist_y_use_g") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.hist_y_use_g = value[0];
        val_get = awb_stats_conf.hist_y_use_g;
    } else if (strcmp(reg, "bin_pixel_clip") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.bin_pixel_clip = value[0];
        val_get = awb_stats_conf.bin_pixel_clip;
    } else if (strcmp(reg, "bin_y_level") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.bin_y_level = value[0];
        val_get = awb_stats_conf.bin_y_level;
    } else if (strcmp(reg, "bin_ratio_level") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.bin_ratio_level = value[0];
        val_get = awb_stats_conf.bin_ratio_level;
    } else if (strcmp(reg, "bin_clip_en") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.bin_clip_en = value[0];
        val_get = awb_stats_conf.bin_clip_en;
    } else if (strcmp(reg, "bin_mode") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.bin_mode = value[0];
        val_get = awb_stats_conf.bin_mode;
    #endif
    } else if (strcmp(reg, "ink_type") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.ink_type = value[0];
        val_get = awb_stats_conf.ink_type;
    } else if (strcmp(reg, "ink_gain") == 0) {
        if (type == ISP_PARSE_WRITE) awb_stats_conf.ink_gain = value[0];
        val_get = awb_stats_conf.ink_gain;
    } else if (strcmp(reg, "weight_ratio") == 0) {
        if (type == ISP_PARSE_WRITE) {
            awb_stats_conf.ink_gain = value[0];
            for (j = 0; j < ISP_AWB3_WEIGHT_NUM; j++) {
                for (i = 0; i < ISP_AWB3_WEIGHT_NUM; i++) {
                    awb_stats_conf.weight_ratio[j][i] = value[j * ISP_AWB3_WEIGHT_NUM + i];
                }
            }
        }
    } else if (strcmp(reg, "weight_y") == 0) {
        if (type == ISP_PARSE_WRITE) {
            awb_stats_conf.ink_gain = value[0];
            for (j = 0; j < ISP_AWB3_WEIGHT_NUM; j++) {
                for (i = 0; i < ISP_AWB3_WEIGHT_NUM; i++) {
                    awb_stats_conf.weight_y[j][i] = value[j * ISP_AWB3_WEIGHT_NUM + i];
                }
            }
        }
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        if (strcmp(reg, "stat_en") == 0) return 0;
        ISP_Set_AWB3_Stats_Conf(&awb_stats_conf);
        return 0;
    }

    if (strcmp(reg, "weight_ratio") == 0) {
        sprintf(ret_string, "[OK] Stats_Conf AWB_%s: ", reg);
        for (j = 0; j < ISP_AWB3_WEIGHT_NUM; j++) {
            for (i = 0; i < ISP_AWB3_WEIGHT_NUM; i++) {
                strCatVal(ret_string, awb_stats_conf.weight_ratio[j][i]);
            }
        }
    } else if (strcmp(reg, "weight_y") == 0) {
        sprintf(ret_string, "[OK] Stats_Conf AWB_%s: ", reg);
        for (j = 0; j < ISP_AWB3_WEIGHT_NUM; j++) {
            for (i = 0; i < ISP_AWB3_WEIGHT_NUM; i++) {
                strCatVal(ret_string, awb_stats_conf.weight_y[j][i]);
            }
        }
    } else {
        sprintf(ret_string, "[OK] Stats_Conf AWB_%s: %d", reg, val_get);
    }

    return 0;
}

static int ISP_AWB_Stats_Conf_Parse(int type, char* reg, int *value, char* ret_string)
{
    ISP_AWB3_Stats_Conf_Parse(type, reg, value, ret_string);

    return 0;
}
#endif

static int ISP_AWB3_Calib_Bin_Parse(int type, char* reg, int *value, char* ret_string)
{
    const wb3_cal_bin_t *wb3_cal = {0};

    img_param_get_wb3_cal(&wb3_cal);
    if (type == ISP_PARSE_WRITE) {
        img_param_set_wb3_cal((wb3_cal_bin_t *)&value);
        return 0;
    }
    sprintf(ret_string, "[OK] AWB_%s", reg);

    return 0;
}

static int ISP_WB_Info_Calib_Parse(int type, char* reg, int *value, char* ret_string)
{
    int idx = 0, idx_get = 0;
    const wb_info_t *wb_info = {0};
    wb_info_t *wb_info_temp = {0};

    if(checkRegIdx(reg, "wb_info_", 0, MAX_ENTRY_NUM_AWB_INFO_TBL - 1, &idx_get) == 1)  idx = idx_get;

    img_param_get_wb_info(idx, &wb_info);
    if (type == ISP_PARSE_WRITE) {
        wb_info_temp->rb_gain.r_gain = value[0];
        wb_info_temp->rb_gain.b_gain = value[1];
        wb_info_temp->color_temp = value[2];
        img_param_set_wb_info(idx, wb_info_temp);
        return 0;
    }
    sprintf(ret_string, "[OK] AWB_%s", reg);

    return 0;
}

static int ISP_AWB_Calib_Parse(int type, char* reg, int *value, char* ret_string)
{
    if (strstr(reg, "bin") != NULL) {
        ISP_AWB3_Calib_Bin_Parse(type, reg, value, ret_string);
    } else if (strstr(reg, "wb_info_") != NULL) {
        ISP_WB_Info_Calib_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return 0;
}


static int ISP_AWB_SW_Parse(int type, char* reg, int value, char* ret_string)
{
    int64_t val_get = 0;

    //AWB_ALGO algo;
    awb_param_t *param = {0};
    int special_color_en = 0;

    img_param_sw_get_awb_manual(&param);
    /*algo = imgproc_get_awb_algo();
    if (strcmp(reg, "version") == 0) {
        if (type == ISP_PARSE_WRITE) return -1;
        awb_get_algo_version(&val_get);
    } else */if (strcmp(reg, "manual_mode") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_awb_manual_mode(value);
        val_get = img_param_sw_get_awb_manual_mode();
    //} else if (strcmp(reg, "speed") == 0) {
    //    if (type == ISP_PARSE_WRITE) attr.auto_attr.speed = value;
    //    val_get = attr.auto_attr.speed;
    } else if (strcmp(reg, "r_gain") == 0) {
        if (type == ISP_PARSE_WRITE) param->r_gain = value;
        val_get =  param->r_gain;
    } else if (strcmp(reg, "gr_gain") == 0) {
        if (type == ISP_PARSE_WRITE)  param->gr_gain = value;
        val_get =  param->gr_gain;
    } else if (strcmp(reg, "gb_gain") == 0) {
        if (type == ISP_PARSE_WRITE)  param->gb_gain = value;
        val_get =  param->gb_gain;
    } else if (strcmp(reg, "b_gain") == 0) {
        if (type == ISP_PARSE_WRITE)  param->b_gain = value;
        val_get =  param->b_gain;
    /*} else if (strcmp(reg, "register") == 0) {
        if (type == ISP_PARSE_WRITE) {
            xmedia_awb_register(pipe, value);
            return 0;
        }
        val_get = imgproc_get_awb_algo();
    } else if (strcmp(reg, "unregister") == 0) { //read only
        if (type == ISP_PARSE_READ) {
            xmedia_awb_unregister(pipe);
            val_get = imgproc_get_awb_algo();
        } else return -1;
    } else if (strcmp(reg, "cal_rgain_by_temp") == 0) {
        if (type == ISP_PARSE_WRITE) {
            xmedia_awb_cal_gain_by_temp(pipe, &attr, (xmedia_u16)value, &r_gain, &b_gain);
            val_get = r_gain;
            sprintf(ret_string, "[OK] AWB_%s: %ld", reg, (long)val_get);
            return 0;
        }
    } else if (strcmp(reg, "cal_bgain_by_temp") == 0) {
        if (type == ISP_PARSE_WRITE) {
            xmedia_awb_cal_gain_by_temp(pipe, &attr, (xmedia_u16)value, &r_gain, &b_gain);
            val_get = b_gain;
            sprintf(ret_string, "[OK] AWB_%s: %ld", reg, (long)val_get);
            return 0;
        }
    } else if (strcmp(reg, "wb_ct_num") == 0) {
        if (type == ISP_PARSE_WRITE) attr.auto_attr.wb_ct_num = value;
        val_get = attr.auto_attr.wb_ct_num;
*/

#if 0

    } else if (strcmp(reg, "color_temp") == 0) {
        if (type == ISP_PARSE_WRITE) {
            info.color_temp = value;
            MM_ISP_SetAwbInfo(&info);
            imgproc_set_2A_keep_reg("CT", NULL, NULL, value);
        }
        val_get = info.color_temp;
        
    } else if (strcmp(reg, "algo") == 0) {
        if (type == ISP_PARSE_WRITE) MM_ISP_AWBAlgo(value);
        val_get = algo;
#endif
/*
    } else if (strcmp(reg, "special_color_flag") == 0) {
        if (type == ISP_PARSE_WRITE) awb_set_special_color_flag(value);
        awb_get_special_color_flag((int *)&val_get);
    } else if (strcmp(reg, "preference_gain_en") == 0) {
        if (type == ISP_PARSE_WRITE) attr.auto_attr.preference_gain_en = value;
        val_get = attr.auto_attr.preference_gain_en;*/
    } else if (strcmp(reg, "special_color_en") == 0) {
        if (type == ISP_PARSE_WRITE) {
            img_param_sw_set_awb_special_color_en(value);
            //awb_set_special_color_flag(1);
        }
        img_param_sw_get_awb_special_color_en(&special_color_en);
        val_get = special_color_en;
    } else if (strcmp(reg, "wb_info_num") == 0) {
        if (type == ISP_PARSE_WRITE) {
            img_param_set_num_wb_info(value);
        }
        val_get = img_param_get_num_wb_info();
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        img_param_sw_set_awb_manual(param);
        return 0;
    }
    sprintf(ret_string, "[OK] AWB_%s: %ld", reg, (long)val_get);

    return 0;
}

#if 0
int ISP_AWB_Stats_Parse(int type, char* reg, int value, char* ret_string)
{
    int64_t val_get = 0;
    int pipe = 0;
    xmedia_isp_awb_statistics_cfg awb3_sum;

    if (type == ISP_PARSE_WRITE) return 0;

    xmedia_isp_get_awb_statistics_config(pipe, &awb3_sum);
    if (strcmp(reg, "r_sum") == 0) {
        val_get = awb3_sum.r_sum;
    } else if (strcmp(reg, "g_sum") == 0) {
        val_get = awb3_sum.g_sum;
    } else if (strcmp(reg, "b_sum") == 0) {
        val_get = awb3_sum.b_sum;
    } else if (strcmp(reg, "r_sum_hex") == 0) {
        val_get = awb3_sum.r_sum_hex;
    } else if (strcmp(reg, "g_sum_hex") == 0) {
        val_get = awb3_sum.g_sum_hex;
    } else if (strcmp(reg, "b_sum_hex") == 0) {
        val_get = awb3_sum.b_sum_hex;
    } else if (strcmp(reg, "pixel_hex") == 0) {
        val_get = awb3_sum.pixel_hex;
    } else if (strcmp(reg, "r_ratio_hex_y") == 0) {
        val_get = awb3_sum.r_ratio_hex_y;
    } else if (strcmp(reg, "b_ratio_hex_y") == 0) {
        val_get = awb3_sum.b_ratio_hex_y;
    } else if (strcmp(reg, "weight_hex_y") == 0) {
        val_get = awb3_sum.weight_hex_y;
    } else if (strcmp(reg, "pixel_hex_y") == 0) {
        val_get = awb3_sum.pixel_hex_y;
    } else if (strcmp(reg, "r_ratio_grid") == 0) {
        val_get = awb3_sum.r_ratio_grid;
    } else if (strcmp(reg, "b_ratio_grid") == 0) {
        val_get = awb3_sum.b_ratio_grid;
    } else if (strcmp(reg, "weight_grid") == 0) {
        val_get = awb3_sum.weight_grid;
    } else if (strcmp(reg, "pixel_grid") == 0) {
        val_get = awb3_sum.pixel_grid;
    } else {
        return -1;
    }

    sprintf(ret_string, "[OK] Stats AWB_%s: %ld", reg, (long)val_get);

    return 0;
}
#endif
static int ISP_AWB_Parse(int reg_type, int type, char* reg, int value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
        //ret = ISP_AWB_HW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_AWB_FW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_AWB_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}

#if 0
#if 0//NONE_LOWCOST
static int ISP_AWB3_Stats_Hist_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    uint32_t bucket_num;
    uint64_t val_l = 0, val_u = 0, r_ratio = 0, b_ratio = 0;
    uint32_t weight = 0, pixel= 0;
    isp_awb3_bin_mode_e mode;

    if (type == ISP_PARSE_WRITE) {
        return -1;
    }

    ipro_get_AWB3_bin_mode(&mode);
    ipro_get_AWB3_hist_bin_num(&bucket_num);
    ipro_get_AWB3_hist(&awb_stats_hist);

    sprintf(ret_string, "[OK] AWB3_%s %d: ", reg, (int)bucket_num);
    #if 1
    for (i = 0; i < bucket_num; i++) {
        #if 1
        aos_cli_printf("%d: %lu, %lu, %u, %u,\r\n", i, awb_stats_hist.r_ratio[i], awb_stats_hist.b_ratio[i], awb_stats_hist.weight[i], awb_stats_hist.pixel[i]);
        #else
        if (strcmp(reg, "hist_r_ratio") == 0) strCatVal_u64(ret_string, awb_stats_hist.r_ratio[i]);
        else if (strcmp(reg, "hist_b_ratio") == 0) strCatVal_u64(ret_string, awb_stats_hist.b_ratio[i]);
        else if (strcmp(reg, "hist_weight") == 0) strCatVal(ret_string, awb_stats_hist.weight[i]);
        else if (strcmp(reg, "hist_pixel") == 0) strCatVal(ret_string, awb_stats_hist.pixel[i]);
        else {
            if (mode == 0) return -1;
            strCatVal_u64(ret_string, awb_stats_hist.r_ratio[i]);
            strCatVal_u64(ret_string, awb_stats_hist.b_ratio[i]);
            strCatVal(ret_string, awb_stats_hist.weight[i]);
            strCatVal(ret_string, awb_stats_hist.pixel[i]);
        }
        #endif
    }
    #else
    for (i = 0; i < bucket_num; i++) {
        val_u = ((awb_stats_hist.bin[i].footprint[3] << 8) & (0xffffffff00));
        val_l = ((awb_stats_hist.bin[i].footprint[2] >> 24) & (0xff));
        r_ratio = val_u + val_l;//40bit

        val_u = ((awb_stats_hist.bin[i].footprint[2] & (0x00ffffff)) << 16 & (0xffffff0000));
        val_l = (awb_stats_hist.bin[i].footprint[1] >> 16 & (0xffff));
        b_ratio = val_u + val_l;//40bit

        val_u = ((awb_stats_hist.bin[i].footprint[1] & (0x0000ffff)) << 12 & (0xffff000));
        val_l = (awb_stats_hist.bin[i].footprint[0] >> 20 & (0xfff));
        weight = val_u + val_l;//28bit

        val_u = awb_stats_hist.bin[i].footprint[0] & (0x000fffff);
        pixel = val_u;//20bit

        if (i < bucket_num){
            #if 0
            aos_cli_printf("%d: %lu, %lu, %u, %u,\r\n", i, r_ratio, b_ratio, weight, pixel);
            #else
            if (strcmp(reg, "hist_r_ratio") == 0) strCatVal_u64(ret_string, r_ratio);
            else if (strcmp(reg, "hist_b_ratio") == 0) strCatVal_u64(ret_string, b_ratio);
            else if (strcmp(reg, "hist_weight") == 0) strCatVal(ret_string, weight);
            else if (strcmp(reg, "hist_pixel") == 0) strCatVal(ret_string, pixel);
            else {
                if (mode == 0) return -1;
                strCatVal_u64(ret_string, r_ratio);
                strCatVal_u64(ret_string, b_ratio);
                strCatVal(ret_string, weight);
                strCatVal(ret_string, pixel);
            }
            #endif
        }
    }
#endif
    return 0;
}
#endif

int ISP_AWB_Array_Parse(int type, char* reg, int *value, char* ret_string)
{
    /*if (strstr(reg, "hist") != NULL) {
        //ISP_AWB3_Stats_Hist_Parse(type, reg, value, ret_string);
    } else */if (strstr(reg, "rgb_gain") != NULL) {
        //ISP_AWB3_rgb_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return 0;
}


static int ISP_AWB3_Preference_Tbl_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0, idx = 0, idx_get = 0, idx_2 = 0, idx_get_2 = 0;
    xmedia_u32 pipe = 0;
    xmedia_awb_attr attr = {0};

    if(checkRegIdx_Idx(reg, "preference_tbl_", 0, XMEDIA_ISP_ISO_MAX_COUNT - 1, &idx_get, 0, XMEDIA_ISP_AWB_PREFERENCE_NUM - 1, &idx_get_2) == 1) {
        idx = idx_get;
        idx_2 = idx_get_2;
    }

    xmedia_awb_get_attr(pipe, &attr);
    if (type == ISP_PARSE_WRITE) {
        attr.auto_attr.preference_gain[idx][idx_2].r_gain = value[0];
        attr.auto_attr.preference_gain[idx][idx_2].b_gain = value[1];
        xmedia_awb_set_attr(pipe, &attr);
        imgproc_check_2A_module(ISP_ADJ_AWB);
        return 0;
    }

    sprintf(ret_string, "[OK] awb preference gain %d: ", idx);
    strCatVal(ret_string, attr.auto_attr.preference_gain[idx][idx_2].r_gain);
    strCatVal(ret_string, attr.auto_attr.preference_gain[idx][idx_2].b_gain);

    return 0;

}
#endif
static int ISP_AWB3_Special_Coloe_Parse(int type, char* reg, int *value, char* ret_string)
{

    int idx = 0, idx_get = 0;
     awb_special_color_t *cfg = {0};

    if(checkRegIdx(reg, "special_color_", 0, MAX_ENTRY_NUM_AWB3_SPECIAL_COLOR - 1, &idx_get) == 1)  idx = idx_get;

    img_param_sw_get_awb_special_color(idx, &cfg);
    if (type == ISP_PARSE_WRITE) {
        cfg->r_gain = value[0];
        cfg->b_gain = value[1];
        cfg->weight = value[2];
        img_param_sw_set_awb_special_color(idx, cfg);
        //awb_set_special_color_flag(1);
        //imgproc_check_2A_module(ISP_ADJ_AWB);
        return 0;
    }

    sprintf(ret_string, "[OK] awb_%s %d: ", reg, idx);
    strCatVal(ret_string, cfg->r_gain);
    strCatVal(ret_string, cfg->b_gain);
    strCatVal(ret_string, cfg->weight);

    return 0;

}
#if 0
static int ISP_AWB3_WB_CT_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0, idx = 0, idx_get = 0;
    xmedia_u32 pipe = 0;
    xmedia_awb_attr attr = {0};

    if(checkRegIdx(reg, "wb_ct_", 0, XMEDIA_ISP_AWB_WB_CT_MAX_NUM - 1, &idx_get) == 1)  idx = idx_get;

    xmedia_awb_get_attr(pipe, &attr);
    if (type == ISP_PARSE_WRITE) {
        attr.auto_attr.wb_ct[idx].r_gain = value[0];
        attr.auto_attr.wb_ct[idx].b_gain = value[1];
        attr.auto_attr.wb_ct[idx].color_temp = value[2];
        xmedia_awb_set_attr(pipe, &attr);
        imgproc_check_2A_module(ISP_ADJ_AWB);
        return 0;
    }

    sprintf(ret_string, "[OK] awb wb_ct: ");
    strCatVal(ret_string, attr.auto_attr.wb_ct[idx].r_gain);
    strCatVal(ret_string, attr.auto_attr.wb_ct[idx].b_gain);
    strCatVal(ret_string, attr.auto_attr.wb_ct[idx].color_temp);

    return 0;
}
#endif

int ISP_AWB_Auto_Parse(int reg_type, int type, char* reg, int *value, char* ret_string)
{
    if (reg_type == REG_FW) {
        /*if (strstr(reg, "preference_tbl_") != NULL) {
            ISP_AWB3_FW_Preference_Tbl_Parse(type, reg, value, ret_string);
        } else if (strstr(reg, "special_color_") != NULL) {
            ISP_AWB3_FW_Special_Coloe_Parse(type, reg, value, ret_string);
        } else if (strstr(reg, "wb_ct_") != NULL) {
            ISP_AWB3_FW_WB_CT_Parse(type, reg, value, ret_string);
        } else return -1;*/
    } else if (reg_type == REG_SW) {
        /*if (strstr(reg, "preference_tbl_") != NULL) {
            ISP_AWB3_Preference_Tbl_Parse(type, reg, value, ret_string);
        } else*/ if (strstr(reg, "special_color_") != NULL) {
            ISP_AWB3_Special_Coloe_Parse(type, reg, value, ret_string);
        //} else if (strstr(reg, "wb_ct_") != NULL) {
        //    ISP_AWB3_WB_CT_Parse(type, reg, value, ret_string);
        } else return -1;
    } else return -1;

    return 0;
}





static int ISP_CT_SW_Parse(int type, char* reg, int value, char* ret_string)
{

    int idx = 0, idx_get = 0, val_get = 0;
    ct_param_t *param;

    if(checkRegIdx(reg, "coef_", 0, MAX_ENTRY_CT_COEF_NUM - 1, &idx_get) == 1)  idx = idx_get;
    if(checkRegIdx(reg, "pre_",  0, MAX_ENTRY_CT_OFFSET_NUM - 1, &idx_get) == 1)  idx = idx_get;
    if(checkRegIdx(reg, "post_", 0, MAX_ENTRY_CT_OFFSET_NUM - 1, &idx_get) == 1)  idx = idx_get;

    img_param_sw_get_ct_manual(&param);
    if (strcmp(reg, "color_gamut") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_ct_color_gamut((CT_GAMUT_E)value);
        else img_param_sw_get_ct_color_gamut((CT_GAMUT_E*)&val_get);
    } else if (strstr(reg, "coef_") != NULL) {
        if (type == ISP_PARSE_WRITE) param->coef[idx] = value;
        else val_get = param->coef[idx];
    } else if (strstr(reg, "pre_") != NULL) {
        if (type == ISP_PARSE_WRITE) param->pre_offset[idx] = value;
        else val_get = param->pre_offset[idx];
    } else if (strstr(reg, "post_") != NULL) {
        if (type == ISP_PARSE_WRITE) param->post_offset[idx] = value;
        else val_get = param->pre_offset[idx];
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        img_param_sw_set_ct_manual(param);
        return 0;
    }
    sprintf(ret_string, "[OK] CT_%s: %d", reg, val_get);

    return 0;
}

static int ISP_CT_Parse(int reg_type, int type, char* reg, int value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
        //ret = ISP_CT_HW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_CT_FW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_CT_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;

}

static int ISP_CT_Coef_SW_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    int /*idx = 0,*/ idx_get = 0;
    ct_param_t *param;
    int16_t *arr = NULL;
    int arr_len = 0;

    img_param_sw_get_ct_manual(&param);

    /* single index: coef_0..coef_8, pre_offset_0..2, post_offset_0..2 */
    if (checkRegIdx(reg, "coef_", 0, MAX_ENTRY_CT_COEF_NUM - 1, &idx_get) == 1) {
        if (type == ISP_PARSE_WRITE) { param->coef[idx_get] = value[0]; img_param_sw_set_ct_manual(param); return 0; }
        sprintf(ret_string, "[OK] CT_COEF_%s: %d", reg, param->coef[idx_get]);
        return 0;
    } else if (checkRegIdx(reg, "pre_offset_", 0, MAX_ENTRY_CT_OFFSET_NUM - 1, &idx_get) == 1) {
        if (type == ISP_PARSE_WRITE) { param->pre_offset[idx_get] = value[0]; img_param_sw_set_ct_manual(param); return 0; }
        sprintf(ret_string, "[OK] CT_COEF_%s: %d", reg, param->pre_offset[idx_get]);
        return 0;
    } else if (checkRegIdx(reg, "post_offset_", 0, MAX_ENTRY_CT_OFFSET_NUM - 1, &idx_get) == 1) {
        if (type == ISP_PARSE_WRITE) { param->post_offset[idx_get] = value[0]; img_param_sw_set_ct_manual(param); return 0; }
        sprintf(ret_string, "[OK] CT_COEF_%s: %d", reg, param->post_offset[idx_get]);
        return 0;
    }

    /* array: coef(9), pre_offset(3), post_offset(3) */
    if (strcmp(reg, "coef") == 0) {
        arr = param->coef;
        arr_len = MAX_ENTRY_CT_COEF_NUM;
    } else if (strcmp(reg, "pre_offset") == 0) {
        arr = param->pre_offset;
        arr_len = MAX_ENTRY_CT_OFFSET_NUM;
    } else if (strcmp(reg, "post_offset") == 0) {
        arr = param->post_offset;
        arr_len = MAX_ENTRY_CT_OFFSET_NUM;
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        for (i = 0; i < arr_len; i++) {
            arr[i] = value[i];
        }
        img_param_sw_set_ct_manual(param);
        return 0;
    }

    sprintf(ret_string, "[OK] CT_COEF_%s: ", reg);
    for (i = 0; i < arr_len; i++) {
        strCatVal(ret_string, arr[i]);
    }

    return 0;
}

static int ISP_CT_Coef_Parse(int reg_type, int type, char* reg, int *value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
        //ret = ISP_CT_Coef_HW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_CT_Coef_FW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
       ret = ISP_CT_Coef_SW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_CAL) {
       //ret = ISP_CT_Calib_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}
static int ISP_Gamma_SW_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;

    if (strcmp(reg, "mode") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_gamma_curve_mode((GAMMA_CURVE_MODE_E)value);
        img_param_sw_get_gamma_curve_mode((GAMMA_CURVE_MODE_E*)&val_get);
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) return 0;
    sprintf(ret_string, "[OK] SW GAMMA_%s: %d", reg, val_get);

    return 0;
}

static int ISP_Gamma_Parse(int reg_type, int type, char* reg, int value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_SW) {
        ret = ISP_Gamma_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}

int ISP_Gamma_Curve_SW_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    gamma_curve_t *curve;

    img_param_sw_get_gamma(&curve);
    if (type == ISP_PARSE_WRITE) {
        for (i = 0; i < MAX_ENTRY_NUM_GAMMA_COEF_NUM; i++) {
            curve->coef[i] = value[i];
        }
        img_param_sw_set_gamma(curve);
        return 0;
    }

    sprintf(ret_string, "[OK] SW GAMMA_%s: ", reg);
    for (i = 0; i < MAX_ENTRY_NUM_GAMMA_COEF_NUM; i++) {
        strCatVal(ret_string, curve->coef[i]);
    }
    return 0;
}

static int ISP_Gamma_Curve_Parse(int reg_type, int type, char* reg, int *value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
        //ret = ISP_Gamma_Curve_HW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_Gamma_Curve_FW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_Gamma_Curve_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}


static int ISP_TM_Auto_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0, idx = 0, idx_get = 0;
    tm_param_t *param = {0};

    if(checkRegIdx(reg, "dark_weight_", 0, MAX_ENTRY_NUM_TM_IDX_AUTO - 1, &idx_get) == 1)  idx = idx_get;
    if(checkRegIdx(reg, "bright_weight_", 0, MAX_ENTRY_NUM_TM_IDX_AUTO - 1, &idx_get) == 1)  idx = idx_get;

    if (type == ISP_PARSE_WRITE) {
        for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
            img_param_sw_get_tm(i, &param);
            if (strcmp(reg, "curve_weight") == 0) param->curve_weight = value[i];
            else if (strcmp(reg, "dark_max_ratio") == 0) param->dark_max_ratio = value[i];
            else if (strcmp(reg, "bright_max_ratio") == 0) param->bright_max_ratio = value[i];
            else if (strstr(reg, "dark_weight_") != NULL) param->dark_weight[idx] = value[i];
            else if (strstr(reg, "bright_weight_") != NULL) param->bright_weight[idx] = value[i];
            else return -1;
            img_param_sw_set_tm(i, param);
        }
//        imgproc_check_2A_module(ISP_ADJ_AE);
        return 0;
    }

    sprintf(ret_string, "[OK] auto TM_%s: ", reg);
    for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
        img_param_sw_get_tm(i, &param);
        if (strcmp(reg, "curve_weight") == 0) strCatVal(ret_string, param->curve_weight);
        else if (strcmp(reg, "dark_max_ratio") == 0) strCatVal(ret_string, param->dark_max_ratio);
        else if (strcmp(reg, "bright_max_ratio") == 0) strCatVal(ret_string, param->bright_max_ratio);
        else if (strstr(reg, "dark_weight_") != NULL) strCatVal(ret_string, param->dark_weight[idx]);
        else if (strstr(reg, "bright_weight_") != NULL) strCatVal(ret_string, param->bright_weight[idx]);
        else return -1;
    }

    return 0;
}

#if 0
static int ISP_TM_Reg_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;

    if (strcmp(reg, "version") == 0) {
        if (type == ISP_PARSE_WRITE) return -1;
        tm_get_algo_version(&val_get);
    } else {
        return -1;
    }

    sprintf(ret_string, "[OK] TM_%s: %d", reg, val_get);

    return 0;
}
#endif

static int ISP_TM_SW_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;
    int idx= 0, idx_get = 0;
    tm_param_t *param = {0};
    tm_curve_t *curve = {0};

    if(checkRegIdx(reg, "dark_weight_", 0, MAX_ENTRY_NUM_TM_IDX_AUTO - 1, &idx_get) == 1)  idx = idx_get;
    if(checkRegIdx(reg, "bright_weight_", 0, MAX_ENTRY_NUM_TM_IDX_AUTO - 1, &idx_get) == 1)  idx = idx_get;
    if (strcmp(reg, "curve_weight") != 0) {
        if(checkRegIdx(reg, "curve_", 0, MAX_ENTRY_NUM_TM_CURVE_COEF_NUM - 1, &idx_get) == 1)  idx = idx_get;
    }

    img_param_sw_get_tm_manual(&param);
    if (strcmp(reg, "manual_mode") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_tm_manual_mode(value);
        val_get = img_param_sw_get_tm_manual_mode();
    } else if (strcmp(reg, "type") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_tm_type((const TM_TYPE_E)value);
        img_param_sw_get_tm_type((TM_TYPE_E *)&val_get);
    } else  if (strcmp(reg, "curve_weight") == 0) {
        if (type == ISP_PARSE_WRITE) param->curve_weight = value;
        val_get = param->curve_weight;
    } else if (strcmp(reg, "dark_max_ratio") == 0) {
        if (type == ISP_PARSE_WRITE) param->dark_max_ratio = value;
        val_get = param->dark_max_ratio;
    } else if (strcmp(reg, "bright_max_ratio") == 0) {
        if (type == ISP_PARSE_WRITE) param->bright_max_ratio = value;
        val_get = param->bright_max_ratio;
    } else if (strstr(reg, "dark_weight_") != NULL) {
        if (type == ISP_PARSE_WRITE) param->dark_weight[idx] = value;
        val_get = param->dark_weight[idx];
    } else if (strstr(reg, "bright_weight_") != NULL) {
        if (type == ISP_PARSE_WRITE) param->bright_weight[idx] = value;
        val_get = param->bright_weight[idx];
    } else if (strstr(reg, "curve_") != NULL) {
        img_param_sw_get_tm_manual_curve(&curve);
        if (type == ISP_PARSE_WRITE) curve->coef[idx] = value;
        val_get = curve->coef[idx];
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        if (strcmp(reg, "curve_weight") == 0) {
            img_param_sw_set_tm_manual(param);
        } else if (strstr(reg, "curve_") != NULL) {
            img_param_sw_set_tm_manual_curve(curve);
        } else {
            img_param_sw_set_tm_manual(param);
        }
        return 0;
    }
    sprintf(ret_string, "[OK] SW TM_%s: %d", reg, val_get);

    return 0;
}

static int ISP_TM_Parse(int reg_type, int type, char* reg, int value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_SW) {
        ret = ISP_TM_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}

static int ISP_TM_Curve_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0, idx = 0, idx_get = 0;
    tm_curve_t *cfg = {0};

    if(checkRegIdx(reg, "auto_curve_", 0, MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO - 1, &idx_get) == 1)  idx = idx_get;
printf("ISP_TM_Curve_Parse \r\n");
    if (type == ISP_PARSE_WRITE) {
        if (strcmp(reg, "manual_curve") == 0) {
            printf("ISP_TM_Curve_Parse  %s\r\n", reg);
            img_param_sw_get_tm_manual_curve(&cfg);
            for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) cfg->coef[i] = value[i];
            img_param_sw_set_tm_manual_curve(cfg);
        } else if (strstr(reg, "auto_curve_") != NULL) {
            printf("ISP_TM_Curve_Parse  %s\r\n", reg);
            img_param_sw_get_tm_auto_curve(idx, &cfg);
            for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) cfg->coef[i] = value[i];
            img_param_sw_set_tm_auto_curve(idx, cfg);
        } else
            return -1;
//        imgproc_check_2A_module(ISP_ADJ_AE);
        imgproc_update_force();
        return 0;
    }

    sprintf(ret_string, "[OK] auto TM_%s: ", reg);
    if (strcmp(reg, "manual_curve") == 0) {
        img_param_sw_get_tm_manual_curve(&cfg);
    } else if (strstr(reg, "auto_curve_") != NULL) {
        img_param_sw_get_tm_auto_curve(i, &cfg);
    } else 
        return -1;

    for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) strCatVal(ret_string, cfg->coef[idx]);

    return 0;
}

static int ISP_DEFOG_SW_Auto_Parse(int type, char* reg, int *value, char* ret_string)
{

    int i = 0;
    defog_param_t *param = {0};
    
    if (type == ISP_PARSE_WRITE) {
        for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
            img_param_sw_get_defog(i, &param);
            if (strcmp(reg, "enh_weight") == 0) param->enh_weight = value[i];
            else if (strcmp(reg, "bright_enh_level") == 0) param->bright_enh_level = value[i];
            else if (strcmp(reg, "dark_enh_level") == 0) param->dark_enh_level  = value[i];
            else return -1;
            img_param_sw_set_defog(i, param);
        }
        return 0;
    }

    sprintf(ret_string, "[OK] auto DEFOG_%s: ", reg);
    for (i = 0; i < MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO; i++) {
        img_param_sw_get_defog(i, &param);
        if (strcmp(reg, "enh_weight") == 0) strCatVal(ret_string, param->enh_weight);
        else if (strcmp(reg, "bright_enh_level") == 0) strCatVal(ret_string, param->bright_enh_level);
        else if (strcmp(reg, "dark_enh_level") == 0) strCatVal(ret_string, param->dark_enh_level);
        else return -1;
    }
    
    return 0;
}

static int ISP_DEFOG_Auto_Parse(int reg_type, int type, char* reg, int *value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
      //  ret = ISP_DEFOG_HW_Auto_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_DEFOG_FW_Auto_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_DEFOG_SW_Auto_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}

static int ISP_DEFOG_Reg_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;

    if (strcmp(reg, "version") == 0) {
        if (type == ISP_PARSE_WRITE) return -1;
      //  defog_get_algo_version(&val_get);
    } else {
        return -1;
    }

    sprintf(ret_string, "[OK] DEFOG_%s: %d", reg, val_get);

    return 0;
}

static int ISP_DEFOG_SW_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;
    defog_param_t *param = {0};

    img_param_sw_get_defog_manual(&param);
    if (strcmp(reg, "manual_mode") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_defog_manual_mode(value);
        val_get = img_param_sw_get_defog_manual_mode();
    } else if (strcmp(reg, "enh_weight") == 0) {
        if (type == ISP_PARSE_WRITE) param->enh_weight = value;
        val_get = param->enh_weight;
    } else if (strcmp(reg, "bright_enh_level") == 0) {
        if (type == ISP_PARSE_WRITE) param->bright_enh_level = value;
        val_get = param->bright_enh_level;
    } else if (strcmp(reg, "dark_enh_level") == 0) {
        if (type == ISP_PARSE_WRITE) param->dark_enh_level = value;
        val_get = param->dark_enh_level;
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        img_param_sw_set_defog_manual(param);
        return 0;
    }
    sprintf(ret_string, "[OK] SW DEFOG_%s: %d", reg, val_get);

    return 0;
}

static int ISP_DEFOG_Parse(int reg_type, int type, char* reg, int value, char* ret_string)
{
    int ret = -1;

    if (strcmp(reg, "version") == 0) {
       ret = ISP_DEFOG_Reg_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_HW) {
      //  ret = ISP_DEFOG_HW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_DEFOG_FW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_DEFOG_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}

static int ISP_CS_SW_Auto_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    cs_param_t *param = {0};

    if (type == ISP_PARSE_WRITE) {
        for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
            img_param_sw_get_cs(i, &param);
            if (strcmp(reg, "strength") == 0) param->strength = value[i];
            else return -1;
            img_param_sw_set_cs(i, param);
        }
        return 0;
    }

    sprintf(ret_string, "[OK] auto CS_%s: ", reg);
    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        img_param_sw_get_cs(i, &param);
        if (strcmp(reg, "strength") == 0) strCatVal(ret_string, param->strength);
        else return -1;
    }

    return 0;
}

static int ISP_CS_Auto_Parse(int reg_type, int type, char* reg, int *value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
      //  ret = ISP_CS_HW_Auto_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_CS_FW_Auto_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_CS_SW_Auto_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}

static int ISP_CS_Coef_SW_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    cs_trans_tbl_t *trans_tbl = {0};

    if (type == ISP_PARSE_WRITE) {
        for (i = 0; i < MAX_ENTRY_CS_TRANS_TBL_NUM; i++) {
            img_param_sw_get_cs_trnas_tbl(i, &trans_tbl);
            if (strcmp(reg, "strength") == 0) trans_tbl->strength = value[i];
            else if (strcmp(reg, "weight") == 0) trans_tbl->weight = value[i];
            else if (strcmp(reg, "grey_thr") == 0) trans_tbl->grey_thr = value[i];
            else return -1;
            img_param_sw_set_cs_trnas_tbl(i, trans_tbl);
        }
        return 0;
    }

    sprintf(ret_string, "[OK] CS_COEF_%s: ", reg);
    for (i = 0; i < MAX_ENTRY_CS_TRANS_TBL_NUM; i++) {
        img_param_sw_get_cs_trnas_tbl(i, &trans_tbl);
        if (strcmp(reg, "strength") == 0) strCatVal(ret_string, trans_tbl->strength);
        else if (strcmp(reg, "weight") == 0) strCatVal(ret_string, trans_tbl->weight);
        else if (strcmp(reg, "grey_thr") == 0) strCatVal(ret_string, trans_tbl->grey_thr);
        else return -1;
    }

    return 0;
}

static int ISP_CS_Coef_Parse(int reg_type, int type, char* reg, int *value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_SW) {
        ret = ISP_CS_Coef_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}

static int ISP_CS_SW_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;
    int idx= 0, idx_get = 0;
    cs_param_t *param = {0};
    cs_trans_tbl_t *trans_tbl = {0};


    if(checkRegIdx(reg, "strength_", 0, MAX_ENTRY_CS_TRANS_TBL_NUM - 1, &idx_get) == 1)  idx = idx_get;
    if(checkRegIdx(reg, "weight_", 0, MAX_ENTRY_CS_TRANS_TBL_NUM - 1, &idx_get) == 1)  idx = idx_get;
    if(checkRegIdx(reg, "grey_thr_", 0, MAX_ENTRY_CS_TRANS_TBL_NUM - 1, &idx_get) == 1)  idx = idx_get;

    img_param_sw_get_cs_manual(&param);
    img_param_sw_get_cs_trnas_tbl(idx, &trans_tbl);
    if (strcmp(reg, "manual_mode") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_cs_manual_mode(value);
        val_get = img_param_sw_get_cs_manual_mode();
    } else if (strcmp(reg, "strength") == 0) {
        if (type == ISP_PARSE_WRITE) param->strength = value;
        val_get = param->strength;
    } else if (strstr(reg, "strength_") != NULL) {
        if (type == ISP_PARSE_WRITE) trans_tbl->strength = value;
        val_get = trans_tbl->strength;
    } else if (strstr(reg, "weight_") != NULL) {
        if (type == ISP_PARSE_WRITE) trans_tbl->weight = value;
        val_get = trans_tbl->weight;
    } else if (strstr(reg, "grey_thr_") != NULL) {
        if (type == ISP_PARSE_WRITE) trans_tbl->grey_thr = value;
        val_get = trans_tbl->grey_thr;
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        img_param_sw_set_cs_manual(param);
        img_param_sw_set_cs_trnas_tbl(idx, trans_tbl);
        return 0;
    }
    sprintf(ret_string, "[OK] SW CS_%s: %d", reg, val_get);

    return 0;
}

static int ISP_CS_Parse(int reg_type, int type, char* reg, int value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
      //  ret = ISP_CS_HW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_CS_FW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_CS_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}


static int ISP_YUVCTRL_SW_Auto_Parse(int type, char* reg, int *value, char* ret_string)
{
    int i = 0;
    yuvctrl_param_t *param = {0};

    if (type == ISP_PARSE_WRITE) {
        for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
            img_param_sw_get_yuvctrl(i, &param);
            if (strcmp(reg, "brightness") == 0) param->brightness = value[i];
            else if (strcmp(reg, "contrast") == 0) param->contrast = value[i];
            else if (strcmp(reg, "saturation") == 0) param->saturation = value[i];
            else return -1;
            img_param_sw_set_yuvctrl(i, param);
        }
        return 0;
    }

    sprintf(ret_string, "[OK] auto YUVCTRL_%s: ", reg);
    for (i = 0; i < MAX_ENTRY_NUM_AE_ISO_AUTO; i++) {
        img_param_sw_get_yuvctrl(i, &param);
        if (strcmp(reg, "brightness") == 0) strCatVal(ret_string, param->brightness);
        else if (strcmp(reg, "contrast") == 0) strCatVal(ret_string, param->contrast);
        else if (strcmp(reg, "saturation") == 0) strCatVal(ret_string, param->saturation);
        else return -1;
    }

    return 0;
}


static int ISP_YUVCTRL_Auto_Parse(int reg_type, int type, char* reg, int *value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
        //ret = ISP_YUVCTRL_HW_Auto_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_YUVCTRL_FW_Auto_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_YUVCTRL_SW_Auto_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}


static int ISP_YUVCTRL_SW_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;
    yuvctrl_param_t *param = {0};

    img_param_sw_get_yuvctrl_manual(&param);
    if (strcmp(reg, "manual_mode") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_yuvctrl_manual_mode((bool)value);
        val_get = img_param_sw_get_yuvctrl_manual_mode();
    } else if (strcmp(reg, "brightness") == 0) {
        if (type == ISP_PARSE_WRITE) param->brightness = value;
        val_get = param->brightness;
    } else if (strcmp(reg, "contrast") == 0) {
        if (type == ISP_PARSE_WRITE) param->contrast = value;
        val_get = param->contrast;
    } else if (strcmp(reg, "saturation") == 0) {
        if (type == ISP_PARSE_WRITE) param->saturation = value;
        val_get = param->saturation;
    } else {
        return -1;
    }

    if (type == ISP_PARSE_WRITE) {
        img_param_sw_set_yuvctrl_manual(param);
        return 0;
    }
    sprintf(ret_string, "[OK] SW YUVCTRL_%s: %d", reg, val_get);

    return 0;
}

static int ISP_YUVCTRL_Parse(int reg_type, int type, char* reg, int value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
       // ret = ISP_YUVCTRL_HW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_YUVCTRL_FW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
       ret = ISP_YUVCTRL_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}


static int ISP_YUVADJ_SW_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;
    yuvadj_param_t *param = {0};

    img_param_sw_get_yuvadj_manual(&param);
    if (strcmp(reg, "mode") == 0) {
        if (type == ISP_PARSE_WRITE) img_param_sw_set_yuvadj_mode((YUVADJ_RANGE_MODE_E)value);
        img_param_sw_get_yuvadj_mode((YUVADJ_RANGE_MODE_E*)&val_get);
    } else if (strcmp(reg, "y_luma") == 0) {
        if (type == ISP_PARSE_WRITE) param->y_luma = value;
        val_get = param->y_luma;
    } else if (strcmp(reg, "y_mul0") == 0) {
        if (type == ISP_PARSE_WRITE) param->y_mul0 = value;
        val_get = param->y_mul0;
    } else if (strcmp(reg, "y_mul1") == 0) {
        if (type == ISP_PARSE_WRITE) param->y_mul1 = value;
        val_get = param->y_mul1;
    } else if (strcmp(reg, "y_min") == 0) {
        if (type == ISP_PARSE_WRITE) param->y_min = value;
        val_get = param->y_min;
    } else if (strcmp(reg, "y_max") == 0) {
        if (type == ISP_PARSE_WRITE) param->y_max = value;
        val_get = param->y_max;
    } else if (strcmp(reg, "uv_mul0") == 0) {
        if (type == ISP_PARSE_WRITE) param->uv_mul0 = value;
        val_get = param->uv_mul0;
    } else if (strcmp(reg, "uv_mul1") == 0) {
        if (type == ISP_PARSE_WRITE) param->uv_mul1 = value;
        val_get = param->uv_mul1;
    } else if (strcmp(reg, "uv_min") == 0) {
        if (type == ISP_PARSE_WRITE) param->uv_min = value;
        val_get = param->uv_min;
    } else if (strcmp(reg, "uv_max") == 0) {
        if (type == ISP_PARSE_WRITE) param->uv_max = value;
        val_get = param->uv_max;
    } else {
        return -1;
    }


    if (type == ISP_PARSE_WRITE) {
        img_param_sw_set_yuvadj_manual(param);
        return 0;
    }
    sprintf(ret_string, "[OK] SW YUVADJ_%s: %d", reg, val_get);

    return 0;
}

static int ISP_YUVADJ_Parse(int reg_type, int type, char* reg, int value, char* ret_string)
{
    int ret = -1;

    if (reg_type == REG_HW) {
        //ret = ISP_YUVADJ_HW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_FW) {
        //ret = ISP_YUVADJ_FW_Parse(type, reg, value, ret_string);
    } else if (reg_type == REG_SW) {
        ret = ISP_YUVADJ_SW_Parse(type, reg, value, ret_string);
    } else {
        return -1;
    }

    return ret;
}

#define ISP_FW_VERSION "IPRO_v1_0_0"

static int ISP_SYS_Parse(int type, char* reg, int value, char* ret_string)
{
    int val_get = 0;

    if (strcmp(reg, "isp_status") == 0) {
        /* read-only: 0=RUN, 1=PAUSE, 2=STOP */
        imgproc_status(&val_get);
    } else if (strcmp(reg, "version") == 0) {
        sprintf(ret_string, "[OK] SYS_%s: %s", reg, ISP_FW_VERSION);
        return 0;
    } else if (strcmp(reg, "input_resolution") == 0) {
        uint16_t w = 0, h = 0;
        extern void isp_get_input_resolution(uint16_t *width, uint16_t *height);
        isp_get_input_resolution(&w, &h);
        sprintf(ret_string, "[OK] SYS_%s: %dx%d", reg, w, h);
        return 0;
    } else if (strcmp(reg, "sensor_name") == 0) {
        sprintf(ret_string, "[OK] SYS_%s: %s", reg, imgproc_get_sensor_name());
        return 0;
    } else {
        return -1;
    }

    sprintf(ret_string, "[OK] SYS_%s: %d", reg, val_get);
    return 0;
}

static int swich_cmd(char *str)
{
    if (strcmp(str, "module-state") == 0) return MODULE_STATE;
    else if (strcmp(str, "ae-auto") == 0) return  AE_AUTO;
    else if (strcmp(str, "ae-reg") == 0) return  AE_REG;
    /*else if (strcmp(str, "ae-array") == 0) return  AE_ARRAY;
    else if (strcmp(str, "ae-info") == 0) return  AE_INFO;
    else if (strcmp(str, "awb-info") == 0) return  AWB_INFO;*/
    else if (strcmp(str, "awb-auto") == 0) return  AWB_AUTO;
    /*else if (strcmp(str, "awb-stats-conf") == 0) return  AWB_STATS_CONF;
    else if (strcmp(str, "awb-stats") == 0) return  AWB_STATS;*/
    else if (strcmp(str, "awb-cal") == 0) return  AWB_CAL;
    else if (strcmp(str, "awb-reg") == 0) return  AWB_REG;
    /*else if (strcmp(str, "awb-array") == 0) return  AWB_ARRAY;*/
    else if (strcmp(str, "ct-reg") == 0) return  CT_REG;
    else if (strcmp(str, "ct-coef") == 0) return  CT_COEF;
    else if (strcmp(str, "gamma-curve") == 0) return  GAMMA_CURVE;
    else if (strcmp(str, "gamma-reg") == 0) return  GAMMA_REG;
    else if (strcmp(str, "tm-auto") == 0) return  TM_AUTO;
    else if (strcmp(str, "tm-reg") == 0) return TM_REG;
    else if (strcmp(str, "tm-curve") == 0) return  TM_CURVE;
    else if (strcmp(str, "defog-auto") == 0) return  DEFOG_AUTO;
    else if (strcmp(str, "defog-reg") == 0) return  DEFOG_REG;
    else if (strcmp(str, "cs-auto") == 0) return  CS_AUTO;
    else if (strcmp(str, "cs-reg") == 0) return  CS_REG;
    else if (strcmp(str, "cs-coef") == 0) return  CS_COEF;
    else if (strcmp(str, "yuvctrl-auto") == 0) return YUVCTRL_AUTO;
    else if (strcmp(str, "yuvctrl-reg") == 0) return YUVCTRL_REG;
    else if (strcmp(str, "yuvadj-reg") == 0) return  YUVADJ_REG;
    else if (strcmp(str, "sys-reg") == 0) return  SYS_REG;
    else return -1;

}

int parse_isp_cmd(char* module, char* argv_reg, char* argv_val, int payload_len_in, char* ret_string)
{
    int arr_num = 0;
    int ret = -1;
    int *array = NULL;
    char reg_in[32] = {0}, reg[32] = {0};
    int reg_type;

    printf("swich_cmd(module) %d\r\n", swich_cmd(module));
    printf(" module %s, argv_reg %s, argv_val %s\r\n", module, argv_reg, argv_val);

    if (argv_val != NULL) {
        array = malloc(sizeof(int) * payload_len_in);
        memset(array, 0, sizeof(int) * payload_len_in);
        isIntgerArray(argv_val, array, &arr_num); // because token,so argv_val will be change
    }
    printf("arr_num %d\r\n", arr_num);

    int ch = swich_cmd(module);
    switch (ch)
    {

    case MODULE_STATE: {
        if (arr_num != 0) {
            ret = ISP_Module_State_Parse(ISP_PARSE_WRITE, argv_reg, array[0], ret_string);
        } else {
           ret = ISP_Module_State_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case AE_AUTO: {
        if (arr_num != 0) {
            if ((arr_num != MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO) && (arr_num != AE_STATS_GRID_NUM)) ret = -1;
            else ret = ISP_AE_Auto_Parse(ISP_PARSE_WRITE, argv_reg, array, ret_string);
        } else {
            ret = ISP_AE_Auto_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case AE_REG: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_AE_Parse(reg_type, ISP_PARSE_WRITE, reg, array[0], ret_string);
        } else {
            ret = ISP_AE_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    #if 0
    case AE_ARRAY: {
        if (arr_num != 0) {
            ret = ISP_AE_Array_Parse(ISP_PARSE_WRITE, argv_reg, array, ret_string);
        } else {
            ret = ISP_AE_Array_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case AE_INFO: {
        if (arr_num == 0) {
            ret = ISP_AE_Info_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case AWB_INFO: {
        if (arr_num == 0) {
            ret = ISP_AWB_Info_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    #endif
    case AWB_AUTO: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_AWB_Auto_Parse(reg_type, ISP_PARSE_WRITE, reg, array, ret_string);
        } else {
            ret = ISP_AWB_Auto_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    #if 0
    case AWB_STATS: {
        ret = ISP_AWB_Stats_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        if (ret < 0) goto FAIL;
        break;
    }
    case AWB_STATS_CONF: {
        if (arr_num != 0) {
            ret = ISP_AWB_Stats_Conf_Parse(ISP_PARSE_WRITE, argv_reg, array, ret_string);
        } else {
            ret = ISP_AWB_Stats_Conf_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    #endif
    case AWB_CAL: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            if ((arr_num != AWB_CAL_WB_INFO_NUM) && (arr_num != AWB_CAL_AWB_NUM)) ret = -1;
            else ret = ISP_AWB_Calib_Parse(ISP_PARSE_WRITE, reg, array, ret_string);
        } else {
            ret = ISP_AWB_Calib_Parse(ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case AWB_REG: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_AWB_Parse(reg_type, ISP_PARSE_WRITE, reg, array[0], ret_string);
        } else {
            ret = ISP_AWB_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    #if 0
    case AWB_ARRAY: {
        if (arr_num != 0) {
            ret = ISP_AWB_Array_Parse(ISP_PARSE_WRITE, argv_reg, array, ret_string);
        } else {
            ret = ISP_AWB_Array_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    #endif


    case CT_REG: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_CT_Parse(reg_type, ISP_PARSE_WRITE, reg, array[0], ret_string);
        } else {
            ret = ISP_CT_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case CT_COEF: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            if (1/*(arr_num == CT_COEF_NUM) || (arr_num == CT_CAL_REG_NUM)*/) {
                ret = ISP_CT_Coef_Parse(reg_type, ISP_PARSE_WRITE, reg, array, ret_string);
            } else {
                ret = -1;
            }
        } else {
            ret = ISP_CT_Coef_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case GAMMA_CURVE: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            //if ((arr_num == GAMMA_COEF_NUM) || (arr_num == GAMMA_CAL_REG_NUM * 3)) {
                ret = ISP_Gamma_Curve_Parse(reg_type, ISP_PARSE_WRITE, reg, array, ret_string);
            //} else {
            //    ret = -1;
            //}
        } else {
            ret = ISP_Gamma_Curve_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case GAMMA_REG: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_Gamma_Parse(reg_type, ISP_PARSE_WRITE, reg, array[0], ret_string);
        } else {
            ret = ISP_Gamma_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case TM_AUTO: {
        if (arr_num != 0) {
            if (arr_num != MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO) ret = -1;
            else ret = ISP_TM_Auto_Parse(ISP_PARSE_WRITE, argv_reg, array, ret_string);
        } else {
            ret = ISP_TM_Auto_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case TM_REG: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_TM_Parse(reg_type, ISP_PARSE_WRITE, reg, array[0], ret_string);
        } else {
            ret = ISP_TM_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case TM_CURVE: {
        if (arr_num != 0) {
            if (arr_num != MAX_ENTRY_NUM_TM_CURVE_COEF_NUM) ret = -1;
            else ret = ISP_TM_Curve_Parse(ISP_PARSE_WRITE, argv_reg, array, ret_string);
        } else {
            ret = ISP_TM_Curve_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case DEFOG_AUTO: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            if (arr_num != MAX_ENTRY_NUM_AE_SYS_GAIN_AUTO) ret = -1;
            else ret = ISP_DEFOG_Auto_Parse(reg_type, ISP_PARSE_WRITE, reg, array, ret_string);
        } else {
            ret = ISP_DEFOG_Auto_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }

    case DEFOG_REG: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_DEFOG_Parse(reg_type, ISP_PARSE_WRITE, reg, array[0], ret_string);
        } else {
            ret = ISP_DEFOG_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case CS_AUTO: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            if (arr_num != MAX_ENTRY_NUM_AE_ISO_AUTO) ret = -1;
            else ret = ISP_CS_Auto_Parse(reg_type, ISP_PARSE_WRITE, reg, array, ret_string);
        } else {
            ret = ISP_CS_Auto_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case CS_REG: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_CS_Parse(reg_type, ISP_PARSE_WRITE, reg, array[0], ret_string);
        } else {
            ret = ISP_CS_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case CS_COEF: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            if (arr_num != MAX_ENTRY_CS_TRANS_TBL_NUM) ret = -1;
            else ret = ISP_CS_Coef_Parse(reg_type, ISP_PARSE_WRITE, reg, array, ret_string);
        } else {
            ret = ISP_CS_Coef_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case YUVCTRL_AUTO: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            if (arr_num != MAX_ENTRY_NUM_AE_ISO_AUTO) ret = -1;
            else ret = ISP_YUVCTRL_Auto_Parse(reg_type, ISP_PARSE_WRITE, reg, array, ret_string);
        } else {
            ret = ISP_YUVCTRL_Auto_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case YUVCTRL_REG: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_YUVCTRL_Parse(reg_type, ISP_PARSE_WRITE, reg, array[0], ret_string);
        } else {
            ret = ISP_YUVCTRL_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case YUVADJ_REG: {
        sprintf(reg_in, argv_reg);
        reg_type = parseToolReg((char*)&reg_in, (char*)&reg);
        if (arr_num != 0) {
            ret = ISP_YUVADJ_Parse(reg_type, ISP_PARSE_WRITE, reg, array[0], ret_string);
        } else {
            ret = ISP_YUVADJ_Parse(reg_type, ISP_PARSE_READ, reg, 0, ret_string);
        }
        if (ret < 0) goto FAIL;
        break;
    }
    case SYS_REG: {
        ret = ISP_SYS_Parse(ISP_PARSE_READ, argv_reg, 0, ret_string);
        if (ret < 0) goto FAIL;
        break;
    }
    default:
       goto FAIL;
    }

    if (argv_val != NULL) {
        free(array);
    }

    return 0;

FAIL:
    memcpy(ret_string, "[NG]\0", sizeof(char) * 5);
    return -1;

}

