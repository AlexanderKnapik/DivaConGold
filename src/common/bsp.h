/*****************************************************************************/
/*                Board Support Package Hardware Configuration               */
/*****************************************************************************/
#ifndef BSP_H_
#define BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Number of capacitive slider touch pads */
#define SLIDER_ELECTRODE_COUNT (32U)
#define SLIDER_LEDS_COUNT (64U)
#define LEDS_PER_ELECTRODE (2U)

#define MPR121_I2C_TIMEOUT_MS (100U)

/* GPIO Configuration */
/* 0 is active low, 1 is active high */
#define BUTTON_ACTIVE_LEVEL (0U) /* Active Low */
#define BUTTON_SLEW_RATE (GPIO_SLEW_RATE_SLOW)
#define BUTTON_DRIVE_STRENGTH (GPIO_DRIVE_STRENGTH_2MA)

#define BUTTON_LED_ACTIVE_LEVEL (1U) /* Active High */
#define BUTTON_LED_SLEW_RATE (GPIO_SLEW_RATE_SLOW)
#define BUTTON_LED_DRIVE_STRENGTH (GPIO_DRIVE_STRENGTH_4MA)

#define SLIDER_LEDS_PIO_INSTANCE (pio0)
#define SLIDER_LED_DATA_SLEW_RATE (GPIO_SLEW_RATE_SLOW)
#define SLIDER_LED_DATA_DRIVE_STRENGTH (GPIO_DRIVE_STRENGTH_4MA)

#define SLIDER_I2C_INSTANCE (i2c1)
#define SLIDER_I2C_SPEED_HZ (400000) /* 400 kHz */
#define SLIDER_I2C_SDA_Pin (18U)
#define SLIDER_I2C_SCL_Pin (19U)
#define SLIDER_I2C_SLEW_RATE (GPIO_SLEW_RATE_SLOW)
#define SLIDER_I2C_DRIVE_STRENGTH (GPIO_DRIVE_STRENGTH_2MA)

#define DISPLAY_I2C_INSTANCE (i2c0)
#define DISPLAY_I2C_SPEED_HZ (400000) /* 400 kHz */
#define DISPLAY_I2C_SDA_Pin (20U)
#define DISPLAY_I2C_SCL_Pin (21U)
#define DISPLAY_I2C_SLEW_RATE (GPIO_SLEW_RATE_SLOW)
#define DISPLAY_I2C_DRIVE_STRENGTH (GPIO_DRIVE_STRENGTH_2MA)

#define DISPLAY_WIDTH (128U)
#define DISPLAY_HEIGHT (64U)

/* GPIO Pin numbers */
#define BUTTON_R1_Pin (2U)
#define BUTTON_R2_Pin (1U)
#define BUTTON_R3_Pin (0U)

#define BUTTON_L1_Pin (10U)
#define BUTTON_L2_Pin (9U)
#define BUTTON_L3_Pin (8U)

#define BUTTON_UP_Pin (11U)
#define BUTTON_DOWN_Pin (12U)
#define BUTTON_LEFT_Pin (4U)
#define BUTTON_RIGHT_Pin (3U)

#define BUTTON_SELECT_Pin (5U)
#define BUTTON_HOME_Pin (6U)
#define BUTTON_START_Pin (7U)

#define BUTTON_CIRCLE_Pin (13U)
#define BUTTON_CROSS_Pin (14U)
#define BUTTON_SQUARE_Pin (15U)
#define BUTTON_TRIANGLE_Pin (16U)
#define SLIDER_LED_DATA_Pin (17U)

#define LED_CIRCLE_Pin (22U)
#define LED_CROSS_Pin (26U)
#define LED_SQUARE_Pin (27U)
#define LED_TRIANGLE_Pin (28U)

#ifdef __cplusplus
}
#endif

#endif // BSP_H_
