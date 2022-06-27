#include <PN532/PN532-I2C.h>
#include <PNHelpers/PNHelpers.h>

#include "DebouncedPin.h"

static const constexpr size_t BUTTON_DEBOUNCE_TIME_US = 50 * 1000;
static const constexpr size_t MAX_PAGE = 0xFF;

struct PageInfo
{
    PN532::DynamicNFCID NfcID;
    bool HasID = false;
};

void PrintDynamicNFCID(const PN532::DynamicNFCID &dynamicNfcid, hwlib::terminal &console)
{
    console << hwlib::hex;

    auto nfcidSize = PN532::CascadeLevelToSize(dynamicNfcid.GetCascadeLevel());
    for (size_t i = 0; i < nfcidSize; i++)
    {
        console << dynamicNfcid.IDBytes[i];
        if (i < nfcidSize - 1)
            console << ":";
    }

    console << hwlib::dec
            << hwlib::flush;
}

static uint8_t CurrentPage = 0;
PageInfo Pages[MAX_PAGE];

void PageUp()
{
    if (CurrentPage < MAX_PAGE)
        CurrentPage++;
}

void PageDown()
{
    if (CurrentPage > 0)
        CurrentPage--;
}

PageInfo &GetCurrentPage()
{
    return Pages[CurrentPage];
}

__attribute__((noreturn))
int main()
{
    auto upButtonPin = hwlib::target::pin_in(hwlib::target::PIN_GP8);
    auto selectButtonPin = hwlib::target::pin_in(hwlib::target::PIN_GP7);
    auto downButtonPin = hwlib::target::pin_in(hwlib::target::PIN_GP6);

    upButtonPin.pullup_enable();
    selectButtonPin.pullup_enable();
    downButtonPin.pullup_enable();

    auto debouncedUpPin = DebouncedPin(upButtonPin, BUTTON_DEBOUNCE_TIME_US, false, true);
    auto debouncedSelectPin = DebouncedPin(selectButtonPin, BUTTON_DEBOUNCE_TIME_US, false, true);
    auto debouncedDownPin = DebouncedPin(downButtonPin, BUTTON_DEBOUNCE_TIME_US, false, true);

    auto i2cBus = hwlib::target::hardware_i2c(i2c0, hwlib::target::PIN_GP21, hwlib::target::PIN_GP20);
    PN532::I2C::Driver pnDriver(i2cBus);

    hwlib::glcd_oled_i2c_128x64_buffered oled(i2cBus);
    oled.flush();

    auto window = hwlib::part(oled, hwlib::xy(0, 0), hwlib::xy(128, 64));
    hwlib::font_default_8x8 font;
    auto oledConsole = hwlib::terminal_from(window, font);

    PN532::SAMConfiguration samConfiguration {
            .Mode = PN532::SAM_MODE_Normal,
            .Timeout = 20,
            .EnableIRQ = false
    };

    PN532::DriverResult result = pnDriver.SetSAMConfiguration(samConfiguration);
    HandleDriverResult(oledConsole, result);

    result = pnDriver.SetParameters(static_cast<PN532::SetParametersFlags>(PN532::PARAMETERS_AutomaticATR_RES | PN532::PARAMETERS_ISO14443_4_PICC));
    HandleDriverResult(oledConsole, result);

    while (true)
    {
        debouncedUpPin.refresh();
        debouncedSelectPin.refresh();
        debouncedDownPin.refresh();

        if (!debouncedUpPin.read())
            PageUp();

        if (!debouncedDownPin.read())
            PageDown();

        PageInfo &currentPage = GetCurrentPage();

        if (!debouncedSelectPin.read())
        {
            if (currentPage.HasID)
            {
                oledConsole << "Emulating..." << hwlib::flush;
                PN532::InitAsTargetResponse initAsTargetResponse;

                result = EmulateCard(initAsTargetResponse, pnDriver, currentPage.NfcID);
                HandleDriverResult(oledConsole, result);

                oledConsole << "\r" << "Connected.    " << hwlib::endl << hwlib::flush;
                pnDriver.InDeselect(0);
            }
            else
            {
                oledConsole << "Scanning..." << hwlib::endl << hwlib::flush;
                ScanMifareCardUUID(currentPage.NfcID, pnDriver);
                currentPage.HasID = true;
            }
        }

        oledConsole << "\f" << "Page #" << CurrentPage << hwlib::endl << hwlib::endl;
        if (currentPage.HasID)
            oledConsole << NFCIDToString(currentPage.NfcID);
        else
            oledConsole << "Empty Page.";

        oledConsole << hwlib::flush << hwlib::endl;
    }
}
