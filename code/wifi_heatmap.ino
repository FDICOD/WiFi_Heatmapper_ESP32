#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// ===== TFT DISPLAY =====
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// ===== BUTTON =====
#define BUTTON_PIN 34

// ===== GRID SETTINGS =====
#define GRID_COLS 8
#define GRID_ROWS 6

int cellW, cellH;
int currentCell = 0;

bool completed = false;
bool showingSummary = false;
bool inSelection = true;

int rssiGrid[GRID_ROWS][GRID_COLS];

// ===== WIFI =====
#define MAX_NETWORKS 10

String ssids[MAX_NETWORKS];
int rssis[MAX_NETWORKS];

int networkCount = 0;
int selectedIndex = 0;

String selectedSSID = "";

// ===== BUTTON STATE =====
unsigned long pressStart = 0;
bool buttonHeld = false;

// ===== RSSI COLOR MAPPING =====
uint16_t getColor(int rssi) {

  uint16_t ORANGE = tft.color565(255, 120, 0);

  if (rssi >= -45)
    return ST77XX_CYAN;

  else if (rssi >= -55)
    return ST77XX_BLUE;

  else if (rssi >= -65)
    return ST77XX_MAGENTA;

  else if (rssi >= -72)
    return ST77XX_YELLOW;

  else if (rssi >= -80)
    return ORANGE;

  else
    return ST77XX_RED;
}

// ===== WIFI SCAN =====
void scanNetworks() {

  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Scanning WiFi...");

  networkCount = WiFi.scanNetworks();

  if (networkCount > MAX_NETWORKS)
    networkCount = MAX_NETWORKS;

  for (int i = 0; i < networkCount; i++) {

    ssids[i] = WiFi.SSID(i);
    rssis[i] = WiFi.RSSI(i);
  }
}

// ===== NETWORK SELECTION UI =====
void drawSelection() {

  tft.fillScreen(ST77XX_BLACK);

  for (int i = 0; i < networkCount; i++) {

    if (i == selectedIndex)
      tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);

    else
      tft.setTextColor(ST77XX_WHITE);

    tft.setCursor(10, 30 + i * 20);
    tft.print(ssids[i]);

    tft.setCursor(180, 30 + i * 20);
    tft.print(rssis[i]);
  }
}

// ===== RSSI ACQUISITION =====
int getRSSI() {

  int n = WiFi.scanNetworks();

  for (int i = 0; i < n; i++) {

    if (WiFi.SSID(i) == selectedSSID)
      return WiFi.RSSI(i);
  }

  return -100;
}

int getAvgRSSI() {

  return getRSSI();
}

// ===== GRID DRAWING =====
void drawGrid() {

  tft.fillScreen(ST77XX_BLACK);

  cellW = tft.width() / GRID_COLS;
  cellH = tft.height() / GRID_ROWS;

  for (int r = 0; r < GRID_ROWS; r++) {

    for (int c = 0; c < GRID_COLS; c++) {

      tft.drawRect(
        c * cellW,
        r * cellH,
        cellW,
        cellH,
        ST77XX_WHITE
      );
    }
  }
}

// ===== CELL HIGHLIGHT =====
void highlightCell(int index, uint16_t color) {

  int r = index / GRID_COLS;
  int c = index % GRID_COLS;

  tft.drawRect(
    c * cellW,
    r * cellH,
    cellW,
    cellH,
    color
  );
}

// ===== CELL FILL =====
void fillCell(int index, int rssi) {

  int r = index / GRID_COLS;
  int c = index % GRID_COLS;

  uint16_t color = getColor(rssi);

  tft.fillRect(
    c * cellW + 2,
    r * cellH + 2,
    cellW - 4,
    cellH - 4,
    color
  );

  tft.setTextColor(ST77XX_BLACK);

  tft.setCursor(
    c * cellW + 5,
    r * cellH + 5
  );

  tft.print(rssi);
}

// ===== SUMMARY SCREEN =====
void showSummaryOverlay() {

  int best = -100;
  int worst = 0;
  int total = 0;

  for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {

    int r = i / GRID_COLS;
    int c = i % GRID_COLS;

    int val = rssiGrid[r][c];

    total += val;

    if (val > best)
      best = val;

    if (val < worst)
      worst = val;
  }

  int avg = total / (GRID_ROWS * GRID_COLS);

  tft.fillRect(20, 40, 200, 140, ST77XX_BLACK);
  tft.drawRect(20, 40, 200, 140, ST77XX_WHITE);

  tft.setTextColor(ST77XX_WHITE);

  tft.setCursor(30, 50);
  tft.println("SUMMARY");

  tft.setCursor(30, 70);
  tft.print("Best: ");
  tft.println(best);

  tft.setCursor(30, 90);
  tft.print("Worst: ");
  tft.println(worst);

  tft.setCursor(30, 110);
  tft.print("Avg: ");
  tft.println(avg);
}

// ===== SETUP =====
void setup() {

  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT);

  tft.init(240, 320);
  tft.setRotation(1);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(100);

  scanNetworks();
  drawSelection();
}

// ===== MAIN LOOP =====
void loop() {

  int state = digitalRead(BUTTON_PIN);

  // ===== NETWORK SELECTION =====
  if (inSelection) {

    if (state == LOW && !buttonHeld) {

      buttonHeld = true;
      pressStart = millis();
    }

    if (state == HIGH && buttonHeld) {

      unsigned long pressTime = millis() - pressStart;

      buttonHeld = false;

      if (pressTime < 600) {

        selectedIndex++;

        if (selectedIndex >= networkCount)
          selectedIndex = 0;

        drawSelection();
      }

      else {

        selectedSSID = ssids[selectedIndex];

        inSelection = false;

        delay(500);

        drawGrid();

        highlightCell(currentCell, ST77XX_MAGENTA);
      }
    }

    return;
  }

  // ===== HEATMAP MODE =====
  if (!completed && state == LOW && !showingSummary) {

    delay(20);

    int rssi = getAvgRSSI();

    int r = currentCell / GRID_COLS;
    int c = currentCell % GRID_COLS;

    rssiGrid[r][c] = rssi;

    fillCell(currentCell, rssi);

    currentCell++;

    if (currentCell >= GRID_COLS * GRID_ROWS) {

      completed = true;
    }

    else {

      highlightCell(currentCell, ST77XX_MAGENTA);
    }
  }

  // ===== SUMMARY DISPLAY =====
  if (completed && buttonHeld && !showingSummary) {

    if (millis() - pressStart > 600) {

      showingSummary = true;

      showSummaryOverlay();
    }
  }
}