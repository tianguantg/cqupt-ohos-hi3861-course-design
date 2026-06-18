#ifndef COMFORT_FAN_NETWORK_CONFIG_H
#define COMFORT_FAN_NETWORK_CONFIG_H

/*
 * Local demo configuration template.
 * Copy this file to network_config.h and fill in your own Wi-Fi credentials.
 * Do not commit real passwords to a public repository.
 */

#define CF_WIFI_SSID       "YOUR_WIFI_SSID"
#define CF_WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

/* For Windows Mobile Hotspot, this is usually tcp://192.168.137.1:1883. */
#define CF_MQTT_URI        "tcp://192.168.137.1:1883"
#define CF_MQTT_USERNAME   ""
#define CF_MQTT_PASSWORD   ""

#define CF_DEVICE_ID       "qihang01"

#endif
