/*
 *
 *    Copyright (c) 2019 Nest Labs, Inc.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *          Utilities for accessing persisted device configuration on
 *          platforms based on the Silicon Labs SDK.
 */

#ifndef EFR32_CONFIG_H
#define EFR32_CONFIG_H

#include <functional>

#include <Weave/DeviceLayer/internal/WeaveDeviceLayerInternal.h>

#include "nvm3.h"
#include "nvm3_hal_flash.h"

namespace nl {
namespace Weave {
namespace DeviceLayer {
namespace Internal {

/**
 *
 * This implementation uses the Silicon Labs EFR32 NVM3 flash data storage library
 * as the underlying storage layer.
 *
 * NOTE: This class is designed to be mixed-in to the concrete subclass of the
 * GenericConfigurationManagerImpl<> template.  When used this way, the class
 * naturally provides implementations for the delegated members referenced by
 * the template class (e.g. the ReadConfigValue() method).
 */

// Silabs NVM3 objects use a 20-bit number, however User key range is
// restricted to 16 bits i.e. 0x0000 -> 0xFFFF.
// e.g. key = 0xA201
// 'A2' = the nv group base offest (Factory, Config or Counter)
// '01' = the id offset inside the group.
constexpr inline uint32_t EFR32ConfigKey(uint8_t keyBaseOffset, uint8_t id)
{
    return static_cast<uint32_t>(keyBaseOffset) << 8 | id;
}

class EFR32Config
{
public:
public:
    // Definitions for Silicon Labs EFR32 NVM3 driver:-

    using Key = uint32_t;

    // NVM3 key base offsets used by the OpenWeave Device Layer.
    static constexpr uint8_t kWeaveFactory_KeyBase =
        0xA2; // Persistent config values set at manufacturing time. Retained during factory reset.
    static constexpr uint8_t kWeaveConfig_KeyBase =
        0xA3; // Persistent config values set at runtime. Cleared during factory reset.
    static constexpr uint8_t kWeaveCounter_KeyBase =
        0xA4; // Persistent counter values set at runtime. Retained during factory reset.

    // Key definitions for well-known configuration values.
    // Factory config keys
    static constexpr Key kConfigKey_SerialNum            = EFR32ConfigKey(kWeaveFactory_KeyBase, 0x00);
    static constexpr Key kConfigKey_MfrDeviceId          = EFR32ConfigKey(kWeaveFactory_KeyBase, 0x01);
    static constexpr Key kConfigKey_MfrDeviceCert        = EFR32ConfigKey(kWeaveFactory_KeyBase, 0x02);
    static constexpr Key kConfigKey_MfrDevicePrivateKey  = EFR32ConfigKey(kWeaveFactory_KeyBase, 0x03);
    static constexpr Key kConfigKey_ManufacturingDate    = EFR32ConfigKey(kWeaveFactory_KeyBase, 0x04);
    static constexpr Key kConfigKey_PairingCode          = EFR32ConfigKey(kWeaveFactory_KeyBase, 0x05);
    static constexpr Key kConfigKey_MfrDeviceICACerts    = EFR32ConfigKey(kWeaveFactory_KeyBase, 0x06);
    // Weave Config Keys
    static constexpr Key kConfigKey_FabricId             = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x00);
    static constexpr Key kConfigKey_ServiceConfig        = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x01);
    static constexpr Key kConfigKey_PairedAccountId      = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x02);
    static constexpr Key kConfigKey_ServiceId            = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x03);
    static constexpr Key kConfigKey_FabricSecret         = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x04);
    static constexpr Key kConfigKey_LastUsedEpochKeyId   = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x05);
    static constexpr Key kConfigKey_FailSafeArmed        = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x06);
    static constexpr Key kConfigKey_GroupKey             = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x07);
    static constexpr Key kConfigKey_ProductRevision             = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x08);
    static constexpr Key kConfigKey_OperationalDeviceId         = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x09);
    static constexpr Key kConfigKey_OperationalDeviceCert       = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x0A);
    static constexpr Key kConfigKey_OperationalDeviceICACerts   = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x0B);
    static constexpr Key kConfigKey_OperationalDevicePrivateKey = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x0C);

    static constexpr Key kConfigKey_GroupKeyBase = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x0D);
    static constexpr Key kConfigKey_GroupKeyMax =
        EFR32ConfigKey(kWeaveConfig_KeyBase, 0x1C); // Allows 16 Group Keys to be created.

    // Set key id limits for each group.
    static constexpr Key kMinConfigKey_WeaveFactory = EFR32ConfigKey(kWeaveFactory_KeyBase, 0x00);
    static constexpr Key kMaxConfigKey_WeaveFactory = EFR32ConfigKey(kWeaveFactory_KeyBase, 0x06);
    static constexpr Key kMinConfigKey_WeaveConfig  = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x00);
    static constexpr Key kMaxConfigKey_WeaveConfig  = EFR32ConfigKey(kWeaveConfig_KeyBase, 0x1C);
    static constexpr Key kMinConfigKey_WeaveCounter = EFR32ConfigKey(kWeaveCounter_KeyBase, 0x00);
    static constexpr Key kMaxConfigKey_WeaveCounter =
        EFR32ConfigKey(kWeaveCounter_KeyBase, 0x1F); // Allows 32 Counters to be created.

    static WEAVE_ERROR Init(void);

    // Configuration methods used by the GenericConfigurationManagerImpl<> template.
    static WEAVE_ERROR ReadConfigValue(Key key, bool &val);
    static WEAVE_ERROR ReadConfigValue(Key key, uint32_t &val);
    static WEAVE_ERROR ReadConfigValue(Key key, uint64_t &val);
    static WEAVE_ERROR ReadConfigValueStr(Key key, char *buf, size_t bufSize, size_t &outLen);
    static WEAVE_ERROR ReadConfigValueBin(Key key, uint8_t *buf, size_t bufSize, size_t &outLen);
    static WEAVE_ERROR ReadConfigValueCounter(uint8_t counterIdx, uint32_t &val);
    static WEAVE_ERROR WriteConfigValue(Key key, bool val);
    static WEAVE_ERROR WriteConfigValue(Key key, uint32_t val);
    static WEAVE_ERROR WriteConfigValue(Key key, uint64_t val);
    static WEAVE_ERROR WriteConfigValueStr(Key key, const char *str);
    static WEAVE_ERROR WriteConfigValueStr(Key key, const char *str, size_t strLen);
    static WEAVE_ERROR WriteConfigValueBin(Key key, const uint8_t *data, size_t dataLen);
    static WEAVE_ERROR WriteConfigValueCounter(uint8_t counterIdx, uint32_t val);
    static WEAVE_ERROR ClearConfigValue(Key key);
    static bool        ConfigValueExists(Key key);
    static WEAVE_ERROR FactoryResetConfig(void);
    static bool        ValidConfigKey(Key key);

    static void RunConfigUnitTest(void);
    static void RepackNvm3Flash(void);

protected:
    using ForEachRecordFunct = std::function<WEAVE_ERROR(const Key &nvm3Key, const size_t &length)>;
    static WEAVE_ERROR ForEachRecord(Key firstKey, Key lastKey, bool addNewRecord, ForEachRecordFunct funct);

private:
    static WEAVE_ERROR MapNvm3Error(Ecode_t nvm3Res);
};

} // namespace Internal
} // namespace DeviceLayer
} // namespace Weave
} // namespace nl

#endif // EFR32_CONFIG_H
