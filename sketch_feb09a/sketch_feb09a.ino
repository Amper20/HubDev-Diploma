// libs
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

//Do not modify
int device_is_host = 1;
WiFiServer server(80);
WiFiClient wifiClient;


//Configurable
char dev_ssid[20] = "smart-light";
char dev_psswd[20] = "123321123";

char wifi_ssid[20] = "horseman";
char wifi_psswd[20] = "123321123";

char id[25] = "None";

char server_loc[30]= "http://192.168.0.101:5001/";

// headers
String http_request(char* url, char **params, int n);
void get_power(double x, int out[2]);
void parse_request(String s);

// Device specific Setup
int led_pin_hot = D5;
int led_pin_cold = D6;
int active_pin = D7;
bool first_start = 1;

unsigned long time_now = 0;
unsigned long curr_millis = 0;

unsigned long time_update_server_prev_millis = 0;
unsigned long time_update_server_rate = 30*60*1000; // 30 min

unsigned long time_update_local_prev_millis = 0;
unsigned long time_update_local_rate = 1000; //1s

unsigned long time_power_prev_millis = 0;
unsigned long time_power_update_rate = 1*5*1000; //5 s


void setup_(){
   pinMode(led_pin_hot, OUTPUT);
   pinMode(led_pin_cold, OUTPUT);
   pinMode(active_pin, INPUT);
   ap_down();
   Serial.begin(115200);
}

//Device specific actions
void work(){
    
    curr_millis = millis();

    if((curr_millis - time_update_server_prev_millis >= time_update_server_rate || first_start) && !device_is_host) {

        time_update_server_prev_millis = curr_millis;
    
        char url[60];
        strcpy(url, server_loc);
        strcat(url, "get_seconds_from_midnight");
        Serial.println(url);
        long time_from_server = http_request(url, NULL, 0).toInt();
        if(time_from_server >= 0)
            time_now = time_from_server;
    }

    if(curr_millis - time_power_prev_millis >= time_power_update_rate || first_start) {

        time_power_prev_millis = curr_millis;
        Serial.println(time_now);
        double time_now_hour = ((double)time_now)/(60.0*60.0);
        int power[2] = {0, 0};

        get_power(time_now_hour, power);
        
        analogWrite(led_pin_hot, power[0]);
        analogWrite(led_pin_cold, power[1]);

        Serial.println(power[0]);
        Serial.println(power[1]);
    }

    if(curr_millis - time_update_local_prev_millis >= time_update_local_rate) {
        int diff = curr_millis - time_update_local_prev_millis;
        time_update_local_prev_millis = curr_millis;
        time_now += diff/1000;
        Serial.println(time_now);
    }

    first_start = 0;
}

//power setup
double power_min = 0, power_max = 950;
double e = 2.71828;

double change_period = 0.5;
double cahnge_factor = change_period/10, day_factor_a = 0.5, day_factor_b = 0.5;

double sunrise_end = 7, sunset_end = 23;
double sunrise_start = sunrise_end - change_period, sunset_start = sunset_end - change_period;

void get_power(double x, int out[2]){
    
    if(x >= sunrise_start && x <= sunrise_end) {
        double x_local = (x - sunrise_start);
        double x_local_pow_a = change_period/2 - x_local;
        out[0] = (int)((power_max-power_min) * pow(e, (-(x_local_pow_a*x_local_pow_a)/cahnge_factor)));
        double x_local_pow_b = x_local - change_period;
        out[1] = (int)((power_max-power_min) * day_factor_b * pow(e, (-(x_local_pow_b*x_local_pow_b)/cahnge_factor)));
    }

    else if(x >= sunset_start && x <= sunset_end) {

        double x_local = (x - sunset_start);
        double x_local_pow_a = change_period/2 - x_local;
        out[0] = (int)((power_max-power_min) * pow(e, (-(x_local_pow_a*x_local_pow_a)/cahnge_factor)));
        double x_local_pow_b = x_local;
        out[1] = (int)((power_max-power_min) * day_factor_b * pow(e, (-(x_local_pow_b*x_local_pow_b)/cahnge_factor)));
    }
    else if(x >= sunrise_start && x <= sunset_end) {

        out[0] = (int)((power_max-power_min)*day_factor_a);
        out[1] = (int)((power_max-power_min)*day_factor_b);
    }
    else {
        out[0] = (int)power_min;
        out[1] = (int)power_min;
    }
}

typedef struct{
    char key[15];
    char type[10];
    void *value;
} param;

void parse_request(String s){
    
    // modify for more parameters
    int param_cnt = 9;
    param x[10] = {
        {"power_min", "double", &power_min},
        {"power_max", "double", &power_max},
        {"day_factor_a", "double", &day_factor_a},
        {"day_factor_b", "double", &day_factor_b},
        {"change_period", "double", &change_period},
        {"sunrise_end", "double", &sunrise_end},
        {"sunrise_start", "double", &sunrise_start},
        {"dev_id", "string", id},
        {"time_now", "ulong", &time_now},
    };
    if(strstr(s.c_str(), "setting") != NULL){
        for (int i = 0 ; i < param_cnt; i++){
            char *key = NULL;
            if(key = strstr(s.c_str(), x[i].key)){ // Key matches
                char value[10];
                key += strlen(x[i].key) + 1; // skip key and '='
                for(int j = 0; key && *key != '&' && *key != ' '; j++, key++, value[j] = '\0'){
                    value[j] = *key;
                }
                if(strstr(x[i].type, "double") != NULL ){
                    char *eptr;
                    *(double *)(x[i].value) = strtod(value, &eptr);
                }
                if(strstr(x[i].type, "int") != NULL ){
                    *(int *)(x[i].value) = atoi(value); 
                }
                if(strstr(x[i].type, "ulong") != NULL ){
                    *(unsigned long *)(x[i].value) = atoi(value); 
                }
                if(strstr(x[i].type, "string") != NULL ){
                    strcpy((char *)x[i].value, value);
                }
                Serial.print(x[i].key);
                Serial.print(" ");
                Serial.println(value);
            }
        }
    }
}

String http_request(char *url, char **params, int n){
    
    String payload;
    
    if(n) strcat(url, "?");
    for (int i = 0; i < n; i++){
        strcat(url, params[i]);
        if (i != n-1)
            strcat(url, "&");
    }
    
    Serial.println(url);

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(wifiClient, url);
        int httpCode = http.GET();
        if (httpCode == 200) {
            payload = http.getString();   
            Serial.println(payload);
        }else
            payload = "-1";
        http.end();
    }
    Serial.print("Payload: ");
    Serial.println(payload);
    return payload;
}

void setup() {
    setup_();
}

void ap_active(){
    if (!device_is_host) {
        Serial.println("Setting soft-AP ... ");
        WiFi.mode(WIFI_AP_STA);
        while(!device_is_host)
            device_is_host = WiFi.softAP(dev_ssid, dev_psswd) ? 1 : 0;
        server.begin();
    }
}

void ap_down(){
    if(device_is_host) {
        while(device_is_host)
            device_is_host = WiFi.softAPdisconnect(true) ? 1 : 0;
        WiFi.mode(WIFI_STA);
        first_start = 1;
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.begin(wifi_ssid, wifi_psswd);
            while (WiFi.status() != WL_CONNECTED) {
                delay(1000);
                Serial.print("...");
            }
        } else{
            Serial.println("connected to wifi");
        }
    }
}

void handle_server() {

    WiFiClient client = server.available();
    Serial.println(WiFi.softAPIP());
    delay(1000);
    if(!client) {
        return;
    }
    
    String request = client.readStringUntil('\r');
    
    Serial.println(request);
    client.flush();
    
    String value;
    
    if(request.indexOf("/setting/") != -1) {
        parse_request(request);
    }
    
    client.flush();

    String s = "HTTP/1.1 200 OK\r\n";
    s += "Content-Type: application/json\r\n\r\n";
    s += "{\"data\":{\"message\":\"success\",\"value\":\"";
    s += value;
    s += "\"}}\r\n";
    s += "\n";

    client.print(s);
    delay(1);
    Serial.println("Client disconnected");
    client.stop();
}

bool device_is_active(){
    return digitalRead(active_pin) == HIGH;
}

void loop() {

  if (device_is_active()) {
        ap_down();
  } else {
        ap_active();
        handle_server();
  }
  work();
}
