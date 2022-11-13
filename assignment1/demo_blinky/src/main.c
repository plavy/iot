#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

// time between switches on/off in millisecond
#define DELAY 1000

// get a handle on the device-node with the name led0 in the devicetree
// more information on how to access the device tree from C is available at
// https://docs.zephyrproject.org/latest/guides/dts/api-usage.html
#define LED_RED_NODE DT_ALIAS(led0)
#define LED_GREEN_NODE DT_NODELABEL(led1)

// check if the device is set up properly (led0, the red one)
#if DT_NODE_HAS_STATUS(LED_RED_NODE, okay)
// get the label in the device tree of the device node
static const struct gpio_dt_spec red_led =
    GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
#else
// in case the node is not "okay", i.e., the board does not support led
#error "The alias in the devicetree is not defined"
#endif

// check if the device is set up properly (led1, the green one)
#if DT_NODE_HAS_STATUS(LED_GREEN_NODE, okay)
static const struct gpio_dt_spec green_led =
    GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
#else
// in case the node is not "okay", i.e., the board does not support led
#error "The node in the devicetree is not defined"
#endif

void main(void) {
  // flag to memorize the state of the led
  bool led_red = true;
  // store potential error values
  int ret;

  if (!device_is_ready(red_led.port) || !device_is_ready(green_led.port)) {
    // something went wrong with getting the device struct from the
    // device tree
    printk(
        "Something went wrong when obtaining the struct from the device tree");
    return;
  }

  // configure the led_device as output and initialize it to a
  // logical 1
  // all gpio-flags are described at
  // https://docs.zephyrproject.org/latest/reference/peripherals/gpio.html
  ret = gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    return;
  }
  // also configure the green led device for output and
  // initialize it to a logical 0
  ret = gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_INACTIVE);
  if (ret < 0) {
    return;
  }

  // make them blink periodically
  while (1) {
    gpio_pin_set(red_led.port, red_led.pin, (int)led_red);
    gpio_pin_set(green_led.port, green_led.pin, 1 - (int)led_red);
    led_red = !led_red;
    k_msleep(DELAY);
    // TODO: make the blue led blink in between the other two for half the
    // duration of the delay
  }
}
