// Font file is stored on SD card
#include <SD.h>

// Graphics and font library
#include <TFT_eSPI.h>
#include <SPI.h>

uint8_t cfont[72];
long font_count=0;

TFT_eSPI tft = TFT_eSPI();  // Invoke library

// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup(void) {
  Serial.begin(115200); // Used for messages

  // Initialise the SD library before the TFT so the chip select is defined
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // Initialise the TFT after the SD card!
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);

  listDir(SD, "/", 0);

  Serial.println("SD and TFT initialisation done.");
}

// -------------------------------------------------------------------------
// Main loop
// -------------------------------------------------------------------------
void loop() {

  int x=0,y=0;
  int font_color[] = { TFT_WHITE, TFT_BLUE, TFT_RED, TFT_GREEN, TFT_CYAN, TFT_MAGENTA, TFT_YELLOW };
  unsigned char str0[]={ 0xBC, 0xC6, 0 }; // 數
  unsigned char str1[]={ 0xA6, 0xEC, 0 }; // 位
  unsigned char str2[]={ 0xA4, 0xD1, 0 }; // 天
  unsigned char str3[]={ 0xB0, 0xF3, 0 }; // 堂
  unsigned char str4[]={ 0xAD, 0xCA, 0 }; // 倚
  unsigned char str5[]={ 0xA4, 0xA4, 0 }; // 中
  unsigned char str6[]={ 0xA4, 0xE5, 0 }; // 文
  unsigned char str7[]={ 0xA6, 0x72, 0 }; // 字
  unsigned char str8[]={ 0xAB, 0xAC, 0 }; // 型
  unsigned char str9[]={ 0xB4, 0xFA, 0 }; // 測
  unsigned char str10[]={ 0xB8, 0xD5, 0 }; // 試

  tft.fillScreen(TFT_BLACK);
  TFT_write_cfont(str0, 0, 0, 2, TFT_RED);
  TFT_write_cfont(str1, 50, 0, 2, TFT_YELLOW);
  TFT_write_cfont(str2, 100, 0, 2, TFT_GREEN);
  TFT_write_cfont(str3, 150, 0, 2, TFT_CYAN);
  TFT_write_cfont(str4, 0, 50, 2, TFT_BLUE);
  TFT_write_cfont(str2, 50, 50, 2, TFT_WHITE);
  TFT_write_cfont(str5, 100, 50, 2, TFT_MAGENTA);
  TFT_write_cfont(str6, 150, 50, 2, TFT_GREEN);  
  TFT_write_cfont(str7, 200, 50, 2, TFT_YELLOW);
  TFT_write_cfont(str8, 250, 50, 2, TFT_BLUE);
  TFT_write_cfont(str9, 0, 100, 2, TFT_YELLOW);    
  TFT_write_cfont(str10, 50, 100, 2, TFT_MAGENTA);
  delay(2000);

  tft.fillScreen(TFT_BLACK);  
  for(y=0;y<10;y++)  {
    for(x=0;x<13;x++) {
      TFT_write_cfont_offset(font_count*72, x*24, y*24, 1, font_color[(font_count) % 7]);
      if(font_count < 13094) font_count++;
      else font_count=0;
      Serial.println(font_count);
    }
  }
  delay(2000);

}

//計算字型在 stdfont.xx 位置
long ChineseToBig(unsigned char *str) 
{    
    int Hibyte=0,Lobyte=0,StartCode=0;
    long Offset=0;

    Hibyte = str[0];
    Lobyte = str[1];
    
    if(Lobyte  >= 161)
        StartCode = (Hibyte - 161) * 157 + (Lobyte  - 161) + 1 + 63;
    else
        StartCode = (Hibyte - 161) * 157 + (Lobyte  - 64) + 1;
    if(StartCode >= 472 && StartCode <= 5872) 
        Offset = (long)(StartCode-472)*72; //常用字
    else if(StartCode >= 6281 && StartCode <= 13973)
        Offset = (long)(StartCode-6281)*72+388872; //非常用字
    else
        Offset=-1000000; //該字的bitmap沒有出現在stdfont.15中
    return Offset;
}

//顯示中文字型 fillRect
void TFT_write_cfont(unsigned char *font, int poX, int poY, int font_size, int fgcolor)
{
  File etFontfile;
  
  etFontfile = SD.open("/etfont/stdfont.24l", FILE_READ);
  if (!etFontfile) {
    Serial.println("Stdfond.24 not found!");
    // don't do anything more:
    delay(1000);
  }

  etFontfile.seek(ChineseToBig(font));
  etFontfile.read((uint8_t*)cfont, sizeof(cfont));
  etFontfile.close();
  
  for(int i =0; i<24; i++ )  {
    for(int j=0;j<3;j++) {
      int temp = cfont[(i*3)+j];
      for(int k=0;k<8;k++) {
        if((temp>>(7-k))&0x01)
        {
          tft.fillRect(poX+(k+(j*8))*font_size, poY+i*font_size, font_size, font_size, fgcolor);
        }
      }
    }
  }
}

//顯示中文字型 drawXBitmap
void TFT_write_cfont1(unsigned char *font, int poX, int poY, int font_size, int fgcolor)
{
  File etFontfile;
  
  etFontfile = SD.open("/etfont/stdfont.24l", FILE_READ);
  if (!etFontfile) {
    Serial.println("Stdfond.24 not found!");
    // don't do anything more:
    delay(1000);
  }

  etFontfile.seek(ChineseToBig(font));
  etFontfile.read((uint8_t*)cfont, sizeof(cfont));
  etFontfile.close();
  
  tft.drawXBitmap(poX, poY, cfont, 24, 24, fgcolor);
  
}

void TFT_write_cfont_offset(long offset, int poX, int poY, int font_size, int fgcolor)
{
  File etFontfile;
  
  etFontfile = SD.open("/etfont/stdfont.24l", FILE_READ);
  if (!etFontfile) {
    Serial.println("Stdfond.24 not found!");
    // don't do anything more:
    delay(1000);
  }
  etFontfile.seek(offset);
  etFontfile.read((uint8_t*)cfont, sizeof(cfont));
  etFontfile.close();
  
  for (int i =0; i<24; i++ )  {
    for(int j=0;j<3;j++) {
      int temp = cfont[(i*3)+j];
      for(int k=0;k<8;k++) {
        if((temp>>(7-k))&0x01)
        {
          tft.fillRect(poX+(k+(j*8))*size, poY+i*size, font_size, font_size, fgcolor);
        }
      }
    }
  }
}


void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
