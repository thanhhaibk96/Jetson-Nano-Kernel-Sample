#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chau Thanh Hai");
MODULE_DESCRIPTION("A GPIO Toggle Module");

/* Tegra X1 SoC Technical Reference Manual, version 1.3
 *
 * See Chapter 9 "Multi-Purpose I/O Pins", section 9.13 "GPIO Registers"
 * (table 32: GPIO Register Address Map)
 *
 * The GPIO hardware shares PinMux with up to 4 Special Function I/O per
 * pin, and only one of those five functions (SFIO plus GPIO) can be routed to
 * a pin at a time, using the PixMux.
 *
 * In turn, the PinMux outputs signals to Pads using Pad Control Groups. Pad
 * control groups control things like "drive strength" and "slew rate," and
 * need to be reset after deep sleep. Also, different pads have different
 * voltage tolerance. Pads marked "CZ" can be configured to be 3.3V tolerant
 * and driving; and pads marked "DD" can be 3.3V tolerant when in open-drain
 * mode (only.)
 *
 * The CNF register selects GPIO or SFIO, so setting it to 1 forces the GPIO
 * function. This is convenient for those who have a different pinmux at boot.
 */

//  The only address we really need
#define GPIO_216      0x6000d60C    // Jetson Nano  7[AUDIO_MCLK]
#define GPIO_50       0x6000d108    // Jetson Nano 11[UART2_RTS]
#define GPIO_194      0x6000d600    // Jetson Nano 15[LCD_TE]
#define GPIO_14       0x6000d004    // Jetson Nano 13[SPI2_SCK]
#define GPIO_16       0x6000d008    // Jetson Nano 19[SPI1_MOSI]
#define GPIO_38       0x6000d100    // Jetson Nano 33[GPIO_PE6]
#define GPIO_77       0x6000d204    // Jetson Nano 38[I2S4_SDIN] // J

// From https://github.com/leahneukirchen/linux-jetson-tk1/blob/master/drivers/gpio/gpio-tegra.c
#define GPIO_INT_LVL_MASK		0x010101
#define GPIO_INT_LVL_EDGE_RISING	0x000101
#define GPIO_INT_LVL_EDGE_FALLING	0x000100
#define GPIO_INT_LVL_EDGE_BOTH		0x010100
#define GPIO_INT_LVL_LEVEL_HIGH		0x000001
#define GPIO_INT_LVL_LEVEL_LOW		0x000000

enum INOUT { 
    INPUT, OUTPUT=0xFF
};

//  layout based on the definitions above
//  Each GPIO controller has four ports, each port controls 8 pins, each
//  register is interleaved for the four ports, so
//  REGX: port0, port1, port2, port3
//  REGY: port0, port1, port2, port3
typedef struct {
    uint32_t CNF;
    uint32_t _padding1[3];
    uint32_t OE;
    uint32_t _padding2[3];
    uint32_t OUT;
    uint32_t _padding3[3];
    uint32_t IN;
    uint32_t _padding4[3];
    uint32_t INT_STA;
    uint32_t _padding5[3];
    uint32_t INT_ENB;
    uint32_t _padding6[3];
    uint32_t INT_LVL;
    uint32_t _padding7[3];
    uint32_t INT_CLR;
    uint32_t _padding8[3];
}gpio_register;

static gpio_register volatile *gpiobb00;

static int __init GPIOModule_Init(void)
{
	gpiobb00 = (gpio_register volatile*)ioremap(GPIO_216, sizeof(gpio_register));
	if(gpiobb00 == NULL)
	{
		printk(KERN_ERR "Mapping GPIO Address Failed\n");
	}
	
	gpiobb00->CNF = 0x00FF;
	gpiobb00->OE = OUTPUT;
	gpiobb00->OUT = 0xFF;
	
	gpiobb00->INT_ENB = 0x00;
	
	printk(KERN_INFO "This is GPIO Toggle Program!\n");
	
	printk(KERN_INFO "Address of GBIOBB00: %x\n",  GPIO_216);
	
	uint8_t i = 0;
	for(i = 0; i < 10; i++)
	{
		gpiobb00->OUT ^= 0xFF;
		if(gpiobb00->OUT)	printk(KERN_INFO "GPIOBB00 ON!\n");
		else printk(KERN_INFO "GPIOBB00 OFF!\n");
		mdelay(500);
	}
	
	return 0;
}

static void __exit GPIOModule_Exit(void)
{
	gpiobb00->OUT = 0x00;
	gpiobb00->OE = INPUT;
	iounmap(gpiobb00);
}

module_init(GPIOModule_Init);
module_exit(GPIOModule_Exit);
