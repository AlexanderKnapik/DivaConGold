/*****************************************************************************/
/*                Board Support Package Hardware Configuration               */
/*****************************************************************************/
#ifndef BSP_H_
#define BSP_H_

#ifdef __cplusplus
extern "C" {
#endif


/* 0 is active low, 1 is active high */
#define BUTTON_ACTIVE_LEVEL (0U)
#define BUTTON_LED_ACTIVE_LEVEL (1U)

#define I2C_SLEW_RATE (GPIO_SLEW_RATE_HIGH)
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

#define SLIDER_I2C1_SDA_Pin (18U)
#define SLIDER_I2C1_SCL_Pin (19U)
#define DISPLAY_I2C0_SDA_Pin (20U)
#define DISPLAY_I2C0_SCL_Pin (21U)

#ifdef __cplusplus
}
#endif

#endif // BSP_H_
