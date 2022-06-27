//
// Created by lukkens on 2022-06-10.
//

#ifndef PNHELPERS_PNHELPERS_H
#define PNHELPERS_PNHELPERS_H

#include <PN532/PN532.h>

#include <string>
#include <sstream>

void HandleDriverResult(hwlib::terminal &console, PN532::DriverResult error)
{
    if (error == PN532::RESULT_Success)
        return;

    console << "\f" << "Error: " << error << hwlib::flush;
    exit(error);
}

PN532::DriverResult ScanMifareCardUUID(PN532::DynamicNFCID &result, PN532::DriverBase &driver)
{
    PN532::InListPassiveTargetResponse targetResponse;
    PN532::DriverResult status = driver.InListPassiveTarget(targetResponse, 1, PN532::CARD_BITRATE_106kbps);
    if (status)
        return status;

    auto *typeAInfo = (PN532::InListPassiveTargetResponse::TargetCardTypeAInfo *)targetResponse.Targets[0].get();
    result = typeAInfo->NfcID;

    status = driver.InRelease(0);
    return status;
}

PN532::DriverResult EmulateCard(PN532::InitAsTargetResponse &result, PN532::DriverBase &driver, const PN532::DynamicNFCID &nfcid)
{
    PN532::MiFareTargetParams miFareTargetParams(0x0400,
            { nfcid.IDBytes[1], nfcid.IDBytes[2], nfcid.IDBytes[3] },
            PN532::SEL_RES_OPTIONS_PICC_EMULATION);

    PN532::FeliCaTargetParams feliCaTargetParams(
            {nfcid.IDBytes[0], nfcid.IDBytes[1], nfcid.IDBytes[2], nfcid.IDBytes[3],
             0x00, 0x00, 0x00, 0x00},
            0x0000);

    return driver.InitAsTarget(result,
            static_cast<PN532::TargetMode>(PN532::TARGET_MODE_PASSIVE_ONLY | PN532::TARGET_MODE_PICC_ONLY),
            miFareTargetParams,
            feliCaTargetParams,
            nfcid);
}

std::string NFCIDToString(const PN532::DynamicNFCID &nfcid)
{
    std::stringstream result;
    size_t idBytesCount = nfcid.IDBytes.size();

    result << std::uppercase;
    for (size_t i = 0; i < idBytesCount; i++)
    {
        result << std::hex << +nfcid.IDBytes[i]; // The implicit '+' seems to be required for 'std::hex' to actually print out an uint8_t as hex.
        if (i < idBytesCount - 1)
            result << ":";
    }

    return result.str();
}

#endif //PNHELPERS_PNHELPERS_H
