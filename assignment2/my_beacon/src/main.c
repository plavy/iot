#include <sys/types.h>

#include <zephyr/types.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "led.h"
#include "hts.h"

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		      BT_UUID_16_ENCODE(BT_UUID_HTS_VAL),
	)
};

/* BLE connection */
struct bt_conn *conn;
/* Notification state */
volatile bool notify_enable;
/* Temperature sensor measurement */
static double temp_val;
/* HTS measurment characteristic */
static uint8_t htm[5];

static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);
	notify_enable = (value == BT_GATT_CCC_NOTIFY);
	printk("Notification %s\n", notify_enable ? "enabled" : "disabled");
}

BT_GATT_SERVICE_DEFINE(my_temp,
					   BT_GATT_PRIMARY_SERVICE(BT_UUID_HTS),
					   BT_GATT_CHARACTERISTIC(BT_UUID_HTS_MEASUREMENT, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
											  BT_GATT_PERM_NONE, NULL, NULL, NULL),
					   BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

static void bt_ready(int err)
{
	char addr_s[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addr = {0};
	size_t count = 1;

	if (err)
	{
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	/* Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err)
	{
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	bt_id_get(&addr, &count);
	bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

	printk("Beacon started, advertising as %s\n", addr_s);
}

static void connected(struct bt_conn *connected, uint8_t err)
{
	if (err)
	{
		printk("Connection failed (err %u)\n", err);
	}
	else
	{
		printk("Connected\n");
		led_to_green();
		if (!conn)
		{
			conn = bt_conn_ref(connected);
		}
	}
}

static void disconnected(struct bt_conn *disconn, uint8_t reason)
{
	if (conn)
	{
		bt_conn_unref(conn);
		conn = NULL;
	}
	led_to_red();
	printk("Disconnected (reason %u)\n", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

void main(void)
{
	int err;

	err = led_init();
	if (err)
	{
		return;
	}
	err = hts_init();
	if (err)
	{
		return;
	}

	temp_val = 0;

	printk("Starting Beacon\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
	if (err)
	{
		printk("Bluetooth init failed (err %d)\n", err);
	}

	/* Send notification periodically */
	while (1)
	{
		temp_val = get_temperature();
		if (temp_val == 500)
		{
			printk("Temperature sensor error!\n");
		}
		printk("Temperature from sensor: %.1f C\n", temp_val); /* print to RTT */
		
		htm[0] = 0; /* celsius */
		sys_put_le24((uint32_t)(temp_val * 100), (uint8_t *)&htm[1]); /* mantissa */
		htm[4] = (uint8_t) -2; /* exponent */

		bt_gatt_notify(NULL, &my_temp.attrs[1], &htm, sizeof(htm));
		
		k_sleep(K_MSEC(1000));
	}
}
