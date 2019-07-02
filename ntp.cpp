#include "ntp.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

WiFiUDP Udp;
const char *serverName;
unsigned int localUDPPort = 8888; // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Sending NTP request...");
  // get a random server from the pool
  WiFi.hostByName(serverName, ntpServerIP);
  Serial.print(serverName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer

      uint8_t ntpVersion = (packetBuffer[0] & 0x38) >> 3;

      if (ntpVersion != 4) {
        Serial.print("Received NTP packet with invalid version ("); Serial.print(ntpVersion); Serial.println(" instead of 4), ignoring!");
        return 0;
      }

      // convert four bytes starting at location 40 to a long integer
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];

      if (secsSince1900 == 0) {
        Serial.println("Received NTP packet with null time, ignoring!");
        return 0;
      }

      Serial.println("Received valid NTP packet, syncing time.");

      return secsSince1900 - 2208988800UL;
    }
  }
  Serial.println("No NTP response!");
  return 0; // return 0 if unable to get the time
}

void setupNTP(const char *ntpServer, int syncInterval) {
  serverName = ntpServer;
  Udp.begin(localUDPPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(syncInterval);
}
