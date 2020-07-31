/*
   Weather station example for e-radionica.com Inkplate 6
   For this example you will need only USB cable and Inkplate 6.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 6 to display API data,
   in this example Metaweather public weather API which provide weather info.
   As a result, you get a functional weather station which shows today's
   forecast and 3 days forecast on your Inkplate display. 

   IMPORTANT:
   Make sure you have changed your desired city, timezone and WiFi credentials below.
   You will also need to install ArduinoJSON library.
   Download from here: https://github.com/bblanchon/ArduinoJson

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   28 July 2020 by e-radionica.com
*/

// ---------------- CHANGE HERE  ---------------:

int timeZone = 2; //Update your timezone here. 2 means UTC+2

char city[128] = "ZAGREB"; //Enter city name you wish to get forecast for

// Change to your WiFi SSID and password
char *ssid = "";
char *pass = "";

#define DELAY_MS 15000 //Delay between screen refreshes goes here

// ----------------------------------------------

#include "Inkplate.h"
#include "Network.h"  //Header file for easier code readability
//Include custom fonts & icons used
#include "Fonts/Roboto_Light_48.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_120.h"
#include "icons.h" //Generated using embedded iconConvert.py script

Inkplate display(INKPLATE_1BIT); //Inkplate object

Network network;  // All our network functions are in this object, see Network.h

//Constants used for drawing icons
char abbrs[32][16] = {"sn", "sl", "h", "t", "hr", "lr", "s", "hc", "lc", "c"};
const uint8_t *logos[16] = {icon_sn, icon_sl, icon_h, icon_t, icon_hr, icon_lr, icon_s, icon_hc, icon_lc, icon_c};

//Variables for storing temperature
char temps[8][4] = {
    "0F",
    "0F",
    "0F",
    "0F",
};

//Variables for storing days of the week
char days[8][4] = {
    "",
    "",
    "",
    "",
};

long refreshes = 0;  //Variable for counting partial refreshes
const int fullRefresh = 10;  //Constant to determine when to full update

//Variables for storing current time and weather info
char currentTemp[16] = "0F";
char currentWind[16] = "0m/s";
char currentTime[16] = "9:41";
char currentWeather[32] = "-";
char currentWeatherAbbr[8] = "th";

// functions defined below
void drawWeather();
void drawCurrent();
void drawTemps();
void drawCity();
void drawTime();

void setup()
{
    //Begin serial and and begin display
    Serial.begin(115200);
    display.begin();   //Call this function only once!

    //Initial cleaning of buffer and physical screen
    display.clearDisplay();
    display.clean();

    //Calling our begin from network.h file
    network.begin(city);

    //If city not found, write error message and stop
    if (network.location == -1)
    {
        display.setCursor(50, 290);
        display.setTextSize(3);
        display.print(F("City not in Metaweather Database"));
        display.display();
        while (1)
            ;
    }

    //Welcome screen
    display.setCursor(50, 290);
    display.setTextSize(3);
    display.print(F("Welcome to Inkplate 6 weather example!"));
    display.display();

    //Wait a bit before proceeding
    delay(3000);
}

void loop()
{
    //Clear display
    display.clearDisplay();

    //Get all relevant data, see Network.cpp for info
    network.getTime(currentTime);
    network.getDays(days[0], days[1], days[2], days[3]);
    network.getData(city, temps[0], temps[1], temps[2], temps[3], currentTemp, currentWind, currentTime, currentWeather, currentWeatherAbbr);

    //Draw data, see functions below in Network.cpp for info
    drawWeather();
    drawCurrent();
    drawTemps();
    drawCity();
    drawTime();

    //Refresh full screen every fullRefresh times
    if (refreshes % fullRefresh == 0)
        display.display();
    else
        display.partialUpdate();

    //Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);
    (void)esp_light_sleep_start();
    ++refreshes;
}

//Function for drawing weather info
void drawWeather()
{
    // Searching for weather state abbreviation
    for (int i = 0; i < 10; ++i)
    {
        //If found draw specified icon, draw it
        if (strcmp(abbrs[i], currentWeatherAbbr) == 0)
            display.drawBitmap(50, 50, logos[i], 152, 152, BLACK);
    }

    //Draw current weather state
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);
    display.setCursor(40, 270);
    display.println(currentWeather);
}

//Function for drawing current time
void drawTime()
{
    //Drawing current time stored in currentTime variable
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(800 - 20 * strlen(currentTime), 35);
    display.println(currentTime);
}

//Function for drawing city name
void drawCity()
{
    //Drawing city name
    display.setTextColor(BLACK, WHITE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(400 - 9 * strlen(city), 570);
    display.println(city);
}

//Function for drawing temperatures
void drawTemps()
{
    //Drawing 4 black rectangles into which temperatures will be written
    int rectWidth = 150;
    int rectSpacing = (800 - rectWidth * 4) / 5;

    display.fillRect(1 * rectSpacing + 0 * rectWidth, 300, rectWidth, 220, BLACK);
    display.fillRect(2 * rectSpacing + 1 * rectWidth, 300, rectWidth, 220, BLACK);
    display.fillRect(3 * rectSpacing + 2 * rectWidth, 300, rectWidth, 220, BLACK);
    display.fillRect(4 * rectSpacing + 3 * rectWidth, 300, rectWidth, 220, BLACK);

    int textMargin = 6;

    //Setting font specifics, writing the actual weather info
    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);

    display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, 300 + textMargin + 70);
    display.println(days[0]);

    display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, 300 + textMargin + 70);
    display.println(days[1]);

    display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, 300 + textMargin + 70);
    display.println(days[2]);

    display.setCursor(4 * rectSpacing + 3 * rectWidth + textMargin, 300 + textMargin + 70);
    display.println(days[3]);

    //Drawing temperature values into black rectangles
    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);

    display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, 300 + textMargin + 160);
    display.print(temps[0]);
    display.println(F("C"));

    display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, 300 + textMargin + 160);
    display.print(temps[1]);
    display.println(F("C"));

    display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, 300 + textMargin + 160);
    display.print(temps[2]);
    display.println(F("C"));

    display.setCursor(4 * rectSpacing + 3 * rectWidth + textMargin, 300 + textMargin + 160);
    display.print(temps[3]);
    display.println(F("C"));
}

//Current weather drawing function
void drawCurrent()
{
    //Drawing current information

    // Temperature:
    display.setFont(&Roboto_Light_120);
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);

    display.setCursor(245, 150);
    display.print(currentTemp);

    int x = display.getCursorX();
    int y = display.getCursorY();

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);

    display.setCursor(x, y);
    display.println(F("C"));

    //Wind:
    display.setFont(&Roboto_Light_120);
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);

    display.setCursor(480, 150);
    display.print(currentWind);

    x = display.getCursorX();
    y = display.getCursorY();

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);

    display.setCursor(x, y);
    display.println(F("m/s"));

    //Labels underneath
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(215, 210);
    display.println(F("TEMPERATURE"));

    display.setCursor(500, 210);
    display.println(F("WIND SPEED"));
}
