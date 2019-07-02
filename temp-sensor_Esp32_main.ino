#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

// WiFi settings
#define WIFI_SSID "YOURSSID"
#define WIFI_PASSWORD "YOURSSIRDPASSWORD"

// Delay between temperature checks (in minutes)
#define CHECK_INTERVAL 15

// Setup a oneWire instance to communicate with a OneWire device on GPIO 15
OneWire oneWire(15);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

DeviceAddress sensor_rack    = { 0x28, 0xF0, 0x66, 0xAE, 0x08, 0x00, 0x00, 0x78 };
DeviceAddress sensor_ac      = { 0x28, 0xFF, 0x9B, 0x1A, 0xA2, 0x16, 0x03, 0xB1 };
DeviceAddress sensor_ambient = { 0x28, 0xFF, 0x01, 0xE4, 0xA2, 0x16, 0x03, 0xDF };

const char *ntpServer = "pool.ntp.org";

static const char integromat_root_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIF2DCCA8CgAwIBAgIQTKr5yttjb+Af907YWwOGnTANBgkqhkiG9w0BAQwFADCB
hTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4G
A1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNV
BAMTIkNPTU9ETyBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAwMTE5
MDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBhTELMAkGA1UEBhMCR0IxGzAZBgNVBAgT
EkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMR
Q09NT0RPIENBIExpbWl0ZWQxKzApBgNVBAMTIkNPTU9ETyBSU0EgQ2VydGlmaWNh
dGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCR
6FSS0gpWsawNJN3Fz0RndJkrN6N9I3AAcbxT38T6KhKPS38QVr2fcHK3YX/JSw8X
pz3jsARh7v8Rl8f0hj4K+j5c+ZPmNHrZFGvnnLOFoIJ6dq9xkNfs/Q36nGz637CC
9BR++b7Epi9Pf5l/tfxnQ3K9DADWietrLNPtj5gcFKt+5eNu/Nio5JIk2kNrYrhV
/erBvGy2i/MOjZrkm2xpmfh4SDBF1a3hDTxFYPwyllEnvGfDyi62a+pGx8cgoLEf
Zd5ICLqkTqnyg0Y3hOvozIFIQ2dOciqbXL1MGyiKXCJ7tKuY2e7gUYPDCUZObT6Z
+pUX2nwzV0E8jVHtC7ZcryxjGt9XyD+86V3Em69FmeKjWiS0uqlWPc9vqv9JWL7w
qP/0uK3pN/u6uPQLOvnoQ0IeidiEyxPx2bvhiWC4jChWrBQdnArncevPDt09qZah
SL0896+1DSJMwBGB7FY79tOi4lu3sgQiUpWAk2nojkxl8ZEDLXB0AuqLZxUpaVIC
u9ffUGpVRr+goyhhf3DQw6KqLCGqR84onAZFdr+CGCe01a60y1Dma/RMhnEw6abf
Fobg2P9A3fvQQoh/ozM6LlweQRGBY84YcWsr7KaKtzFcOmpH4MN5WdYgGq/yapiq
crxXStJLnbsQ/LBMQeXtHT1eKJ2czL+zUdqnR+WEUwIDAQABo0IwQDAdBgNVHQ4E
FgQUu69+Aj36pvE8hI6t7jiY7NkyMtQwDgYDVR0PAQH/BAQDAgEGMA8GA1UdEwEB
/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAArx1UaEt65Ru2yyTUEUAJNMnMvl
wFTPoCWOAvn9sKIN9SCYPBMtrFaisNZ+EZLpLrqeLppysb0ZRGxhNaKatBYSaVqM
4dc+pBroLwP0rmEdEBsqpIt6xf4FpuHA1sj+nq6PK7o9mfjYcwlYRm6mnPTXJ9OV
2jeDchzTc+CiR5kDOF3VSXkAKRzH7JsgHAckaVd4sjn8OoSgtZx8jb8uk2Intzna
FxiuvTwJaP+EmzzV1gsD41eeFPfR60/IvYcjt7ZJQ3mFXLrrkguhxuhoqEwWsRqZ
CuhTLJK7oQkYdQxlqHvLI7cawiiFwxv/0Cti76R7CZGYZ4wUAc1oBmpjIXUDgIiK
boHGhfKppC3n9KUkEEeDys30jXlYsQab5xoq2Z0B15R97QNKyvDb6KkBPvVWmcke
jkk9u+UJueBPSZI9FoJAzMxZxuY67RIuaTxslbH9qh17f4a+Hg4yRvv7E491f0yL
S0Zj/gA0QHDBw7mh3aZw4gSzQbzpgJHqZJx64SIDqZxubw5lT2yHh17zbqD5daWb
QOhTsiedSrnAdyGN/4fy3ryM7xfft0kL0fJuMAsaDk527RH89elWsn2/x20Kk4yl
0MC2Hb46TpSi125sC8KKfPog88Tk5c0NqMuRkrF8hey1FGlmDoLnzc7ILaZRfyHB
NVOFBkpdn627G190
-----END CERTIFICATE-----
)EOF";

void setup(void){
  Serial.begin(115200);
  sensors.begin();

  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to the WiFi network");
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());

  configTime(0, 0, ntpServer);
}

void loop(void){
  Serial.print("Reading temperatures... ");
  sensors.requestTemperatures();
  Serial.println("OK");

  Serial.print("Rack temperature: ");
  Serial.print(sensors.getTempC(sensor_rack));
  Serial.println("°C");

  Serial.print("AC temperature: ");
  Serial.print(sensors.getTempC(sensor_ac));
  Serial.println("°C");

  Serial.print("Ambient temperature: ");
  Serial.print(sensors.getTempC(sensor_ambient));
  Serial.println("°C");

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to get time.");
    return;
  }

  char payload[200];
  snprintf(payload, sizeof(payload), "{\"date\":\"%04d-%02d-%02d %02d:%02d:%02d\",\"rack_temp\":%f,\"ac_temp\":%f,\"ambient_temp\":%f}",
           timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
           sensors.getTempC(sensor_rack), sensors.getTempC(sensor_ac), sensors.getTempC(sensor_ambient));

  HTTPClient http;
  http.begin("https://hook.integromat.com/HookID", integromat_root_ca);
  http.addHeader("Content-Type", "application/json");
  http.POST(payload);

  delay(CHECK_INTERVAL * 60 * 1000);
}
