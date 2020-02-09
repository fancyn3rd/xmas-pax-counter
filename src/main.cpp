#include <Arduino.h>
#include <string.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_timer.h>
#include <esp_log.h>

#include <U8x8lib.h>

#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#define CHANNEL_HOP_INTERVAL_MS   1000;

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE, 22, 21);



/*****************************************************************************/
void setup() {
  Serial.begin(115200);
  Serial.println("Start xmas-pax-counter");

  wifiInitDevices();
  wifiInit();

  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.clear();
}

void loop() {
  wifiChannelHop();
  wifiCountDevices();
  delay(1000);
}

/*****************************************************************************/
void wifiInit() {
  const static wifi_country_t wifi_country = {
    "EU",
    1,
    13,
    100,
    WIFI_COUNTRY_POLICY_MANUAL
  };

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  cfg.nvs_enable = 0; // we don't need any wifi settings from NVRAM
  wifi_promiscuous_filter_t filter = {.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT}; // we need only MGMT frames
  esp_wifi_init(&cfg);
  esp_wifi_set_country(&wifi_country);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);	// we don't need NVRAM
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_set_promiscuous_filter(&filter);	// set MAC frame filter
  esp_wifi_set_promiscuous_rx_cb(&wifiCallback);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);			// now switch on monitor mode
}



void wifiChannelHop() {
  uint8_t currentChannel;
  wifi_second_chan_t dummy;
  esp_wifi_get_channel(&currentChannel, &dummy);

  uint8_t newChannel = currentChannel + 1;
  if (newChannel > 13) {
    newChannel = 1;
  }

  esp_wifi_set_channel(newChannel, WIFI_SECOND_CHAN_NONE);
}

/*****************************************************************************/
void wifiParsePacket(SnifferPacket *snifferPacket) {

  wifi_header_frame_control_t frame = *(wifi_header_frame_control_t *) snifferPacket->data;
  if (frame.subtype != SUBTYPE_PROBE_REQUEST)
    return;

  uint8_t currentChannel;
  wifi_second_chan_t dummy;
  esp_wifi_get_channel(&currentChannel, &dummy);

  wifi_ieee80211_mac_hdr_t * mac_header = (wifi_ieee80211_mac_hdr_t *) snifferPacket->data;
  wifiAddDevice(snifferPacket->rx_ctrl.rssi, mac_header->macSender);
}

void wifiCallback(void *data, wifi_promiscuous_pkt_type_t type) {
  SnifferPacket *snifferPacket = (SnifferPacket*) data;
  if (type != WIFI_PKT_MGMT) return;
  wifiParsePacket(snifferPacket);
}

void printMAC(const uint8_t* mac) {
  char addr[] = "00:00:00:00:00:00";
  sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(addr);
}

/*****************************************************************************/
WifiDevice wifiDevices[WIFI_MAX_DEVICES];

void wifiInitDevices()
{
  for (int i = 0; i < WIFI_MAX_DEVICES; i++)
    wifiDevices[i].countDown = 0;
}

void wifiAddDevice(int rssi, const uint8_t * mac)
{
  int freePlace = -1;
  for (int i = 0; i < WIFI_MAX_DEVICES; i++) {
    WifiDevice * device = &wifiDevices[i];
    if (!device->countDown) {
      freePlace = i;
      continue;
    }
    if (memcmp(device->mac, mac, 6) == 0) {
      device->rssi = rssi;
      device->countDown = WIFI_MAC_TIMEOUT;
      return;
    }
  }

  if (freePlace < 0) return;
  WifiDevice * device = &wifiDevices[freePlace];
  device->rssi = rssi;
  memcpy(device->mac, mac, 6);
  device->countDown = WIFI_MAC_TIMEOUT;

  Serial.println("+");
  Serial.print("New device: ");
  printMAC(mac);
}

int wifiCountDevices()
{
  int count = 0;
  for (int i = 0; i < WIFI_MAX_DEVICES; i++) {
    if (wifiDevices[i].countDown) {
      wifiDevices[i].countDown --;
      if (!wifiDevices[i].countDown) {
        Serial.println("-");
        Serial.print("Del device: ");
        printMAC(wifiDevices[i].mac);
      } else {
        count++;
      }
    }
  }

  Serial.print(count);
  Serial.println();

  if (HAS_DISPLAY) {
    displayWifiCount(count);
  }
}

void displayWifiCount(int deviceCount) {

  u8x8.draw1x2String(0, 0, "Wifi-Devices:");
  char stringBuffer[5];
  u8x8.draw2x2String(0,3,itoa(deviceCount, stringBuffer, 10));
  
  if (deviceCount < 10) {
    u8x8.draw1x2String(2, 3, " ");
    u8x8.draw1x2String(3, 3, " ");
 }

  if (deviceCount < 100) {
    u8x8.draw1x2String(5, 3, " ");
    u8x8.draw1x2String(6, 3, " ");
  }
}
