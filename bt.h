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
#ifndef BT_H
#define BT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../types.h"

    void btInit();

    void btTask();

    void btSetDiscoverable(uint8_t hostIndex);
    void btConnect(uint8_t hostIndex, uint16_t timeout);
    void btDisconnect();
    void btSwitchHost(uint8_t hostIndex);
    void btReadReg(uint8_t reg, uint8_t len);

    void btGetInfo();
    void btSetConfig(module_param_t* param);
    void btGetConfig();
    void btSetName(const char* name);
    void btGetName();
    void btModuleFactoryReset();

    void btSendKeys(report_keyboard_t* report);
    void btSendKeys_nkro(report_keyboard_t* report);
    void btSendConsumer(uint16_t report);
    void btSendSystem(uint16_t report);
    void btSendMouse(uint8_t* report);

    uint16_t btGetBatteryVoltage();
    BluetoothDeviceStatus btGetStatus();
    uint8_t btGetHostIndex();
    KBLeds btGetLeds();

#ifdef __cplusplus
}
#endif

#endif /* BT_H */
