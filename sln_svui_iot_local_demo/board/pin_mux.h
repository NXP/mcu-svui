/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

#define BOARD_INITSWITCHES_IOMUXC_GPR_GPR27_GPIO_MUX2_GPIO_SEL_MASK 0x0210U /*!< GPIO2 and GPIO7 share same IO MUX function, GPIO_MUX2 selects one GPIO function: affected bits mask */

/*!
 * @brief The switches are multiplexed with boot cfg. The pull value should not be changed!
 *
 */
void BOARD_InitSwitches(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitMicrophone(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitEthernet(void);

#define BOARD_INITFLASH_IOMUXC_GPR_GPR26_GPIO_MUX1_GPIO_SEL_MASK 0x20U /*!< GPIO1 and GPIO6 share same IO MUX function, GPIO_MUX1 selects one GPIO function: affected bits mask */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitFlash(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InittJtag(void);

/*!
 * @brief The I2C is shared between the amplifier, the A71CH and the general purpose serial port.
 *
 */
void BOARD_InitLPI2C(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLPSPI(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLPUART(void);

#define BOARD_INITBOOTCONFIG_IOMUXC_GPR_GPR26_GPIO_MUX1_GPIO_SEL_MASK 0x10U /*!< GPIO1 and GPIO6 share same IO MUX function, GPIO_MUX1 selects one GPIO function: affected bits mask */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitBootConfig(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitWifi(void);

#define BOARD_INITBLUETOOTH_IOMUXC_GPR_GPR27_GPIO_MUX2_GPIO_SEL_MASK 0xC0000000U /*!< GPIO2 and GPIO7 share same IO MUX function, GPIO_MUX2 selects one GPIO function: affected bits mask */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitBluetooth(void);

/* GPIO_AD_B1_05 (coord K12), RED_LED */
/* Routed pin properties */
#define BOARD_INITLED_RED_LED_PERIPHERAL                                 FLEXIO3   /*!< Peripheral name */
#define BOARD_INITLED_RED_LED_SIGNAL                                          IO   /*!< Signal name */
#define BOARD_INITLED_RED_LED_CHANNEL                                         5U   /*!< Signal channel */

/* GPIO_AD_B1_06 (coord J12), GREEN_LED */
/* Routed pin properties */
#define BOARD_INITLED_GREEN_LED_PERIPHERAL                               FLEXIO3   /*!< Peripheral name */
#define BOARD_INITLED_GREEN_LED_SIGNAL                                        IO   /*!< Signal name */
#define BOARD_INITLED_GREEN_LED_CHANNEL                                       6U   /*!< Signal channel */

/* GPIO_AD_B1_07 (coord K10), BLUE_LED */
/* Routed pin properties */
#define BOARD_INITLED_BLUE_LED_PERIPHERAL                                FLEXIO3   /*!< Peripheral name */
#define BOARD_INITLED_BLUE_LED_SIGNAL                                         IO   /*!< Signal name */
#define BOARD_INITLED_BLUE_LED_CHANNEL                                        7U   /*!< Signal channel */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLED(void);

#define BOARD_INITMQS_IOMUXC_GPR_GPR27_GPIO_MUX2_GPIO_SEL_MASK 0x04U /*!< GPIO2 and GPIO7 share same IO MUX function, GPIO_MUX2 selects one GPIO function: affected bits mask */

/* GPIO_B0_02 (coord E8), LCDIF_HSYNC */
/* Routed pin properties */
#define BOARD_INITMQS_LCDIF_HSYNC_PERIPHERAL                               GPIO2   /*!< Peripheral name */
#define BOARD_INITMQS_LCDIF_HSYNC_SIGNAL                                 gpio_io   /*!< Signal name */
#define BOARD_INITMQS_LCDIF_HSYNC_CHANNEL                                     2U   /*!< Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITMQS_LCDIF_HSYNC_GPIO                                     GPIO2   /*!< GPIO peripheral base pointer */
#define BOARD_INITMQS_LCDIF_HSYNC_GPIO_PIN                                    2U   /*!< GPIO pin number */
#define BOARD_INITMQS_LCDIF_HSYNC_GPIO_PIN_MASK                       (1U << 2U)   /*!< GPIO pin mask */
#define BOARD_INITMQS_LCDIF_HSYNC_PORT                                     GPIO2   /*!< PORT peripheral base pointer */
#define BOARD_INITMQS_LCDIF_HSYNC_PIN                                         2U   /*!< PORT pin number */
#define BOARD_INITMQS_LCDIF_HSYNC_PIN_MASK                            (1U << 2U)   /*!< PORT pin mask */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitMQS(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
