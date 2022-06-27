#include <PN532/PN532-I2C.h>
#include <PNHelpers/PNHelpers.h>

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
            .Mode = PN532::SAM_MODE_Normal, // Disable the Security Module.
            .Timeout = 20, // 20 * 50ms -> 1000ms timeout.
            .EnableIRQ = false
    };

    PN532::DriverResult result = pnDriver.SetSAMConfiguration(samConfiguration);
    HandleDriverResult(oledConsole, result);

    bool testResult;
    result = pnDriver.DoRAMTest(testResult);
    HandleDriverResult(oledConsole, result);

    result = pnDriver.DoROMTest(testResult);
    HandleDriverResult(oledConsole, result);

    PN532::FirmwareVersionInfo firmwareVersionInfo;
    result = pnDriver.GetFirmwareVersion(firmwareVersionInfo);
    HandleDriverResult(oledConsole, result);

    PN532::GeneralStatusInfo generalStatusInfo;
    result = pnDriver.GetGeneralStatus(generalStatusInfo);
    HandleDriverResult(oledConsole, result);

    result = pnDriver.SetParameters(PN532::PARAMETERS_ISO14443_4_PICC);
    HandleDriverResult(oledConsole, result);

    PN532::ByteBuffer_t registerData;
    result = pnDriver.ReadRegisters(registerData, {PN532::SFR_ADDRESS_P3, PN532::SFR_ADDRESS_CIU_COMMAND});
    HandleDriverResult(oledConsole, result);

    PN532::GPIOInfo gpioInfo;
    result = pnDriver.ReadGPIO(gpioInfo);
    HandleDriverResult(oledConsole, result);

    bool shutdownSuccess;
    result = pnDriver.PowerDown(shutdownSuccess, PN532::WAKEUP_SOURCE_I2C);
    HandleDriverResult(oledConsole, result);

    return 0;
}
