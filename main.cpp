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