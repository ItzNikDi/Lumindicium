#include "globals.h"

US100 sonar(Serial2);
Config gConfig;
State gState;
WavBuffer gWavYellow;
WavBuffer gWavRed;
portMUX_TYPE gMux = portMUX_INITIALIZER_UNLOCKED;
AsyncWebServer server(80);
AsyncEventSource events("/events");