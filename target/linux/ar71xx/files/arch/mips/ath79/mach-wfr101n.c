/*
 *  PISEN WFR101N board support
 *
 *  Copyright (C) 2016 L. D. Pinney <ldpinney@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *  2023122603560 专属适配
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"

#include "dev-usb.h"


#define WFR101N_GPIO_LED_POWER	9
#define WFR101N_GPIO_LED_WAN 4

#define WFR101N_GPIO_LED_WLAN 3
#define WFR101N_GPIO_LED_LAN	21
#define WFR101N_GPIO_LED_SOUND 20
#define WFR101N_GPIO_LED_FAN	2
#define WFR101N_GPIO_LED_USB	22

#define WFR101N_GPIO_BTN_RESET	17

#define WFR101N_GPIO_BTN_WPS	0
#define WFR101N_GPIO_BTN_FAN	1

#define WFR101N_KEYS_POLL_INTERVAL	10	/* msecs */
#define WFR101N_KEYS_DEBOUNCE_INTERVAL (3 * WFR101N_KEYS_POLL_INTERVAL)

static const char *wfr101n_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data wfr101n_flash_data = {
	.part_probes	= wfr101n_part_probes,
};

static struct gpio_led wfr101n_leds_gpio[] __initdata = {
		{
		.name		= "wfr101n:blue:wan",
		.gpio		= WFR101N_GPIO_LED_WAN,
		.active_low	= 1,
	}, {
		.name		= "wfr101n:blue:power",
		.gpio		= WFR101N_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "wfr101n:blue:wlan",
		.gpio		= WFR101N_GPIO_LED_WLAN,
		.active_low	= 1,
	}, {
		.name		= "wfr101n:blue:lan",
		.gpio		= WFR101N_GPIO_LED_LAN,
		.active_low	= 1,
	}, {
		.name		= "wfr101n:blue:sound",
		.gpio		= WFR101N_GPIO_LED_SOUND,
		.active_low	= 1,
	}, {
		.name		= "wfr101n:blue:fan",
		.gpio		= WFR101N_GPIO_LED_FAN,
		.active_low	= 0,
	}, {
		.name		= "wfr101n:blue:usb",
		.gpio		= WFR101N_GPIO_LED_USB,
		.active_low	= 1,
	},
};

static struct gpio_keys_button wfr101n_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WFR101N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WFR101N_GPIO_BTN_RESET,
		.active_low	= 1,
   	}, {
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WFR101N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WFR101N_GPIO_BTN_WPS,
		.active_low	= 0,
   	}, {
   		.desc		= "FAN button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WFR101N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WFR101N_GPIO_BTN_FAN,
		.active_low	= 0,
	}
};

static void __init wfr101n_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ath79_gpio_function_setup(AR934X_GPIO_FUNC_JTAG_DISABLE,
				 AR934X_GPIO_FUNC_CLK_OBS4_EN);

	ath79_register_m25p80(&wfr101n_flash_data);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(wfr101n_leds_gpio),
				wfr101n_leds_gpio);

	ath79_register_gpio_keys_polled(1, WFR101N_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(wfr101n_gpio_keys),
				wfr101n_gpio_keys);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_PHY_SWAP);

	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, mac, -1);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 0);

	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask = BIT(0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio1_device.dev;
	ath79_register_eth(0);

	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_register_eth(1);

	ath79_register_wmac(ee, mac);

    ath79_register_usb();

}

MIPS_MACHINE(ATH79_MACH_WFR101N, "WFR101N", "PISEN WFR101N",
	     wfr101n_setup);
