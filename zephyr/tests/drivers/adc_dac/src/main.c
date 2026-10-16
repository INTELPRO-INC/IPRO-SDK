/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * adc_ipro + dac_ipro on the internal channels only, so no wiring is needed:
 * GND (23) reads near zero, Vref (16) and VBAT/2 (18) read plausible values,
 * the temperature sensor pair converts, a multi-channel scan keeps channel
 * order, 14/16-bit averaging works, async read completes, and each DAC
 * channel written with a code is read back within tolerance on ADC channel
 * 12 (DAC A) / 13 (DAC B).
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/dac.h>
#include <stdlib.h>

static const struct device *const adc = DEVICE_DT_GET(DT_NODELABEL(adc0));
static const struct device *const dac = DEVICE_DT_GET(DT_NODELABEL(dac0));

#define CH_DACA 12
#define CH_DACB 13
#define CH_TSEN_P 14
#define CH_TSEN_N 15
#define CH_VREF 16
#define CH_VBAT 18
#define CH_GND 23

static int setup_channel(uint8_t id)
{
	struct adc_channel_cfg cfg = {
		.gain = ADC_GAIN_1,
		.reference = ADC_REF_INTERNAL,
		.acquisition_time = ADC_ACQ_TIME_DEFAULT,
		.channel_id = id,
	};

	return adc_channel_setup(adc, &cfg);
}

static int read_one(uint8_t id, uint8_t resolution, uint8_t oversampling, uint16_t *out)
{
	struct adc_sequence seq = {
		.channels = BIT(id),
		.buffer = out,
		.buffer_size = sizeof(*out),
		.resolution = resolution,
		.oversampling = oversampling,
	};

	return adc_read(adc, &seq);
}

static int32_t to_mv(uint16_t raw, uint8_t resolution)
{
	int32_t mv = raw;

	zassert_ok(adc_raw_to_millivolts(adc_ref_internal(adc), ADC_GAIN_1, resolution, &mv));
	return mv;
}

static void *suite_setup(void)
{
	zassert_true(device_is_ready(adc));
	zassert_true(device_is_ready(dac));
	zassert_ok(setup_channel(CH_GND));
	zassert_ok(setup_channel(CH_VREF));
	zassert_ok(setup_channel(CH_VBAT));
	zassert_ok(setup_channel(CH_TSEN_P));
	zassert_ok(setup_channel(CH_TSEN_N));
	zassert_ok(setup_channel(CH_DACA));
	zassert_ok(setup_channel(CH_DACB));
	return NULL;
}

ZTEST(ipro_adc_dac, test_internal_channels_12bit)
{
	uint16_t gnd, vref, vbat, tp, tn;

	zassert_ok(read_one(CH_GND, 12, 0, &gnd));
	zassert_ok(read_one(CH_VREF, 12, 0, &vref));
	zassert_ok(read_one(CH_VBAT, 12, 0, &vbat));
	zassert_ok(read_one(CH_TSEN_P, 12, 0, &tp));
	zassert_ok(read_one(CH_TSEN_N, 12, 0, &tn));

	int32_t gnd_mv = to_mv(gnd, 12), vref_mv = to_mv(vref, 12), vbat_mv = to_mv(vbat, 12);

	printk("GND %u (%d mV) Vref %u (%d mV) VBAT/2 %u (%d mV) TSEN %u/%u\n", gnd, gnd_mv, vref,
	       vref_mv, vbat, vbat_mv, tp, tn);
	zassert_true(gnd_mv < 60, "GND reads %d mV", gnd_mv);
	/* Internal Vref channel: the 1.x V bandgap-derived reference. */
	zassert_true(vref_mv > 600 && vref_mv < 2000, "Vref reads %d mV", vref_mv);
	/* VBAT/2 with a 3.3 V supply: about 1650 mV (USB-powered EVB). */
	zassert_true(vbat_mv > 1200 && vbat_mv < 2100, "VBAT/2 reads %d mV", vbat_mv);
	zassert_true(tp > 0 && tn > 0, "temperature sensor channels read zero");
}

ZTEST(ipro_adc_dac, test_resolutions)
{
	uint16_t v12, v14, v16;

	zassert_ok(read_one(CH_VBAT, 12, 0, &v12));
	zassert_ok(read_one(CH_VBAT, 14, 4, &v14));
	zassert_ok(read_one(CH_VBAT, 16, 7, &v16));

	int32_t a = to_mv(v12, 12), b = to_mv(v14, 14), c = to_mv(v16, 16);

	printk("VBAT/2 12b %d mV, 14b %d mV, 16b %d mV\n", a, b, c);
	zassert_true(abs(a - b) < 60 && abs(a - c) < 60, "resolutions disagree: %d %d %d", a, b, c);
	zassert_equal(read_one(CH_VBAT, 12, 4, &v12), -EINVAL, "12-bit takes no oversampling");
	zassert_equal(read_one(CH_VBAT, 10, 0, &v12), -EINVAL);
}

ZTEST(ipro_adc_dac, test_scan_order)
{
	uint16_t buf[3] = {0};
	struct adc_sequence seq = {
		.channels = BIT(CH_GND) | BIT(CH_VREF) | BIT(CH_VBAT),
		.buffer = buf,
		.buffer_size = sizeof(buf),
		.resolution = 12,
	};

	zassert_ok(adc_read(adc, &seq));
	/* Sequence order = ascending channel id: VREF(16), VBAT(18), GND(23). */
	printk("scan: %u %u %u\n", buf[0], buf[1], buf[2]);
	zassert_true(buf[2] < buf[0] && buf[2] < buf[1], "GND must be the smallest");
	zassert_true(buf[1] > buf[0], "VBAT/2 should exceed Vref");
	seq.buffer_size = 2;
	zassert_equal(adc_read(adc, &seq), -ENOMEM);
}

ZTEST(ipro_adc_dac, test_async)
{
	uint16_t v = 0;
	struct k_poll_signal sig = K_POLL_SIGNAL_INITIALIZER(sig);
	struct k_poll_event ev = K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, &sig);
	struct adc_sequence seq = {
		.channels = BIT(CH_VBAT), .buffer = &v, .buffer_size = sizeof(v), .resolution = 12,
	};

	zassert_ok(adc_read_async(adc, &seq, &sig));
	zassert_ok(k_poll(&ev, 1, K_MSEC(100)));
	zassert_equal(sig.result, 0);
	zassert_true(v > 0);
}

static void check_dac(uint8_t dac_ch, uint8_t adc_ch, uint32_t code)
{
	uint16_t raw;

	zassert_ok(dac_write_value(dac, dac_ch, code));
	k_msleep(2);
	zassert_ok(read_one(adc_ch, 14, 4, &raw));

	/* GPDAC with the internal reference: Vout = 0.2 V + 1.6 V * code / 1024
	 * (the SDK's DVT formula), read through the ADC on 3.2 V full scale. */
	int32_t mv = to_mv(raw, 14);
	int32_t expect = 200 + (1600 * (int32_t)code) / 1024;

	printk("DAC%c code %u -> ADC ch%u raw %u = %d mV (expect ~%d)\n", 'A' + dac_ch, code, adc_ch,
	       raw, mv, expect);
	zassert_true(abs(mv - expect) < 120, "DAC code %u reads %d mV, expected %d", code, mv,
		     expect);
}

ZTEST(ipro_adc_dac, test_dac_readback)
{
	struct dac_channel_cfg a = {.channel_id = 0, .resolution = 10, .internal = true};
	struct dac_channel_cfg b = {.channel_id = 1, .resolution = 10, .internal = true};
	uint16_t lo, mid, hi;

	zassert_ok(dac_channel_setup(dac, &a));
	zassert_ok(dac_channel_setup(dac, &b));
	zassert_equal(dac_write_value(dac, 0, 1024), -EINVAL, "10-bit range");
	zassert_equal(dac_write_value(dac, 2, 0), -EINVAL);

	check_dac(0, CH_DACA, 0);
	zassert_ok(read_one(CH_DACA, 14, 4, &lo));
	zassert_ok(dac_write_value(dac, 0, 512)); k_msleep(2);
	zassert_ok(read_one(CH_DACA, 14, 4, &mid));
	zassert_ok(dac_write_value(dac, 0, 1023)); k_msleep(2);
	zassert_ok(read_one(CH_DACA, 14, 4, &hi));
	printk("DACA ramp: %u %u %u\n", lo, mid, hi);
	zassert_true(lo < mid && mid < hi, "DAC A not monotonic: %u %u %u", lo, mid, hi);
	/* mid should sit near the middle of lo..hi (+-15 % of span) */
	zassert_true(abs((int)mid - (int)((lo + hi) / 2)) < (int)((hi - lo) * 15 / 100 + 20),
		     "DAC A mid-code off: %u %u %u", lo, mid, hi);

	check_dac(1, CH_DACB, 0);
	check_dac(1, CH_DACB, 1023);
}

ZTEST_SUITE(ipro_adc_dac, NULL, suite_setup, NULL, NULL, NULL);
