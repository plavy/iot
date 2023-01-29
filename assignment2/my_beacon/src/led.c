#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);
// static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(DT_NODELABEL(led2), gpios);
static bool led_ok;

void led_to_green(void) {
    printk("LED to green\n");
    gpio_pin_set(led_red.port, led_red.pin, 0);
    gpio_pin_set(led_green.port, led_green.pin, 1);
}

void led_to_red(void) {
    printk("LED to red\n");
    gpio_pin_set(led_red.port, led_red.pin, 1);
    gpio_pin_set(led_green.port, led_green.pin, 0);

}

int led_init(void)
{
	int ret;

	led_ok = device_is_ready(led_red.port) && device_is_ready(led_green.port);
	if (!led_ok) {
		printk("Error: LED is not ready!\n");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Error %d: failed to configure GPIO %s pin %d\n",
			ret, led_red.port->name, led_red.pin);
	}

    ret = gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		printk("Error %d: failed to configure GPIO %s pin %d\n",
			ret, led_green.port->name, led_green.pin);
	}

	return ret;
}