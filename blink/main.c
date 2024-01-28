// #include <inttypes.h>
// #include <stdbool.h>
// #define BIT(x) (1UL << (x))
// #define PIN(bank, num) ((((bank)-'A') << 8) | (num)) // two byte object(?) left byte is bank representation and right is the num
// #define PINNO(pin) (pin & 255) //8 bit pin number  so get just the pin number (rightmost 8 bits of PIN)
// #define PINBANK(pin) (pin >> 8) //get bank from pin number
// struct gpio {
//     volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
// };
// struct rcc {
//     volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR,AHB2RSTR,
//     AHB3RSTR,RESERVED0, APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, AHB3ENR,
//     RESERVED2, APB1ENR, APB2ENR, RESERVED3[2],
//     AHB1LPENR, AHB2LPENR, AHB3LPENR, RESERVED4, APB1LPENR, APB2LPENR, 
//     RESERVED5[2], BDCR, CSR, RESERVED6[2], SSCGR, PLLI2SCFGR;
//     // volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, AHB3RSTR,
//     // RESERVED0, APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, AHB3ENR,
//     // RESERVED2, APB1ENR, APB2ENR, RESERVED3[2], AHB1LPENR, AHB2LPENR,
//     // AHB3LPENR, RESERVED4, APB1LPENR, APB2LPENR, RESERVED5[2], BDCR, CSR,
//     // RESERVED6[2], SSCGR, PLLI2SCFGR;
// };
// #define GPIO(bank) ((struct gpio *) 0x40020000 + 0x400*(bank))
// #define RCC ((struct rcc *) 0x40023800)
// enum modes{
//     GPIO_MODE_INPUT,
//     GPIO_MODE_OUTPUT,
//     GPIO_MODE_AF, 
//     GPIO_MODE_ANALOG
// };
// static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
//     struct gpio *gpio = GPIO(PINBANK(pin));
//     int n = PINNO(pin);
//     gpio->MODER &= ~(3U << n * 2); //U suffix makes it unsigned ints instead of ints
//     gpio->MODER |= ((mode & 3) << (n*2));
// }
// static inline void gpio_write(uint16_t pin, bool val) { // do this again just for test rn
//   struct gpio *gpio = GPIO(PINBANK(pin));
// //   gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
//   gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
  
// }
// static inline void spin(volatile uint32_t count) {
//   while (count--) (void) 0; //noop instruction a number of times
// }

// int main(void) {
//     uint16_t led = PIN('B', 13); //datasheet
//     //rcc bit at bank of led  needs to be 1 to enable this pin
//     RCC->AHB1ENR |= BIT(PINBANK(led)); //bank b (which is represented as 1 cuz 'B' - 'A' = 1)
//     gpio_set_mode(led, GPIO_MODE_OUTPUT); //set pin to output pin 
//     for(;;) { //do this again i nwant to sleep
//         gpio_write(led, true); //turn bsrr register high
//         spin(999999);
//         gpio_write(led, false);
//         spin(999999);

//     }
//     return 0;
// }
#include <inttypes.h>
#include <stdbool.h>

#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

struct rcc {
  volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, AHB3RSTR,
      RESERVED0, APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, AHB3ENR,
      RESERVED2, APB1ENR, APB2ENR, RESERVED3[2], AHB1LPENR, AHB2LPENR,
      AHB3LPENR, RESERVED4, APB1LPENR, APB2LPENR, RESERVED5[2], BDCR, CSR,
      RESERVED6[2], SSCGR, PLLI2SCFGR;
};
#define RCC ((struct rcc *) 0x40023800)

struct gpio {
  volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};
#define GPIO(bank) ((struct gpio *) (0x40020000 + 0x400 * (bank)))

// Enum values are per datasheet: 0, 1, 2, 3
enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
  struct gpio *gpio = GPIO(PINBANK(pin));  // GPIO bank
  int n = PINNO(pin);                      // Pin number
  gpio->MODER &= ~(3U << (n * 2));         // Clear existing setting
  gpio->MODER |= (mode & 3U) << (n * 2);   // Set new mode
}

static inline void gpio_write(uint16_t pin, bool val) {
  struct gpio *gpio = GPIO(PINBANK(pin));
  gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
}

static inline void spin(volatile uint32_t count) {
  while (count--) asm("nop");
}

int main(void) {
  uint16_t led = PIN('B', 13);            // Blue LED
  RCC->AHB1ENR |= BIT(PINBANK(led));     // Enable GPIO clock for LED
  gpio_set_mode(led, GPIO_MODE_OUTPUT);  // Set blue LED to output mode
  for (;;) {
    gpio_write(led, true);
    spin(999999);
    gpio_write(led, false);
    spin(999999);
  }
  return 0;
}
// Startup code
__attribute__((naked, noreturn)) void _reset(void) {
  // memset .bss to zero, and copy .data section to RAM region
  extern long _sbss, _ebss, _sdata, _edata, _sidata;
  for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
  for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

  main();             // Call main()
  for (;;) (void) 0;  // Infinite loop in the case if main() returns
}

extern void _estack(void);  // Defined in link.ld

// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = {
    _estack, _reset};