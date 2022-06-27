#include <PN532/PN532-I2C.h>
#include <PNHelpers/PNHelpers.h>

static PN532::DynamicNFCID _nfcid({0xAB, 0xCD, 0xEF, 0x12});

__attribute__((noreturn))
int main()
{
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

    while (true)
    {
        oledConsole << '\f' << "UUID: " << hwlib::endl
                    << NFCIDToString(_nfcid)
                    << hwlib::endl << hwlib::flush;
        oledConsole << "Emulating..." << hwlib::flush;
    }
}
