#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>

const struct device* dev;

double get_temperature(void)
{
	struct sensor_value temp;
	if (sensor_sample_fetch(dev) < 0) {
		printk("Sensor sample update error\n");
		return 500;
	}

	if (sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0) {
		printk("Cannot read HTS221 temperature channel\n");
		return 500;
	}

    return sensor_value_to_double(&temp);
}

int hts_init(void)
{
    const struct device *dev_var = DEVICE_DT_GET_ONE(st_hts221);
    dev = dev_var;

	if (!device_is_ready(dev)) {
		printk("Error: Temperature sensor is not ready!\n");
		return -ENODEV;
	}
    return 0;
}