/*
   Arduino ADS1220 SPI interface using BitBang
   Writing following parameter
   PGA 1

*/
#define SPI_MASTER_DUMMY   0xFF
#define CONFIG_REG0_ADDRESS 0x00
#define CONFIG_REG1_ADDRESS 0x01
#define CONFIG_REG2_ADDRESS 0x02
#define CONFIG_REG3_ADDRESS 0x03
#define WREG  0x40
#define RREG  0x20
uint8_t Config_Reg0;
uint8_t Config_Reg1;
uint8_t Config_Reg2;
uint8_t Config_Reg3;

int SCK_ads = 13;
int MISO_ads = 12;
int MOSI_ads = 11;
int DRDY_ads = 6;
int CS_ads = 7;

const byte set_zero = 2;
const byte callibrate = 3;
long int data = 0, weight = 0, val_avg = 0, tare = 0, set = 0, val = 0, old_val_avg = 0;
char arr[5] = {0, 0, 0, 0, 0};


void setup()
{
  Serial.begin(9600);
  pinMode(SCK_ads, OUTPUT);
  pinMode(MOSI_ads, OUTPUT);
  pinMode(CS_ads, OUTPUT);
  pinMode(MISO_ads, INPUT_PULLUP);//pinMode(MISO_ads, INPUT);
  pinMode(DRDY_ads, INPUT);

  pinMode(set_zero, INPUT_PULLUP);
  pinMode(callibrate, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(set_zero), set_tare, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(callibrate), set_callibrate, CHANGE);
  Serial.println("Ready");
  digitalWrite(CS_ads, HIGH);
  write_ads();

}

void loop()
{

  if ((digitalRead(DRDY_ads)) == LOW)
  {
    read_ads();
    //Serial.println("");
  }

}

void read_ads()
{

  data = 0;
  digitalWrite(CS_ads, LOW);
  for (int i = 0; i < 18; i++)//for (int i = 0; i < 24; i++)
  {
    digitalWrite(SCK_ads, HIGH);
    data = data << 1 | (digitalRead(MISO_ads));
    digitalWrite(SCK_ads, LOW);
    delay(5);
  }
  digitalWrite(CS_ads, HIGH);
  val = data;
  arr[0] = arr[1];
  arr[1] = arr[2];
  arr[2] = arr[3];
  arr[3] = arr[4];
  arr[4] = arr[5];//
  arr[5] = arr[6];
  arr[6] = arr[7];
  arr[7] = val;
  val_avg = ((arr[0]) + (arr[1]) + (arr[2]) + (arr[3]) + (arr[4]) + (arr[5]) + (arr[6]) );
  val_avg = (val_avg / 7);
  /*
     Serial.print(data);
    Serial.print(" ");
    Serial.print(data,HEX);
    Serial.print(" ");
    Serial.print(data,BIN);
    Serial.print(" ");
  */

  //if (val_avg != old_val_avg)
  //{
  Serial.print(val_avg);
  Serial.print(" ");
  weight=map(val_avg,4,56,0,1000);
  Serial.println(weight);
  //old_val_avg = val_avg;
  //}

  //Serial.print(val);       Serial.print(" ");
  //Serial.print(val_avg);   Serial.print(" ");

}


void write_ads()
{
  digitalWrite(CS_ads, LOW);

  Config_Reg0 = 0x01;
  Config_Reg1 = 0x04;
  Config_Reg2 = 0x10;
  Config_Reg3 = 0x00;


  writeRegister( CONFIG_REG0_ADDRESS , Config_Reg0);
  writeRegister( CONFIG_REG1_ADDRESS , Config_Reg1);
  writeRegister( CONFIG_REG2_ADDRESS , Config_Reg2);
  writeRegister( CONFIG_REG3_ADDRESS , Config_Reg3);

  delay(100);

  Config_Reg0 = readRegister(CONFIG_REG0_ADDRESS);
  Config_Reg1 = readRegister(CONFIG_REG1_ADDRESS);
  Config_Reg2 = readRegister(CONFIG_REG2_ADDRESS);
  Config_Reg3 = readRegister(CONFIG_REG3_ADDRESS);

  Serial.println("Config_Reg : ");
  Serial.println(Config_Reg0, HEX);
  Serial.println(Config_Reg1, HEX);
  Serial.println(Config_Reg2, HEX);
  Serial.println(Config_Reg3, HEX);
  
  Serial.println(" ");
  digitalWrite(CS_ads, HIGH); //release chip, signal end transfer

  delay(100);
}

void writeRegister(uint8_t address, uint8_t value)
{
  digitalWrite(CS_ads, LOW);
  delay(5);
  ads_transfer(WREG | (address << 2));
  ads_transfer(value);
  delay(5);
  digitalWrite(CS_ads, HIGH);
}

uint8_t readRegister(uint8_t address)
{
  uint8_t data;

  digitalWrite(CS_ads, LOW);
  delay(100);
  ads_transfer(RREG | (address << 2));
  data = ads_transfer(SPI_MASTER_DUMMY);
  delay(5);
  digitalWrite(CS_ads, HIGH);

  return data;
}

uint8_t ads_transfer (uint8_t p) {
  for (unsigned int i = 0; i < 8; ++i)
  {
    digitalWrite(MOSI_ads, (p & 0x80) ? HIGH : LOW);//digitalWrite(MOSI_ads, (b & 0x80) ? HIGH : LOW);
    digitalWrite(SCK_ads, HIGH);
    delayMicroseconds(1);//delayMicroseconds(pulseWidth);
    p = (p << 1) | digitalRead(MISO_ads);
    digitalWrite(SCK_ads, LOW); // slow pulse
    delayMicroseconds(1);//delayMicroseconds(pulseWidth);
  }
  return p;
}

