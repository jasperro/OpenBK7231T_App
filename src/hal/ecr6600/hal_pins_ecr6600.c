#ifdef PLATFORM_ECR6600

#include "../../new_common.h"
#include "../../logging/logging.h"
#include "../../new_cfg.h"
#include "../../new_pins.h"
#include "hal_gpio.h"
#include "hal_pwm.h"
#include "chip_pinmux.h"

extern int g_pwmFrequency;

typedef struct ecrPinMapping_s
{
	const char* name;
	int pin;
} ecrPinMapping_t;

ecrPinMapping_t g_pins[] = {
	{ "GPIO0 (TX2?)",  GPIO_NUM_0 },
	{ "GPIO1 (RX1)",  GPIO_NUM_1 },
	{ "GPIO2 (TX1)",  GPIO_NUM_2 },
	{ "GPIO3",  GPIO_NUM_3 },
	{ "GPIO4",  GPIO_NUM_4 },
	{ "GPIO5 (RX0?)",  GPIO_NUM_5 },
	{ "GPIO6 (TX0?)",  GPIO_NUM_6 },
	{ "GPIO7",  GPIO_NUM_7 },
	{ "GPIO8",  GPIO_NUM_8 },
	{ "GPIO9",  GPIO_NUM_9 },
	{ "GPIO10", GPIO_NUM_10 },
	{ "GPIO11", GPIO_NUM_11 },
	{ "GPIO12", GPIO_NUM_12 },
	{ "GPIO13 (TX2?)", GPIO_NUM_13 },
	{ "GPIO14", GPIO_NUM_14 },
	{ "GPIO15", GPIO_NUM_15 },
	{ "GPIO16", GPIO_NUM_16 },
	{ "GPIO17", GPIO_NUM_17 },
	{ "GPIO18", GPIO_NUM_18 },
	{ "GPIO19", GPIO_NUM_19 }, // Do not use in GPIO mode
	{ "GPIO20", GPIO_NUM_20 },
	{ "GPIO21 (RX0?)", GPIO_NUM_21 },
	{ "GPIO22 (TX0?)", GPIO_NUM_22 },
	{ "GPIO23", GPIO_NUM_23 },
	{ "GPIO24", GPIO_NUM_24 },
	{ "GPIO23", GPIO_NUM_23 },
	{ "GPIO24", GPIO_NUM_24 },
	{ "GPIO25", GPIO_NUM_25 },
};

static int g_numPins = sizeof(g_pins) / sizeof(g_pins[0]);


int PIN_GetPWMIndexForPinIndex(int pin)
{
	// The PWM channels (CH0-CH5) are mapped to the following pins:
	switch(pin)
	{	
		case 0:
		case 22:
			return 0;
		case 1:
		case 23:
			return 1;
		case 2:
		case 24:
		case 16:
			return 2;
		case 3:
		case 25:
			return 3;
		case 4:
		case 14:
			return 4;
		case 15:
		case 17:
			return 5;
		default:
			return -1;
	}
}

int PIN_GetPWMUsedForPinIndex(int pin)
{
	// The PWM channels (CH0-CH5) are mapped to the following pins:
	switch(pin)
	{
		case 0:
			return PWM_CH0_USED_GPIO0;
		case 22:
			return PWM_CH0_USED_GPIO22;
		case 1:
			return PWM_CH1_USED_GPIO1;
		case 23:
			return PWM_CH1_USED_GPIO23;
		case 2:
			return PWM_CH2_USED_GPIO2;
		case 24:
			return PWM_CH2_USED_GPIO24;
		case 16:
			return PWM_CH2_USED_GPIO16;
		case 3:
			return PWM_CH3_USED_GPIO3;
		case 25:
			return PWM_CH3_USED_GPIO25;
		case 4:
			return PWM_CH4_USED_GPIO4;
		case 14:
			return PWM_CH4_USED_GPIO14;
		case 15:
			return PWM_CH5_USED_GPIO15;
		case 17:
			return PWM_CH5_USED_GPIO17;
		default:
			return -1;
	}
}

const char* HAL_PIN_GetPinNameAlias(int index)
{
	if(index >= g_numPins)
		return "error";
	return g_pins[index].name;
}

int HAL_PIN_CanThisPinBePWM(int index)
{
	int ch = PIN_GetPWMIndexForPinIndex(index);
	if(ch == -1) return 0;
	return 1;
}

void HAL_PIN_SetOutputValue(int index, int iVal)
{
	// Has check for index >= g_numPins built in to SDK.
	// Will return different negative values for different errors.
	drv_gpio_write(index, iVal);
}

int HAL_PIN_ReadDigitalInput(int index)
{
	// Has check for index >= g_numPins built in to SDK.
	// Will return different negative values for different errors.
	return drv_gpio_read(index);
}

void HAL_PIN_Setup_Input_Pullup(int index)
{
	// Has check for index >= g_numPins built in to SDK.
	// Will return different negative values for different errors.
	drv_gpio_ioctrl(index, DRV_GPIO_CTRL_PULL_TYPE, DRV_GPIO_ARG_PULL_UP);
}

void HAL_PIN_Setup_Input_Pulldown(int index)
{
	// Has check for index >= g_numPins built in to SDK.
	// Will return different negative values for different errors.
	drv_gpio_ioctrl(index, DRV_GPIO_CTRL_PULL_TYPE, DRV_GPIO_ARG_PULL_DOWN);
}

void HAL_PIN_Setup_Input(int index)
{

	// Has check for index >= g_numPins built in to SDK.
	// Will return different negative values for different errors.

	// Misspelled DIRCTION in SDK
	drv_gpio_ioctrl(index, DRV_GPIO_CTRL_SET_DIRCTION, DRV_GPIO_ARG_DIR_IN);
}

void HAL_PIN_Setup_Output(int index)
{
	// Has check for index >= g_numPins built in to SDK.
	// Will return different negative values for different errors.

	// Misspelled DIRCTION in SDK
	drv_gpio_ioctrl(index, DRV_GPIO_CTRL_SET_DIRCTION, DRV_GPIO_ARG_DIR_OUT);
}

void HAL_PIN_PWM_Stop(int index)
{
	if(index >= g_numPins)
		return;
	int ch = PIN_GetPWMIndexForPinIndex(index);
	if(ch < 0) return;
	drv_pwm_stop(ch);
	drv_pwm_close(ch);
}

void HAL_PIN_PWM_Start(int index)
{
	if(index >= g_numPins)
		return;
	int ch = PIN_GetPWMIndexForPinIndex(index);
	int pwm_used_gpio = PIN_GetPWMUsedForPinIndex(index);
	if(ch < 0 || pwm_used_gpio < 0) return;
	chip_pwm_pinmux_cfg(pwm_used_gpio);
	drv_pwm_open(ch);
}

void HAL_PIN_PWM_Update(int index, float value)
{
	if(index >= g_numPins)
		return;
	int ch = PIN_GetPWMIndexForPinIndex(index);
	if(ch < 0) return;
	if(value < 0)
		value = 0;
	if(value > 100)
		value = 100;
	drv_pwm_config(ch, g_pwmFrequency, (uint32_t)(value * 10));
	drv_pwm_start(ch);
}

unsigned int HAL_GetGPIOPin(int index)
{
	return index;
}

#endif // PLATFORM_ECR6600
