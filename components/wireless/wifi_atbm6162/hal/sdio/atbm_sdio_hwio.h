/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
/* Sdio addr is 4*spi_addr */
#ifndef ATBM_SDIO_HWIO_H
#define ATBM_SDIO_HWIO_H


#define SPI_REG_ADDR_TO_SDIO(spi_reg_addr) (spi_reg_addr<<2)
#define ATBM_SDIO_SPI_DATA_LEN_OFFSET_TO_SDIO(len) ((len+3) >> 2)


#define SDIO_ADDR17BIT_V1(buf_id, mpf, rfu, reg_id_ofs) \
										((((buf_id) & 0x7F) << 8) \
										| (((mpf) & 0x1) << 16) \
										| (((reg_id_ofs) & 0xFF) << 0))
					
#define SDIO_ADDR17BIT_V0(buf_id, mpf, rfu, reg_id_ofs) \
										((((buf_id)    & 0x3F) << 6) \
										| (((rfu)		 & 1) << 5) \
										| (((reg_id_ofs) & 0x1F) << 0))

#define MAX_RETRY		9

int atbm_reg_read_unlock(struct atbmwifi_common *hw_priv, atbm_uint16 addr,
		    void *buf, atbm_uint32 buf_len);
int atbm_reg_write_unlock(struct atbmwifi_common *hw_priv, atbm_uint16 addr,
		     const void *buf, atbm_uint32 buf_len);
int atbm_reg_read(struct atbmwifi_common *hw_priv, atbm_uint16 addr,
		 atbm_void *buf, atbm_uint32 buf_len);
int atbm_reg_write(struct atbmwifi_common *hw_priv, atbm_uint16 addr,
		  const atbm_void *buf, atbm_uint32 buf_len);


int atbm_direct_read_unlock(struct atbmwifi_common *hw_priv, atbm_uint32 addr, atbm_uint32 *val);
int atbm_direct_write_unlock(struct atbmwifi_common *hw_priv, atbm_uint32 addr, atbm_uint32 val);
int atbm_direct_read_reg_32(struct atbmwifi_common *hw_priv, atbm_uint32 addr, atbm_uint32 *val);
int atbm_direct_write_reg_32(struct atbmwifi_common *hw_priv, atbm_uint32 addr, atbm_uint32 val);


int atbm_indirect_read_unlock(struct atbmwifi_common *hw_priv, atbm_uint32 addr, void *buf,
			 atbm_uint32 buf_len, atbm_uint32 prefetch, atbm_uint16 port_addr);
int atbm_indirect_read(struct atbmwifi_common *hw_priv, atbm_uint32 addr, atbm_void *buf,
	  atbm_uint32 buf_len, atbm_uint32 prefetch, atbm_uint16 port_addr);

int atbm_reg_write_16(struct atbmwifi_common *hw_priv,
			atbm_uint16 addr, atbm_uint16 val);
int atbm_reg_read_32(struct atbmwifi_common *hw_priv,	atbm_uint16 addr, atbm_uint32 *val);

int atbm_reg_write_32(struct atbmwifi_common *hw_priv, atbm_uint16 addr, atbm_uint32 val);

int atbm_fw_write(struct atbmwifi_common *priv, atbm_uint32 addr, const atbm_void *buf,
						atbm_uint32 buf_len);
int atbm_ahb_write(struct atbmwifi_common *priv, atbm_uint32 addr, const atbm_void *buf,
			  atbm_uint32 buf_len);

int atbm_ahb_read_unlock(struct atbmwifi_common *hw_priv, atbm_uint32 addr,
				  void *buf, atbm_size_t buf_len);
int atbm_ahb_write_unlock(struct atbmwifi_common *priv, atbm_uint32 addr, const void *buf,
                     atbm_uint32 buf_len);
int atbm_ahb_read_32(struct atbmwifi_common *hw_priv, atbm_uint32 addr, atbm_uint32 *val);

int atbm_ahb_write_32(struct atbmwifi_common *hw_priv, atbm_uint32 addr, atbm_uint32 val);

int atbm_data_force_write(struct atbmwifi_common *hw_priv, const void *buf,
                        int buf_len);
int atbm_sdio_get_rx_len(struct atbmwifi_common *hw_priv, int next_len);
int atbm_sdio_get_tx_free_buffers(struct atbmwifi_common *hw_priv);
int atbm_data_read(struct atbmwifi_common *hw_priv,
		  atbm_void *buf, atbm_uint32 buf_len);
int atbm_data_write(struct atbmwifi_common *hw_priv, const atbm_void *buf,
		 atbm_size_t buf_len);
int atbm_data_write_sgs(struct atbmwifi_common *hw_priv, const atbm_void **buf,
			atbm_size_t *buf_len);

#endif
