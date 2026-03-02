/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v1.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#include "crc8_crc16.h"
#include "..\\..\\User\\Communication\\core\\uproto.h" 
#include "remote_control.h"
#include "bsp_usart.h"
extern RC_ctrl_t rc_ctrl;

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */
/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* TX 闂冪喎鍨径褍鐨敍灞藉讲閹稿娓剁憰浣界殶閺佽揪绱欏▔銊﹀�? RAM 閸楃姷鏁ら敍? */
#ifndef USB_TX_RING_SIZE
#define USB_TX_RING_SIZE 2048
#endif

/* �???婢堆傜濞嗏?冲絺闁胶娈戦崚鍡欏闂?鍨閿涘湣TU閿涘�?绱濋幐澶愭付鐠嬪啯鏆ｉ妴鍌涙暈閹?? USB FS/HS 闂勬劕鍩楅敍鍫滅伐婵?? 512 �??? 64�??? */
#ifndef USB_TX_CHUNK_MAX
#define USB_TX_CHUNK_MAX 512
#endif

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */

/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferHS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferHS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */

//usb閹恒儲鏁归弫鎵�?,閸欐�??浣烘畱閸忋劌�?潻娆庣�?
uint8_t usb_buf[USB_SIZE];
uint16_t usb_buf_len = 0; // 瑜版挸澧犲鍙夋暪閸掍即鏆辨�??
static uint8_t  printf_buf[2];   // 1 鐎涙濡弫鐗堝�? + 1 閸楃姳缍?
static volatile uint8_t tx_done = 1; // 閸欐�??浣哥暚閹存劖鐖ｈ�??

static uint8_t  usb_tx_ring[USB_TX_RING_SIZE];
static volatile uint16_t usb_tx_wpos = 0;   // 娑撳绔存稉顏勫晸閸忋儰缍呯�??
static volatile uint16_t usb_tx_count = 0;  // 闂冪喎鍨稉顓炲嚒閺堝鐡ч懞鍌涙�?

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceHS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

extern uproto_context_t proto_ctx;

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_HS(void);
static int8_t CDC_DeInit_HS(void);
static int8_t CDC_Control_HS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_HS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_HS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */


/* 閸愬懘鍎撮敍姘愁�?缁犳缍嬮崜宥堫嚢娴ｅ秶鐤嗛敍鍫㈩儑�???娑擃亜绶熼崣�???浣哥摟閼哄�?�娈戞担宥囩枂閿?? */
static inline uint16_t usb_tx_read_pos(void)
{
    // 鐠囩粯�?�氶�?? = 閸愭瑦�?�氶�?? - count (mod ring size)
    uint16_t r = (usb_tx_wpos + USB_TX_RING_SIZE - usb_tx_count) % USB_TX_RING_SIZE;
    return r;
}

/* 閸愬懘鍎撮敍姘�? USB 缁屾椽妫介弮鏈电矤闂冪喎鍨崣鎴??浣风瑓娑??濞堢绱欓棃鐐烘▎婵夌儑�?? */
static void start_usb_tx_if_idle(void)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceHS.pClassData;
    if (hcdc == NULL) return;

    /* 婵�?��?�鐏夋径鏍啎閺勫墽銇氳箛娆欑礉閻╁瓨甯存潻鏂挎�?閿涘牆鐨㈤悽鍗炵暚閹存劕娲栫拫鍐埛缂侇厼褰傞柅渚婄�? */
    if (hcdc->TxState != 0) {
        return;
    }

    /* 閸欐牕鍤憰浣稿絺闁胶娈戦梹鍨閿涘牆鏁栭柌蹇氱箾缂侇厾娈戞�??濞堢绱? */
    __disable_irq();
    uint16_t count = usb_tx_count;
    if (count == 0) {
        __enable_irq();
        return; // nothing to send
    }
    uint16_t read_pos = usb_tx_read_pos();
    uint16_t first_chunk = (uint16_t)((USB_TX_RING_SIZE - read_pos) < count ? (USB_TX_RING_SIZE - read_pos) : count);
    /* 闂勬劕鍩楅崡鏇燁偧閸欐垿?浣规付婢堆囨毐鎼达讣绱欓柆鍨帳鐡掑懓绻? USB MTU�??? */
    uint16_t send_len = (first_chunk > USB_TX_CHUNK_MAX) ? USB_TX_CHUNK_MAX : first_chunk;
    __enable_irq();

    /* 鐠佸墽鐤嗘惔鎾舵畱閸欐垿?浣虹处閸愭彃鑻熼幓鎰唉閿涘牓娼梼璇差敚閿?? */
    USBD_CDC_SetTxBuffer(&hUsbDeviceHS, &usb_tx_ring[read_pos], send_len);
    USBD_CDC_TransmitPacket(&hUsbDeviceHS);
}

/* proto_port_write閿涙艾鐨? data 閸忋儵妲﹂敍灞肩瑝闂冭�??
   鏉╂柨娲�??圭偤妾崗銉╂Е閻ㄥ嫬鐡ч懞鍌涙殶閿涘牆褰查懗钘夌毈�??? len閿涘苯缍嬮梼鐔峰灙缁屾椽妫挎稉宥堝喕閺冭绱氶妴?
*/
static size_t proto_port_write(void *user, const uint8_t *data, size_t len)
{
    (void)user;
    if (data == NULL || len == 0) return 0;

    __disable_irq();
    uint16_t free_space = (uint16_t)(USB_TX_RING_SIZE - usb_tx_count);
    if (free_space == 0) {
        __enable_irq();
        return 0; // 闂冪喎鍨⿰鈽呯礉娑撱垹�??
    }

    /* 鐏忎粙鍣洪崘娆忓弳閸忋劑鍎撮敍灞肩稻閼汇儳鈹栭梻缈犵瑝鐡掑啿褰ч崘娆忓弳閼宠棄顔愮痪宕囨畱 */
    uint16_t to_write = (len > free_space) ? free_space : (uint16_t)len;

    uint16_t wpos = usb_tx_wpos;
    uint16_t first = (USB_TX_RING_SIZE - wpos);
    if (first > to_write) first = to_write;

    memcpy(&usb_tx_ring[wpos], data, first);
    if (to_write > first) {
        memcpy(&usb_tx_ring[0], data + first, to_write - first);
        wpos = (uint16_t)(to_write - first);
    } else {
        wpos = (uint16_t)(wpos + first);
        if (wpos >= USB_TX_RING_SIZE) wpos -= USB_TX_RING_SIZE;
    }

    usb_tx_wpos = wpos;
    usb_tx_count = (uint16_t)(usb_tx_count + to_write);
    __enable_irq();

    /* 婵�?��?�鐏? USB 缁屾椽妫介敍�?冃曢崣鎴濆絺闁緤绱欐导姘躬鐎瑰本鍨氶崶鐐剁殶闁插瞼鎴风紒顓ㄧ�? */
    start_usb_tx_if_idle();

    return (size_t)to_write;
}

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_HS =
{
  CDC_Init_HS,
  CDC_DeInit_HS,
  CDC_Control_HS,
  CDC_Receive_HS,
  CDC_TransmitCplt_HS
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC media low layer over the USB HS IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_HS(void)
{
  /* USER CODE BEGIN 8 */
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceHS, UserTxBufferHS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceHS, UserRxBufferHS);
  return (USBD_OK);
  /* USER CODE END 8 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @param  None
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_HS(void)
{
  /* USER CODE BEGIN 9 */
  return (USBD_OK);
  /* USER CODE END 9 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_HS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 10 */
  switch(cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

  case CDC_SET_COMM_FEATURE:

    break;

  case CDC_GET_COMM_FEATURE:

    break;

  case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
  case CDC_SET_LINE_CODING:

    break;

  case CDC_GET_LINE_CODING:

    break;

  case CDC_SET_CONTROL_LINE_STATE:

    break;

  case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 10 */
}

/**
  * @brief Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAILL
  */
static int8_t CDC_Receive_HS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 11 */
  /* Directly feed raw USB bytes to uproto; do not pre-filter/clear Buf */
  if (Len && *Len) {
		if (*Len == RC_FRAME_LENGTH && Buf[0] == 0x0F && Buf[24] == 0x00) {
      // ����SBUS����
      sbus_to_rc_uper(Buf, &rc_ctrl, &remoter);
    }
    else {
      // ��SBUS���ݣ����ݸ�uprotoЭ��ջ����
      uproto_on_rx_bytes(&proto_ctx, Buf, *Len);
			rc_ctrl.rc.s[0] = 2;
    }

  }
  USBD_CDC_SetRxBuffer(&hUsbDeviceHS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceHS);
  return (USBD_OK);
  /* USER CODE END 11 */
}

/**
  * @brief  Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_HS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 12 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceHS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceHS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceHS);
  /* USER CODE END 12 */
  return result;
}

/**
  * @brief  CDC_TransmitCplt_HS
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_HS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 14 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  /* USER CODE END 14 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
int __io_putchar(int ch)
{
	
    if (hUsbDeviceHS.dev_state != USBD_STATE_CONFIGURED) return ch; // 閺堫亜姘ㄧ紒顏嗘纯閹恒儰�??
    printf_buf[0] = (uint8_t)ch;
    tx_done = 0;
    CDC_Transmit_HS(&printf_buf[0], 1); // 閹绘劒姘﹂崡鏇炵摟閼??
    return ch;
}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
