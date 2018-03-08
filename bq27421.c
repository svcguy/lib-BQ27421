#include "bq27421.h"
#include "i2c.h"

bool bq27421_i2c_command_write( uint8_t command, uint16_t data )
{
    HAL_StatusTypeDef ret;
    uint8_t i2c_data[3];

    i2c_data[0] = command;
    i2c_data[1] = ( uint8_t )( data & 0x00FF );
    i2c_data[2] = ( uint8_t )( ( data >> 8 ) & 0x00FF );

    ret = HAL_I2C_Master_Transmit( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, i2c_data, 3, HAL_BQ27421_TIMEOUT );
    if( ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( BQ27421_DELAY );
    
    return true;
}

bool bq27421_i2c_command_read( uint8_t command, uint16_t *data )
{
    HAL_StatusTypeDef ret;
    uint8_t i2c_data[2];
    
    ret = HAL_I2C_Master_Transmit( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, &command, 1, HAL_BQ27421_TIMEOUT );
    if( ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( BQ27421_DELAY );

    ret = HAL_I2C_Master_Receive( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, i2c_data, 2, HAL_BQ27421_TIMEOUT );
    if(ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( BQ27421_DELAY );

    *data = ( i2c_data[1] << 8 ) | i2c_data[0];

    return true;
}

bool bq27421_i2c_control_write( uint16_t subcommand )
{
    HAL_StatusTypeDef ret;
    uint8_t i2c_data[2];

    i2c_data[0] = BQ27421_CONTROL_LOW;
    i2c_data[1] = (uint8_t)( ( subcommand ) & 0x00FF );
    

    ret = HAL_I2C_Master_Transmit( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, i2c_data, 2, HAL_BQ27421_TIMEOUT );
    if( ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( BQ27421_DELAY );

    i2c_data[0] = BQ27421_CONTROL_HIGH;
    i2c_data[1] = (uint8_t)( ( subcommand >> 8 ) & 0x00FF );

    ret = HAL_I2C_Master_Transmit( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, i2c_data, 2, HAL_BQ27421_TIMEOUT );
    if( ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( BQ27421_DELAY );

    return true;
}

bool bq27421_i2c_control_read( uint16_t subcommand, uint16_t *data )
{
    HAL_StatusTypeDef ret;
    uint8_t i2c_data[2];

    i2c_data[0] = BQ27421_CONTROL_LOW;
    i2c_data[1] = (uint8_t)( ( subcommand ) & 0x00FF );

    ret = HAL_I2C_Master_Transmit( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, i2c_data, 2, HAL_BQ27421_TIMEOUT );
    if( ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( BQ27421_DELAY );

    i2c_data[0] = BQ27421_CONTROL_HIGH;
    i2c_data[1] = (uint8_t)( ( subcommand >> 8 ) & 0x00FF );

    ret = HAL_I2C_Master_Transmit( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, i2c_data, 2, HAL_BQ27421_TIMEOUT );
    if( ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( BQ27421_DELAY );

    ret = HAL_I2C_Master_Receive( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, i2c_data, 2, HAL_BQ27421_TIMEOUT );
    if(ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( BQ27421_DELAY );

    *data = ( i2c_data[1] << 8 ) | i2c_data[0];

    return true;
}

bool bq27421_i2c_write_data_block( uint8_t offset, uint8_t *data, uint8_t bytes )
{
    HAL_StatusTypeDef ret;
    uint8_t i2c_data[2], i;

    for( i = 0; i < bytes; i++ )
    {
        i2c_data[0] = BQ27421_BLOCK_DATA_START + offset + i;
        i2c_data[1] = data[i];

        ret = HAL_I2C_Master_Transmit( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, i2c_data, 2, HAL_BQ27421_TIMEOUT );
        if( ret != HAL_OK )
        {
            return false;
        }
        HAL_Delay( BQ27421_DELAY );
    }

    return true;
}

bool bq27421_i2c_read_data_block( uint8_t offset, uint8_t *data, uint8_t bytes )
{
    HAL_StatusTypeDef ret;

    uint8_t i2c_data;

    i2c_data = BQ27421_BLOCK_DATA_START + offset;
 
    ret = HAL_I2C_Master_Transmit( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, &i2c_data, 1, HAL_BQ27421_TIMEOUT );
    if( ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( 5 );

    ret = HAL_I2C_Master_Receive( &HAL_I2C_INSTANCE, (uint16_t)BQ27421_I2C_ADDRESS, data, bytes, HAL_BQ27421_TIMEOUT );
    if( ret != HAL_OK )
    {
        return false;
    }

    HAL_Delay( BQ27421_DELAY );

    return true;
}

bool bq27421_init( uint16_t designCapacity_mAh, uint16_t terminateVoltage_mV, uint16_t taperCurrent_mA )
{
    uint16_t designEnergy_mWh, taperRate, flags, checksumOld, checksumRead;
    uint8_t checksumNew;
    

    designEnergy_mWh = 3.7 * designCapacity_mAh;
    taperRate = designCapacity_mAh / ( 0.1 * taperCurrent_mA );

    // Unseal gauge
    bq27421_i2c_control_write( BQ27421_CONTROL_UNSEAL );
    bq27421_i2c_control_write( BQ27421_CONTROL_UNSEAL );

    // Send CFG_UPDATE
    bq27421_i2c_control_write( BQ27421_CONTROL_SET_CFGUPDATE );

    // Poll flags
    do
    {
        bq27421_i2c_command_read( BQ27421_FLAGS_LOW, &flags );
        if( !(flags & 0x0010) )
        {
            HAL_Delay( 50 );
        }
    }
    while( !(flags & 0x0010) );

    // Enable Block Data Memory Control
    bq27421_i2c_command_write( BQ27421_BLOCK_DATA_CONTROL, 0x0000 );

    HAL_Delay( BQ27421_DELAY );
    
    // Access State subclass
    bq27421_i2c_command_write( BQ27421_DATA_CLASS, 0x0052 );

    // Write the block offset
    bq27421_i2c_command_write( BQ27421_DATA_BLOCK, 0x0000 );

    // Read block checksum
    bq27421_i2c_command_read( BQ27421_BLOCK_DATA_CHECKSUM, &checksumOld );

    // Read 32-byte block of data
    uint8_t block[32];
    for(uint8_t i = 0; i < 32; i++ )
    {
        block[i] = 0x00;
    }

    bq27421_i2c_read_data_block( 0x00, block, 32 );

    // Calculate checksum
    uint8_t checksumCalc = 0x00;

    for(uint8_t i = 0; i < 32; i++ )
    {
        checksumCalc += block[i];
    }
    checksumCalc = 0xFF - checksumCalc;

    // Update design capacity
    block[10] = (uint8_t)( designCapacity_mAh >> 8 );
    block[11] = (uint8_t)( designCapacity_mAh & 0x00FF );
    // Update design energy
    block[12] = (uint8_t)( designEnergy_mWh >> 8 );
    block[13] = (uint8_t)( designEnergy_mWh & 0x00FF );
    // Update terminate voltage
    block[16] = (uint8_t)( terminateVoltage_mV >> 8 );
    block[17] = (uint8_t)( terminateVoltage_mV & 0x00FF );
    // Update taper rate
    block[27] = (uint8_t)( taperRate >> 8 );
    block[28] = (uint8_t)( taperRate & 0x00FF );

    // Calculate new checksum
    checksumNew = 0x00;
    for(int i = 0; i < 32; i++ )
    {
        checksumNew += block[i];
    }
    checksumNew = 0xFF - checksumNew;

    // Enable Block Data Memory Control
    bq27421_i2c_command_write( BQ27421_BLOCK_DATA_CONTROL, 0x0000 );

    HAL_Delay( BQ27421_DELAY );
    
    // Access State subclass
    bq27421_i2c_command_write( BQ27421_DATA_CLASS, 0x0052 );

    // Write the block offset
    bq27421_i2c_command_write( BQ27421_DATA_BLOCK, 0x0000 );

    // Write 32-byte block of updated data
    bq27421_i2c_write_data_block( 0x00, block, 32 );

    // Write new checksum
    bq27421_i2c_command_write( BQ27421_BLOCK_DATA_CHECKSUM, checksumNew );

    // Access State subclass
    bq27421_i2c_command_write( BQ27421_DATA_CLASS, 0x0052 );

    // Write the block offset
    bq27421_i2c_command_write( BQ27421_DATA_BLOCK, 0x0000 );

    // Read block checksum
    bq27421_i2c_command_read( BQ27421_BLOCK_DATA_CHECKSUM, &checksumRead );

    // Verify
    if( checksumRead != (uint8_t)checksumNew )
    {
        return false;
    }

    // Enable Block Data Memory Control
    bq27421_i2c_command_write( BQ27421_BLOCK_DATA_CONTROL, 0x0000 );

    HAL_Delay( BQ27421_DELAY );
    
    // Access Registers subclass
    bq27421_i2c_command_write( BQ27421_DATA_CLASS, 0x0040 );

    // Write the block offset
    bq27421_i2c_command_write( BQ27421_DATA_BLOCK, 0x0000 );

    // Read block checksum
    bq27421_i2c_command_read( BQ27421_BLOCK_DATA_CHECKSUM, &checksumOld );

    // Read 32-byte block of data
    for(uint8_t i = 0; i < 32; i++ )
    {
        block[i] = 0x00;
    }

    bq27421_i2c_read_data_block( 0x00, block, 32 );

    // Calculate checksum
    checksumCalc = 0x00;

    for(uint8_t i = 0; i < 32; i++ )
    {
        checksumCalc += block[i];
    }
    checksumCalc = 0xFF - checksumCalc;

    // Update OpConfig
    block[0] = 0x05;

    // Calculate new checksum
    checksumNew = 0x00;
    for(int i = 0; i < 32; i++ )
    {
        checksumNew += block[i];
    }
    checksumNew = 0xFF - checksumNew;

    // Enable Block Data Memory Control
    bq27421_i2c_command_write( BQ27421_BLOCK_DATA_CONTROL, 0x0000 );

    HAL_Delay( BQ27421_DELAY );
    
    // Access Registers subclass
    bq27421_i2c_command_write( BQ27421_DATA_CLASS, 0x0040 );

    // Write the block offset
    bq27421_i2c_command_write( BQ27421_DATA_BLOCK, 0x0000 );

    // Write 32-byte block of updated data
    bq27421_i2c_write_data_block( 0x00, block, 32 );

    // Write new checksum
    bq27421_i2c_command_write( BQ27421_BLOCK_DATA_CHECKSUM, checksumNew );

    // Access Registers subclass
    bq27421_i2c_command_write( BQ27421_DATA_CLASS, 0x0040 );

    // Write the block offset
    bq27421_i2c_command_write( BQ27421_DATA_BLOCK, 0x0000 );

    // Read block checksum
    bq27421_i2c_command_read( BQ27421_BLOCK_DATA_CHECKSUM, &checksumRead );

    // Verify
    if( checksumRead != (uint8_t)checksumNew )
    {
        return false;
    }

    // Configure BAT_DET
    bq27421_i2c_control_write( BQ27421_CONTROL_BAT_INSERT );

    // Send Soft Reset
    bq27421_i2c_control_write( BQ27421_CONTROL_SOFT_RESET );

    // Poll flags
    do
    {
        bq27421_i2c_command_read( BQ27421_FLAGS_LOW, &flags );
        if( !(flags & 0x0010) )
        {
            HAL_Delay( 50 );
        }
    }
    while( (flags & 0x0010) );

    // Seal gauge
    bq27421_i2c_control_write( BQ27421_CONTROL_SEALED );

    return true;
}

bool bq27421_update( bq27421_info *battery )
{
    uint16_t temp;

    if( !bq27421_readVoltage_mV( &(battery->voltage_mV) ) )
    {
        return false;
    }
    if( !bq27421_readAvgCurrent_mA( &(battery->current_mA) ) )
    {
        return false;
    }
    if( !bq27421_readTemp_degK( &temp ) )
    {
        return false;
    }
    battery->temp_degC = ( (double)temp / 10 ) - 273.15;

    if( !bq27421_readStateofCharge_percent( &(battery->soc_percent) ) )
    {
        return false;
    }
    if( !bq27421_readStateofHealth_percent( &(battery->soh_percent) ) )
    {
        return false;
    }
    if( !bq27421_readDesignCapacity_mAh( &(battery->designCapacity_mAh) ) )
    {
        return false;
    }
    if( !bq27421_readRemainingCapacity_mAh( &(battery->remainingCapacity_mAh) ) )
    {
        return false;
    }
    if( !bq27421_readFullChargeCapacity_mAh( &(battery->fullChargeCapacity_mAh) ) )
    {
        return false;
    }
    if( !bq27421_readFlagsReg( &temp ) )
    {
        return false;
    }
    battery->isCritical = temp & 0x0002;
    battery->isLow = temp & 0x0004;
    battery->isFull = temp & 0x0200;
    if( battery->current_mA <= 0 )
    {
        battery->isDischarging = 1;
        battery->isCharging = 0;
    }
    else
    {
        battery->isDischarging = 0;
        battery->isCharging = 1;
    }

    return true;
}

bool bq27421_readDeviceType( uint16_t *deviceType )
{
    if( !bq27421_i2c_control_write( BQ27421_CONTROL_DEVICE_TYPE ) )
    {
        return false;
    }
    if( !bq27421_i2c_command_read( BQ27421_CONTROL_LOW, deviceType ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readDeviceFWver( uint16_t *deviceFWver )
{
    if( !bq27421_i2c_control_write( BQ27421_CONTROL_FW_VERSION ) )
    {
        return false;
    }
    if( !bq27421_i2c_command_read( BQ27421_CONTROL_LOW, deviceFWver ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readDesignCapacity_mAh( uint16_t *capacity_mAh )
{
    if( !bq27421_i2c_command_read( BQ27421_DESIGN_CAP_LOW, capacity_mAh ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readVoltage_mV( uint16_t *voltage_mV )
{
    if( !bq27421_i2c_command_read( BQ27421_VOLTAGE_LOW, voltage_mV ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readTemp_degK( uint16_t *temp_degKbyTen )
{
    if( !bq27421_i2c_command_read( BQ27421_TEMP_LOW, temp_degKbyTen ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readAvgCurrent_mA( int16_t *avgCurrent_mA )
{
    if( !bq27421_i2c_command_read( BQ27421_AVG_CURRENT_LOW, (uint16_t *)avgCurrent_mA ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readStateofCharge_percent( uint16_t *soc_percent )
{
    if( !bq27421_i2c_command_read( BQ27421_STATE_OF_CHARGE_LOW, soc_percent ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readControlReg( uint16_t *control )
{
    if( !bq27421_i2c_control_write( BQ27421_CONTROL_STATUS ) )
    {
        return false;
    }
    if( !bq27421_i2c_command_read( BQ27421_CONTROL_LOW, control ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readFlagsReg( uint16_t *flags )
{
    if( !bq27421_i2c_command_read( BQ27421_FLAGS_LOW, flags ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readopConfig( uint16_t *opConfig )
{
    if( !bq27421_i2c_command_read( BQ27421_OPCONFIG_LOW, opConfig ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readRemainingCapacity_mAh( uint16_t *capacity_mAh )
{
    if( !bq27421_i2c_command_read( BQ27421_REMAINING_CAP_LOW, capacity_mAh ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readFullChargeCapacity_mAh( uint16_t *capacity_mAh )
{
    if( !bq27421_i2c_command_read( BQ27421_FULL_CHARGE_CAP_LOW, capacity_mAh ) )
    {
        return false;
    }

    return true;
}

bool bq27421_readStateofHealth_percent( uint16_t *soh_percent )
{
    if( !bq27421_i2c_command_read( BQ27421_STATE_OF_HEALTH_LOW, soh_percent ) )
    {
        return false;
    }

    *soh_percent = *soh_percent & 0x00FF;

    return true;
}