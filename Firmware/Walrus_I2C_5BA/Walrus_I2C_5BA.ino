//Dyson_Driver_ShortWave.ino
//v0.0.0

// #define SDA_PIN 3
// #define SDA_PORT portOutputRegister(1)
// #define SCL_PIN 2
// #define SCL_PORT portOutputRegister(1)


// #include "SoftI2CMaster.h"
// #include "WireS.h"
// #include <SoftWire.h>
// #include "SoftwareI2C.h"
#include <SlowSoftWire.h>
#include <Wire.h>
#include <EEPROM.h>
// #include <EEPROM.h> //DEBUG!
//Commands

#define CMD_RESET 0x1E // reset command
#define CMD_ADC_READ 0x00 // ADC read command
#define CMD_ADC_CONV 0x40 // ADC conversion command

#define CMD_PROM 0xA0 // Coefficient location

// //COEFS for MS5803_05BA
#define COEF0 18
#define COEF1 5
#define COEF2 17
#define COEF3 7
#define COEF4 10000
#define COEF5 3
#define COEF6 33
#define COEF7 3
#define COEF8 3
#define COEF9 7
#define COEF10 3
#define COEF11 0
#define COEF12 3
#define COEF13 0
#define COEF14 0
#define COEF15 0

//COEFS for MS5803_02BA
// #define COEF0 17
// #define COEF1 6
// #define COEF2 16
// #define COEF3 7
// #define COEF4 10000
// #define COEF5 1
// #define COEF6 31
// #define COEF7 61
// #define COEF8 4
// #define COEF9 2
// #define COEF10 0
// #define COEF11 20
// #define COEF12 12
// #define COEF13 0
// #define COEF14 0
// #define COEF15 0

//COEFS for MS5803_14BA
// #define COEF0 16
// #define COEF1 7
// #define COEF2 15
// #define COEF3 8
// #define COEF4 1000
// #define COEF5 3
// #define COEF6 33
// #define COEF7 3
// #define COEF8 1
// #define COEF9 5
// #define COEF10 3
// #define COEF11 7
// #define COEF12 4
// #define COEF13 7
// #define COEF14 37
// #define COEF15 1

#define CTRL 0x46  //Define location of onboard control/confiuration register (Schema 1 Page 2 Config byte; was 0x00, which is now the Page 0 schema byte)

//Firmware patch version: bump on any behavioural change visible to the
//library. The hardware version lives in Page 0 (EEPROM), written at
//provisioning; the firmware writes this constant into the served copy of
//Page 0 at 0x0A and recomputes the CRC there (NW-Device-Specification).
#define FW_FW_PATCH 1

//The stored pages are the top 64 bytes of EEPROM (0xC0-0xFF on the
//ATtiny1634): Page 0 (identity) at 0xC0-0xDF, written once by NW-Provision
//and read at boot; Page 1 (calibration, none on Walrus) at 0xE0-0xFF, unused.
#define PAGE0_BASE   (E2END + 1 - 64)
#define REG_I2C_ADDR 0x1F
#define ADR_DEFAULT  0x57  //Schema 1 'W'; used when Page 0 byte 0x1F is 0xFF

//Page 2 Block 0 (NW-Device-Specification): universal status and control.
#define REG_STATUS   0x40
#define REG_CTRL     0x41
#define REG_COUNTER  0x42
#define REG_REQUEST  0x44  //Readings requested, uint16 LE, writable; Walrus has no chip power to hold, so it only accepts the write
#define REG_REPORT   0x47
#define BIT_READY    0x01
#define BIT_PANFAULT 0x80
#define BIT_TRIGGER  0x01
#define CHIP_MS5803  0x02  //Control chip-select bit and status fault bit: chip 0
#define CHIP_MCP9808 0x04  //chip 1
#define BIT_SLEEP    0x80
#define FAULT_MS5803_NOACK  0x01  //chip 0, kind 1
#define FAULT_MCP9808_NOACK 0x21  //chip 1, kind 1
#define NOTICE_UNIT_RESET    0xE6  //unit (7), kind 6: reset since the controller last wrote Control (a notice: no status bit)
#define NOTICE_UNIT_PAGE0    0xE3  //unit (7), kind 3: Page 0 CRC did not match (unprovisioned or corrupt)

const uint8_t PresADR = 0x77;
// const uint8_t TempADR = 0x18; 
const uint8_t TempADR = 0x18; 



// #define CONF_CMD 0x00
// #define ALS_CMD 0x04
// #define WHITE_CMD 0x05

// #define UVA_CMD 0x07
// #define UVB_CMD 0x09
// #define COMP1_CMD 0x0A
// #define COMP2_CMD 0x0B

// #define ADC_CONF 0x01
// #define ADC_CONV 0x00
// #define ADC0 0x4200
// #define ADC1 0x5200
// #define ADC2 0x6200
// #define ADC3 0x7200

// #define VIS_ADR 0x48
// #define UV_ADR 0x10
// #define ADC_ADR 0x49

#define READ 0x01
#define WRITE 0x00

// #define BUF_LENGTH 64 //Length of I2C Buffer, verify with documentation 

// #define LOW_LIM_VIS 10000  //Lower limit to the auto ranging of the VEML6030
// #define HIGH_LIM_VIS 55000 //Upper limit to the auto ranging of the VEML6030

// #define ADR_SEL_PIN 7 //Digital pin 7 is used to test which device address should be used

// #define MODEL 0x5702
// #define GROUPID 0x1701 //Default for lab
// #define INDID 0x0000 //Dummy
// #define FIRMWAREID 0x0001 //Base firmware ID


uint16_t coefficient[8];// Coefficients;

int32_t _temperature_actual;
int32_t _pressure_actual;

float Pressure = 0; // MS5803 pressure
float Temp0 = 0; // Global tempurature from thermistor
float Temp1 = 0; // Global tempurature MS5803
// #define ADR_ALT 0x41 //Alternative device address

bool ms5803Fail = false; //MS5803 did not acknowledge during the last reading
bool mcp9808Fail = false; //MCP9808 did not acknowledge during the last reading
uint8_t StatusReg = 0; //Register to be used to display the status of the sub modules , Bits 0~1 used for status of ADC, bits 2~3 used for MS5803 status

// //Global values for gain and int time of visable light sensor
// uint8_t Gain = 0;
// unsigned int IntTime = 0;
// uint8_t GainValsVis[4] = {0b10, 0b11, 0b00, 0b01}; //Gain values for visible sensor
// uint8_t GainsVis[4] = {1, 2, 8, 16}; //Gain multipliers for visable sensor 
// uint8_t IntTimeValsVis[6] = {0b1100, 0b1000, 0b0000, 0b0001, 0b0010, 0b0011}; //Integration time values for visible sensor

// //Compensation constants
// float a = 1.92;
// float b = 0.55;
// float c = 2.46;
// float d = 0.63;

const uint8_t ModeSelPin = 2; //Pin to select between I2C and RS-485

volatile uint8_t ADR = ADR_DEFAULT; //I2C address: Page 0 byte 0x1F (EEPROM), or ADR_DEFAULT if unprogrammed. Schema 1: 'W' (ASCII mnemonic); former 0x4D clashed with Margay

uint8_t Config = 0; //Global config value

uint8_t Reg[96] = {0}; //Initialize registers; 0x00–0x1F = Page 0 (identity), 0x20–0x3F = Page 1 (calibration: none on Walrus; served from EEPROM as stored), 0x40–0x47 = Page 2 Block 0 (status/control), 0x48–0x5F = Page 2 sensor data
#define DATA_BASE 0x48 //First sensor data register (Page 2 Block 1)
#define DATA_LEN  10   //0x48–0x51: the bytes a reading writes
uint8_t Staged[DATA_LEN] = {0}; //A reading is assembled here and copied into Reg with the counter, so a page read never sees half a reading (spec: atomic rewrite)
bool page0Valid = false; //Page 0 CRC matched what NW-Provision wrote

//CRC-8/SMBUS (poly 0x07, init 0x00), the NW-Device-Specification reference.
uint8_t crc8(const uint8_t* data, uint8_t len) {
  uint8_t crc = 0x00;
  for(uint8_t i = 0; i < len; i++) {
    crc ^= data[i];
    for(uint8_t b = 0; b < 8; b++) crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
  }
  return crc;
}

//Copy Page 0 from EEPROM into the served register array, check its CRC,
//then substitute this firmware's patch version at 0x0A and recompute the
//CRC of the served copy (EEPROM is left as provisioned).
void loadPage0() {
  for(uint8_t i = 0; i < 64; i++) Reg[i] = EEPROM.read(PAGE0_BASE + i); //The stored half, Page 0 and Page 1, byte for byte
  page0Valid = (crc8(Reg, 0x1E) == Reg[0x1E]) && Reg[0x00] == 0x01;
  Reg[0x0A] = FW_FW_PATCH;
  Reg[0x1E] = crc8(Reg, 0x1E);
}

//Registers a controller may write. Everything else is read-only and writes
//to it are ignored (NW-Device-Specification, Page 2 rules).
bool isWritable(uint8_t pos) {
  return pos == REG_CTRL || pos == CTRL || pos == REG_I2C_ADDR
      || pos == REG_REQUEST || pos == REG_REQUEST + 1;
}
bool StartSample = true; //Flag used to start a new converstion, make a conversion on startup
// const unsigned int UpdateRate = 5; //Rate of update
const unsigned int UpdateRate[] = {5, 10, 60, 300}; //FIX with better numbers! 

// SoftWire si(PIN_PA2, PIN_PA3);  //Initialize software I2C
// SoftwareI2C si;
// SoftWire si = SoftWire();
// SoftwareWire si(PIN_PA3,PIN_PA2);

SlowSoftWire si = SlowSoftWire(PIN_PA2, PIN_PA3);
volatile bool StopFlag = false; //Used to indicate a stop condition 
volatile uint8_t RegID = 0; //Used to denote which register will be read from
volatile bool RepeatedStart = false; //Used to show if the start was repeated or not

// #define I2C_MAXWAIT 3

void setup() {

  pinMode(ModeSelPin, OUTPUT);
  digitalWrite(ModeSelPin, LOW); //Set device to I2C mode 
  // Serial.begin(115200); //DEBUG!
  // Serial.println("begin"); //DEBUG!
  Reg[CTRL] = 0x00; //Set Config to POR value
  Reg[REG_STATUS] = 0; //Not ready: no reading yet
  Reg[REG_CTRL] = CHIP_MS5803 | CHIP_MCP9808; //Power-up: every chip selected
  //SETUP HARDWARE!
  //SET CONTROL FOR I2C vs RS-485!!!!!!!!!!!!
  // pinMode(ADR_SEL_PIN, INPUT_PULLUP);
  // pinMode(10, OUTPUT); //DEBUG!
  // pinMode(9, OUTPUT); //DEBUG!
  // digitalWrite(10, HIGH); //DEBUG!
  // digitalWrite(9, LOW); //DEBUG!
  // if(!digitalRead(ADR_SEL_PIN)) ADR = ADR_Alt; //If solder jumper is bridged, use alternate address //DEBUG!

  loadPage0();
  if(Reg[REG_I2C_ADDR] != 0xFF) ADR = Reg[REG_I2C_ADDR]; //Provisioned address; 0xFF = use default
  Reg[REG_REPORT] = page0Valid ? NOTICE_UNIT_RESET : NOTICE_UNIT_PAGE0; //Latched until the controller writes Control
  Wire.begin(ADR);  //Begin slave I2C
	Wire.onRequest(requestEvent);     // register event
  Wire.onReceive(receiveEvent);
    si.begin(); //Must start AFTER hardware I2C!
  
  // EEPROM.write(0, ADR);

  //INIT DEVICES!
  // Start communicating wtih ADC to thermistor and MS5803
   // Wire.begin();
    // initADC();
    // si.i2c_init(); //Begin I2C master

    // si.begin();
    initTemp(); //DEBUG! REPLACE!
    initMS5803(); //DEBUG! REPLACE!
    // delay(500); //DEBUG!
    getValues(); // Update on power on //DEBUG!

  //Setup I2C slave
  // Wire.onAddrReceive(addressEvent); // register event


  
  // Wire.onStop(stopEvent);



  // AutoRange_Vis(); //Auto range for given light conditions
  // digitalWrite(9, HIGH); //DEBUG!
  // digitalWrite(10, LOW); //DEBUG!
}

void loop() {
  // static unsigned int Count = 0; //Counter to determine update rate
  // uint8_t Ctrl = Reg[CTRL]; //Store local value to improve efficiency
  uint8_t UpdateRateBits = Reg[CTRL] & 0x03; 
  static unsigned long Timeout = millis() % (UpdateRate[3]*1000); //Take mod with longest update rate 

  // digitalWrite(10, HIGH); //DEBUG!
  if(Reg[REG_CTRL] & BIT_TRIGGER) StartSample = true; //Controller trigger, in addition to the free-running timer
  if(StartSample == true) {

    // Config = Reg[CTRL]; //Update local register val
    //Read new values in
    // if(BitRead(Reg[CTRL], 2) == 0) {  //Only auto range if configured in Ctrl register 
    //  AutoRange_Vis();  //Run auto range
    //  delay(800); //Wait for new sample
    // }
    // digitalWrite(9, HIGH); //DEBUG!
    //A reading begins: clear ready, take the chip selection, consume the trigger.
    Reg[REG_STATUS] &= ~BIT_READY; //Clear ready flag (Page 2 status byte, bit 0) while new values are being written
    bool doMS5803 = Reg[REG_CTRL] & CHIP_MS5803;
    bool doMCP9808 = Reg[REG_CTRL] & CHIP_MCP9808;
    Reg[REG_CTRL] &= ~(BIT_TRIGGER | BIT_SLEEP); //trigger consumed; sleep not implemented
    ms5803Fail = false;
    mcp9808Fail = false;
    //LOAD VALUES
    if(doMS5803) {
      getMeasurements();
      Pressure = _pressure_actual / (float(COEF4)/100.0);
      Temp1 = _temperature_actual / 100.0;
      SplitAndLoad(0x48, long(Pressure*1000.0));              //Schema 1: pressure, int32, µBar (Block 1)
      SplitAndLoad(0x4C, (unsigned int)(int16_t)_temperature_actual); //Schema 1: temp MS5803, int16, 0.01°C (Block 1)
    }
    if(doMCP9808) {
      Temp0 = getTemp(); //DEBUG!
      SplitAndLoad(0x50, (unsigned int)(int16_t)(Temp0*100.0));       //Schema 1: temp ext, int16, 0.01°C (Block 2)
    }

    //Reading complete: copy the staged data in, load status and fault, bump
    //the counter, set ready. Atomic so a controller's page read never
    //straddles the update or sees a reading half written.
    uint8_t status = BIT_READY;
    if(doMS5803 && ms5803Fail) { status |= CHIP_MS5803; Reg[REG_REPORT] = FAULT_MS5803_NOACK; }
    if(doMCP9808 && mcp9808Fail) { status |= CHIP_MCP9808; Reg[REG_REPORT] = FAULT_MCP9808_NOACK; }
    if(status & 0x7E) status |= BIT_PANFAULT;
    uint16_t count = Reg[REG_COUNTER] | (Reg[REG_COUNTER + 1] << 8);
    count++;
    cli();
    memcpy(Reg + DATA_BASE, Staged, DATA_LEN); //The whole reading appears at once, with its counter
    Reg[REG_COUNTER] = count & 0xFF; Reg[REG_COUNTER + 1] = count >> 8;
    Reg[REG_STATUS] = status; //Set ready flag (Page 2 status byte, bit 0)
    sei();
    // digitalWrite(9, LOW); //DEBUG!
    StartSample = false; //Clear flag when new values updated  
  }

  //Make sure there is not a protnetial logic problem when changing update rate!!!!!!
  if(millis() % (UpdateRate[3]*1000) - Timeout > UpdateRate[UpdateRateBits]*1000) {  
    StartSample = true; //Set flag if number of updates have rolled over 
    Timeout = millis() % (UpdateRate[3]*1000); //Restart timer
    // digitalWrite(10, LOW); //DEBUG!
  }

  // if(BitRead(Reg[CTRL], 3) == 1) {  //If manual autorange is commanded
  //  AutoRange_Vis(); //Call autorange
  //  delay(800); //Wait for new data
  //  Reg[CTRL] &= 0xF7; //Clear auto range bit to inform user autorange is complete
  // }

  // if(Reg[CTRL] != Config) {
  //  Config = Reg[CTRL]; //Update local register 
  //  Timeout = millis() % (UpdateRate[3]*1000); //Reset counter if control register changes
  // }
  delay(100);
}


bool BitRead(uint8_t Val, uint8_t Pos) //Read the bit value at the specified position
{
  return (Val >> Pos) & 0x01;
}

uint8_t SendCommand(uint8_t Adr, uint8_t Command)
{
    // si.i2c_start((Adr << 1) | WRITE);
    si.beginTransmission(Adr);
    // bool Error = si.i2c_write(Command);
    si.write(Command);
    si.endTransmission(); //RETURN??
    return 1; //DEBUG!
}

uint8_t WriteByte(uint8_t Adr, uint8_t Command, uint8_t Data)  //Writes value to 16 bit register
{
  // si.i2c_start((Adr << 1) | WRITE);
  si.beginTransmission(Adr);
  // si.i2c_write(Command); //Write Command value
  si.write(Command);
  // uint8_t Error = si.i2c_write(Data); //Write Data
  si.write(Data);
  // si.i2c_stop();
  uint8_t Error = si.endTransmission();
  return Error;  //Invert error so that it will return 0 is works
}

uint8_t WriteWord(uint8_t Adr, uint8_t Command, unsigned int Data)  //Writes value to 16 bit register
{
  // si.i2c_start((Adr << 1) | WRITE);
  si.beginTransmission(Adr);
  // si.i2c_write(Command); //Write Command value
  si.write(Command);
  // si.i2c_write(Data & 0xFF); //Write LSB
  si.write(Data & 0xFF);
  // uint8_t Error = si.i2c_write((Data >> 8) & 0xFF); //Write MSB
  si.write((Data >> 8) & 0xFF );
  uint8_t Error = si.endTransmission();
  // si.i2c_stop();
  return Error;  //Invert error so that it will return 0 is works
}

uint8_t WriteWord_LE(uint8_t Adr, uint8_t Command, unsigned int Data)  //Writes value to 16 bit register
{
  // si.i2c_start((Adr << 1) | WRITE);
  si.beginTransmission(Adr);
  // si.i2c_write(Command); //Write Command value
  si.write(Command);
  // si.i2c_write(Data & 0xFF); //Write LSB
  si.write((Data >> 8) & 0xFF );
  si.write(Data & 0xFF);
  // uint8_t Error = si.i2c_write((Data >> 8) & 0xFF); //Write MSB
  uint8_t Error = si.endTransmission();
  // si.i2c_stop();
  return Error;  //Invert error so that it will return 0 is works

  // si.i2c_start((Adr << 1) | WRITE);
  // si.i2c_write(Command); //Write Command value
  // si.i2c_write((Data >> 8) & 0xFF); //Write MSB
  // si.i2c_write(Data & 0xFF); //Write LSB
  // si.i2c_stop();
  // return Error;  //Invert error so that it will return 0 is works
}

// uint8_t WriteConfig(uint8_t Adr, uint8_t NewConfig)
// {
//  si.i2c_start((Adr << 1) | WRITE);
//  si.i2c_write(CONF_CMD);  //Write command code to Config register
//  uint8_t Error = si.i2c_write(NewConfig);
//  si.i2c_stop();
//  if(Error == true) {
//    Config = NewConfig; //Set global config if write was sucessful 
//    return 0;
//  }
//  else return -1; //If write failed, return failure condition
// }

int ReadByte(uint8_t Adr, uint8_t Command, uint8_t Pos) //Send command value, and high/low byte to read, returns desired byte
{
  bool Error = SendCommand(Adr, Command);
  // si.i2c_rep_start((Adr << 1) | READ);
  si.requestFrom(Adr, 2);
  // uint8_t ValLow = si.i2c_read(false);
  uint8_t ValLow = si.read();
  // uint8_t ValHigh = si.i2c_read(false);
  uint8_t ValHigh = si.read();
  // si.i2c_stop();
  si.endTransmission();
  Error = true; //DEBUG!
  if(Error == true) {
    if(Pos == 0) return ValLow;
    if(Pos == 1) return ValHigh;
  }
  else return -1; //Return error if read failed

}

int ReadWord(uint8_t Adr, uint8_t Command)  //Send command value, returns entire 16 bit word
{
  bool Error = SendCommand(Adr, Command);
  // Serial.print("Error = "); Serial.println(Error); //DEBUG!
  si.requestFrom(Adr, 2);
  // si.i2c_rep_start((Adr << 1) | READ);
  // uint8_t ByteLow = si.i2c_read(false);  //Read in high and low bytes (big endian)
  uint8_t ByteLow = si.read();
  // uint8_t ByteHigh = si.i2c_read(false);
  uint8_t ByteHigh = si.read();
  // si.i2c_stop();
  si.endTransmission();
  // if(Error == true) return ((ByteHigh << 8) | ByteLow); //If read succeeded, return concatonated value
  // else return -1; //Return error if read failed
  return ((ByteHigh << 8) | ByteLow); //DEBUG!
}

int ReadWord_LE(uint8_t Adr, uint8_t Command)  //Send command value, returns entire 16 bit word
{
  bool Error = SendCommand(Adr, Command);
  // Serial.print("Error = "); Serial.println(Error); //DEBUG!
  si.requestFrom(Adr, 2);
  // si.i2c_rep_start((Adr << 1) | READ);
  // uint8_t ByteLow = si.i2c_read(false);  //Read in high and low bytes (big endian)
  uint8_t ByteHigh = si.read();
  // uint8_t ByteHigh = si.i2c_read(false);
  uint8_t ByteLow = si.read();
  // si.i2c_stop();
  si.endTransmission();
  // if(Error == true) return ((ByteHigh << 8) | ByteLow); //If read succeeded, return concatonated value
  // else return -1; //Return error if read failed
  return ((ByteHigh << 8) | ByteLow); //DEBUG!
  // bool Error = SendCommand(Adr, Command);
  // si.i2c_stop();
  // si.i2c_start((Adr << 1) | READ);
  // uint8_t ByteHigh = si.i2c_read(false);  //Read in high and low bytes (big endian)
  // uint8_t ByteLow = si.i2c_read(false);
  // si.i2c_stop();
  // // if(Error == true) return ((ByteHigh << 8) | ByteLow); //If read succeeded, return concatonated value
  // // else return -1; //Return error if read failed
  // return ((ByteHigh << 8) | ByteLow); //DEBUG!
}

void SplitAndLoad(uint8_t Pos, unsigned int Val) //Write 16 bits into the staged reading; Pos is the Page 2 register address
{
  uint8_t Len = sizeof(Val);
  for(int i = Pos; i < Pos + Len; i++) {
    Staged[i - DATA_BASE] = (Val >> (i - Pos)*8) & 0xFF; //Pullout the next byte
  }
}

void SplitAndLoad(uint8_t Pos, long Val)  //Write 32 bits into the staged reading
{
  uint8_t Len = sizeof(Val);
  for(int i = Pos; i < Pos + Len; i++) {
    Staged[i - DATA_BASE] = (Val >> (i - Pos)*8) & 0xFF; //Pullout the next byte
  }
}

boolean addressEvent(uint16_t address, uint8_t count)
{
  RepeatedStart = (count > 0 ? true : false);
  return true; // send ACK to master
}

void requestEvent()
{ 
  //Serve up to one full page from the requested register with auto-increment.
  //The slave clocks out only as many bytes as the controller asks for; the
  //rest of the buffer is discarded at the stop condition. Reads past the end
  //of the array wrap, so a controller never receives bytes from outside it.
  for(uint8_t i = 0; i < 32; i++) {
    uint16_t k = (uint16_t)RegID + i;
    Wire.write(k < sizeof(Reg) ? Reg[k] : 0x00); //Past the last page: zeros, never a wrap onto Page 0
  }
}

void receiveEvent(int DataLen) 
{
    //Write data to appropriate location
    if(DataLen == 2){
      //Remove while loop?? 
      while(Wire.available() < 2); //Only option for writing would be register address, and single 8 bit value
      uint8_t Pos = Wire.read();
      uint8_t Val = Wire.read();
      if(!isWritable(Pos)) return; //Read-only register: ignore the write
      Reg[Pos] = Val; //Set register value
      if(Pos == REG_CTRL) Reg[REG_REPORT] = 0; //A control write acknowledges the report
      if(Pos == REG_I2C_ADDR) EEPROM.update(PAGE0_BASE + REG_I2C_ADDR, Val); //Persist I2C address (compare-before-write); takes effect on next boot
  }

  if(DataLen == 1){
    RegID = Wire.read(); //Read in the register ID to be used for subsequent read
  }
}

void stopEvent() 
{
  StopFlag = true;
  //End comunication
}

uint8_t getValues()
{
    // Update global values from sensors
    getMeasurements();
    Pressure = _pressure_actual / (float(COEF4)/100.0);
    Temp0 = getTemp(); //DEBUG!
    // Temp0 = 26.25; //DEBUG!
    Temp1 = _temperature_actual / 100.0;
    return StatusReg; // FIX to give status indication!
}

void initTemp()
{
  //Any config required??
  // si.i2c_start_wait((TempADR << 1) | I2C_WRITE);    //Wire.beginTransmission(_i2caddr);   si.i2c_write((uint8_t)reg);   si.i2c_write(value >> 8);
 //   si.i2c_write(value & 0xFF);
 //   si.i2c_stop();
}

// void initADC()
// {
//  Wire.beginTransmission(ADC_ADR);
//  Wire.write(0x9C); // Set ADC to continuious conversion, 18 bits, 1V/V gain
//  StatusReg = StatusReg | Wire.endTransmission(); // Lower 2 bits for ADC status
// }

void initMS5803()
{
	sendCommand(CMD_RESET);
	delay(3); //Reset system 
   uint8_t i;
   uint8_t Data[2] = {0};

   for(i = 0; i <= 7; i++){
       sendCommand(CMD_PROM + (i * 2));
       si.requestFrom(PresADR, 2);
       Data[0] = si.read();
       Data[1] = si.read();
       coefficient[i] = (Data[0] << 8)|Data[1];
   }
   //Bits 2~3 show the status of the MS5803
   StatusReg = StatusReg | (sendCommand(CMD_RESET) << 2);
   delay(3);
}

void getMeasurements()
// Gets resuts from ADC and stores them into internal variables
{
	//Retrieve ADC result
	// int32_t temperature_raw = getADCconversionMS5803(TEMPERATURE, _precision);
	// int32_t pressure_raw = getADCconversionMS5803(PRESSURE, _precision);

	int32_t temperature_raw = getADCconversionMS5803(0x10);
	int32_t pressure_raw = getADCconversionMS5803(0x00);
	
	
	//Create Variables for calculations
	int32_t temp_calc;
	int32_t pressure_calc;
	
	int32_t dT;
		
	//Now that we have a raw temperature, let's compute our actual.
	dT = temperature_raw - ((int32_t)coefficient[5] << 8);
	temp_calc = (((int64_t)dT * coefficient[6]) >> 23) + 2000;
	
	// TODO TESTING  _temperature_actual = temp_calc;
	
	//Now we have our first order Temperature, let's calculate the second order.
	int64_t T2, OFF2, SENS2, OFF, SENS; //working variables

	if (temp_calc < 2000) 
	// If temp_calc is below 20.0C
	{	
		T2 = COEF5 * (((int64_t)dT * dT) >> COEF6);
		OFF2 = COEF7 * ((temp_calc - 2000) * (temp_calc - 2000)) / (pow(2,COEF8));
		SENS2 = COEF9 * ((temp_calc - 2000) * (temp_calc - 2000)) / (pow(2,COEF10));
		
		if(temp_calc < -1500)
		// If temp_calc is below -15.0C 
		{
			OFF2 = OFF2 + COEF11 * ((temp_calc + 1500) * (temp_calc + 1500));
			SENS2 = SENS2 + COEF12 * ((temp_calc + 1500) * (temp_calc + 1500));
		}
    } 
	else
	// If temp_calc is above 20.0C
	{ 
		T2 = COEF13 * ((uint64_t)dT * dT)/pow(2,COEF14);
		OFF2 = COEF15 * ((temp_calc - 2000) * (temp_calc - 2000)) / 16;
		SENS2 = 0;
	}
	
	// Now bring it all together to apply offsets 
	
	OFF = ((int64_t)coefficient[2] << COEF0) + (((coefficient[4] * (int64_t)dT)) >> COEF1);
	SENS = ((int64_t)coefficient[1] << COEF2) + (((coefficient[3] * (int64_t)dT)) >> COEF3);
	
	temp_calc = temp_calc - T2;
	OFF = OFF - OFF2;
	SENS = SENS - SENS2;

	// Now lets calculate the pressure
	

	pressure_calc = (((SENS * pressure_raw) / 2097152 ) - OFF) / 32768;
	
	_temperature_actual = temp_calc ;
	_pressure_actual = pressure_calc ; // 10;// pressure_calc;
	

}

uint32_t getADCconversionMS5803(uint8_t _measurement)
// Retrieve ADC measurement from the MS5803 device.
// Select measurement type and precision
// TODO:  Set up for asynchronous conversion? - ie, w/o waits
{
   uint8_t Data[3] = {0};
   sendCommand(CMD_ADC_CONV + _measurement + 0x08);
   // Wait for conversion to complete
   delay(20);

   StatusReg = StatusReg | (sendCommand(CMD_ADC_READ) << 2);

   si.requestFrom(PresADR, 3);

   while(si.available()) //REPLACE!
   {
       Data[0] = si.read();
       Data[1] = si.read();
       Data[2] = si.read();
   }
   // return (long(Data[0]) << 16) + (long(Data[1]) << 8) + Data[2];
   return ((uint32_t)Data[0] << 16) + ((uint32_t)Data[1] << 8) + Data[2];
}

uint8_t sendCommand(uint8_t Command)
{
	si.beginTransmission(PresADR);
    si.write(Command);
    uint8_t Error = si.endTransmission();
    if(Error) ms5803Fail = true; //No acknowledge: chip 0 fault on this reading
    return Error;
   // si.beginTransmission(PresADR);
   // si.write(Command);
   // return si.endTransmission();
}

float getTemp()
{
  // WriteByte(TempADR, 0x00, 0x05); //Set to Temp register
  // for(int i = 0; i < 128; i++) {
  //  si.i2c_start_wait((i << 1) | I2C_WRITE);
  //  bool Error = si.i2c_write(0x00);
  //  si.i2c_stop();
  //  delay(10);
  // }
  const unsigned long MaxSampleTime = 250; //max ms before a new sample should be available 
  while(millis() < MaxSampleTime); //Make sure at least 250ms have gone by before reading

  uint8_t ByteHigh = 0;
  uint8_t ByteLow = 0;

  si.beginTransmission(TempADR);
  si.write(0x05);
  if(si.endTransmission()) mcp9808Fail = true; //No acknowledge: chip 1 fault on this reading

  si.requestFrom(TempADR, 2);
  ByteHigh = si.read();
  ByteLow = si.read();

  ByteHigh = ByteHigh & 0x1F; //Clear flags
  uint16_t TempVal = (ByteHigh << 8) | ByteLow; //Concatonate to 16 bit value (12 bit range)
  

  float Temp = 0;
  Temp = TempVal & 0x0FFF; //Clear flags
  Temp = Temp/16.0;
  if(TempVal & 0x1000) { //If negative value 
    Temp = Temp - 256; 
  }
  return Temp;

  // ByteHigh = ByteHigh & 0x1F; //Clear flags
  // if((ByteHigh & 0x10) == 0x10) { //If temp < 0ºC
  //   ByteHigh = ByteHigh & 0x0F; //Clear sign bit
  //   return (256.0 - (float(ByteHigh)*16.0 + float(ByteLow)/16.0));
  // }
  // else {
  //   return (float(ByteHigh)*16.0 + float(ByteLow)/16.0);
  // }

  // int16_t TempBits = (ByteLow >> 2) | (ByteHigh << 6); 
  // return float(TempBits)*0.0625; //Multiply by LSB //DEBUG
  // return ByteLow; //DEBUG!

   //  uint8_t Data[3];
   //  float ThermB = 3380; //Basic B value for thermistor
   //  // Coefficients for enhanced accuracy
   //  // float ThermCoefs[4] = \
   //  //        {0.003354016, 0.0003074038, 1.019153E-05, 9.093712E-07};
   //  float VRef = 1.8; //Voltage referance used for thermistor
   //  float R0 = 10000; //Series resistor value with thermistor
   //  float ThermR = 10000; //Nominal resistace value for the thermistor

    // Wire.requestFrom(ADC_ADR, 4);
   //  delay(5); //DEBUG!
   //  // Get 3 bytes of potential data
    // if(Wire.available() == 4)
    // {
    //    Data[0] = Wire.read();
   //     Data[1] = Wire.read();
   //     Data[2] = Wire.read(); //DEBUG!
    // }
   //  StatusReg = StatusReg | Wire.endTransmission(); //DEBUG!

   //  // Voltage divider output
   //  float Val = ( ( (long(Data[0]) & 0x03 ) << 16 )
   //                + (long(Data[1]) << 8)
   //                + Data[2] )
   //                * 1.5625e-5;
   //  // Voltage is measured across thermistor, not relative to ground
   //  Val = VRef - Val;

   //  float Rt = (VRef/Val)*R0 - R0;
   //  float T = 1/((1/ThermB)*log(Rt/ThermR) + 1/298.15);
   //  T = T - 273.15; // Convert to C
   //  return T; //DEBUG!

   
//   Wire.beginTransmission(TempADR);
//   Wire.write(0x00); //Pointer register
//   Wire.write(0x05); //Request temp data register 
//   uint8_t Error = Wire.endTransmission(); //Store error value
//
//   if(Error != 0) return -9999.0; //Return error state on I2C fail
//
//   Wire.requestFrom(TempADR, 2); //Get word from device
//   uint16_t MSB = Wire.read(); //Read in both bytes
//   uint16_t LSB = Wire.read();
//   int8_t Sign = BitRead(MSB, 4); //Get sign bit
//   float Data = (((MSB << 8) & 0x0F) | LSB) * (1 - 2*Sign) * 0.0625; //Bit step = 0.0625 ºC
//   return Data; 

}

unsigned char getCRC(const unsigned char * data, const unsigned int size)
{
    // Cyclic redundancy check?
    unsigned char crc = 0;
    for ( unsigned int i = 0; i < size; ++i )
    {
        unsigned char inbyte = data[i];
        for ( unsigned char j = 0; j < 8; ++j )
        {
            unsigned char mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if ( mix ) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}

uint8_t charToInt(char *data)
{
    return (data[0] - 48)*10 + (data[1] - 48);
}
