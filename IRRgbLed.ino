#include <IRremote.h>
#include <WS2812FX.h>

#define LED_COUNT 149
#define LED_PIN A4
#define IR_PIN A5

IRrecv irRecv(IR_PIN);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

decode_results results;
unsigned long lastCommand;
unsigned long lastLoop;
int currentMode;
int brightness;
#define POWER_OFF 0xFFA25D
#define POWER_ON 0xFFE21D
#define INTENSITY_DOWN 0x0FFE01F
#define INTENSITY_UP 0xFFA857
#define NEXT_MODE 0xFF906F
#define PRESET_1 0xFF30CF
#define PRESET_2 0xFF18E7
#define PRESET_3 0xFF7A85
#define PRESET_4 0xFF10EF
#define PRESET_5 0xFF38C7
#define PRESET_6 0xFF5AA5
#define PRESET_7 0xFF42BD
#define PRESET_8 0xFF4AB5
#define PRESET_9 0xFF52AD
#define BRIGHTNESS_STEPS 30;

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
    ws2812fx.setColor(0xFF, 0x80, 0x00);
    currentMode = FX_MODE_STATIC;
    
    ws2812fx.setMode(currentMode);//FX_MODE_RAINBOW_CYCLE);
   // ws2812fx.start();    
    lastLoop = millis();
}

void loop()
{
    int waitTimeMs = 100;
    unsigned long now = millis();

    if (now - lastLoop > 100)
    {
         Serial.println("processing loop");

        if (irRecv.decode(&results))
        {
           // Serial.println(results.value, HEX);
            unsigned long currentCommand = results.value;

            if (currentCommand == REPEAT)
            {
                currentCommand = lastCommand;
            }

            switch(currentCommand)
            {
                case POWER_OFF:
                {
                    ws2812fx.stop();
                    Serial.println(F("Power Off"));
                    waitTimeMs = 100;
                    break;
                }
                case POWER_ON:
                {
                    ws2812fx.start();
                    Serial.println(F("Power On"));
                    waitTimeMs = 100;
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
                    Serial.println(F("Next Mode"));
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
                case REPEAT: 
                {
                    Serial.println(F("Repeat Last"));
                    break;
                }
            }

            if (results.value != REPEAT)
            {
                lastCommand = results.value;
            }

            irRecv.resume(); // Receive the next value
        }

        lastLoop = now;
    }        
    
    if (irRecv.isIdle())
    {
        ws2812fx.service();
    }
}
