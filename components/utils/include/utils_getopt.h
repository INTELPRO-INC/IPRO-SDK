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

#ifndef __GETOPT_H__
#define __GETOPT_H__
/**
 * @brief Parameters needed to parse the command line
 *
 */
typedef struct getopt_env {
    char *optarg;    /*!< if the option accepts parameters, then optarg point to the option parameter*/
    int optind;      /*!< current index of argv*/
    int opterr;      /*!< non-zero enable error message output, while 0,no error message output*/
    int optopt;      /*!< contain unrecognized option character*/
    int __optpos;    
} getopt_env_t;

/**
 * @brief Initialize struct getopt_env
 *
 * @param env pointer to struct getopt_env
 * @param opterr set error message output method
 *
 * @return 
 *     -  0: success
 *     - -1: fail
 */
int utils_getopt_init(getopt_env_t *env, int opterr);

/**
 * @brief Parses the command-line arguments
 *
 * @param env pointer to struct getopt_env
 * @param argc the argument count 
 * @param argv the argument array
 *
 * @return 
 *     -  option character : an option was successfully found
 *     - -1 : all command-line options have been parsed 
 *     - '?' : option character was not in optstring
 *     - ':' or '?' : If utils_getopt() encounters an option with a missing argument, then the return value depends on the first character in optstring: if it is ':', then ':' is returned; otherwise '?' is returned
 *
 * @note Example
 * @code
 *
 * #include <utils_getopt.h>
 * #include <stdio.h>
 *
 * void cmd(char *buf, int len, int argc, char **argv)
 * {
 *     int opt;
       getopt_env_t getopt_env;
       utils_getopt_init(&getopt_env, 0);
 *     //put ':' in the starting of the string so that program can distinguish between '?' and ':'
 *     while ((opt = utils_getopt(&getopt_env, argc, argv, ":if:lr")) != -1) {
 *         switch(opt)
 *         {
 *             case 'i':
 *             case 'l':
 *             case 'r':
 *                 printf("option: %c\r\n", opt);
 *                 break;
 *             case 'f':
 *                 printf("filename: %s\r\n", getopt_env.optarg);
 *                 break;
 *             case ':':
                   printf("%s: %c requires an argument\r\n", *argv, getopt_env.optopt);
 *                 break;
 *             case '?':
 *                 printf("unknow option: %c\r\n", getopt_env.optopt);
 *                 break;
 *          }
 *      }
 *      //optind is for the extra arguments which are not parsed
 *      for(; getopt_env.optind < argc; getopt_env.optind++){
 *          printf("extra arguments: %s\r\n", argv[getopt_env.optind]);
 *      }
 *
 *  }
 *  @endcode
 */
int utils_getopt(getopt_env_t *env, int argc, char * const argv[], const char *optstring);

#endif /* __GETOPT_H__ */
