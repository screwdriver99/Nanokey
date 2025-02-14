/* Copyright 2021 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "bt.h"

#include <math.h>
#include <string.h>

#include "../hal.h"
#include "main.h"

#define PAYLOAD_BUF_SIZE 64

enum BTPairMode
{
    BTPM_Default = 0x00,
    BTPM_JustWorks,  // Pair with no security checks
    BTPM_Passkey,    // Require a code entry
    BTPM_LESC_SSP,   // LESC = Low Energy Secure Connection, used for BLE.
                     // SSP = Secure Simple Pairing, used for classic BT.
                     // Both used to just confirm "yes - no" if a displayed code is correct
    BTPM_Invalid
};

enum BTMode
{
    BTM_Default,
    BTM_Classic,
    BTM_BLE,  // Note: CKBT51 doesn't support BLE
    BTM_Invalid,
};

enum BTCommand  // to BT device
{
    /* HID Report  */
    BTC_SendKeyboard       = 0x11,
    BTC_SendKeyboard_NKRO  = 0x12,
    BTC_SendConsumerReport = 0x13,
    BTC_SendSystemReport   = 0x14,
    /* Bluetooth connections */
    BTC_SetDiscoverable    = 0x21,
    BTC_Connect            = 0x22,
    BTC_Disconnect         = 0x23,
    BTC_SwitchHost         = 0x24,
    BTC_ReadReg            = 0x25,
    /* Set/get parameters */
    BTC_GetDeviceInfo      = 0x40,
    BTC_SetConfig          = 0x41,
    BTC_GetConfig          = 0x42,
    BTC_SetName            = 0x45,
    BTC_GetName            = 0x46,
    /* MISC */
    BTC_FactoryReset       = 0x71,
    /* Event */
    BTC_ConnEventAck       = 0xA4,
};

enum BTAck
{
    BTA_Success = 0x00,
    BTA_ChecksumError,
    BTA_FifoHalfWarn,
    BTA_FifoFullError,
};

enum BTDeviceEvent  // from BT device
{
    BTDE_Ack           = 0xA1,
    BTDE_QueryResponse = 0xA2,
    BTDE_OtaResponse   = 0xA3,  // not used
    BTDE_Reset         = 0xB0,
    BTDE_Connection_LE = 0xB1,
    BTDE_HostType      = 0xB2,
    BTDE_Connection    = 0xB3,
    BTDE_HIDEvent      = 0xB4,
    BTDE_Battery       = 0xB5,
};

enum BTConnEvent
{
    BTCE_Connection            = 0x20,
    BTCE_Discoverable          = 0x21,
    BTCE_Reconnecting          = 0x22,
    BTCE_Disconnection         = 0x23,
    BTCE_EnterPincodeInsertion = 0x24,
    BTCE_LeavePincodeInsertion = 0x25,
    BTCE_Reset,
};

enum BTConnStatus
{
    BTCS_Reset,
    BTCS_Disconnected,
    BTCS_Connected,
    BTCS_PincodeInsertion,
};

uint16_t voltage;
uint8_t readingReg           = 0xFF;
BluetoothDeviceStatus status = BDS_Disconnected;
uint8_t hostIdx              = 0;
KBLeds kbLeds                = {0};

void updateBatteryVoltage(uint16_t value) { voltage = floor(((float)value) * 2.2900f); }

void send(uint8_t* payload, uint8_t size, bool ack, bool retry)
{
    static uint8_t txsn = 0;
    uint8_t i;
    uint8_t pkt[64];
    memset(pkt, 0, 64);

    if (!retry) ++txsn;
    if (txsn == 0) ++txsn;

    writePin(WUP_BT, false);  // wake up BT module
    delayms(1);
    writePin(WUP_BT, true);
    delayms(1);
    writePin(WUP_BT, false);
    delayms(1);
    writePin(WUP_BT, true);
    delayms(1);

    uint16_t checksum = 0;
    for (i = 0; i < size; i++) checksum += payload[i];

    i        = 0;
    pkt[i++] = 0xAA;
    pkt[i++] = ack ? 0x56 : 0x55;
    pkt[i++] = size + 2;
    pkt[i++] = ~(size + 2) & 0xFF;
    pkt[i++] = txsn;
    memcpy(pkt + i, payload, size);
    i += size;
    pkt[i++] = checksum & 0xFF;
    pkt[i++] = (checksum >> 8) & 0xFF;

    uartSendBlock(pkt, i);
}

void requestBatteryVoltage() { btReadReg(0x05, 0x02); }

// Send ack to connection event, bluetooth module will retry 2 times if no ack received
void sendConnectionChangeAck(void)
{
    uint8_t payload = BTC_ConnEventAck;
    send(&payload, 1, false, false);
}

//========================================HANDLERS============================================

void queryResponseHandler(uint8_t* data, uint8_t len)
{
    (void)len;
    if (data[2]) return;

    switch (data[1])
    {
        case BTC_ReadReg:
            switch (readingReg)
            {
                case 0x05:
                    // battery data
                    updateBatteryVoltage(data[3] | (data[4] << 8));
                    break;
            }
            readingReg = 0xFF;
            break;
        default:
            break;
    }
}

void ackHandler(uint8_t* data, uint8_t len)
{
    (void)len;

    switch (data[1])
    {
        case BTC_SendKeyboard:
        case BTC_SendKeyboard_NKRO:
        case BTC_SendConsumerReport:
        case BTC_SendSystemReport:
            // data[2] may be:
            // ACK_SUCCESS
            // ACK_FIFO_HALF_WARNING
            // ACK_FIFO_FULL_ERROR
            break;
    }

    if (data[2] == BTA_FifoHalfWarn || data[2] == BTA_FifoFullError)
    {
        __NOP();  // place a breakpoint here to monitor errors
    }
}

void mainEventHandler(uint8_t event, uint8_t* buf, uint8_t size, uint8_t actionCounter)
{
    (void)actionCounter;

    switch (event)
    {
        case BTDE_Ack:  // BT device acked a message
            ackHandler(buf, size);
            break;

        case BTDE_Reset:  // BT device reset
            // "reason" -> buf[0];
            break;

        case BTDE_Connection_LE:  // BLE not supported
            break;

        case BTDE_HostType:
            break;

        case BTDE_Connection:  // BT device connection status change
            sendConnectionChangeAck();

            switch (buf[0])
            {
                case BTCE_Connection:
                    status = BDS_Connected;
                    break;
                case BTCE_Discoverable:
                    status = BDS_Pairing;
                    break;
                case BTCE_Reconnecting:
                    status = BDS_Pairing;
                    break;
                case BTCE_Disconnection:
                    status = BDS_Disconnected;
                    break;
                case BTCE_EnterPincodeInsertion:
                    // not managed
                    break;
                case BTCE_LeavePincodeInsertion:
                    // not managed
                    break;
            }
            hostIdx = buf[2];
            break;

        case BTDE_HIDEvent:  // HID event indicator change
            kbLeds.capslock = buf[0];
            break;

        case BTDE_QueryResponse:  // response to a previous query received
            queryResponseHandler(buf, size);
            break;

        case BTDE_OtaResponse:  // response to an OTA update request (not implemented)
            break;

        case BTDE_Battery:  // battery data incoming
            if (buf[0] == 0x01) updateBatteryVoltage(buf[1] | (buf[2] << 8));
            break;

        default:
            break;
    }
}

//=====================================HANDLERS - END===========================================

void checksumCheck(uint8_t* buf, uint8_t size, uint8_t actionCounter)
{
    uint16_t checksum = 0;
    for (uint8_t i = 0; i < size - 2; i++) checksum += buf[i];

    if ((uint8_t)(checksum & 0x00ff) != buf[size - 2] || (uint8_t)((checksum >> 8) & 0x00ff) != buf[size - 1])
        return;  // discard frame

    // wipe last two bytes
    buf[size - 1] = 0x00;
    buf[size - 2] = 0x00;

    mainEventHandler(buf[0], buf + 1, size - 3, actionCounter);
}

// This function receives all data from the serial peripheral
void receive()
{
    static uint8_t payload[PAYLOAD_BUF_SIZE];
    static uint8_t parsingIdx    = 0;
    static uint8_t payloadSize   = 0;
    static uint8_t actionCounter = 0;
    uint8_t ch                   = 0;

    while (uartRecvChar(&ch))
    {
        if (parsingIdx == 0 && ch == 0xaa)  // parsing start of frame
            parsingIdx++;
        else if (parsingIdx == 1 && ch == 0x57)  // parsing sender
            parsingIdx++;
        else if (parsingIdx == 2)  // parsing len
        {
            payloadSize = ch;
            parsingIdx++;
        }
        else if (parsingIdx == 3)  // parsing ~len
        {
            if (ch == (uint8_t)~payloadSize)  // check if len is correct
                parsingIdx++;
            else
                parsingIdx = 0;
        }
        else if (parsingIdx == 4)  // parsing sn
        {
            actionCounter = ch;
            parsingIdx++;
        }
        else if (parsingIdx > 4 && parsingIdx < (payloadSize + 5))  // capturing payload
        {
            payload[parsingIdx - 5] = ch;
            parsingIdx++;

            if (parsingIdx == payloadSize + 5)  // end of packet
            {
                checksumCheck(payload, payloadSize, actionCounter);
                parsingIdx    = 0;
                payloadSize   = 0;
                actionCounter = 0;
            }
        }
        else
        {
            parsingIdx    = 0;
            payloadSize   = 0;
            actionCounter = 0;
        }
    }
}

void btInit()
{
    voltage = 0;

    writePin(BTRESET, false);  // reset the BT module
    delayms(50);
    writePin(BTRESET, true);

    writePin(WUP_BT, true);

    delayms(100);

    module_param_t param = {.event_mode             = 0x02,
                            .connected_idle_timeout = 7200,
                            .pairing_timeout        = 180,
                            .pairing_mode           = 1,
                            .reconnect_timeout      = 5,
                            .report_rate            = 90,
                            .vendor_id_source       = 1,
                            .verndor_id             = 0x3434,
                            .product_id             = 0x1212};

    btSetConfig(&param);
}

void btTask()
{
    static uint16_t taskCounter = 0;

    receive();

    if (taskCounter == 500)
    {
        // approx once every roughly 13 seconds with 1ms/11ms main event loop
        taskCounter = 0;
        requestBatteryVoltage();
    }

    taskCounter++;
}

void btSetDiscoverable(uint8_t hostIndex)
{
    uint8_t payload[16];
    uint8_t i        = 0;
    uint16_t timeout = 180;

    payload[i++] = BTC_SetDiscoverable;
    payload[i++] = hostIndex;              // host index to overwrite in case of connection
    payload[i++] = timeout & 0xff;         // timeout value (LSB)
    payload[i++] = (timeout >> 8) & 0xff;  // timeout value (MSB)
    payload[i++] = BTPM_JustWorks;         // simplest pairing mode
    payload[i++] = BTM_Classic;            // classic mode (non BLE)
    payload[i++] = 0;                      // not used

    send(payload, i, true, false);
}

void btConnect(uint8_t hostIndex, uint16_t timeout)
{
    uint8_t payload[16];
    uint8_t i = 0;

    payload[i++] = BTC_Connect;
    payload[i++] = hostIndex;       // Host index
    payload[i++] = timeout & 0xff;  // Timeout
    payload[i++] = (timeout >> 8) & 0xff;

    send(payload, i, true, false);
}

void btDisconnect()
{
    uint8_t payload[16];
    uint8_t i = 0;

    payload[i++] = BTC_Disconnect;
    payload[i++] = 0;  // Sleep mode

    send(payload, i, true, false);
}

void btSwitchHost(uint8_t hostIndex)
{
    uint8_t payload[16];
    uint8_t i = 0;

    payload[i++] = BTC_SwitchHost;
    payload[i++] = hostIndex;

    send(payload, i, true, false);
}

void btReadReg(uint8_t reg, uint8_t len)
{
    uint8_t payload[16];
    uint8_t i = 0;

    readingReg = reg;

    payload[i++] = BTC_ReadReg;
    payload[i++] = reg;
    payload[i++] = len;

    send(payload, i, false, false);
}

void btGetInfo()
{
    uint8_t payload[16];
    uint8_t i = 0;

    payload[i++] = BTC_GetDeviceInfo;
    send(payload, i, false, false);
}

void btSetConfig(module_param_t* param)
{
    uint8_t payload[32];
    uint8_t i = 0;

    payload[i++] = BTC_SetConfig;
    memcpy(payload + i, param, sizeof(module_param_t));
    i += sizeof(module_param_t);

    send(payload, i, true, false);
}

void btGetConfig()
{
    uint8_t payload[16];
    uint8_t i = 0;

    payload[i++] = BTC_GetConfig;

    send(payload, i, false, false);
}

void btSetName(const char* name)
{
    uint8_t payload[16];
    uint8_t i   = 0;
    uint8_t len = strlen(name);

    len = (len > 15) ? 15 : len;

    payload[i++] = BTC_SetName;
    memcpy(payload + i, name, len);
    i += len;

    send(payload, i, true, false);
}

void btGetName(void)
{
    uint8_t payload[16];
    uint8_t i = 0;

    payload[i++] = BTC_GetName;

    send(payload, i, false, false);
}

void btModuleFactoryReset(void)
{
    uint8_t payload[16];
    uint8_t i = 0;

    payload[i++] = BTC_FactoryReset;

    send(payload, i, false, false);
}

void btSendKeys(report_keyboard_t* report)
{
    uint8_t payload[16];
    uint8_t i = 0;

    payload[i++] = BTC_SendKeyboard;
    memcpy(payload + i, report, 8);
    i += 8;

    send(payload, i, true, false);
}

void btSendKeys_nkro(report_keyboard_t* report)
{
    uint8_t payload[32];
    uint8_t i = 0;

    payload[i++] = BTC_SendKeyboard_NKRO;
    memcpy(payload + i, report, 16);
    i += 16;

    send(payload, i, true, false);
}

void btSendConsumer(uint16_t report)
{
    uint8_t payload[8];
    uint8_t i = 0;

    payload[i++] = BTC_SendConsumerReport;
    payload[i++] = report & 0xFF;
    payload[i++] = ((report) >> 8) & 0xFF;

    payload[i++] = 0;  // skip other 2 reports
    payload[i++] = 0;
    payload[i++] = 0;
    payload[i++] = 0;

    send(payload, i, true, false);
}

void btSendSystem(uint16_t report)
{
    /* CKBT51 supports only System Sleep */
    if ((report & 0xFF) != 0x82) return;

    uint8_t payload[8];
    uint8_t i = 0;

    payload[i++] = BTC_SendSystemReport;
    payload[i++] = 0x01 << ((report & 0xFF) - 0x82);

    send(payload, i, true, false);
}

uint16_t btGetBatteryVoltage() { return voltage; }

BluetoothDeviceStatus btGetStatus() { return status; }

uint8_t btGetHostIndex() { return hostIdx; }

KBLeds btGetLeds() { return kbLeds; }
