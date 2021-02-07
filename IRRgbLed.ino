#include <IRremote.h>
#include <WS2812FX.h>

#define LED_COUNT 149
#define LED_PIN A4
#define IR_PIN A5

IRrecv irRecv(IR_PIN);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

decode_results results;
unsigned long lastCommandTime;
unsigned long lastCommand;
unsigned long lastLoop;
uint8_t red;
uint8_t green;
uint8_t blue;

int currentMode;
int brightness;
bool powerOn;
int8_t sunriseState;
unsigned long lastSunRiseLoopTime;
uint8_t sunriseLevel;

#define INTENSITY_DOWN 0x11
#define INTENSITY_UP 0x10
#define NEXT_MODE 0x20
#define PREV_MODE 0x21
#define PRESET_1 0x01
#define PRESET_2 0x02
#define PRESET_3 0x03
#define PRESET_4 0x04
#define PRESET_5 0x05
#define PRESET_6 0x06
#define PRESET_7 0x07
#define PRESET_8 0x08
#define PRESET_9 0x09
#define BRIGHTNESS_STEPS 30
#define COLOR_STEPS 30
#define POWER_TOGGLE 0x0C
#define UD_RED 0x6D
#define UD_GREEN 0x6E
#define UD_BLUE 0x70
#define SUNRISE_ON 0x0D

#define SUNRISE_NONE -1
#define SUNRISE_UP 1
#define SUNRISE_DOWN 2


uint16_t mySunriseEffect(void)
{
    WS2812FX::Segment* seg = ws2812fx.getSegment(); // get the current segment

    if (sunriseState == SUNRISE_UP)
    {
        for(uint16_t i=seg->stop; i>seg->start; i--)
        {
            ws2812fx.setPixelColor(i, ws2812fx.getPixelColor(i-1));
        }

        uint8_t r = sunriseLevel;
        int g = sunriseLevel > 127 ? sunriseLevel - 127 : 0;
        int b = sunriseLevel > 200 ? sunriseLevel - 200 : 0;
        ws2812fx.setPixelColor(seg->start, r, g, b);

        if (sunriseLevel < 254)
        {
            ++sunriseLevel;
        }
    }
    else if (sunriseState == SUNRISE_DOWN)
    {
        for(uint16_t i=seg->start; i < seg->stop; i++)
        {
            ws2812fx.setPixelColor(i, ws2812fx.getPixelColor(i+1));
        }

        uint32_t color = ws2812fx.getPixelColor(seg->stop);
        int r = (color >> 16 & 0xFF);
        int g = (color >> 8  & 0xFF);
        int b = (color       & 0xFF);
        if (r > 0) --r;
        if (g > 0) --g;
        if (b > 0) --b;
        ws2812fx.setPixelColor(seg->stop, r, g, b);

        if (sunriseLevel > 0) --sunriseLevel;

        if (ws2812fx.getPixelColor(seg->start) == 0)
        {
            powerOn = false;
            sunriseState = SUNRISE_NONE;
            ws2812fx.setBrightness(200);
            ws2812fx.setMode(FX_MODE_STATIC);
            ws2812fx.stop();
            Serial.println(F("Sunrise Power Off"));
        }
    }

    return seg->speed; // return the delay until the next animation step (in msec)
}

void setup()
{
    Serial.begin(115200);
    irRecv.enableIRIn(); // Start the receiver

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_PIN);

    ws2812fx.init();
    brightness = 255;
    ws2812fx.setBrightness(brightness);
    ws2812fx.setSpeed(7000);

    red = 0xFF;//0xFF;
    green = 0xFF;//0x80;
    blue = 0x00;
    ws2812fx.setColor(red, green, blue);
    currentMode = FX_MODE_STATIC;

    ws2812fx.setMode(currentMode);//FX_MODE_RAINBOW_CYCLE);
    ws2812fx.setCustomMode(FX_MODE_CUSTOM_0, mySunriseEffect);
    lastLoop = millis();
    lastCommandTime = millis();
    powerOn = false;
    sunriseState = SUNRISE_NONE;
    sunriseLevel = 0;
}

void loop()
{
    unsigned long now = millis();

    if (now - lastLoop > 200)
    {
        if (irRecv.decode(&results))
        {
            unsigned long currentCommand = results.value & 0xFF;
            bool repeat = ((now - lastCommandTime) < 500) && (currentCommand == lastCommand);

            Serial.print("Received: 0x");
            Serial.print(currentCommand, HEX);
            Serial.print(", repeat: ");
            Serial.println(repeat, HEX);

            switch(currentCommand)
            {
                case POWER_TOGGLE:
                {
                    if (!repeat)
                    {
                        if (powerOn)
                        {
                            powerOn = false;
                            ws2812fx.stop();
                            Serial.println(F("Power Off"));
                        }
                        else
                        {
                            powerOn = true;
                            //When current mode = sunrise, set it to a static mode
                            if (ws2812fx.getMode() == FX_MODE_CUSTOM_0)
                            {
                                ws2812fx.setBrightness(200);
                                ws2812fx.setMode(FX_MODE_STATIC);
                            }
                            ws2812fx.start();
                            Serial.println(F("Power On"));
                        }
                    }
                    break;
                }
                case SUNRISE_ON:
                {
                    if (!repeat)
                    {
                        if (!powerOn)
                        {
                            Serial.println(F("Start Sunrise On"));
                            powerOn = true;
                            ws2812fx.setBrightness(255);
                            ws2812fx.setColor(0x00, 0x00, 0x00);
                            ws2812fx.setMode(FX_MODE_CUSTOM_0);
                            ws2812fx.setSpeed(1000);
                            ws2812fx.start();
                            sunriseState = SUNRISE_UP;
                            sunriseLevel = 0;
                        }
                        else
                        {
                            Serial.println(F("Start Sunrise Off"));
                            if (sunriseState == SUNRISE_UP)
                            {
                                sunriseState = SUNRISE_DOWN;
                                ws2812fx.setSpeed(20);
                            }
                        }
                    }

                    break;
                }
                case INTENSITY_DOWN:
                {
                    if (ws2812fx.getMode() != FX_MODE_CUSTOM_0)
                    {
                        brightness -= 255/BRIGHTNESS_STEPS;
                        if (brightness <= 0) brightness = 1;
                        ws2812fx.setBrightness(brightness);

                        Serial.println(F("Intensity Down"));
                    }
                    break;
                }
                case INTENSITY_UP:
                {
                    if (ws2812fx.getMode() != FX_MODE_CUSTOM_0)
                    {
                        brightness += 255/BRIGHTNESS_STEPS;
                        if (brightness >= 255) brightness = 255;
                        ws2812fx.setBrightness(brightness);

                        Serial.println(F("Intensity Up"));
                        lastSunRiseLoopTime = 0; //Force update
                    }
                    break;
                }
                case NEXT_MODE:
                {
                    currentMode += 1;
                    if (currentMode > MODE_COUNT) currentMode = FX_MODE_STATIC;
                    ws2812fx.setMode(currentMode);
                    Serial.print(F("Next, Mode: "));
                    Serial.println(ws2812fx.getModeName(currentMode));
                    break;
                }
                case PREV_MODE:
                {
                    currentMode -= 1;
                    if (currentMode < FX_MODE_STATIC) currentMode = FX_MODE_STATIC;
                    ws2812fx.setMode(currentMode);
                    Serial.print(F("Prev, Mode: "));
                    Serial.println(ws2812fx.getModeName(currentMode));
                    break;
                }
                case PRESET_1:
                {
                    Serial.println(F("Preset 1"));
                    break;
                }
                case PRESET_2:
                {
                    Serial.println(F("Preset 2"));
                    break;
                }
                case PRESET_3:
                {
                    Serial.println(F("Preset 3"));
                    break;
                }
                case PRESET_4:
                {
                    Serial.println(F("Preset 4"));
                    break;
                }
                case PRESET_5:
                {
                    Serial.println(F("Preset 5"));
                    break;
                }
                case PRESET_6:
                {
                    Serial.println(F("Preset 6"));
                    break;
                }
                case PRESET_7:
                {
                    Serial.println(F("Preset 7"));
                    break;
                }
                case PRESET_8:
                {
                    Serial.println(F("Preset 8"));
                    break;
                }
                case PRESET_9:
                {
                    Serial.println(F("Preset 9"));
                    break;
                }
                case UD_RED:
                {
                    red += 255/COLOR_STEPS;
                    if (red >= 255) red = 255;
                    ws2812fx.setColor(red, green, blue);
                    ws2812fx.show();

                    Serial.println(F("Red Up"));
                    break;
                }
                case UD_GREEN:
                {
                    green += 255/COLOR_STEPS;
                    if (green >= 255) green = 255;
                    ws2812fx.setColor(red, green, blue);
                    ws2812fx.show();

                    Serial.println(F("Green Up"));
                    break;
                }
                case UD_BLUE:
                {
                    blue += 255/COLOR_STEPS;
                    if (blue >= 255) blue = 255;
                    ws2812fx.setColor(red, green, blue);
                    ws2812fx.show();

                    Serial.println(F("Blue Up"));
                    break;
                }
            }

            lastCommandTime = now;
            lastCommand = currentCommand;
            irRecv.resume(); // Receive the next value
        }

        lastLoop = now;
    }

    if (irRecv.isIdle())
    {
        ws2812fx.service();
    }
}

