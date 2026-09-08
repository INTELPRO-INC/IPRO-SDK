# Copyright (c) 2025 IPRO
# SPDX-License-Identifier: Apache-2.0

# IPRO7 Zephyr Drivers

# GPIO Driver
zephyr_library_sources_ifdef(CONFIG_GPIO_IPRO7 gpio/gpio_ipro7.c)

# PWM Driver  
zephyr_library_sources_ifdef(CONFIG_PWM_IPRO7 pwm/pwm_ipro7.c)

# SPI Driver
zephyr_library_sources_ifdef(CONFIG_SPI_IPRO7 spi/spi_ipro7.c)

# I2C Driver
zephyr_library_sources_ifdef(CONFIG_I2C_IPRO7 i2c/i2c_ipro7.c)

# ADC Driver
zephyr_library_sources_ifdef(CONFIG_ADC_IPRO7 adc/adc_ipro7.c)

# Include paths for IPRO driver headers
zephyr_include_directories(${IPRO_SDK_BASE}/bsp/drivers/ipro7_soc/std_drv/inc)
zephyr_include_directories(${IPRO_SDK_BASE}/bsp/drivers/ipro7_soc/regs)
