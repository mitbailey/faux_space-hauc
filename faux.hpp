/**
 * @file faux.hpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief 
 * @version See Git tags for version information.
 * @date 2021.08.12
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdint.h>

/**
 * @brief Numeric identifiers for determining what module a command is for.
 * 
 */
enum MODULE_ID
{
    INVALID_ID = 0x0,
    ACS_ID = 0x1,
    EPS_ID = 0x2,
    XBAND_ID = 0x3,
    SW_UPD_ID = 0xf,
    ACS_UPD_ID = 0xe,
    SYS_VER_MAGIC = 0xd,
    SYS_RESTART_PROG = 0xff,
    SYS_REBOOT = 0xfe,
    SYS_CLEAN_SHBYTES = 0xfd
};

/**
 * @brief The ACS update data format sent from SPACE-HAUC to Ground.
 * 
 *  else if (module_id == ACS_UPD_ID)
 *  {
 *      acs_upd.vbatt = eps_vbatt;
 *      acs_upd.vboost = eps_mvboost;
 *      acs_upd.cursun = eps_cursun;
 *      acs_upd.cursys = eps_cursys;
 *      output->retval = 1;
 *      output->data_size = sizeof(acs_uhf_packet);
 *      memcpy(output->data, &acs_upd, output->data_size);
 *  }
 * 
 * @return typedef struct 
 */
#ifndef __fp16
#define __fp16 uint16_t
#endif // __fp16
typedef struct __attribute__((packed))
{
    uint8_t ct;      // Set in acs.c.
    uint8_t mode;    // Set in acs.c.
    __fp16 bx;       // Set in acs.c.
    __fp16 by;       // Set in acs.c.
    __fp16 bz;       // Set in acs.c.
    __fp16 wx;       // Set in acs.c.
    __fp16 wy;       // Set in acs.c.
    __fp16 wz;       // Set in acs.c.
    __fp16 sx;       // Set in acs.c.
    __fp16 sy;       // Set in acs.c.
    __fp16 sz;       // Set in acs.c.
    uint16_t vbatt;  // Set in cmd_parser.
    uint16_t vboost; // Set in cmd_parser.
    uint16_t cursun; // Set in cmd_parser.
    uint16_t cursys; // Set in cmd_parser.
} acs_upd_output_t;

/**
 * @brief Command structure that SPACE-HAUC receives.
 * 
 */
typedef struct __attribute__((packed))
{
    uint8_t mod;
    uint8_t cmd;
    int unused;
    int data_size;
    unsigned char data[46];
} cmd_input_t;

/**
 * @brief Command structure that SPACE-HAUC transmits to Ground.
 * 
 */
typedef struct __attribute__((packed))
{
    uint8_t mod;            // 1
    uint8_t cmd;            // 1
    int retval;             // 4
    int data_size;          // 4
    unsigned char data[46]; // 46
} cmd_output_t;

/// From SPACE-HAUC/uhf_gst ///
#define GST_MAX_PAYLOAD_SIZE 56
#define GST_MAX_PACKET_SIZE 64
#define GST_GUID 0x6f35
#define GST_TERMINATION 0x0d0a // CRLF
typedef struct __attribute__((packed))
{
    uint16_t guid;
    uint16_t crc;
    uint8_t payload[GST_MAX_PAYLOAD_SIZE];
    uint16_t crc1;
    uint16_t termination;
} gst_frame_t;
#define GST_MAX_FRAME_SIZE sizeof(gst_frame_t)

enum GST_ERRORS
{
    GST_ERROR = -1,            //!< General error
    GST_TOUT = 0,              //!< Operation timed out
    GST_SUCCESS = 1,           //!<
    GST_PACKET_INCOMPLETE = 2, //!< Incomplete data received
    GST_GUID_ERROR = 3,        //!< GUID mismatch
    GST_CRC_MISMATCH = 4,      //!< CRC mismatch
    GST_CRC_ERROR = 5,         //!< Wrong CRC
};
///////////////////////////////

/*
 * this is the CCITT CRC 16 polynomial X^16  + X^12  + X^5  + 1.
 * This works out to be 0x1021, but the way the algorithm works
 * lets us use 0x8408 (the reverse of the bit pattern).  The high
 * bit is always assumed to be set, thus we only use 16 bits to
 * represent the 17 bit value.
 */
static inline uint16_t crc16(unsigned char *data_p, uint16_t length)
{
#define CRC16_POLY 0x8408
    unsigned char i;
    unsigned int data;
    unsigned int crc = 0xffff;

    if (length == 0)
        return (~crc);

    do
    {
        for (i = 0, data = (unsigned int)0xff & *data_p++;
             i < 8;
             i++, data >>= 1)
        {
            if ((crc & 0x0001) ^ (data & 0x0001))
                crc = (crc >> 1) ^ CRC16_POLY;
            else
                crc >>= 1;
        }
    } while (--length);

    crc = ~crc;
    data = crc;
    crc = (crc << 8) | (data >> 8 & 0xff);

    return (crc);
}