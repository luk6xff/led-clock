/**
 *  @brief:  Implementation of a SX1278 radio functions
 *  @author: luk6xff based on SEMTCH code: https://github.com/Lora-net/LoRaMac-node
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2020-08-07
 */
#include "sx1278.h"
#include "common.h"

#include <math.h>
#include <string.h>

/**
 * ============================================================================
 * @brief Private global constants
 * ============================================================================
 */
//-----------------------------------------------------------------------------
/**
 * Precomputed FSK bandwidth registers values
 */
static const BandwidthMap_t SX1278FskBandwidths[] =
{
    { 2600  , 0x17 },
    { 3100  , 0x0F },
    { 3900  , 0x07 },
    { 5200  , 0x16 },
    { 6300  , 0x0E },
    { 7800  , 0x06 },
    { 10400 , 0x15 },
    { 12500 , 0x0D },
    { 15600 , 0x05 },
    { 20800 , 0x14 },
    { 25000 , 0x0C },
    { 31300 , 0x04 },
    { 41700 , 0x13 },
    { 50000 , 0x0B },
    { 62500 , 0x03 },
    { 83333 , 0x12 },
    { 100000, 0x0A },
    { 125000, 0x02 },
    { 166700, 0x11 },
    { 200000, 0x09 },
    { 250000, 0x01 },
    { 300000, 0x00 }, // Invalid Bandwidth
};

//-----------------------------------------------------------------------------
/**
 * @brief Precomputed LORA bandwidth registers values
 */
static const BandwidthMap_t SX1278LoRaBandwidths[] =
{
    {   7800, 0 }, //  7.8 kHz requires TCXO
    {  10400, 1 }, // 10.4 kHz requires TCXO
    {  15600, 2 }, // 15.6 kHz requires TCXO
    {  20800, 3 }, // 20.8 kHz requires TCXO
    {  31250, 4 }, // 31.25 kHz requires TCXO
    {  41700, 5 }, // 41.7 kHz requires TCXO
    {  62500, 6 }, // 62.5 kHz requires TCXO
    { 125000, 7 }, // 125 kHz the LoRa protocol default
    { 250000, 8 }, // 250 kHz
    { 500000, 9 }, // 500 kHz
    { 600000, 10 },// Invalid Bandwidth, reserved
 };

//-----------------------------------------------------------------------------
/**
 * @brief Radio hardware registers initialization definition
 */
static const RadioRegisters_t SX1278RadioRegsInit[] =
{                                                 \
    { MODEM_FSK , REG_LNA                , 0x23 },\
    { MODEM_FSK , REG_RXCONFIG           , 0x1E },\
    { MODEM_FSK , REG_RSSICONFIG         , 0xD2 },\
    { MODEM_FSK , REG_AFCFEI             , 0x01 },\
    { MODEM_FSK , REG_PREAMBLEDETECT     , 0xAA },\
    { MODEM_FSK , REG_OSC                , 0x07 },\
    { MODEM_FSK , REG_SYNCCONFIG         , 0x12 },\
    { MODEM_FSK , REG_SYNCVALUE1         , 0xC1 },\
    { MODEM_FSK , REG_SYNCVALUE2         , 0x94 },\
    { MODEM_FSK , REG_SYNCVALUE3         , 0xC1 },\
    { MODEM_FSK , REG_PACKETCONFIG1      , 0xD8 },\
    { MODEM_FSK , REG_FIFOTHRESH         , 0x8F },\
    { MODEM_FSK , REG_IMAGECAL           , 0x02 },\
    { MODEM_FSK , REG_DIOMAPPING1        , 0x00 },\
    { MODEM_FSK , REG_DIOMAPPING2        , 0x30 },\
    { MODEM_LORA, REG_LR_PAYLOADMAXLENGTH, 0x40 },\
};


/**
 * ============================================================================
 * @brief Private functions prototypes
 * ============================================================================
 */

/**
 * @brief Performs the Rx chain calibration for LF and HF bands
 * @note Must be called just after the reset so all registers are at their
 *         default values
 */
static void RxChainCalibration(sx1278* const dev);

/**
 * Returns the known FSK bandwidth registers value
 *
 * @param[in] bandwidth Bandwidth value in Hz
 * @retval regValue Bandwidth register value.
 */
static uint8_t GetFskBandwidthRegValue(uint32_t bandwidth);

/**
 * Returns the known LORA bandwidth registers value
 *
 * @param[in] bandwidth Bandwidth value in Hz
 * @retval regValue Bandwidth register value.
 */
static uint8_t GetLoRaBandwidthRegValue(uint32_t bandwidth);


/**
 * @brief DIO 0 IRQ callback
 */
static void on_dio0_irq(void* ctx);

/**
 * @brief DIO 1 IRQ callback
 */
static void on_dio1_irq(void* ctx);

/**
 * @brief DIO 2 IRQ callback
 */
static void on_dio2_irq(void* ctx);

/**
 * @brief DIO 3 IRQ callback
 */
static void on_dio3_irq(void* ctx);

/**
 * @brief DIO 4 IRQ callback
 */
static void on_dio4_irq(void* ctx);

/**
 * @brief DIO 5 IRQ callback
 */
static void on_dio5_irq(void* ctx);

/**
 * @brief Tx & Rx timeout timer callback
 */
static void on_timeout_irq();


/**
 * ============================================================================
 * @brief Public functions definitions
 * ============================================================================
 */
//-----------------------------------------------------------------------------
bool sx1278_init(sx1278* const dev)
{
    // Register Hardware DIO IRQ callback for dio0 - dio5
    dev->dio_irq[0] = on_dio0_irq;
    dev->dio_irq[1] = on_dio1_irq;
    dev->dio_irq[2] = on_dio2_irq;
    dev->dio_irq[3] = on_dio3_irq;
    dev->dio_irq[4] = on_dio4_irq;
    dev->dio_irq[5] = on_dio5_irq;

    // Initialize IO interfaces
    sx1278_io_init(dev);
    sx1278_reset(dev);
    RxChainCalibration(dev);
    sx1278_set_op_mode(dev, RF_OPMODE_SLEEP);

    sx1278_ioirq_init(dev, dev->dio_irq);
    sx1278_radio_registers_init(dev);
    sx1278_set_modem(dev, MODEM_FSK);
    dev->settings.State = RF_IDLE;
    return true;
}

//-----------------------------------------------------------------------------
void sx1278_radio_registers_init(sx1278* const dev)
{
    uint8_t i = 0;
    for(i = 0; i < sizeof(SX1278RadioRegsInit) / sizeof(RadioRegisters_t); i++)
    {
        sx1278_set_modem(dev, SX1278RadioRegsInit[i].modem);
        sx1278_write(dev, SX1278RadioRegsInit[i].addr, SX1278RadioRegsInit[i].value);
    }
}

//-----------------------------------------------------------------------------
RadioState_t sx1278_get_status(sx1278* const dev)
{
    return dev->settings.State;
}

//-----------------------------------------------------------------------------
void sx1278_set_channel(sx1278* const dev, uint32_t freq)
{
    dev->settings.Channel = freq;
    freq = (uint32_t)((double)freq / (double)FREQ_STEP);
    sx1278_write(dev, REG_FRFMSB, (uint8_t)((freq >> 16) & 0xFF));
    sx1278_write(dev, REG_FRFMID, (uint8_t)((freq >> 8) & 0xFF));
    sx1278_write(dev, REG_FRFLSB, (uint8_t)(freq & 0xFF));
}

//-----------------------------------------------------------------------------
bool sx1278_is_channel_free(sx1278* const dev, RadioModems_t modem, uint32_t freq, int16_t rssiThresh, uint32_t maxCarrierSenseTime)
{
    // TODO handle carrierSenseTime
    int16_t rssi = 0;

    sx1278_set_modem(dev, modem);

    sx1278_set_channel(dev, freq);

    sx1278_set_op_mode(dev, RF_OPMODE_RECEIVER);

    sx1278_delay_ms(1);

    rssi = sx1278_get_rssi(dev, modem);

    sx1278_set_sleep(dev);
    if (rssi > rssiThresh)
    {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
uint32_t sx1278_random(sx1278* const dev)
{
    uint8_t i;
    uint32_t rnd = 0;

    /*
     * Radio setup for random number generation
     */
    // Set LoRa modem ON
    sx1278_set_modem(dev, MODEM_LORA);

    // Disable LoRa modem interrupts
    sx1278_write(dev, REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
                  RFLR_IRQFLAGS_RXDONE |
                  RFLR_IRQFLAGS_PAYLOADCRCERROR |
                  RFLR_IRQFLAGS_VALIDHEADER |
                  RFLR_IRQFLAGS_TXDONE |
                  RFLR_IRQFLAGS_CADDONE |
                  RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                  RFLR_IRQFLAGS_CADDETECTED);

    // Set radio in continuous reception
    sx1278_set_op_mode(dev, RF_OPMODE_RECEIVER);

    for(i = 0; i < 32; i++)
    {
        sx1278_delay_ms(1);
        // Unfiltered RSSI value SX1278Reading. Only takes the LSB value
        rnd |= ((uint32_t)sx1278_read(dev, REG_LR_RSSIWIDEBAND) & 0x01) << i;
    }

    sx1278_set_sleep(dev);

    return rnd;
}

//-----------------------------------------------------------------------------
void sx1278_set_rx_config(sx1278* const dev, RadioModems_t modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, bool fixLen,
                         uint8_t payloadLen,
                         bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                         bool iqInverted, bool rxContinuous)
{
    sx1278_set_modem(dev, modem);

    switch (modem)
    {
        case MODEM_FSK:
        {
            dev->settings.Fsk.Bandwidth = bandwidth;
            dev->settings.Fsk.Datarate = datarate;
            dev->settings.Fsk.BandwidthAfc = bandwidthAfc;
            dev->settings.Fsk.FixLen = fixLen;
            dev->settings.Fsk.PayloadLen = payloadLen;
            dev->settings.Fsk.CrcOn = crcOn;
            dev->settings.Fsk.IqInverted = iqInverted;
            dev->settings.Fsk.RxContinuous = rxContinuous;
            dev->settings.Fsk.PreambleLen = preambleLen;
            dev->settings.Fsk.RxSingleTimeout = symbTimeout * ((1.0 / (double)datarate) * 8.0) * 1000;

            datarate = (uint16_t)((double)XTAL_FREQ / (double)datarate);
            sx1278_write(dev, REG_BITRATEMSB, (uint8_t)(datarate >> 8));
            sx1278_write(dev, REG_BITRATELSB, (uint8_t)(datarate & 0xFF));

            sx1278_write(dev, REG_RXBW, GetFskBandwidthRegValue(bandwidth));
            sx1278_write(dev, REG_AFCBW, GetFskBandwidthRegValue(bandwidthAfc));

            sx1278_write(dev, REG_PREAMBLEMSB, (uint8_t)((preambleLen >> 8) & 0xFF));
            sx1278_write(dev, REG_PREAMBLELSB, (uint8_t)(preambleLen & 0xFF));

            if (fixLen == 1)
            {
                sx1278_write(dev, REG_PAYLOADLENGTH, payloadLen);
            }
            else
            {
                sx1278_write(dev, REG_PAYLOADLENGTH, 0xFF); // Set payload length to the maximum
            }

            sx1278_write(dev, REG_PACKETCONFIG1,
                         (sx1278_read(dev, REG_PACKETCONFIG1) &
                           RF_PACKETCONFIG1_CRC_MASK &
                           RF_PACKETCONFIG1_PACKETFORMAT_MASK) |
                           ((fixLen == 1) ? RF_PACKETCONFIG1_PACKETFORMAT_FIXED : RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE) |
                           (crcOn << 4));
            sx1278_write(dev, REG_PACKETCONFIG2, (sx1278_read(dev, REG_PACKETCONFIG2) | RF_PACKETCONFIG2_DATAMODE_PACKET));
        }
        break;
        case MODEM_LORA:
        {
            if (bandwidth > 11) // specified in Hz, needs mapping
            {
            	bandwidth = GetLoRaBandwidthRegValue(bandwidth);
            }
            if (bandwidth > LORA_BANDWIDTH_500kHz)
            {
                // Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
                while(1);
            }
            dev->settings.LoRa.Bandwidth = bandwidth;
            dev->settings.LoRa.Datarate = datarate;
            dev->settings.LoRa.Coderate = coderate;
            dev->settings.LoRa.PreambleLen = preambleLen;
            dev->settings.LoRa.FixLen = fixLen;
            dev->settings.LoRa.PayloadLen = payloadLen;
            dev->settings.LoRa.CrcOn = crcOn;
            dev->settings.LoRa.FreqHopOn = freqHopOn;
            dev->settings.LoRa.HopPeriod = hopPeriod;
            dev->settings.LoRa.IqInverted = iqInverted;
            dev->settings.LoRa.RxContinuous = rxContinuous;

            if (datarate > LORA_SF12)
            {
                datarate = LORA_SF12;
            }
            else if (datarate < LORA_SF6)
            {
                datarate = LORA_SF6;
            }

            if (((bandwidth == LORA_BANDWIDTH_125kHz) && ((datarate == LORA_SF11) || (datarate == LORA_SF12))) ||
               ((bandwidth == LORA_BANDWIDTH_250kHz) && (datarate == LORA_SF12)))
            {
                dev->settings.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                dev->settings.LoRa.LowDatarateOptimize = 0x00;
            }

            sx1278_write(dev, REG_LR_MODEMCONFIG1,
                         (sx1278_read(dev, REG_LR_MODEMCONFIG1) &
                           RFLR_MODEMCONFIG1_BW_MASK &
                           RFLR_MODEMCONFIG1_CODINGRATE_MASK &
                           RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK) |
                           (bandwidth << 4) | (coderate << 1) |
                           fixLen);

            sx1278_write(dev, REG_LR_MODEMCONFIG2,
                         (sx1278_read(dev, REG_LR_MODEMCONFIG2) &
                           RFLR_MODEMCONFIG2_SF_MASK &
                           RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK &
                           RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK) |
                           (datarate << 4) | (crcOn << 2) |
                           ((symbTimeout >> 8) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK));

            sx1278_write(dev, REG_LR_MODEMCONFIG3,
                         (sx1278_read(dev, REG_LR_MODEMCONFIG3) &
                           RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK) |
                           (dev->settings.LoRa.LowDatarateOptimize << 3));

            sx1278_write(dev, REG_LR_SYMBTIMEOUTLSB, (uint8_t)(symbTimeout & 0xFF));

            sx1278_write(dev, REG_LR_PREAMBLEMSB, (uint8_t)((preambleLen >> 8) & 0xFF));
            sx1278_write(dev, REG_LR_PREAMBLELSB, (uint8_t)(preambleLen & 0xFF));

            if (fixLen == 1)
            {
                sx1278_write(dev, REG_LR_PAYLOADLENGTH, payloadLen);
            }

            if (dev->settings.LoRa.FreqHopOn == true)
            {
                sx1278_write(dev, REG_LR_PLLHOP, (sx1278_read(dev, REG_LR_PLLHOP) & RFLR_PLLHOP_FASTHOP_MASK) | RFLR_PLLHOP_FASTHOP_ON);
                sx1278_write(dev, REG_LR_HOPPERIOD, dev->settings.LoRa.HopPeriod);
            }

            if ((bandwidth == LORA_BANDWIDTH_500kHz) && (dev->settings.Channel > RF_MID_BAND_THRESH))
            {
                // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
                sx1278_write(dev, REG_LR_HIGHBWOPTIMIZE1, 0x02);
                sx1278_write(dev, REG_LR_HIGHBWOPTIMIZE2, 0x64);
            }
            else if (bandwidth == LORA_BANDWIDTH_500kHz)
            {
                // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
                sx1278_write(dev, REG_LR_HIGHBWOPTIMIZE1, 0x02);
                sx1278_write(dev, REG_LR_HIGHBWOPTIMIZE2, 0x7F);
            }
            else
            {
                // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
                sx1278_write(dev, REG_LR_HIGHBWOPTIMIZE1, 0x03);
            }

            if (datarate == LORA_SF6)
            {
                sx1278_write(dev, REG_LR_DETECTOPTIMIZE,
                             (sx1278_read(dev, REG_LR_DETECTOPTIMIZE) &
                               RFLR_DETECTIONOPTIMIZE_MASK) |
                               RFLR_DETECTIONOPTIMIZE_SF6);
                sx1278_write(dev, REG_LR_DETECTIONTHRESHOLD,
                             RFLR_DETECTIONTHRESH_SF6);
            }
            else
            {
                sx1278_write(dev, REG_LR_DETECTOPTIMIZE,
                            (sx1278_read(dev, REG_LR_DETECTOPTIMIZE) &
                            RFLR_DETECTIONOPTIMIZE_MASK) |
                            RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12);
                sx1278_write(dev, REG_LR_DETECTIONTHRESHOLD,
                            RFLR_DETECTIONTHRESH_SF7_TO_SF12);
            }
        }
        break;
    }
}

//-----------------------------------------------------------------------------
void sx1278_set_tx_config(sx1278* const dev, RadioModems_t modem, int8_t power, uint32_t fdev,
                        uint32_t bandwidth, uint32_t datarate,
                        uint8_t coderate, uint16_t preambleLen,
                        bool fixLen, bool crcOn, bool freqHopOn,
                        uint8_t hopPeriod, bool iqInverted, uint32_t timeout)
{
    sx1278_set_modem(dev, modem);
    sx1278_set_rf_tx_power(dev, power);

    switch (modem)
    {
        case MODEM_FSK:
        {
            dev->settings.Fsk.Power = power;
            dev->settings.Fsk.Fdev = fdev;
            dev->settings.Fsk.Bandwidth = bandwidth;
            dev->settings.Fsk.Datarate = datarate;
            dev->settings.Fsk.PreambleLen = preambleLen;
            dev->settings.Fsk.FixLen = fixLen;
            dev->settings.Fsk.CrcOn = crcOn;
            dev->settings.Fsk.IqInverted = iqInverted;
            dev->settings.Fsk.TxTimeout = timeout;

            fdev = (uint16_t)((double)fdev / (double)FREQ_STEP);
            sx1278_write(dev, REG_FDEVMSB, (uint8_t)(fdev >> 8));
            sx1278_write(dev, REG_FDEVLSB, (uint8_t)(fdev & 0xFF));

            datarate = (uint16_t)((double)XTAL_FREQ / (double)datarate);
            sx1278_write(dev, REG_BITRATEMSB, (uint8_t)(datarate >> 8));
            sx1278_write(dev, REG_BITRATELSB, (uint8_t)(datarate & 0xFF));

            sx1278_write(dev, REG_PREAMBLEMSB, (preambleLen >> 8) & 0x00FF);
            sx1278_write(dev, REG_PREAMBLELSB, preambleLen & 0xFF);

            sx1278_write(dev, REG_PACKETCONFIG1,
                        (sx1278_read(dev, REG_PACKETCONFIG1) &
                        RF_PACKETCONFIG1_CRC_MASK &
                        RF_PACKETCONFIG1_PACKETFORMAT_MASK) |
                        ((fixLen == 1) ? RF_PACKETCONFIG1_PACKETFORMAT_FIXED : RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE) |
                        (crcOn << 4));
            sx1278_write(dev, REG_PACKETCONFIG2, (sx1278_read(dev, REG_PACKETCONFIG2) | RF_PACKETCONFIG2_DATAMODE_PACKET));
        }
        break;
        case MODEM_LORA:
        {
            dev->settings.LoRa.Power = power;
            if (bandwidth > LORA_BANDWIDTH_500kHz)
            {
                // Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
                while(1);
            }

            dev->settings.LoRa.Bandwidth = bandwidth;
            dev->settings.LoRa.Datarate = datarate;
            dev->settings.LoRa.Coderate = coderate;
            dev->settings.LoRa.PreambleLen = preambleLen;
            dev->settings.LoRa.FixLen = fixLen;
            dev->settings.LoRa.FreqHopOn = freqHopOn;
            dev->settings.LoRa.HopPeriod = hopPeriod;
            dev->settings.LoRa.CrcOn = crcOn;
            dev->settings.LoRa.IqInverted = iqInverted;
            dev->settings.LoRa.TxTimeout = timeout;

            if (datarate > LORA_SF12)
            {
                datarate = LORA_SF12;
            }
            else if (datarate < LORA_SF6)
            {
                datarate = LORA_SF6;
            }
            if (((bandwidth == LORA_BANDWIDTH_125kHz) && ((datarate == LORA_SF11) || (datarate == LORA_SF12))) ||
                ((bandwidth == LORA_BANDWIDTH_250kHz) && (datarate == LORA_SF12)))
            {
                dev->settings.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                dev->settings.LoRa.LowDatarateOptimize = 0x00;
            }

            if (dev->settings.LoRa.FreqHopOn == true)
            {
                sx1278_write(dev, REG_LR_PLLHOP, (sx1278_read(dev, REG_LR_PLLHOP) & RFLR_PLLHOP_FASTHOP_MASK) | RFLR_PLLHOP_FASTHOP_ON);
                sx1278_write(dev, REG_LR_HOPPERIOD, dev->settings.LoRa.HopPeriod);
            }

            sx1278_write(dev, REG_LR_MODEMCONFIG1,
                         (sx1278_read(dev, REG_LR_MODEMCONFIG1) &
                           RFLR_MODEMCONFIG1_BW_MASK &
                           RFLR_MODEMCONFIG1_CODINGRATE_MASK &
                           RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK) |
                           (bandwidth << 4) | (coderate << 1) |
                           fixLen);

            sx1278_write(dev, REG_LR_MODEMCONFIG2,
                         (sx1278_read(dev, REG_LR_MODEMCONFIG2) &
                           RFLR_MODEMCONFIG2_SF_MASK &
                           RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK) |
                           (datarate << 4) | (crcOn << 2));

            sx1278_write(dev, REG_LR_MODEMCONFIG3,
                         (sx1278_read(dev, REG_LR_MODEMCONFIG3) &
                           RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK) |
                           (dev->settings.LoRa.LowDatarateOptimize << 3));

            sx1278_write(dev, REG_LR_PREAMBLEMSB, (preambleLen >> 8) & 0x00FF);
            sx1278_write(dev, REG_LR_PREAMBLELSB, preambleLen & 0xFF);

            if (datarate == LORA_SF6)
            {
                sx1278_write(dev, REG_LR_DETECTOPTIMIZE,
                             (sx1278_read(dev, REG_LR_DETECTOPTIMIZE) &
                               RFLR_DETECTIONOPTIMIZE_MASK) |
                               RFLR_DETECTIONOPTIMIZE_SF6);
                sx1278_write(dev, REG_LR_DETECTIONTHRESHOLD,
                             RFLR_DETECTIONTHRESH_SF6);
            }
            else
            {
                sx1278_write(dev, REG_LR_DETECTOPTIMIZE,
                             (sx1278_read(dev, REG_LR_DETECTOPTIMIZE) &
                             RFLR_DETECTIONOPTIMIZE_MASK) |
                             RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12);
                sx1278_write(dev, REG_LR_DETECTIONTHRESHOLD,
                             RFLR_DETECTIONTHRESH_SF7_TO_SF12);
            }
        }
        break;
    }
}

//-----------------------------------------------------------------------------
uint32_t SX1278TimeOnAir(sx1278* const dev, RadioModems_t modem, uint8_t pktLen)
{
    uint32_t airTime = 0;

    switch (modem)
    {
    case MODEM_FSK:
        {
            airTime = rint((8 * (dev->settings.Fsk.PreambleLen +
                                ((sx1278_read(dev, REG_SYNCCONFIG) & ~RF_SYNCCONFIG_SYNCSIZE_MASK) + 1) +
                                ((dev->settings.Fsk.FixLen == 0x01) ? 0.0 : 1.0) +
                                (((sx1278_read(dev, REG_PACKETCONFIG1) & ~RF_PACKETCONFIG1_ADDRSFILTERING_MASK) != 0x00) ? 1.0 : 0) +
                                pktLen +
                                ((dev->settings.Fsk.CrcOn == 0x01) ? 2.0 : 0)) /
                                dev->settings.Fsk.Datarate) * 1000);
        }
        break;
    case MODEM_LORA:
        {
            double bw = 0.0;
            // NOTE: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
            switch (dev->settings.LoRa.Bandwidth)
            {
                case LORA_BANDWIDTH_7kHz: // 7.8 kHz
                    bw = 78e2;
                    break;
                case LORA_BANDWIDTH_10kHz: // 10.4 kHz
                    bw = 104e2;
                    break;
                case LORA_BANDWIDTH_15kHz: // 15.6 kHz
                    bw = 156e2;
                    break;
                case LORA_BANDWIDTH_20kHz: // 20.8 kHz
                    bw = 208e2;
                    break;
                case LORA_BANDWIDTH_31kHz: // 31.25 kHz
                    bw = 312e2;
                    break;
                case LORA_BANDWIDTH_41kHz: // 41.7 kHz
                    bw = 414e2;
                    break;
                case LORA_BANDWIDTH_62kHz: // 62.5 kHz
                    bw = 625e2;
                    break;
                case LORA_BANDWIDTH_125kHz: // 125 kHz
                    bw = 125e3;
                    break;
                case LORA_BANDWIDTH_250kHz: // 250 kHz
                    bw = 250e3;
                    break;
                case LORA_BANDWIDTH_500kHz: // 500 kHz
                    bw = 500e3;
                break;
            }

            // Symbol rate : time for one symbol (secs)
            double rs = bw / (1 << dev->settings.LoRa.Datarate);
            double ts = 1 / rs;
            // time of preamble
            double tPreamble = (dev->settings.LoRa.PreambleLen + 4.25) * ts;
            // Symbol length of payload and time
            double tmp = ceil((8 * pktLen - 4 * dev->settings.LoRa.Datarate +
                                 28 + 16 * dev->settings.LoRa.CrcOn -
                                 (dev->settings.LoRa.FixLen ? 20 : 0)) /
                                 (double)(4 * (dev->settings.LoRa.Datarate -
                                 ((dev->settings.LoRa.LowDatarateOptimize > 0) ? 2 : 0)))) *
                                 (dev->settings.LoRa.Coderate + 4);
            double nPayload = 8 + ((tmp > 0) ? tmp : 0);
            double tPayload = nPayload * ts;
            // Time on air
            double tOnAir = tPreamble + tPayload;
            // return ms secs
            airTime = floor(tOnAir * 1000 + 0.999);
        }
        break;
    }
    return airTime;
}

//-----------------------------------------------------------------------------
void sx1278_send(sx1278* const dev, uint8_t* buffer, uint8_t size)
{
    uint32_t txTimeout = 0;

    switch (dev->settings.modem)
    {
        case MODEM_FSK:
        {
            dev->settings.FskPacketHandler.NbBytes = 0;
            dev->settings.FskPacketHandler.Size = size;

            if (dev->settings.Fsk.FixLen == false)
            {
                sx1278_write_fifo(dev, (uint8_t*)&size, 1);
            }
            else
            {
                sx1278_write(dev, REG_PAYLOADLENGTH, size);
            }

            if ((size > 0) && (size <= 64))
            {
                dev->settings.FskPacketHandler.ChunkSize = size;
            }
            else
            {
                memcpy(dev->rx_tx_buffer, buffer, size);
                dev->settings.FskPacketHandler.ChunkSize = 32;
            }

            // Write payload buffer
            sx1278_write_fifo(dev, buffer, dev->settings.FskPacketHandler.ChunkSize);
            dev->settings.FskPacketHandler.NbBytes += dev->settings.FskPacketHandler.ChunkSize;
            txTimeout = dev->settings.Fsk.TxTimeout;
        }
        break;
        case MODEM_LORA:
        {
            if (dev->settings.LoRa.IqInverted == true)
            {
                sx1278_write(dev, REG_LR_INVERTIQ, ((sx1278_read(dev, REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_ON));
                sx1278_write(dev, REG_LR_INVERTIQ2, RFLR_INVERTIQ2_ON);
            }
            else
            {
                sx1278_write(dev, REG_LR_INVERTIQ, ((sx1278_read(dev, REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF));
                sx1278_write(dev, REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF);
            }

            dev->settings.LoRaPacketHandler.Size = size;

            // Initializes the payload size
            sx1278_write(dev, REG_LR_PAYLOADLENGTH, size);

            // Full buffer used for Tx
            sx1278_write(dev, REG_LR_FIFOTXBASEADDR, 0);
            sx1278_write(dev, REG_LR_FIFOADDRPTR, 0);

            // FIFO operations can not take place in sx1278_set_sleep mode
            if ((sx1278_read(dev, REG_OPMODE) & ~RF_OPMODE_MASK) == RF_OPMODE_SLEEP)
            {
                sx1278_set_standby(dev);
                //sx1278_delay_ms(1); //TODO crashes on ESP32
            }
            // sx1278_write payload buffer
            sx1278_write_fifo(dev, buffer, size);
            txTimeout = dev->settings.LoRa.TxTimeout;
        }
        break;
    }

    sx1278_set_tx(dev, txTimeout);
}

//-----------------------------------------------------------------------------
void sx1278_set_sleep(sx1278* const dev)
{
    sx1278_set_timeout(dev, TXTimeoutTimer, NULL, 0);
    sx1278_set_timeout(dev, RXTimeoutTimer, NULL, 0);

    sx1278_set_op_mode(dev, RF_OPMODE_SLEEP);
    //TODO Disable TCXO radio is in SLEEP mode if available
    dev->settings.State = RF_IDLE;
}

//-----------------------------------------------------------------------------
void sx1278_set_standby(sx1278* const dev)
{
    sx1278_set_timeout(dev, TXTimeoutTimer, NULL, 0);
    sx1278_set_timeout(dev, RXTimeoutTimer, NULL, 0);
    sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, NULL, 0);

    sx1278_set_op_mode(dev, RF_OPMODE_STANDBY);
    dev->settings.State = RF_IDLE;
}

//-----------------------------------------------------------------------------
void sx1278_set_rx(sx1278* const dev, uint32_t timeout)
{
    bool rxContinuous = false;

    switch (dev->settings.modem)
    {
    case MODEM_FSK:
        {
            rxContinuous = dev->settings.Fsk.RxContinuous;

            // DIO0=PayloadSX1278Ready
            // DIO1=FifoLevel
            // DIO2=SyncAddr
            // DIO3=FifoEmpty
            // DIO4=Preamble
            // DIO5=ModeSX1278Ready
            sx1278_write(dev, REG_DIOMAPPING1, (sx1278_read(dev, REG_DIOMAPPING1) & RF_DIOMAPPING1_DIO0_MASK &
                                                                        RF_DIOMAPPING1_DIO1_MASK &
                                                                        RF_DIOMAPPING1_DIO2_MASK) |
                                                                        RF_DIOMAPPING1_DIO0_00 |
                                                                        RF_DIOMAPPING1_DIO1_00 |
                                                                        RF_DIOMAPPING1_DIO2_11);

            sx1278_write(dev, REG_DIOMAPPING2, (sx1278_read(dev, REG_DIOMAPPING2) & RF_DIOMAPPING2_DIO4_MASK &
                                                                        RF_DIOMAPPING2_MAP_MASK) |
                                                                        RF_DIOMAPPING2_DIO4_11 |
                                                                        RF_DIOMAPPING2_MAP_PREAMBLEDETECT);

            dev->settings.FskPacketHandler.FifoThresh = sx1278_read(dev, REG_FIFOTHRESH) & 0x3F;

            sx1278_write(dev, REG_RXCONFIG, RF_RXCONFIG_AFCAUTO_ON | RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT);

            dev->settings.FskPacketHandler.PreambleDetected = false;
            dev->settings.FskPacketHandler.SyncWordDetected = false;
            dev->settings.FskPacketHandler.NbBytes = 0;
            dev->settings.FskPacketHandler.Size = 0;
        }
        break;
    case MODEM_LORA:
        {
            if (dev->settings.LoRa.IqInverted == true)
            {
                sx1278_write(dev, REG_LR_INVERTIQ, ((sx1278_read(dev, REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_ON | RFLR_INVERTIQ_TX_OFF));
                sx1278_write(dev, REG_LR_INVERTIQ2, RFLR_INVERTIQ2_ON);
            }
            else
            {
                sx1278_write(dev, REG_LR_INVERTIQ, ((sx1278_read(dev, REG_LR_INVERTIQ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF));
                sx1278_write(dev, REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF);
            }

            // ERRATA 2.3 - Receiver Spurious Reception of a LoRa Signal
            if (dev->settings.LoRa.Bandwidth < LORA_BANDWIDTH_500kHz)
            {
                sx1278_write(dev, REG_LR_DETECTOPTIMIZE, sx1278_read(dev, REG_LR_DETECTOPTIMIZE) & 0x7F);
                sx1278_write(dev, REG_LR_IFFREQ2, 0x00);
                switch (dev->settings.LoRa.Bandwidth)
                {
                    case LORA_BANDWIDTH_7kHz: // 7.8 kHz
                        sx1278_write(dev, REG_LR_IFFREQ1, 0x48 );
                        sx1278_set_channel(dev, dev->settings.Channel + 7810);
                        break;
                    case LORA_BANDWIDTH_10kHz: // 10.4 kHz
                        sx1278_write(dev, REG_LR_IFFREQ1, 0x44 );
                        sx1278_set_channel(dev, dev->settings.Channel + 10420);
                        break;
                    case LORA_BANDWIDTH_15kHz: // 15.6 kHz
                        sx1278_write(dev, REG_LR_IFFREQ1, 0x44 );
                        sx1278_set_channel(dev, dev->settings.Channel + 15620);
                        break;
                    case LORA_BANDWIDTH_20kHz: // 20.8 kHz
                        sx1278_write(dev, REG_LR_IFFREQ1, 0x44 );
                        sx1278_set_channel(dev, dev->settings.Channel + 20830);
                        break;
                    case LORA_BANDWIDTH_31kHz: // 31.25 kHz
                        sx1278_write(dev, REG_LR_IFFREQ1, 0x44 );
                        sx1278_set_channel(dev, dev->settings.Channel + 31250);
                        break;
                    case LORA_BANDWIDTH_41kHz: // 41.4 kHz
                        sx1278_write(dev, REG_LR_IFFREQ1, 0x44 );
                        sx1278_set_channel(dev, dev->settings.Channel + 41670);
                        break;
                    case LORA_BANDWIDTH_62kHz: // 62.5 kHz
                        sx1278_write(dev, REG_LR_IFFREQ1, 0x40 );
                        break;
                    case LORA_BANDWIDTH_125kHz: // 125 kHz
                        sx1278_write(dev, REG_LR_IFFREQ1, 0x40 );
                        break;
                    case LORA_BANDWIDTH_250kHz: // 250 kHz
                        sx1278_write(dev, REG_LR_IFFREQ1, 0x40 );
                        break;
                }
            }
            else
            {
                sx1278_write(dev, REG_LR_DETECTOPTIMIZE, sx1278_read(dev, REG_LR_DETECTOPTIMIZE) | 0x80);
            }

            rxContinuous = dev->settings.LoRa.RxContinuous;

            if (dev->settings.LoRa.FreqHopOn == true)
            {
                sx1278_write(dev, REG_LR_IRQFLAGSMASK,//RFLR_IRQFLAGS_RXTIMEOUT |
                                                //RFLR_IRQFLAGS_RXDONE |
                                                //RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                                RFLR_IRQFLAGS_VALIDHEADER |
                                                RFLR_IRQFLAGS_TXDONE |
                                                RFLR_IRQFLAGS_CADDONE |
                                                //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                                RFLR_IRQFLAGS_CADDETECTED);

                // DIO0=RxDone, DIO2=FhssChangeChannel
                sx1278_write(dev, REG_DIOMAPPING1, (sx1278_read(dev, REG_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO2_MASK ) | RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO2_00);
            }
            else
            {
                sx1278_write(dev, REG_LR_IRQFLAGSMASK,//RFLR_IRQFLAGS_RXTIMEOUT |
                                                //RFLR_IRQFLAGS_RXDONE |
                                                //RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                                RFLR_IRQFLAGS_VALIDHEADER |
                                                RFLR_IRQFLAGS_TXDONE |
                                                RFLR_IRQFLAGS_CADDONE |
                                                RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                                RFLR_IRQFLAGS_CADDETECTED);

                // DIO0=RxDone
                sx1278_write(dev, REG_DIOMAPPING1, (sx1278_read(dev, REG_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK) | RFLR_DIOMAPPING1_DIO0_00);
            }
            sx1278_write(dev, REG_LR_FIFORXBASEADDR, 0);
            sx1278_write(dev, REG_LR_FIFOADDRPTR, 0);
        }
        break;
    }

    memset(dev->rx_tx_buffer, 0, (size_t)RX_BUFFER_SIZE);

    dev->settings.State = RF_RX_RUNNING;
    if (timeout != 0)
    {
        sx1278_set_timeout(dev, RXTimeoutTimer, &on_timeout_irq, timeout);
    }

    if (dev->settings.modem == MODEM_FSK)
    {
        sx1278_set_op_mode(dev, RF_OPMODE_RECEIVER);

        if (rxContinuous == false)
        {
            sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, &on_timeout_irq, dev->settings.Fsk.RxSingleTimeout);
        }
    }
    else // MODEM_LORA
    {
        if (rxContinuous == true)
        {
            sx1278_set_op_mode(dev, RFLR_OPMODE_RECEIVER);
        }
        else
        {
            sx1278_set_op_mode(dev, RFLR_OPMODE_RECEIVER_SINGLE);
        }
    }
}

//-----------------------------------------------------------------------------
void sx1278_set_tx(sx1278* const dev, uint32_t timeout)
{

    switch (dev->settings.modem)
    {
    case MODEM_FSK:
        {
            // DIO0=PacketSent
            // DIO1=FifoEmpty
            // DIO2=FifoFull
            // DIO3=FifoEmpty
            // DIO4=LowBat
            // DIO5=ModeSX1278Ready
            sx1278_write(dev, REG_DIOMAPPING1, (sx1278_read(dev, REG_DIOMAPPING1) & RF_DIOMAPPING1_DIO0_MASK &
                                                                        RF_DIOMAPPING1_DIO1_MASK &
                                                                        RF_DIOMAPPING1_DIO2_MASK) |
                                                                        RF_DIOMAPPING1_DIO1_01);

            sx1278_write(dev, REG_DIOMAPPING2, (sx1278_read(dev, REG_DIOMAPPING2) & RF_DIOMAPPING2_DIO4_MASK &
                                                                            RF_DIOMAPPING2_MAP_MASK));
            dev->settings.FskPacketHandler.FifoThresh = sx1278_read(dev, REG_FIFOTHRESH) & 0x3F;
        }
        break;
    case MODEM_LORA:
        {
            if (dev->settings.LoRa.FreqHopOn == true)
            {
                sx1278_write(dev, REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
                                                  RFLR_IRQFLAGS_RXDONE |
                                                  RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                                  RFLR_IRQFLAGS_VALIDHEADER |
                                                  //RFLR_IRQFLAGS_TXDONE |
                                                  RFLR_IRQFLAGS_CADDONE |
                                                  //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                                  RFLR_IRQFLAGS_CADDETECTED);

                // DIO0=TxDone, DIO2=FhssChangeChannel
                sx1278_write(dev, REG_DIOMAPPING1, (sx1278_read(dev, REG_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO2_MASK) | RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO2_00);
            }
            else
            {
                sx1278_write(dev, REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
                                                  RFLR_IRQFLAGS_RXDONE |
                                                  RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                                  RFLR_IRQFLAGS_VALIDHEADER |
                                                  //RFLR_IRQFLAGS_TXDONE |
                                                  RFLR_IRQFLAGS_CADDONE |
                                                  RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                                  RFLR_IRQFLAGS_CADDETECTED);

                // DIO0=TxDone
                sx1278_write(dev, REG_DIOMAPPING1, (sx1278_read(dev, REG_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK) | RFLR_DIOMAPPING1_DIO0_01);
            }
        }
        break;
    }

    dev->settings.State = RF_TX_RUNNING;
    sx1278_set_timeout(dev, TXTimeoutTimer, &on_timeout_irq, timeout);
    sx1278_set_op_mode(dev, RF_OPMODE_TRANSMITTER);
}

//-----------------------------------------------------------------------------
void sx1278_start_cad(sx1278* const dev)
{
    switch (dev->settings.modem)
    {
    case MODEM_FSK:
        {

        }
        break;
    case MODEM_LORA:
        {
            sx1278_write(dev, REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
                                        RFLR_IRQFLAGS_RXDONE |
                                        RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                        RFLR_IRQFLAGS_VALIDHEADER |
                                        RFLR_IRQFLAGS_TXDONE |
                                        //RFLR_IRQFLAGS_CADDONE |
                                        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL // |
                                        //RFLR_IRQFLAGS_CADDETECTED
                                       );

            if (dev->dio_irq[3])
            {
                // DIO3=CADDone
                sx1278_write(dev, REG_DIOMAPPING1, (sx1278_read(dev, REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO3_MASK ) | RFLR_DIOMAPPING1_DIO3_00 );
            }
            else
            {
                // DIO0=CADDone
                sx1278_write(dev, REG_DIOMAPPING1, (sx1278_read(dev, REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_10 );
            }

            dev->settings.State = RF_CAD;
            sx1278_set_op_mode(dev, RFLR_OPMODE_CAD);
        }
        break;
    default:
        break;
    }
}

//-----------------------------------------------------------------------------
void sx1278_set_tx_continuous_wave(sx1278* const dev, uint32_t freq, int8_t power, uint16_t time)
{
    uint32_t timeout = (uint32_t)(time);

    sx1278_set_channel(dev, freq);

    sx1278_set_tx_config(dev, MODEM_FSK, power, 0, 0, 4800, 0, 5, false, false, 0, 0, 0, timeout);

    sx1278_write(dev, REG_PACKETCONFIG2, (sx1278_read(dev, REG_PACKETCONFIG2) & RF_PACKETCONFIG2_DATAMODE_MASK));
    // Disable radio interrupts
    sx1278_write(dev, REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_11 | RF_DIOMAPPING1_DIO1_11);
    sx1278_write(dev, REG_DIOMAPPING2, RF_DIOMAPPING2_DIO4_10 | RF_DIOMAPPING2_DIO5_10);

    dev->settings.State = RF_TX_RUNNING;
    sx1278_set_timeout(dev, TXTimeoutTimer, &on_timeout_irq, timeout);
    sx1278_set_op_mode(dev, RF_OPMODE_TRANSMITTER);
}

//-----------------------------------------------------------------------------
int16_t sx1278_get_rssi(sx1278* const dev, RadioModems_t modem)
{
    int16_t rssi = 0;

    switch (modem)
    {
        case MODEM_FSK:
            rssi = -(sx1278_read(dev, REG_RSSIVALUE) >> 1);
            break;
        case MODEM_LORA:
            if (dev->settings.Channel > RF_MID_BAND_THRESH)
            {
                rssi = RSSI_OFFSET_HF + sx1278_read(dev, REG_LR_RSSIVALUE);
            }
            else
            {
                rssi = RSSI_OFFSET_LF + sx1278_read(dev, REG_LR_RSSIVALUE);
            }
            break;
        default:
            rssi = -1;
            break;
    }
    return rssi;
}

//-----------------------------------------------------------------------------
int32_t sx1278_get_frequency_error(sx1278* const dev, RadioModems_t modem )
{
    int32_t val = 0;

    if (modem != MODEM_LORA)
        return 0;

    val = (sx1278_read(dev, REG_LR_FEIMSB) & 0b1111) << 16; // high word, 4 valid bits only
    val |= ((sx1278_read(dev, REG_LR_FEIMID) << 8) | sx1278_read(dev, REG_LR_FEILSB)); // high byte, low byte
    if (val & 0x80000) //convert sign bit
        val |= 0xfff00000;

    int32_t bandwidth = 0;
    for (int i = 0; i < (int)(sizeof(SX1278LoRaBandwidths) / sizeof(BandwidthMap_t)) -1; i++ ) {
        if (SX1278LoRaBandwidths[i].reg_value == dev->settings.LoRa.Bandwidth) {
            bandwidth = SX1278LoRaBandwidths[i].bandwidth;
            break;
        }
    }
    if (!bandwidth)
    	return 0;

    float bandWidthkHz = (float)bandwidth/1000;

    int32_t hz = (((float)val * (float)(1<<24)) / ((float)XTAL_FREQ)) * (bandWidthkHz / 500.0);

    return hz;
}

//-----------------------------------------------------------------------------
void sx1278_set_op_mode(sx1278* const dev, uint8_t opMode)
{
    // if(opMode == RF_OPMODE_SLEEP ) // TODO NOT USED on RA-01
    // {
    //     SX1278SetAntSwLowPower( true );
    // }
    // else
    // {
    //     // Enable TCXO if operating mode different from SLEEP.
    //     SX1278SetBoardTcxo( true );
    //     SX1278SetAntSwLowPower( false );
    //     SX1278SetAntSw( opMode );
    // }
    sx1278_write(dev, REG_OPMODE, (sx1278_read(dev, REG_OPMODE) & RF_OPMODE_MASK) | opMode);
}

//-----------------------------------------------------------------------------
void sx1278_set_modem(sx1278* const dev, RadioModems_t modem)
{
    if ((sx1278_read(dev, REG_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_ON) != 0)
    {
        dev->settings.modem = MODEM_LORA;
    }
    else
    {
        dev->settings.modem = MODEM_FSK;
    }

    if (dev->settings.modem == modem)
    {
        return;
    }

    dev->settings.modem = modem;
    switch (dev->settings.modem)
    {
        default:
        case MODEM_FSK:
            sx1278_set_sleep(dev);
            sx1278_write(dev, REG_OPMODE, (sx1278_read(dev, REG_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_MASK) | RFLR_OPMODE_LONGRANGEMODE_OFF);

            sx1278_write(dev, REG_DIOMAPPING1, 0x00);
            sx1278_write(dev, REG_DIOMAPPING2, 0x30); // DIO5=ModeSX1278Ready
            break;
        case MODEM_LORA:
            sx1278_set_sleep(dev);
            sx1278_write(dev, REG_OPMODE, (sx1278_read(dev, REG_OPMODE) & RFLR_OPMODE_LONGRANGEMODE_MASK) | RFLR_OPMODE_LONGRANGEMODE_ON);

            sx1278_write(dev, REG_DIOMAPPING1, 0x00);
            sx1278_write(dev, REG_DIOMAPPING2, 0x00);
            break;
    }
}

//-----------------------------------------------------------------------------
void sx1278_set_max_payload_length(sx1278* const dev, RadioModems_t modem, uint8_t max)
{
    sx1278_set_modem(dev, modem);

    switch (modem)
    {
    case MODEM_FSK:
        if (dev->settings.Fsk.FixLen == false)
        {
            sx1278_write(dev, REG_PAYLOADLENGTH, max);
        }
        break;
    case MODEM_LORA:
        sx1278_write(dev, REG_LR_PAYLOADMAXLENGTH, max);
        break;
    }
}

//-----------------------------------------------------------------------------
void sx1278_set_public_network(sx1278* const dev, bool enable)
{
    sx1278_set_modem(dev, MODEM_LORA);
    dev->settings.LoRa.PublicNetwork = enable;
    if (enable == true)
    {
        // Change LoRa modem SyncWord
        sx1278_write(dev, REG_LR_SYNCWORD, LORA_MAC_PUBLIC_SYNCWORD);
    }
    else
    {
        // Change LoRa modem SyncWord
        sx1278_write(dev, REG_LR_SYNCWORD, LORA_MAC_PRIVATE_SYNCWORD);
    }
}

//-----------------------------------------------------------------------------
void on_timeout_irq(sx1278* const dev)
{
    switch (dev->settings.State)
    {
    case RF_RX_RUNNING:
        if (dev->settings.modem == MODEM_FSK)
        {
            dev->settings.FskPacketHandler.PreambleDetected = false;
            dev->settings.FskPacketHandler.SyncWordDetected = false;
            dev->settings.FskPacketHandler.NbBytes = 0;
            dev->settings.FskPacketHandler.Size = 0;

            // Clear Irqs
            sx1278_write(dev, REG_IRQFLAGS1, RF_IRQFLAGS1_RSSI |
                                        RF_IRQFLAGS1_PREAMBLEDETECT |
                                        RF_IRQFLAGS1_SYNCADDRESSMATCH);
            sx1278_write(dev, REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN);

            if (dev->settings.Fsk.RxContinuous == true)
            {
                // Continuous mode restart Rx chain
                sx1278_write(dev, REG_RXCONFIG, sx1278_read(dev, REG_RXCONFIG) | RF_RXCONFIG_RESTARTRXWITHOUTPLLLOCK);
                sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, &on_timeout_irq, dev->settings.Fsk.RxSingleTimeout);
            }
            else
            {
                dev->settings.State = RF_IDLE;
                sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, NULL, 0);
            }
        }
        if ((dev->events != NULL) && (dev->events->RxTimeout != NULL))
        {
            dev->events->RxTimeout();
        }
        break;
    case RF_TX_RUNNING:
        // Tx timeout shouldn't happen.
        // But it has been observed that when it happens it is a result of a corrupted SPI transfer
        // it depends on the platform design.
        //
        // The workaround is to put the radio in a known state. Thus, we re-initialize it.

        // BEGIN WORKAROUND

        // Reset the radio
        sx1278_reset(dev);

        // Calibrate Rx chain
        RxChainCalibration(dev);

        // Initialize radio default values
        sx1278_set_op_mode(dev, RF_OPMODE_SLEEP);
        sx1278_radio_registers_init(dev);

        sx1278_set_modem(dev, MODEM_FSK);

        // Restore previous network type setting.
        sx1278_set_public_network(dev, dev->settings.LoRa.PublicNetwork);
        // END WORKAROUND

        dev->settings.State = RF_IDLE;
        if ((dev->events != NULL) && (dev->events->TxTimeout != NULL))
        {
            dev->events->TxTimeout();
        }
        break;
    default:
        break;
    }
}

//-----------------------------------------------------------------------------
void sx1278_set_rf_tx_power(sx1278* const dev, int8_t power)
{
    uint8_t paConfig = 0;
    uint8_t paDac = 0;

    paConfig = sx1278_read(dev, REG_PACONFIG);
    paDac = sx1278_read(dev, REG_PADAC);

    paConfig = (paConfig & RF_PACONFIG_PASELECT_MASK) | sx1278_get_pa_select(dev, dev->settings.Channel);
    paConfig = (paConfig & RF_PACONFIG_MAX_POWER_MASK) | 0x70;

    if((paConfig & RF_PACONFIG_PASELECT_PABOOST) == RF_PACONFIG_PASELECT_PABOOST)
    {
        if(power > 17)
        {
            paDac = (paDac & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_ON;
        }
        else
        {
            paDac = (paDac & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_OFF;
        }
        if((paDac & RF_PADAC_20DBM_ON) == RF_PADAC_20DBM_ON)
        {
            if(power < 5)
            {
                power = 5;
            }
            if(power > 20)
            {
                power = 20;
            }
            paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | (uint8_t)((uint16_t)(power - 5) & 0x0F);
        }
        else
        {
            if(power < 2)
            {
                power = 2;
            }
            if(power > 17)
            {
                power = 17;
            }
            paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | (uint8_t)((uint16_t)(power - 2) & 0x0F);
        }
    }
    else
    {
        if(power < -1)
        {
            power = -1;
        }
        if(power > 14)
        {
            power = 14;
        }
        paConfig = (paConfig & RF_PACONFIG_OUTPUTPOWER_MASK) | (uint8_t)((uint16_t)(power + 1) & 0x0F);
    }
    sx1278_write(dev, REG_PACONFIG, paConfig);
    sx1278_write(dev, REG_PADAC, paDac);
}

//-----------------------------------------------------------------------------
uint8_t sx1278_get_pa_select(sx1278* const dev, uint32_t channel)
{
    if(channel > RF_MID_BAND_THRESH)
    {
        return RF_PACONFIG_PASELECT_PABOOST;
    }
    else
    {
        return RF_PACONFIG_PASELECT_RFO;
    }
}

//-----------------------------------------------------------------------------
bool sx1278_check_rf_frequency(sx1278* const dev, uint32_t frequency)
{
    // Implement check. Currently all frequencies are supported
    return true;
}

//-----------------------------------------------------------------------------
void sx1278_write(sx1278* const dev, uint8_t addr, uint8_t data)
{
    sx1278_write_buffer(dev, addr, &data, 1);
}

//-----------------------------------------------------------------------------
uint8_t sx1278_read(sx1278* const dev, uint8_t addr)
{
    uint8_t data;
    sx1278_read_buffer(dev, addr, &data, 1);
    return data;
}

//-----------------------------------------------------------------------------
void sx1278_write_fifo(sx1278* const dev, uint8_t* buffer, uint8_t size)
{
    sx1278_write_buffer(dev, 0, buffer, size);
}

//-----------------------------------------------------------------------------
void sx1278_read_fifo(sx1278* const dev, uint8_t* buffer, uint8_t size)
{
    sx1278_read_buffer(dev, 0, buffer, size);
}


//-----------------------------------------------------------------------------



/**
 * ============================================================================
 * @brief Private functions definitions
 * ============================================================================
 */
//-----------------------------------------------------------------------------
void RxChainCalibration(sx1278* const dev)
{
    uint8_t regPaConfigInitVal;
    uint32_t initialFreq;

    // Save context
    regPaConfigInitVal = sx1278_read(dev, REG_PACONFIG);
    initialFreq = (double)(((uint32_t)sx1278_read(dev, REG_FRFMSB) << 16) |
                              ((uint32_t)sx1278_read(dev, REG_FRFMID) << 8) |
                              ((uint32_t)sx1278_read(dev, REG_FRFLSB))) * (double)FREQ_STEP;

    // Cut the PA just in case, RFO output, power = -1 dBm
    sx1278_write(dev, REG_PACONFIG, 0x00);

    // Launch Rx chain calibration for LF band
    sx1278_write(dev, REG_IMAGECAL, (sx1278_read(dev, REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_MASK) | RF_IMAGECAL_IMAGECAL_START);
    while((sx1278_read(dev, REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_RUNNING) == RF_IMAGECAL_IMAGECAL_RUNNING)
    {
    }

    // Sets a Frequency in HF band
    sx1278_set_channel(dev, 868000000);

    // Launch Rx chain calibration for HF band
    sx1278_write(dev, REG_IMAGECAL, (sx1278_read(dev, REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_MASK) | RF_IMAGECAL_IMAGECAL_START);
    while((sx1278_read(dev, REG_IMAGECAL) & RF_IMAGECAL_IMAGECAL_RUNNING) == RF_IMAGECAL_IMAGECAL_RUNNING)
    {
    }

    // Restore context
    sx1278_write(dev, REG_PACONFIG, regPaConfigInitVal);
    sx1278_set_channel(dev, initialFreq);
}

//-----------------------------------------------------------------------------
uint8_t GetFskBandwidthRegValue(uint32_t bandwidth)
{
    uint8_t i;

    for (i = 0; i < (sizeof(SX1278FskBandwidths) / sizeof(BandwidthMap_t)) - 1; i++)
    {
        if ((bandwidth >= SX1278FskBandwidths[i].bandwidth) && (bandwidth < SX1278FskBandwidths[i + 1].bandwidth))
        {
            return SX1278FskBandwidths[i].reg_value;
        }
    }
    // ERROR: Value not found
    while(1);
}

//-----------------------------------------------------------------------------
uint8_t GetLoRaBandwidthRegValue(uint32_t bandwidth)
{
    uint8_t i;

    for (i = 0; i < (sizeof(SX1278LoRaBandwidths) / sizeof(BandwidthMap_t)) - 1; i++)
    {
        if ((bandwidth >= SX1278LoRaBandwidths[i].bandwidth) && (bandwidth < SX1278LoRaBandwidths[i + 1].bandwidth))
        {
            return SX1278LoRaBandwidths[i].reg_value;
        }
    }
    // ERROR: Value not found
    while(1);
}

//-----------------------------------------------------------------------------
ISR_PREFIX void on_dio0_irq(void* ctx)
{
    sx1278* const dev = (sx1278*)ctx;
    volatile uint8_t irqFlags = 0;

    switch (dev->settings.State)
    {
        case RF_RX_RUNNING:
            //TimerStop(&RxTimeoutTimer);
            // RxDone interrupt
            switch (dev->settings.modem)
            {
            case MODEM_FSK:
                if (dev->settings.Fsk.CrcOn == true)
                {
                    irqFlags = sx1278_read(dev, REG_IRQFLAGS2);
                    if ((irqFlags & RF_IRQFLAGS2_CRCOK) != RF_IRQFLAGS2_CRCOK)
                    {
                        // Clear Irqs
                        sx1278_write(dev, REG_IRQFLAGS1, RF_IRQFLAGS1_RSSI |
                                                   RF_IRQFLAGS1_PREAMBLEDETECT |
                                                   RF_IRQFLAGS1_SYNCADDRESSMATCH);
                        sx1278_write(dev, REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN);

                        sx1278_set_timeout(dev, RXTimeoutTimer, NULL, 0);

                        if (dev->settings.Fsk.RxContinuous == false)
                        {
                            sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, NULL, 0);
                            dev->settings.State = RF_IDLE;
                        }
                        else
                        {
                            // Continuous mode restart Rx chain
                            sx1278_write(dev, REG_RXCONFIG, sx1278_read(dev, REG_RXCONFIG) | RF_RXCONFIG_RESTARTRXWITHOUTPLLLOCK);
                            sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, &on_timeout_irq, dev->settings.Fsk.RxSingleTimeout);
                        }

                        if ((dev->events != NULL) && (dev->events->RxError != NULL))
                        {
                            dev->events->RxError();
                        }
                        dev->settings.FskPacketHandler.PreambleDetected = false;
                        dev->settings.FskPacketHandler.SyncWordDetected = false;
                        dev->settings.FskPacketHandler.NbBytes = 0;
                        dev->settings.FskPacketHandler.Size = 0;
                        break;
                    }
                }

                // sx1278_read received packet size
                if ((dev->settings.FskPacketHandler.Size == 0) && (dev->settings.FskPacketHandler.NbBytes == 0))
                {
                    if (dev->settings.Fsk.FixLen == false)
                    {
                        sx1278_read_fifo(dev, (uint8_t*)&dev->settings.FskPacketHandler.Size, 1);
                    }
                    else
                    {
                        dev->settings.FskPacketHandler.Size = sx1278_read(dev, REG_PAYLOADLENGTH);
                    }
                    sx1278_read_fifo(dev, dev->rx_tx_buffer + dev->settings.FskPacketHandler.NbBytes, dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes);
                    dev->settings.FskPacketHandler.NbBytes += (dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes);
                }
                else
                {
                    sx1278_read_fifo(dev, dev->rx_tx_buffer + dev->settings.FskPacketHandler.NbBytes, dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes);
                    dev->settings.FskPacketHandler.NbBytes += (dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes);
                }

                sx1278_set_timeout(dev, RXTimeoutTimer, NULL, 0);

                if (dev->settings.Fsk.RxContinuous == false)
                {
                    dev->settings.State = RF_IDLE;
                    sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, NULL, 0);
                }
                else
                {
                    // Continuous mode restart Rx chain
                    sx1278_write(dev, REG_RXCONFIG, sx1278_read(dev, REG_RXCONFIG) | RF_RXCONFIG_RESTARTRXWITHOUTPLLLOCK);
                    sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, &on_timeout_irq, dev->settings.Fsk.RxSingleTimeout);
                }

                if ((dev->events != NULL) && (dev->events->RxDone != NULL))
                {
                    dev->events->RxDone(dev->rx_tx_buffer, dev->settings.FskPacketHandler.Size, dev->settings.FskPacketHandler.RssiValue, 0);
                }
                dev->settings.FskPacketHandler.PreambleDetected = false;
                dev->settings.FskPacketHandler.SyncWordDetected = false;
                dev->settings.FskPacketHandler.NbBytes = 0;
                dev->settings.FskPacketHandler.Size = 0;
                break;
            case MODEM_LORA:
                {
                    // Clear Irq
                    sx1278_write(dev, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE);

                    irqFlags = sx1278_read(dev, REG_LR_IRQFLAGS);
                    if ((irqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR_MASK) == RFLR_IRQFLAGS_PAYLOADCRCERROR)
                    {
                        // Clear Irq
                        sx1278_write(dev, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR);

                        if (dev->settings.LoRa.RxContinuous == false)
                        {
                            dev->settings.State = RF_IDLE;
                        }
                        sx1278_set_timeout(dev, RXTimeoutTimer, NULL, 0);

                        if ((dev->events != NULL) && (dev->events->RxError != NULL))
                        {
                            dev->events->RxError();
                        }
                        break;
                    }
                    // Returns SNR value [dB] rounded to the nearest integer value
                    dev->settings.LoRaPacketHandler.SnrValue = (((int8_t)sx1278_read(dev, REG_LR_PKTSNRVALUE)) + 2) >> 2;
                    int16_t rssi = sx1278_read(dev, REG_LR_PKTRSSIVALUE);
                    if (dev->settings.LoRaPacketHandler.SnrValue < 0)
                    {
                        if (dev->settings.Channel > RF_MID_BAND_THRESH)
                        {
                            dev->settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_HF + rssi + ( rssi >> 4 ) +
                                                                   dev->settings.LoRaPacketHandler.SnrValue;
                        }
                        else
                        {
                            dev->settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_LF + rssi + ( rssi >> 4 ) +
                                                                   dev->settings.LoRaPacketHandler.SnrValue;
                        }
                    }
                    else
                    {
                        if(dev->settings.Channel > RF_MID_BAND_THRESH)
                        {
                            dev->settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_HF + rssi + ( rssi >> 4 );
                        }
                        else
                        {
                            dev->settings.LoRaPacketHandler.RssiValue = RSSI_OFFSET_LF + rssi + ( rssi >> 4 );
                        }
                    }

                    dev->settings.LoRaPacketHandler.Size = sx1278_read(dev,  REG_LR_RXNBBYTES);
                    sx1278_write(dev,  REG_LR_FIFOADDRPTR, sx1278_read(dev,  REG_LR_FIFORXCURRENTADDR));
                    sx1278_read_fifo(dev, dev->rx_tx_buffer, dev->settings.LoRaPacketHandler.Size);

                    if(dev->settings.LoRa.RxContinuous == false)
                    {
                        dev->settings.State = RF_IDLE;
                    }

                    sx1278_set_timeout(dev, RXTimeoutTimer, NULL, 0);

                    if ((dev->events != NULL) && (dev->events->RxDone != NULL))
                    {
                        dev->events->RxDone(dev->rx_tx_buffer, dev->settings.LoRaPacketHandler.Size, dev->settings.LoRaPacketHandler.RssiValue, dev->settings.LoRaPacketHandler.SnrValue);
                    }
                }
                break;
            default:
                break;
            }
            break;
        case RF_TX_RUNNING:
            sx1278_set_timeout(dev, TXTimeoutTimer, NULL, 0);
            // TxDone interrupt
            switch (dev->settings.modem)
            {
            case MODEM_LORA:
                // Clear Irq
                sx1278_write(dev, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE);
                // Intentional fall through
            case MODEM_FSK:
            default:
                dev->settings.State = RF_IDLE;
                if ((dev->events != NULL) && (dev->events->TxDone != NULL))
                {
                    dev->events->TxDone();
                }
                break;
            }
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
ISR_PREFIX void on_dio1_irq(void* ctx)
{
    sx1278* const dev = (sx1278*)ctx;
    switch (dev->settings.State)
    {
        case RF_RX_RUNNING:
            switch (dev->settings.modem)
            {
                case MODEM_FSK:
                    // Stop Timer
                    sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, NULL, 0);

                    // FifoLevel interrupt
                    // sx1278_read received packet size
                    if ((dev->settings.FskPacketHandler.Size == 0) && (dev->settings.FskPacketHandler.NbBytes == 0))
                    {
                        if (dev->settings.Fsk.FixLen == false)
                        {
                            sx1278_read_fifo(dev, (uint8_t*)&dev->settings.FskPacketHandler.Size, 1);
                        }
                        else
                        {
                            dev->settings.FskPacketHandler.Size = sx1278_read(dev, REG_PAYLOADLENGTH);
                        }
                    }
                    // ERRATA 3.1 - PayloadReady Set for 31.25ns if FIFO is Empty
                    //
                    //              When FifoLevel interrupt is used to offload the
                    //              FIFO, the microcontroller should  monitor  both
                    //              PayloadReady  and FifoLevel interrupts, and
                    //              read only (FifoThreshold-1) bytes off the FIFO
                    //              when FifoLevel fires
                    if ((dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes) > dev->settings.FskPacketHandler.FifoThresh)
                    {
                        sx1278_read_fifo(dev, (dev->rx_tx_buffer + dev->settings.FskPacketHandler.NbBytes), dev->settings.FskPacketHandler.FifoThresh);
                        dev->settings.FskPacketHandler.NbBytes += dev->settings.FskPacketHandler.FifoThresh;
                    }
                    else
                    {
                        sx1278_read_fifo(dev, (dev->rx_tx_buffer + dev->settings.FskPacketHandler.NbBytes), dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes);
                        dev->settings.FskPacketHandler.NbBytes += (dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes);
                    }
                    break;
                case MODEM_LORA:
                    // Sync time out
                    sx1278_set_timeout(dev, RXTimeoutTimer, NULL, 0);
                    // Clear Irq
                    sx1278_write(dev, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXTIMEOUT);

                    dev->settings.State = RF_IDLE;
                    if ((dev->events != NULL) && (dev->events->RxTimeout != NULL))
                    {
                        dev->events->RxTimeout();
                    }
                    break;
                default:
                    break;
                }
                break;
            case RF_TX_RUNNING:
                switch (dev->settings.modem)
                {
                case MODEM_FSK:
                    // FifoEmpty interrupt
                    if ((dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes) > dev->settings.FskPacketHandler.ChunkSize)
                    {
                        sx1278_write_fifo(dev, (dev->rx_tx_buffer + dev->settings.FskPacketHandler.NbBytes), dev->settings.FskPacketHandler.ChunkSize);
                        dev->settings.FskPacketHandler.NbBytes += dev->settings.FskPacketHandler.ChunkSize;
                    }
                    else
                    {
                        // sx1278_write the last chunk of data
                        sx1278_write_fifo(dev, dev->rx_tx_buffer + dev->settings.FskPacketHandler.NbBytes, dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes);
                        dev->settings.FskPacketHandler.NbBytes += dev->settings.FskPacketHandler.Size - dev->settings.FskPacketHandler.NbBytes;
                    }
                    break;
                case MODEM_LORA:
                    break;
                default:
                    break;
                }
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
ISR_PREFIX void on_dio2_irq(void* ctx)
{
    sx1278* const dev = (sx1278*)ctx;
    switch (dev->settings.State)
    {
        case RF_RX_RUNNING:
            switch (dev->settings.modem)
            {
                case MODEM_FSK:
                    // Checks if DIO4 is connected. If it is not PreambleDtected is set to true.
                    if (dev->dio_irq[4] == NULL)
                    {
                        dev->settings.FskPacketHandler.PreambleDetected = true;
                    }

                    if ((dev->settings.FskPacketHandler.PreambleDetected == true) && (dev->settings.FskPacketHandler.SyncWordDetected == false))
                    {
                        sx1278_set_timeout(dev, RXTimeoutSyncWordTimer, NULL, 0);

                        dev->settings.FskPacketHandler.SyncWordDetected = true;

                        dev->settings.FskPacketHandler.RssiValue = -(sx1278_read(dev, REG_RSSIVALUE) >> 1);

                        dev->settings.FskPacketHandler.AfcValue = (int32_t)(double)(((uint16_t)sx1278_read(dev, REG_AFCMSB) << 8) |
                                                                            (uint16_t)sx1278_read(dev, REG_AFCLSB)) *
                                                                            (double)FREQ_STEP;
                        dev->settings.FskPacketHandler.RxGain = (sx1278_read(dev, REG_LNA) >> 5) & 0x07;
                    }
                    break;
                case MODEM_LORA:
                    if (dev->settings.LoRa.FreqHopOn == true)
                    {
                        // Clear Irq
                        sx1278_write(dev, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL);

                        if ((dev->events != NULL) && (dev->events->FhssChangeChannel != NULL))
                        {
                            dev->events->FhssChangeChannel((sx1278_read(dev, REG_LR_HOPCHANNEL) & RFLR_HOPCHANNEL_CHANNEL_MASK));
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case RF_TX_RUNNING:
            switch (dev->settings.modem)
            {
                case MODEM_FSK:
                    break;
                case MODEM_LORA:
                    if (dev->settings.LoRa.FreqHopOn == true)
                    {
                        // Clear Irq
                        sx1278_write(dev, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL);

                        if ((dev->events != NULL) && (dev->events->FhssChangeChannel != NULL))
                        {
                            dev->events->FhssChangeChannel((sx1278_read(dev, REG_LR_HOPCHANNEL) & RFLR_HOPCHANNEL_CHANNEL_MASK));
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
ISR_PREFIX void on_dio3_irq(void* ctx)
{
    sx1278* const dev = (sx1278*)ctx;
    switch (dev->settings.modem)
    {
        case MODEM_FSK:
            break;
        case MODEM_LORA:
            if ((sx1278_read(dev, REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_CADDETECTED) == RFLR_IRQFLAGS_CADDETECTED)
            {
                // Clear Irq
                sx1278_write(dev, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED | RFLR_IRQFLAGS_CADDONE);
                if ((dev->events != NULL) && (dev->events->CadDone != NULL))
                {
                    dev->events->CadDone(true);
                }
            }
            else
            {
                // Clear Irq
                sx1278_write(dev, REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE);
                if ((dev->events != NULL) && (dev->events->CadDone != NULL))
                {
                    dev->events->CadDone(false);
                }
            }
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
ISR_PREFIX void on_dio4_irq(void* ctx)
{
    sx1278* const dev = (sx1278*)ctx;
    switch (dev->settings.modem)
    {
        case MODEM_FSK:
        {
            if (dev->settings.FskPacketHandler.PreambleDetected == false)
            {
                dev->settings.FskPacketHandler.PreambleDetected = true;
            }
            break;
        }
        case MODEM_LORA:
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
ISR_PREFIX void on_dio5_irq(void* ctx)
{
    sx1278* const dev = (sx1278*)ctx;
    switch (dev->settings.modem)
    {
    case MODEM_FSK:
        break;
    case MODEM_LORA:
        break;
    default:
        break;
    }
}

//-----------------------------------------------------------------------------