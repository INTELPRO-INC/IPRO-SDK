#ifndef _SEC_MUTEX_H
#define _SEC_MUTEX_H

#include "platform_common.h"

void ipro_sec_mutex_init(void);
int ipro_sec_aes_mutex_take(void);
int ipro_sec_aes_mutex_give(void);
int ipro_sec_sha_mutex_take(void);
int ipro_sec_sha_mutex_give(void);
int ipro_sec_pka_mutex_take(void);
int ipro_sec_pka_mutex_give(void);

#endif
