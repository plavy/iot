#include <zephyr/kernel.h>

int main() {
  printk("Starting system.\n");

  while (true) {
    printk("Test.\n");
    k_msleep(500);
  }
}
