#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

// initial time between led switch in milliseconds
#define DELAY 400

// get a handle on the device-node with the name led0 in the devicetree
// more information on how to access the device tree from C is available at
// https://docs.zephyrproject.org/latest/guides/dts/api-usage.html
#define LED_RED_NODE DT_ALIAS(led0) // get red led usign alias
#define LED_GREEN_NODE DT_NODELABEL(led1) // get green led using node label
#define LED_BLUE_NODE DT_PATH(leds, led_2) // get blue led using path
#define SW0_NODE DT_ALIAS(sw0) // get button using alias

// check if the device is set up properly (button)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
                                                              {0});
static struct gpio_callback button_cb_data;

// check if the device is set up properly (led0, the red one)
#if DT_NODE_HAS_STATUS(LED_RED_NODE, okay)
// get the label in the device tree of the device node
static const struct gpio_dt_spec red_led =
    GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
#else
// in case the node is not "okay", i.e., the board does not support led
#error "The node in the devicetree is not defined"
#endif

// check if the device is set up properly (led1, the green one)
#if DT_NODE_HAS_STATUS(LED_GREEN_NODE, okay)
static const struct gpio_dt_spec green_led =
    GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
#else
// in case the node is not "okay", i.e., the board does not support led
#error "The node in the devicetree is not defined"
#endif

// check if the device is set up properly (led2, the blue one)
#if DT_NODE_HAS_STATUS(LED_BLUE_NODE, okay)
static const struct gpio_dt_spec blue_led =
    GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);
#else
// in case the node is not "okay", i.e., the board does not support led
#error "The node in the devicetree is not defined"
#endif

// initialize current delay to the initial one
int delay = DELAY;

// initialize speed iterator
int i = 0;

// change the delay on button press
void button_pressed(const struct device *dev, struct gpio_callback *cb,
                    uint32_t pins)
{
  printk("Button pressed at %" PRIu32 ". Changing speed\n", k_cycle_get_32());
  // increase speed iterator going in circle through 0, 1, 2
  i = (i + 1) % 3;
  // update the delay based on speed iterator
  delay = DELAY * (i + 1);
}

void main(void)
{

  // store potential error values
  int ret;

  if (!device_is_ready(red_led.port) || !device_is_ready(green_led.port) || !device_is_ready(blue_led.port) || !device_is_ready(button.port))
  {
    // something went wrong with getting the device struct from the
    // device tree
    printk(
        "Something went wrong when obtaining the struct from the device tree");
    return;
  }

  // configure button as input
  ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
  if (ret != 0)
  {
    printk("Error %d: failed to configure %s pin %d\n",
           ret, button.port->name, button.pin);
    return;
  }

  // set interrupt of the button and bind it to function `button_pressed`
  ret = gpio_pin_interrupt_configure_dt(&button,
                                        GPIO_INT_EDGE_TO_ACTIVE);
  if (ret != 0)
  {
    printk("Error %d: failed to configure interrupt on %s pin %d\n",
           ret, button.port->name, button.pin);
    return;
  }
  gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
  gpio_add_callback(button.port, &button_cb_data);
  printk("Set up button at %s pin %d\n", button.port->name, button.pin);

  // configure 3 leds for output and
  // initialize them to a logical 0
  // all gpio-flags are described at
  // https://docs.zephyrproject.org/latest/reference/peripherals/gpio.html
  ret = gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_INACTIVE);
  if (ret < 0)
  {
    return;
  }
  ret = gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_INACTIVE);
  if (ret < 0)
  {
    return;
  }
  ret = gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT_INACTIVE);
  if (ret < 0)
  {
    return;
  }
  printk("Set up 3 leds\n");

  // start and repeat the led sequence
  while (1)
  {
    printk("Starting from beginning!\n");
    gpio_pin_set(red_led.port, red_led.pin, 1);
    k_msleep(delay);
    gpio_pin_set(green_led.port, green_led.pin, 1);
    k_msleep(delay);
    gpio_pin_set(blue_led.port, blue_led.pin, 1);
    k_msleep(delay);
    gpio_pin_set(green_led.port, green_led.pin, 0);
    k_msleep(delay);
    gpio_pin_set(red_led.port, red_led.pin, 0);
    k_msleep(delay);
    gpio_pin_set(blue_led.port, blue_led.pin, 0);
    k_msleep(delay);
    printk("Starting the second part!\n");
    gpio_pin_set(blue_led.port, blue_led.pin, 1);
    k_msleep(delay);
    gpio_pin_set(green_led.port, green_led.pin, 1);
    k_msleep(delay);
    gpio_pin_set(red_led.port, red_led.pin, 1);
    k_msleep(delay);
    gpio_pin_set(green_led.port, green_led.pin, 0);
    k_msleep(delay);
    gpio_pin_set(blue_led.port, blue_led.pin, 0);
    k_msleep(delay);
    gpio_pin_set(red_led.port, red_led.pin, 0);
    k_msleep(delay);
  }
}
