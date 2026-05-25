#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <vector>

// --- CONFIGURACIÓN DE PINES ---
#define TFT_DC   2
#define TFT_RST  4
#define TFT_CS  -1 
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

#define SD_CS   33
#define SD_MOSI 25
#define SD_SCK  22
#define SD_MISO 32
SPIClass hspi(HSPI);

#define BTN_NEXT 27
#define BTN_PREV 26

// --- VARIABLES DE CONTROL ---
std::vector<String> fileList;
int currentFileIndex = 0;
#define BUFFPIXEL 60 

bool autoMode = true; // Empieza en modo automático por defecto
unsigned long lastAutoAdvance = 0;
unsigned long buttonsPressedStart = 0;
const unsigned long AUTO_INTERVAL = 5000; // 5 segundos
const unsigned long MODE_SWITCH_TIME = 3000; // 3 segundos pulsando ambos

void setup() {
  Serial.begin(115200);

  // Generar una semilla aleatoria usando un pin analógico vacío
  randomSeed(analogRead(34)); 

  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Cargando Galeria...");

  hspi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, hspi)) {
    tft.fillScreen(ILI9341_RED);
    tft.setCursor(10, 10);
    tft.println("ERROR: No hay SD");
    while (1);
  }

  loadFileList();
  
  if (fileList.size() > 0) {
    // Elegir una foto aleatoria para empezar
    currentFileIndex = random(0, fileList.size());
    showImage(currentFileIndex);
    Serial.print("Foto inicial aleatoria: ");
    Serial.println(fileList[currentFileIndex]);
  } else {
    tft.println("No hay .bmp en SD");
  }
}

void loop() {
  if (fileList.size() == 0) return;

  // 1. LÓGICA PARA CAMBIAR DE MODO (Pulsar ambos 3 segundos)
  if (digitalRead(BTN_NEXT) == LOW && digitalRead(BTN_PREV) == LOW) {
    if (buttonsPressedStart == 0) {
      buttonsPressedStart = millis();
    } else if (millis() - buttonsPressedStart >= MODE_SWITCH_TIME) {
      autoMode = !autoMode;
      mostrarFeedbackModo();
      buttonsPressedStart = 0;
      lastAutoAdvance = millis();
      delay(1000); 
    }
  } else {
    buttonsPressedStart = 0;
  }

  // 2. LÓGICA MODO AUTOMÁTICO
  if (autoMode) {
    if (millis() - lastAutoAdvance >= AUTO_INTERVAL) {
      currentFileIndex = (currentFileIndex + 1) % fileList.size();
      showImage(currentFileIndex);
    }
  } 
  
  // 3. LÓGICA MANUAL (Solo si un solo botón está presionado)
  else {
    if (digitalRead(BTN_NEXT) == LOW && digitalRead(BTN_PREV) == HIGH) {
      currentFileIndex = (currentFileIndex + 1) % fileList.size();
      showImage(currentFileIndex);
      delay(300);
    }
    if (digitalRead(BTN_PREV) == LOW && digitalRead(BTN_NEXT) == HIGH) {
      currentFileIndex--;
      if (currentFileIndex < 0) currentFileIndex = fileList.size() - 1;
      showImage(currentFileIndex);
      delay(300);
    }
  }
}

void mostrarFeedbackModo() {
  tft.fillRect(0, 0, 320, 35, ILI9341_BLACK);
  tft.setCursor(10, 10);
  if (autoMode) {
    tft.setTextColor(ILI9341_GREEN);
    tft.print("MODO: AUTOMATICO");
  } else {
    tft.setTextColor(ILI9341_RED);
    tft.print("MODO: MANUAL");
  }
  delay(1500);
  showImage(currentFileIndex); 
}

void showImage(int index) {
  drawBMP(fileList[index].c_str(), 0, 0);
  lastAutoAdvance = millis(); 
}

void loadFileList() {
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    String name = entry.name();
    if (name.endsWith(".bmp") || name.endsWith(".BMP")) {
      if (!name.startsWith("/")) name = "/" + name;
      fileList.push_back(name);
    }
    entry.close();
  }
  root.close();
}

void drawBMP(const char *filename, int x, int y) {
  File bmpFile = SD.open(filename);
  if (!bmpFile) return;
  if (read16(bmpFile) != 0x4D42) { bmpFile.close(); return; }
  read32(bmpFile); read32(bmpFile);
  uint32_t offset = read32(bmpFile);
  read32(bmpFile);
  int w = read32(bmpFile);
  int h = read32(bmpFile);
  uint32_t rowSize = (w * 3 + 3) & ~3;
  uint16_t *lcdbuffer = (uint16_t *)malloc(BUFFPIXEL * sizeof(uint16_t));
  uint8_t sdbuffer[3 * BUFFPIXEL];
  
  tft.startWrite();
  tft.setAddrWindow(x, y, w, h);
  for (int row = 0; row < h; row++) {
    bmpFile.seek(offset + (h - 1 - row) * rowSize);
    for (int col = 0; col < w; col += BUFFPIXEL) {
      int chunk = (col + BUFFPIXEL < w) ? BUFFPIXEL : (w - col);
      bmpFile.read(sdbuffer, 3 * chunk);
      for (int i = 0; i < chunk; i++) {
        uint8_t b = sdbuffer[i * 3], g = sdbuffer[i * 3 + 1], r = sdbuffer[i * 3 + 2];
        lcdbuffer[i] = tft.color565(r, g, b);
      }
      tft.writePixels(lcdbuffer, chunk);
    }
  }
  tft.endWrite();
  free(lcdbuffer);
  bmpFile.close();
}

uint16_t read16(File &f) { uint16_t result; f.read((uint8_t *)&result, sizeof(result)); return result; }
uint32_t read32(File &f) { uint32_t result; f.read((uint8_t *)&result, sizeof(result)); return result; }