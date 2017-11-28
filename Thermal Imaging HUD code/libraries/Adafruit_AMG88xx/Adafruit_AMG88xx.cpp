#include "Adafruit_AMG88xx.h"

bool Adafruit_AMG88xx::begin(uint8_t addr)
{
	_i2caddr = addr;
	
	_i2c_init();
	
	//enter normal mode
	_pctl.PCTL = AMG88xx_NORMAL_MODE;
	write8(AMG88xx_PCTL, _pctl.get());
	
	//software reset
	_rst.RST = AMG88xx_INITIAL_RESET;
	write8(AMG88xx_RST, _rst.get());
	
	//disable interrupts by default
	disableInterrupt();
	
	//set to 10 FPS
	_fpsc.FPS = AMG88xx_FPS_10;
	write8(AMG88xx_FPSC, _fpsc.get());

	return true;
}

void Adafruit_AMG88xx::disableInterrupt()
{
	_intc.INTEN = 0;
	this->write8(AMG88xx_INTC, _intc.get());
}

//void Adafruit_AMG88xx::readPixels(int8_t *buf)
//{
	//uint16_t recast;
	//float converted;
	//uint8_t rawArray[bytesToRead];
	//this->read(AMG88xx_PIXEL_OFFSET, rawArray, bytesToRead);
	
	//for(int i=0; i<AMG88xx_PIXEL_ARRAY_SIZE; i++){
		//uint8_t pos = i << 1;
		//recast = ((uint16_t)rawArray[pos + 1] << 8) | ((uint16_t)rawArray[pos]);
		
		//converted = signedMag12ToFloat(recast) * AMG88xx_PIXEL_TEMP_CONVERSION;
		//buf[i] = (int8_t) converted;
	//}
//}

void Adafruit_AMG88xx::readPixels(int8_t *buf)
{
	this->read(AMG88xx_PIXEL_OFFSET, rawArray, bytesToRead);
	
	for(int i=0; i<AMG88xx_PIXEL_ARRAY_SIZE; i++){
		uint8_t pos = i << 1;
		recast = (((uint16_t)rawArray[pos + 1] << 8) | ((uint16_t)rawArray[pos])) >> 2;
		if (recast > MAXTEMP) recast = MAXTEMP;
		else if (recast < MINTEMP) recast = MINTEMP;
		
		buf[i] = 
	}
}

void Adafruit_AMG88xx::write8(byte reg, byte value)
{
	this->write(reg, &value, 1);
}

uint8_t Adafruit_AMG88xx::read8(byte reg)
{
	uint8_t ret;
	this->read(reg, &ret, 1);
	
	return ret;
}

void Adafruit_AMG88xx::_i2c_init()
{
	Wire.begin();
}

void Adafruit_AMG88xx::read(uint8_t reg, uint8_t *buf, uint8_t num)
{
	uint8_t value;
	uint8_t pos = 0;
	
	//on arduino we need to read in 32 byte chunks
	while(pos < num){
		
		uint8_t read_now = min(32, num - pos);
		Wire.beginTransmission((uint8_t)_i2caddr);
		Wire.write((uint8_t)reg + pos);
		Wire.endTransmission();
		Wire.requestFrom((uint8_t)_i2caddr, read_now);
		
		for(int i=0; i<read_now; i++){
			buf[pos] = Wire.read();
			pos++;
		}
	}
}

void Adafruit_AMG88xx::write(uint8_t reg, uint8_t *buf, uint8_t num)
{
	Wire.beginTransmission((uint8_t)_i2caddr);
	Wire.write((uint8_t)reg);
	Wire.write((uint8_t *)buf, num);
	Wire.endTransmission();
}

