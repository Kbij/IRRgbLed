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

    red = 0x10;//0xFF;
    green = 0x10;//0x80;
    blue = 0x00;
    ws2812fx.setColor(red, green, blue);
    currentMode = FX_MODE_STATIC;
    
    ws2812fx.setMode(currentMode);//FX_MODE_RAINBOW_CYCLE);
   // ws2812fx.start();    
    lastLoop = millis();
    lastCommandTime = millis();
    powerOn = false;
}

void loop()
{
    int waitTimeMs = 100;
    unsigned long now = millis();

    if (now - lastLoop > 200)
    {
        if (irRecv.decode(&results))
        {
            unsigned long currentCommand = results.value & 0xFF;
           // int repeat = (results.value >> 15) && 0x01;
            bool repeat = ((now - lastCommandTime) < 500) && (currentCommand == lastCommand);

            Serial.print("Received: 0x");

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
                            ws2812fx.start();
                            Serial.println(F("Power On"));
                        }
                    }
                    break;
                }
                case INTENSITY_DOWN:
                {
                    brightness -= 255/BRIGHTNESS_STEPS;
                    if (brightness <= 0) brightness = 1;
                    ws2812fx.setBrightness(brightness);

                    Serial.println(F("Intensity Down"));
                    waitTimeMs = 500;
                    break;
                }
                case INTENSITY_UP:
                {
                    brightness += 255/BRIGHTNESS_STEPS;
                    if (brightness >= 255) brightness = 255;
                    ws2812fx.setBrightness(brightness);

                    Serial.println(F("Intensity Up"));
                    waitTimeMs = 500;
                    break;  
                }
                case NEXT_MODE:
                {
                    currentMode += 1;
                    if (currentMode > MODE_COUNT) currentMode = FX_MODE_STATIC;
                    ws2812fx.setMode(currentMode);
                    Serial.print(F("Next, Mode: "));
                    Serial.println(ws2812fx.getModeName(currentMode));
                    waitTimeMs = 1000;
                    break;
                }
                case PREV_MODE:
                {
                    currentMode -= 1;
                    if (currentMode < FX_MODE_STATIC) currentMode = FX_MODE_STATIC;
                    ws2812fx.setMode(currentMode);
                    Serial.print(F("Prev, Mode: "));
                    Serial.println(ws2812fx.getModeName(currentMode));
                    waitTimeMs = 1000;
                    break;
                }                
                case PRESET_1:
                {
                    Serial.println(F("Preset 1"));
                    waitTimeMs = 1000;
                    break;
                }            
                case PRESET_2:
                {
                    Serial.println(F("Preset 2"));
                    waitTimeMs = 1000;
                    break;
                }  
                case PRESET_3:
                {
                    Serial.println(F("Preset 3"));
                    waitTimeMs = 1000;
                    break;
                }  
                case PRESET_4:
                {
                    Serial.println(F("Preset 4"));
                    waitTimeMs = 1000;
                    break;
                }  
                case PRESET_5:
                {
                    Serial.println(F("Preset 5"));
                    waitTimeMs = 1000;
                    break;
                }  
                case PRESET_6:
                {
                    Serial.println(F("Preset 6"));
                    waitTimeMs = 1000;
                    break;
                }  
                case PRESET_7:
                {
                    Serial.println(F("Preset 7"));
                    waitTimeMs = 1000;
                    break;
                }  
                case PRESET_8:
                {
                    Serial.println(F("Preset 8"));
                    waitTimeMs = 1000;
                    break;
                }  
                case PRESET_9:
                {
                    Serial.println(F("Preset 9"));
                    waitTimeMs = 1000;
                    break;
                }                                                                                      
                case UD_RED:
                {
                    red += 255/COLOR_STEPS;
                    if (red >= 255) red = 255;
                    ws2812fx.setColor(red, green, blue);

                    Serial.println(F("Red Up"));
                    waitTimeMs = 500;                    
                    break;
                }
                case UD_GREEN:
                {
                    green += 255/COLOR_STEPS;
                    if (green >= 255) green = 255;
                    ws2812fx.setColor(red, green, blue);

                    Serial.println(F("Green Up"));
                    waitTimeMs = 500;                    
                    break;
                }
                case UD_BLUE:
                {
                    blue += 255/COLOR_STEPS;
                    if (blue >= 255) blue = 255;
                    ws2812fx.setColor(red, green, blue);

                    Serial.println(F("Blue Up"));
                    waitTimeMs = 500;                    
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
