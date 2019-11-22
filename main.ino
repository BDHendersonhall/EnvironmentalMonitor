#include <Adafruit_DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11

unsigned long lastPublish = 0; // A time variable to know when we published to the cloud last
double temp = 0; // Current temperature
double humidity = 0; // Current humidity
char tempC[15]; // empty var to store temp as a char so we can publish to .publish(), since it requires a char and not an int
char humidityC[15]; // empty var to store humidity as a char so we can publish to .publish(), since it requires a char and not an int
        
short int maxTempThreshold = 90; // Maximum ideal temperature, default value of 90 degrees
short int minTempThreshold = 80; // Minimum ideal temperature, default value of 80 degrees

int publishInterval = 15 * 60; // Temperature and humidity publish frequency, default value of 15 minutes

DHT dht(DHTPIN, DHTTYPE);

bool shouldPublish() {
        return Time.now() > lastPublish + publishInterval;
}

void publishAction() {
        Particle.publish("Temperature", tempC, PRIVATE); // Publish the temperature as an event
        Particle.publish("Humidity", humidityC, PRIVATE); // Publish the humidity as an event
        lastPublish = Time.now();
}

void changeRGBColor() {
    if(temp > maxTempThreshold) {
        RGB.color(255, 0, 0);   // If temperature exceeds maximum ideal temperature, change LED to RED
    } else if(temp < minTempThreshold) {
        RGB.color(0, 0, 255);   // If temperature falls below minimum ideal temperature, change LED to BLUE
    } else {
        RGB.color(0, 255, 0);   // Default state of GREEN when temperature is within ideal range
    }
}

int setMaxTemp(String userValue) {  // Function for setting the maximum ideal or GREEN temperature threshold
    maxTempThreshold = userValue.toInt();
    return maxTempThreshold; 
}

int setMinTemp(String userValue) {  // Function for setting the minimum ideal or GREEN temperature threshold
    minTempThreshold = userValue.toInt();
    return minTempThreshold; 
}

int changeInterval(String userValue) {  // Function for setting publish frequency in one minute increments
    publishInterval = userValue.toInt() * 60;
    return publishInterval; 
}

void setup() {
    Particle.variable("Temperature", tempC);    // Enables console to pull current Temperature value
    Particle.variable("Humidity", humidityC);   // Enables console to pull current Humidity value
    Particle.function("Set_Max_Temp", setMaxTemp);  // Allows setting maximum ideal or GREEN temperature threshold in console
    Particle.function("Set_Min_Temp", setMinTemp);  // Allows setting minimum ideal or GREEN temperature threshold in console
    Particle.function("Set_Interval", changeInterval);  // Allows setting frequency of publish events to console in one minute increments
    RGB.control(true);  // Enables control of device's integrated RGB LED
}

void loop() {
    if(shouldPublish()) {
        publishAction();
    }
    changeRGBColor();
    humidity = dht.getHumidity();
    temp = dht.getTempFarenheit();
    if(temp!=0) {
        sprintf(tempC, "%.1f", temp); // assign temp as a char to tempC
        sprintf(humidityC, "%.1f", humidity); // assign humidity as a char to humidityC
    }
    delay(100);
}
