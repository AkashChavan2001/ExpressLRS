#include "LoRa_SX1276.h"
#include "LoRa_lowlevel.h"
#include "debug.h"

SX1276::SX1276(int rst, int dio0, int dio1, int txpin, int rxpin)
    : SX127xDriver(rst, dio0, dio1, txpin, rxpin)
{
    // no local init at the moment
}

uint8_t SX1276::Config(Bandwidth bw, SpreadingFactor sf, CodingRate cr, uint32_t freq, uint8_t syncWord)
{
    if (freq == 0)
        freq = currFreq;
    if (syncWord == 0)
        syncWord = _syncWord;

    uint8_t status = ERR_NONE;
    uint8_t newBandwidth, newSpreadingFactor, newCodingRate;

    // check the supplied BW, CR and SF values
    switch (bw)
    {
        case BW_62_50_KHZ:
            newBandwidth = SX1276_BW_62_50_KHZ;
            break;
        case BW_125_00_KHZ:
            newBandwidth = SX1276_BW_125_00_KHZ;
            break;
        case BW_250_00_KHZ:
            newBandwidth = SX1276_BW_250_00_KHZ;
            break;
        case BW_500_00_KHZ:
            newBandwidth = SX1276_BW_500_00_KHZ;
            break;
        default:
            return (ERR_INVALID_BANDWIDTH);
    }

    switch (sf)
    {
        case SF_6:
            newSpreadingFactor = SX127X_SF_6;
            break;
        case SF_7:
            newSpreadingFactor = SX127X_SF_7;
            break;
        case SF_8:
            newSpreadingFactor = SX127X_SF_8;
            break;
        case SF_9:
            newSpreadingFactor = SX127X_SF_9;
            break;
        case SF_10:
            newSpreadingFactor = SX127X_SF_10;
            break;
        case SF_11:
            newSpreadingFactor = SX127X_SF_11;
            break;
        case SF_12:
            newSpreadingFactor = SX127X_SF_12;
            break;
        default:
            return (ERR_INVALID_SPREADING_FACTOR);
    }

    switch (cr)
    {
        case CR_4_5:
            newCodingRate = SX1276_CR_4_5;
            break;
        case CR_4_6:
            newCodingRate = SX1276_CR_4_6;
            break;
        case CR_4_7:
            newCodingRate = SX1276_CR_4_7;
            break;
        case CR_4_8:
            newCodingRate = SX1276_CR_4_8;
            break;
        default:
            return (ERR_INVALID_CODING_RATE);
    }

    if ((freq < 137000000) || (freq > 1020000000))
    {
        DEBUG_PRINT("Invalid Frequnecy!: ");
        DEBUG_PRINTLN(freq);
        return (ERR_INVALID_FREQUENCY);
    }

    // configure common registers
    status = SX127xConfig(newBandwidth, newSpreadingFactor, newCodingRate, freq, syncWord);
    if (status != ERR_NONE)
    {
        return (status);
    }

    writeRegister(SX127X_REG_OCP, SX127X_OCP_ON | 18); // 15 (120mA) -> 18 (150mA)

    writeRegister(SX1278_REG_PA_DAC, (0x80 | SX127X_PA_BOOST_OFF));

    status = setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_AGC_AUTO_ON, 2, 2);

    // enable LNA gain setting by register and set low datarate optimizations for SF11/SF12 with 125 kHz bandwidth
    if ((bw == SX1278_BW_125_00_KHZ) && ((sf == SX127X_SF_11) || (sf == SX127X_SF_12)))
    {
        status = setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_ON, 0, 0);
    }
    else
    {
        status = setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_OFF, 0, 0);
    }
    if (status != ERR_NONE)
    {
        return (status);
    }

    uint8_t cfg1_reg = bw | cr;
    if (headerExplMode == false)
        cfg1_reg |= SX1276_HEADER_IMPL_MODE;
    status = setRegValue(SX127X_REG_MODEM_CONFIG_1, cfg1_reg);

    if (bw == SX1276_BW_500_00_KHZ)
    {
        //datasheet errata reconmendation http://caxapa.ru/thumbs/972894/SX1276_77_8_ErrataNote_1.1_STD.pdf
        status = setRegValue(0x36, 0x02);
        status = setRegValue(0x3a, 0x64);
    }
    else
    {
        status = setRegValue(0x36, 0x03);
    }
    if (status != ERR_NONE)
    {
        return (status);
    }

    // configuration successful, save the new settings
    currBW = bw;
    currSF = sf;
    currCR = cr;
    currFreq = freq;
    return (ERR_NONE);
}

/***************************************/

uint8_t SX1276configCommon(SX127xDriver *drv, uint8_t bw, uint8_t sf, uint8_t cr, uint32_t freq, uint8_t syncWord);

uint8_t SX1276config(SX127xDriver *drv, Bandwidth bw, SpreadingFactor sf, CodingRate cr, uint32_t freq, uint8_t syncWord)
{
    uint8_t status = ERR_NONE;
    uint8_t newBandwidth, newSpreadingFactor, newCodingRate;

    // check the supplied BW, CR and SF values
    switch (bw)
    {
        case BW_62_50_KHZ:
            newBandwidth = SX1276_BW_62_50_KHZ;
            break;
        case BW_125_00_KHZ:
            newBandwidth = SX1276_BW_125_00_KHZ;
            break;
        case BW_250_00_KHZ:
            newBandwidth = SX1276_BW_250_00_KHZ;
            break;
        case BW_500_00_KHZ:
            newBandwidth = SX1276_BW_500_00_KHZ;
            break;
        default:
            return (ERR_INVALID_BANDWIDTH);
    }

    switch (sf)
    {
        case SF_6:
            newSpreadingFactor = SX127X_SF_6;
            break;
        case SF_7:
            newSpreadingFactor = SX127X_SF_7;
            break;
        case SF_8:
            newSpreadingFactor = SX127X_SF_8;
            break;
        case SF_9:
            newSpreadingFactor = SX127X_SF_9;
            break;
        case SF_10:
            newSpreadingFactor = SX127X_SF_10;
            break;
        case SF_11:
            newSpreadingFactor = SX127X_SF_11;
            break;
        case SF_12:
            newSpreadingFactor = SX127X_SF_12;
            break;
        default:
            return (ERR_INVALID_SPREADING_FACTOR);
    }

    switch (cr)
    {
        case CR_4_5:
            newCodingRate = SX1276_CR_4_5;
            break;
        case CR_4_6:
            newCodingRate = SX1276_CR_4_6;
            break;
        case CR_4_7:
            newCodingRate = SX1276_CR_4_7;
            break;
        case CR_4_8:
            newCodingRate = SX1276_CR_4_8;
            break;
        default:
            return (ERR_INVALID_CODING_RATE);
    }

    if ((freq < 137000000) || (freq > 1020000000))
    {
        DEBUG_PRINT("Invalid Frequnecy!: ");
        DEBUG_PRINTLN(freq);
        return (ERR_INVALID_FREQUENCY);
    }

    // execute common part
    status = SX1276configCommon(drv, newBandwidth, newSpreadingFactor, newCodingRate, freq, syncWord);
    if (status != ERR_NONE)
    {
        return (status);
    }

    // configuration successful, save the new settings
    drv->currBW = bw;
    drv->currSF = sf;
    drv->currCR = cr;
    drv->currFreq = freq;
    return (ERR_NONE);
}

uint8_t SX1276configCommon(SX127xDriver *drv, uint8_t bw, uint8_t sf, uint8_t cr, uint32_t freq, uint8_t syncWord)
{
    // configure common registers
    uint8_t status = drv->SX127xConfig(bw, sf, cr, freq, syncWord);
    if (status != ERR_NONE)
    {
        return (status);
    }

    writeRegister(SX127X_REG_OCP, SX127X_OCP_ON | 18); // 15 (120mA) -> 18 (150mA)

    writeRegister(SX1278_REG_PA_DAC, (0x80 | SX127X_PA_BOOST_OFF));

    status = setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_AGC_AUTO_ON, 2, 2);

    // enable LNA gain setting by register and set low datarate optimizations for SF11/SF12 with 125 kHz bandwidth
    if ((bw == SX1278_BW_125_00_KHZ) && ((sf == SX127X_SF_11) || (sf == SX127X_SF_12)))
    {
        status = setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_ON, 0, 0);
    }
    else
    {
        status = setRegValue(SX1278_REG_MODEM_CONFIG_3, SX1278_LOW_DATA_RATE_OPT_OFF, 0, 0);
    }
    if (status != ERR_NONE)
    {
        return (status);
    }

    uint8_t cfg1_reg = bw | cr;
    if (drv->headerExplMode == false)
        cfg1_reg |= SX1276_HEADER_IMPL_MODE;
    status = setRegValue(SX127X_REG_MODEM_CONFIG_1, cfg1_reg);

    if (bw == SX1276_BW_500_00_KHZ)
    {
        //datasheet errata reconmendation http://caxapa.ru/thumbs/972894/SX1276_77_8_ErrataNote_1.1_STD.pdf
        status = setRegValue(0x36, 0x02);
        status = setRegValue(0x3a, 0x64);
    }
    else
    {
        status = setRegValue(0x36, 0x03);
    }

    return (status);
}
