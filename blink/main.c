#include <inttypes.h>
#include <stdbool.h>
// #include <stdio.h>
#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num)) // two byte object(?) left byte is bank representation and right is the num
#define PINNO(pin) (pin & 255)                         // 8 bit pin number  so get just the pin number (rightmost 8 bits of PIN)
#define PINBANK(pin) (pin >> 8)                        // get bank from pin number
struct gpio
{
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};
struct rcc
{
    volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR,
        AHB3RSTR, RESERVED0, APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, AHB3ENR,
        RESERVED2, APB1ENR, APB2ENR, RESERVED3[2],
        AHB1LPENR, AHB2LPENR, AHB3LPENR, RESERVED4, APB1LPENR, APB2LPENR,
        RESERVED5[2], BDCR, CSR, RESERVED6[2], SSCGR, PLLI2SCFGR;
};
// #define GPIO(bank) ((struct gpio *) (0x40020000 + (0x400 * (bank)))) //not certain why this did not work

#define RCC ((struct rcc *)0x40023800)

enum modes
{
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,
    GPIO_MODE_ANALOG
};
static inline struct gpio * GPIO(int bank) {
    return ((struct gpio *) (0x40020000 + (0x400 * (bank))));
}
static inline void gpio_set_mode(uint16_t pin, uint8_t mode)
{
    volatile struct gpio *gpio = GPIO(PINBANK(pin));
    int n = PINNO(pin);            // 13->D
    gpio->MODER &= ~(3U << n * 2); // 11 bit because 2 bits represent one MODER
    gpio->MODER |= ((mode & 3) << (n * 2));
}
static inline void gpio_write(uint16_t pin, bool val)
{ // do this again just for test rn
    volatile struct gpio *gpio = GPIO(PINBANK(pin));
    //   gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
    gpio->ODR = (1U << PINNO(pin)) << (val ? 0 : 16);
}
static inline void spin(volatile uint32_t count)
{
    while (count--)
        (void)0; // noop instruction a number of times
}
static inline uint32_t gpio_read(uint16_t pin) // trying to debug the flash
{
    volatile struct gpio *gpio = GPIO(PINBANK(pin));
    // reading from IDR in the pinNumber'th bit
    uint32_t input = gpio->IDR; //lower 16 bits return result
    // printf("%ld\n", input);
    return input;
}

int main(void)
{
    uint16_t led = PIN('A', 5); // datasheet
    uint16_t button = PIN('C', 13);

    // rcc bit at bank of led  needs to be 1 to enable this pin
    RCC->AHB1ENR |= BIT(PINBANK(led)); // bank b (which is represented as 1 cuz 'B' - 'A' = 1) resets bit
    RCC->AHB1ENR |= BIT(PINBANK(button));
    gpio_set_mode(led, GPIO_MODE_OUTPUT); // set pin to output pin
    gpio_set_mode(button, GPIO_MODE_INPUT);

    for (;;)
    {
        // gpio_read(button);
        uint32_t b_input = gpio_read(button);
        if (b_input >> PINNO(button) != 1U) { // then its truthy
            gpio_write(led, true); // turn bsrr register high
            // spin(1000);
            // gpio_write(led, false);
            // spin(999999);
        }
        else{
            gpio_write(led, false);
        }
        
    }
    return 0;
}
// Startup code
__attribute__((naked, noreturn)) void _reset(void)
{
    // memset .bss to zero, and copy .data section to RAM region
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    for (long *dst = &_sbss; dst < &_ebss; dst++)
        *dst = 0;
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;)
        *dst++ = *src++;

    main(); // Call main()
    for (;;)
        (void)0; // Infinite loop in the case if main() returns
}

extern void _estack(void); // Defined in link.ld

// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const tab[16 + 103])(void) = {
    _estack, _reset};