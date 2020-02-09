#pragma once

#include <esp_wifi.h>

#define HAS_DISPLAY true

#define SUBTYPE_PROBE_REQUEST 0x04

#define WIFI_CHANNEL_COUNT  13
#define WIFI_MAX_DEVICES    512
#define WIFI_MAC_TIMEOUT    30

struct wifi_header_frame_control_t{
    unsigned protocol:2;
    unsigned type:2;
    unsigned subtype:4;
    unsigned to_ds:1;
    unsigned from_ds:1;
    unsigned more_frag:1;
    unsigned retry:1;
    unsigned pwr_mgmt:1;
    unsigned more_data:1;
    unsigned wep:1;
    unsigned strict:1;
};

struct wifi_ieee80211_mac_hdr_t{
    wifi_header_frame_control_t frame_ctrl;
    uint8_t macReceiver[6]; /* receiver address */
    uint8_t macSender[6]; /* sender address */
    uint8_t macFilter[6]; /* filtering address */
    unsigned sequence_ctrl:16;
    uint8_t addr4[6]; /* optional */
};

#define DATA_LENGTH  112
struct SnifferPacket{
    wifi_pkt_rx_ctrl_t rx_ctrl;
    uint8_t data[DATA_LENGTH];
    uint16_t cnt;
    uint16_t len;
};

struct WifiDevice {
  int rssi;
  uint8_t mac[6];
  int countDown;
};

void wifiInit();
void wifiChannelHop();
void wifiParsePacket(SnifferPacket *snifferPacket);

void wifiInitDevices();
int wifiCountDevices();
void wifiAddDevice(int rssi, const uint8_t * mac);
void displayWifiCount(int deviceCount);

static void printMAC(const uint8_t* mac);
static void wifiCallback(void *data, wifi_promiscuous_pkt_type_t type);
