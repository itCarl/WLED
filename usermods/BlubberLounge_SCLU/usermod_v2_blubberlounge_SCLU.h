#pragma once

#include "wled.h"
/*
 *
 *
 *
 *
 */

typedef struct Laser {
  int8_t id;
  int8_t pin;
  bool   isOn = true;
  bool   isOutput = true;
} Laser;

// typedef struct test {
//   int number;
// } test;





/*
 *
 *
 *
 *
 *
 */
class UsermodBlubberLoungeSCLU : public Usermod 
{
  private:
    /* configuration (available in API and stored in flash) */
    bool enabled = false;
    int8_t currentEffect = 3;
    static const int8_t totalLaser = 8;


    /* runtime variables */
    bool initDone = false;
    unsigned long segment_delay_ms = 150;

    //Private class members. You can declare variables and functions only accessible to your usermod here
    unsigned long lastTime = 0;

    PinManagerPinType LaserPins[totalLaser];
    Laser laserList[totalLaser];

    // strings to reduce flash memory usage (used more than twice)
    static const char _name[];
    static const char _enabled[];
    static const char _alias[];

    /*
     *  setup all pins and laser objects
     */
    void createLaserList(uint8_t laserCount, PinManagerPinType* pinManagerPins, Laser* initLaser) 
    {
      // TODO at pins for the other chips later
      // available pins for esp8266
      uint8_t avai_pins[8] = {0, 1, 4, 5, 12, 13, 14, 15};

      // check if microchip has enough pins available
      // if(laserCount > sizeof(avai_pins)/sizeof(*avai_pins) ) 
      // {
      //   DEBUG_PRINTLN(F("Error: not enough pins available"));
      //   // limit the total laser
      //   totalLaser = sizeof(avai_pins)/sizeof(*avai_pins);
      // } 

      for(uint8_t i = 0; i < *(&avai_pins + 1) - avai_pins; i++) 
      {
        initLaser[i].id = i;
        initLaser[i].pin = avai_pins[i];

        pinManagerPins[i].pin = initLaser[i].pin;
        pinManagerPins[i].isOutput = initLaser[i].isOutput;
      }

      // D1mini labels and GPIO
      // PinManagerPinType LaserPins[totalLaser] = {
      //     {  0, true },    // D3 - GPIO0
      //     {  1, true },    // TX - GPIO1
      //     {  4, true },    // D4 - GPIO4
      //     {  5, true },    // D1 - GPIO5
      //     { 12, true },    // D6 - GPIO12
      //     { 13, true },    // D7 - GPIO13
      //     { 14, true },    // D5 - GPIO14
      //     { 15, true },    // D8 - GPIO14
      // };
    }

    // int a = 10;
    // test b[2];
    // int* aptr = &a;

    // void teeest(int &c, test *d) {
    //   c = 11;
    //   d[0].number = 34;
    // }

  public:
    //Functions called by WLED

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() 
    {
      // DEBUG_PRINTLN( a );
      // DEBUG_PRINTLN( (int)aptr );
      // DEBUG_PRINTLN( b[0].number );

      // teeest(a, &b[0]);

      // DEBUG_PRINTLN( a );
      // DEBUG_PRINTLN( b[0].number );

      createLaserList(totalLaser, LaserPins, laserList);

      // when WLED_DEBUG is defined this will throw an Error
      // IO1 is reserved for Debugging see wled.cpp line 311
      if (!pinManager.allocateMultiplePins(LaserPins, totalLaser, PinOwner::UM_BLUBBERLOUNGE_SCLU)) 
      {
        DEBUG_PRINTLN(F("Error: not all pins got allocated"));

      }

      initDone = true;
    }


    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() 
    {
      //Serial.println("Connected to WiFi!");
    }


    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     * 
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     *    Additionally, "if (WLED_MQTT_CONNECTED)" is available to check for a connection to an MQTT broker.
     * 
     * 2. Try to avoid using the delay() function. NEVER use delays longer than 10 milliseconds.
     *    Instead, use a timer check as shown here.
     */
    void loop() 
    {
      if (!enabled || strip.isUpdating()) return;

      updateLaser();
    }

    void updateLaser() 
    {
      if ((millis() - lastTime) > segment_delay_ms) 
      {
        lastTime = millis();
      }
    }


    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     * Below it is shown how this could be used for e.g. a light sensor
     */
    void addToJsonInfo(JsonObject& root)
    {
      JsonObject sclu = root["u"];
      if (sclu.isNull()) sclu = root.createNestedObject("u");

      JsonArray usermodEnabled = sclu.createNestedArray("Shisha coaster Laser");  // name
      
      String btn = F("<button class=\"btn infobtn cbtn\" onclick=\"requestJson({sclu:{enabled:");
      if (enabled) {
        btn += F("false}});\"style=\"background-color:var(--cc-g);\">");
        btn += F("enabled");
      } else {
        btn += F("true}});\"style=\"background-color:var(--cc-r);\">");
        btn += F("disabled");
      }
      btn += F("</button>");
      usermodEnabled.add(btn);
    }


    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject& root)
    {
      JsonArray sclu = root[FPSTR(_alias)];
      if (sclu.isNull()) sclu = root.createNestedArray(FPSTR(_alias));

      /* laser states */
      JsonArray laser = sclu.createNestedArray();
      for(uint8_t i=0; i < totalLaser; i++) {
        JsonObject las = laser.createNestedObject();
        las["id"] = laserList[i].id;
        las["on"] = laserList[i].isOn;
      }

      /* add effect to the GUI laser effect list */
      JsonArray effects = sclu.createNestedArray();
      for(uint8_t i=0; i < 7; i++) {
        JsonObject fx = effects.createNestedObject();
        fx["id"] = i;
        fx["name"] = "Effect #"+(String)(i+1);
      }

      JsonObject meta = sclu.createNestedObject();
      meta["fx"] = currentEffect;

      DEBUG_PRINT(F("---DEBUG "));
      DEBUG_PRINT(FPSTR(_name));
      DEBUG_PRINTLN(F("---"));
      DEBUG_PRINTLN(F("JsonState exposed in API."));
    }


    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root)
    {
      if (!initDone) return;  // prevent crash on boot applyPreset()

      JsonObject sclu = root[FPSTR(_alias)];
      if (!sclu.isNull()) 
      {
        // if (sclu[FPSTR(_enabled)].is<bool>()) 
        // {
        //   enabled = sclu[FPSTR(_enabled)].as<bool>();
        // } else 
        // {
        //   String str = sclu[FPSTR(_enabled)]; // checkbox -> off or on
        //   enabled = (bool)(str!="off"); // off is guaranteed to be present
        // }
        
        // currentEffect = sclu[2]["fx"];
        // DEBUG_PRINT(F("!!! currentFX: "));
        // DEBUG_PRINTLN(currentEffect);

        for(uint8_t i=0; i < totalLaser; i++) 
        {
          if(laserList[i].id == (int8_t)sclu["id"])
          {
            laserList[i].isOn = sclu["on"];

            // DEBUG_PRINT(F("!!! Laser ID: "));
            // DEBUG_PRINTLN(laserList[i].id);
            // DEBUG_PRINT(F("!!! Laser State: "));
            // DEBUG_PRINTLN((bool)laserList[i].isOn);
          }
        }

      }


      DEBUG_PRINT(F("---DEBUG "));
      DEBUG_PRINT(FPSTR(_name));
      DEBUG_PRINTLN(F("---"));
      DEBUG_PRINTLN(F("state read from API."));
    }


    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     * 
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     * 
     * addToConfig() will make your settings editable through the Usermod Settings page automatically.
     *
     * Usermod Settings Overview:
     * - Numeric values are treated as floats in the browser.
     *   - If the numeric value entered into the browser contains a decimal point, it will be parsed as a C float
     *     before being returned to the Usermod.  The float data type has only 6-7 decimal digits of precision, and
     *     doubles are not supported, numbers will be rounded to the nearest float value when being parsed.
     *     The range accepted by the input field is +/- 1.175494351e-38 to +/- 3.402823466e+38.
     *   - If the numeric value entered into the browser doesn't contain a decimal point, it will be parsed as a
     *     C int32_t (range: -2147483648 to 2147483647) before being returned to the usermod.
     *     Overflows or underflows are truncated to the max/min value for an int32_t, and again truncated to the type
     *     used in the Usermod when reading the value from ArduinoJson.
     * - Pin values can be treated differently from an integer value by using the key name "pin"
     *   - "pin" can contain a single or array of integer values
     *   - On the Usermod Settings page there is simple checking for pin conflicts and warnings for special pins
     *     - Red color indicates a conflict.  Yellow color indicates a pin with a warning (e.g. an input-only pin)
     *   - Tip: use int8_t to store the pin value in the Usermod, so a -1 value (pin not set) can be used
     *
     * See usermod_v2_auto_save.h for an example that saves Flash space by reusing ArduinoJson key name strings
     * 
     * If you need a dedicated settings page with custom layout for your Usermod, that takes a lot more work.  
     * You will have to add the setting to the HTML, xml.cpp and set.cpp manually.
     * See the WLED Soundreactive fork (code and wiki) for reference.  https://github.com/atuline/WLED
     * 
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    void addToConfig(JsonObject& root)
    {
      JsonObject sclu = root[FPSTR(_alias)];
      if (sclu.isNull()) sclu = root.createNestedObject(FPSTR(_alias));

      sclu[FPSTR(_enabled)] = enabled;

      DEBUG_PRINT(F("---DEBUG "));
      DEBUG_PRINT(FPSTR(_name));
      DEBUG_PRINTLN(F("---"));
      DEBUG_PRINTLN(F("config saved."));
    }


    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     * 
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     * 
     * Return true in case the config values returned from Usermod Settings were complete, or false if you'd like WLED to save your defaults to disk (so any missing values are editable in Usermod Settings)
     * 
     * getJsonValue() returns false if the value is missing, or copies the value into the variable provided and returns true if the value is present
     * The configComplete variable is true only if the "exampleUsermod" object and all values are present.  If any values are missing, WLED will know to call addToConfig() to save them
     * 
     * This function is guaranteed to be called on boot, but could also be called every time settings are updated
     */
    bool readFromConfig(JsonObject& root)
    {
      JsonObject sclu = root[FPSTR(_alias)];
      if (sclu.isNull()) {
        DEBUG_PRINT(F("--- DEBUG "));
        DEBUG_PRINT(FPSTR(_name));
        DEBUG_PRINTLN(F("---"));
        DEBUG_PRINTLN(F(": No config found. (Using defaults.)"));
        return false;
      }

     
      enabled = sclu[FPSTR(_enabled)] | enabled;


      DEBUG_PRINT(F("--- DEBUG "));
      DEBUG_PRINT(FPSTR(_name));
      DEBUG_PRINTLN(F("---"));
      if (!initDone) {
        // first run: reading from cfg.json
        DEBUG_PRINTLN(F(" config loaded."));
      } else {

      }


      // bool configComplete = !top.isNull();
      // configComplete &= getJsonValue(top["great"], userVar0);
      // configComplete &= getJsonValue(top["testBool"], testBool);
      // configComplete &= getJsonValue(top["testULong"], testULong);
      // configComplete &= getJsonValue(top["testFloat"], testFloat);
      // configComplete &= getJsonValue(top["testString"], testString);

      // // A 3-argument getJsonValue() assigns the 3rd argument as a default value if the Json value is missing
      // configComplete &= getJsonValue(top["testInt"], testInt, 42);  
      // configComplete &= getJsonValue(top["testLong"], testLong, -42424242);
      // configComplete &= getJsonValue(top["pin"][0], testPins[0], -1);
      // configComplete &= getJsonValue(top["pin"][1], testPins[1], -1);

      // use "return !configComplete["newestParameter"].isNull();" when updating Usermod with new features
      return true;
    }

   
    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_BLUBBERLOUNGE_SCLU;
    }
};

// strings to reduce flash memory usage (used more than twice)
const char UsermodBlubberLoungeSCLU::_name[]                      PROGMEM = "Shisha coaster Laser";
const char UsermodBlubberLoungeSCLU::_alias[]                     PROGMEM = "sclu";
const char UsermodBlubberLoungeSCLU::_enabled[]                   PROGMEM = "enabled";