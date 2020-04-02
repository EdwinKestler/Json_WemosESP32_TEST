const char *NTP_Server = "0.north-america.pool.ntp.org";

struct WConfig
{
    char    SSID[12]= "HITRON-2F50";
    char    password[13]= "PSZ2ZFT2BF77";
};

struct Settings
{
    /* data */
    int NTP_port;
    int NTP_interval;
    char MQTT_SErver[30];
    char MQTT_Port[6] = "1883";
    char MQTT_User[24];
    char MQTT_Password[24];
    char NTPClient_Server[29] = "0.north-america.pool.ntp.org";
};

struct Settings Connection;

int main()
{
    Connection.NTP_port = 3600;
    Connection.NTP_interval = 60000;
    return 0;
}


