/**
 * @file main.cpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief 
 * @version See Git tags for version information.
 * @date 2021.08.12
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <si446x.h>
#include "faux.hpp"
#include "meb_debug.hpp"

int main ()
{
    si446x_init();

    gst_frame_t inbound[1];
    // cmd_input_t *input;
    cmd_output_t output[1];
    gst_frame_t outbound[1];
    acs_upd_output_t data[1];
    int16_t RSSI = -1;
    int retval = -1;

    // This is REQUIRED because si446x radio must SEND prior to receiving. Who knows why.
    si446x_en_pipe();

    while(1)
    {
        memset(inbound, 0x0, sizeof(gst_frame_t));
        // memset(input, 0x0, sizeof(cmd_input_t));
        memset(output, 0x0, sizeof(cmd_output_t));
        memset(data, 0x0, sizeof(acs_upd_output_t));
        memset(outbound, 0x0, sizeof(gst_frame_t));

        dbprintlf(GREEN_FG "Reading...");
        retval = si446x_read(inbound, sizeof(gst_frame_t), &RSSI);

        if (retval != sizeof(gst_frame_t))
        {
            dbprintlf(RED_FG "Invalid data read: %d.", retval);
            continue;
        }
        
        dbprintlf(BLUE_FG "Read %d bytes.", retval);

        cmd_input_t *input = (cmd_input_t *)inbound->payload;

        if (input->mod != ACS_UPD_ID)
        {
            dbprintlf(RED_FG "Packet was not ACS update: 0x%02x.", input->mod);
            continue;
        }

        dbprintlf(BLUE_FG "Successfully received ACS update packet, sending randomized ACS data...");

        data->ct = rand() % 0xff;
        data->mode = rand() % 0xff;
        data->bx = rand() % 0xffff;
        data->by = rand() % 0xffff;
        data->bz = rand() % 0xffff;
        data->wx = rand() % 0xffff;
        data->wy = rand() % 0xffff;
        data->wz = rand() % 0xffff;
        data->sx = rand() % 0xffff;
        data->sy = rand() % 0xffff;
        data->sz = rand() % 0xffff;
        data->vbatt = rand() % 0xffff;
        data->vboost = rand() % 0xffff;
        data->cursun = rand() % 0xffff;
        data->cursys = rand() % 0xffff;

        output->mod = ACS_UPD_ID;
        output->data_size = sizeof(cmd_output_t);
        memcpy(output->data, data, output->data_size);
        memcpy(outbound, output, sizeof(cmd_output_t));
        outbound->guid = GST_GUID;
        outbound->crc = crc16(outbound->payload, GST_MAX_PAYLOAD_SIZE);
        outbound->crc1 = outbound->crc;
        outbound->termination = GST_TERMINATION;
        si446x_write(output, sizeof(cmd_output_t));
    }
    
    return 0;
}

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