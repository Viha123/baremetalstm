#include <stdint.h>
#define GPIOA ((struct gpio *) 0x40020000)
#define GPIO(bank) ((struct gpio *) 0x40020000 + 0x400*(bank))
#define PIN(bank, num) ((((bank)-'A') << 8) | (num)) // two byte object(?) left byte is bank representation and right is the num
#define PINNO(pin) (pin & 255) //8 bit pin number  so get just the pin number (rightmost 8 bits of PIN)
#define PINBANK(pin) (pin >> 8) //get bank from pin number
struct gpio {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};
enum modes{
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF, 
    GPIO_MODE_ANALOG
};
static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
    // gpio->MODER &= ~(3U << (pin * 2)); //first clear existing setting at that pin
    // gpio->MODER |= (mode & 3) << (pin * 2); //set new mode
    struct gpio *gpio = GPIO(PINBANK(pin));
    uint8_t n = PINNO(pin);
    gpio->MODER &= ~(3U << n * 2); //U suffix makes it unsigned ints instead of ints
    gpio->MODER |= ((mode & 3) << (n*2));
}
uint16_t pin1 = PIN('A', 3); //pin A3
uint16_t pin2 = PIN('G', 11);
gpio_set_mode(pin1, GPIO_MODE_OUTPUT);

