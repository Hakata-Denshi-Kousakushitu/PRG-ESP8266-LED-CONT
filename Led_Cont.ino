// Ver 0.9 2015/10/15 02:10
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "AppleMidi.h"
#include <Adafruit_NeoPixel.h>

IPAddress ip(192, 168, 2, 82);//192.168.2.81～192.168.2.89
char module_name[] ="N0.2";//No.1（192.68.2.1)～No.9(192.168.2.89)

char Version[]="Ver0.9";

#define PIN 5 //GPIO5

#define WIPE  72
#define TH_CHASE  74
#define RAINBOW 76
#define TH_CH_RAIN  77
#define NOTE_OFF  0
#define NOTE_NULL  255

unsigned char Red[3] = {255, 0, 0};
unsigned char Orange[3] = {255, 127, 0};
unsigned char Kon[3] = {32, 64, 150};
unsigned char Blue[3] = {0, 0, 250};
unsigned char White[3] = {240, 240, 240};
unsigned char Black[3] = {0, 0, 0};
unsigned char Cyan[3] = {0, 255, 255};

unsigned char *Col_ptr;

char ssid[] = "logitec70"; //  your network SSID (name)
char pass[] = "F5DFFC8G2DBB2";    // your network password (use for WPA, or use as key for WEP)
//char ssid[] = "elecom2g-be43ed"; //  your network SSID (name)
//char pass[] = "1243665377722";    // your network password (use for WPA, or use as key for WEP)

unsigned long t0 = millis();
bool isConnected = false;

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI); // see definition in AppleMidi_Defs.h
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);

unsigned char midi_event_flag = NOTE_OFF;
unsigned char pre_event = NOTE_OFF;
bool restart_pat = true;

void setup() {
  // Serial communications and wait for port to open:
  Serial.begin(115200);

  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  */
  WiFi.config(IPAddress(ip), WiFi.gatewayIP(), WiFi.subnetMask()); 
  WiFi.begin(ssid, pass);

  Serial.println();
  Serial.print(Version);
 
  Serial.println();
  Serial.print("Getting IP address...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");


  Serial.println();
  Serial.print("IP address is ");
  Serial.println(WiFi.localIP());

  Serial.println("OK, now make sure you an rtpMIDI session that is Enabled");
  Serial.print("Add device named Arduino with Host/Port ");
  Serial.print(WiFi.localIP());
  Serial.println(":5004");
  Serial.println("Then press the Connect button");
  Serial.println("Then open a MIDI listener (eg MIDI-OX) and monitor incoming notes");

  // Create a session and wait for a remote host to connect to us
  AppleMIDI.begin(module_name);

  AppleMIDI.OnConnected(OnAppleMidiConnected);
  AppleMIDI.OnDisconnected(OnAppleMidiDisconnected);

  AppleMIDI.OnReceiveNoteOn(OnAppleMidiNoteOn);
  AppleMIDI.OnReceiveNoteOff(OnAppleMidiNoteOff);

  //Serial.println("Sending NoteOn/Off of note 45, every second");
  
  // LED init
  Col_ptr = Black;

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  unsigned long interval;

  // Listen to incoming notes
  AppleMIDI.run();

  // send a note every second
  // (dont cáll delay(1000) as it will stall the pipeline)

  //midi_event_flag = TH_CH_RAIN;//TEST
  //Col_ptr = Cyan;

  if (midi_event_flag == RAINBOW)
  {
    interval = 20;
  }
  else
  {
    interval = 50;
  }
  if ((millis() - t0) > interval) //50ms
  {
    t0 = millis();
    //Serial.print(".");
    if (pre_event != midi_event_flag) //バカ避け NOTE OFFが来なくて、別のパターンの場合
    {
      restart_pat = true;
    }
    switch (midi_event_flag)
    {
      case WIPE:
        Wipe();
        break;
      case TH_CHASE:
        TheaterChase();
        break;
      case RAINBOW:
        Rainbow();
        break;
      case TH_CH_RAIN:
        theaterChaseRainbow();
        break;
      case NOTE_OFF:
        All_off();
        break;
      default:
        break;
    }
    pre_event = midi_event_flag;
  }
}

// ====================================================================================
// Event handlers for incoming MIDI messages
// ====================================================================================

// -----------------------------------------------------------------------------
// rtpMIDI session. Device connected
// -----------------------------------------------------------------------------
void OnAppleMidiConnected(char* name) {
  isConnected  = true;
    Serial.print("Connected to session ");
    Serial.println(name);
}

// -----------------------------------------------------------------------------
// rtpMIDI session. Device disconnected
// -----------------------------------------------------------------------------
void OnAppleMidiDisconnected() {
  isConnected  = false;
    Serial.println("Disconnected");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOn(byte channel, byte note, byte velocity) {
  Serial.print("Incoming NoteOn from channel:");
  Serial.print(channel);
  Serial.print(" note:");
  Serial.print(note);
  Serial.print(" velocity:");
  Serial.print(velocity);
  Serial.println();

  switch (note)
  {
    case WIPE:  //C4
      midi_event_flag = WIPE;
      break;
    case TH_CHASE:  //D4
      midi_event_flag = TH_CHASE;
      if (velocity < 13) //10%未満
      {
        Col_ptr = Black;
      }
      else if (velocity < 26)
      {
        Col_ptr = Red;
      }
      else if (velocity < 39)
      {
        Col_ptr = Orange;
      }
      else if (velocity < 51)
      {
        Col_ptr = Kon;
      }
      else if (velocity < 64)
      {
        Col_ptr = Blue;
      }
      else if (velocity < 77) //60%未満
      {
        Col_ptr = White;
      }
      else //if (velocity <89)
      {
        Col_ptr = Cyan;
      }
      break;
    case RAINBOW:  //E4
      midi_event_flag = RAINBOW;
      break;
    case TH_CH_RAIN: //F4
      midi_event_flag = TH_CH_RAIN;
      break;
    default:
      break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOff(byte channel, byte note, byte velocity) {
  Serial.print("Incoming NoteOff from channel:");
  Serial.print(channel);
  Serial.print(" note:");
  Serial.print(note);
  Serial.print(" velocity:");
  Serial.print(velocity);
  Serial.println();

  midi_event_flag = NOTE_OFF;
  restart_pat = true;
}

// ====================================================================================
// Subroutine for LED
// Fill the dots one after the other with a color
// ====================================================================================
void All_off()
{
  int i;
  uint32_t c;

  c = strip.Color(0, 0, 0);
  for (i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
  }
  strip.show();
  midi_event_flag = NOTE_NULL;
}
/*
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
*/
void Wipe(void)
{
  static int i = 0;
  static int j = 0;
  static int loop_end;
  static uint32_t c;

  if (restart_pat == true)
  {
    i = 0;
    loop_end = strip.numPixels();
    switch (j)
    {
      case 0:
        c = strip.Color(255, 0, 0);
        break;
      case 1:
        c = strip.Color( 0, 255, 0);
        break;
      case 2:
        c = strip.Color( 0, 0, 255);
        break;
      default:
        break;
    }
    j++;
    if (j >= 3) j = 0;
    restart_pat = false;
  }
  strip.setPixelColor(i, c);
  strip.show();
  i++;
  if (i >= loop_end)
  {
    restart_pat = true;
  }
}
/*
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
*/
void Rainbow(void)
{
  static int j = 0;

  if (restart_pat == true)
  {
    j = 0;
    restart_pat = false;
  }
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i + j) & 255));
  }
  strip.show();
  j++;
  if (j >= 256) j = 0;
}
/*
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
*/
/*
//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
*/
void TheaterChase(void)
{
  static int j = 0;
  static int k = 0;
  static int q = 0;
  static bool even = true;
  static uint32_t c;

  if (restart_pat == true)
  {
    j = 0;
    q = 0;
    even = true;
    switch (k)
    {
      case 0:
        c = strip.Color(Col_ptr[0], Col_ptr[1], Col_ptr[2]);
        break;
      case 1:
        c = strip.Color(Col_ptr[0], Col_ptr[1], Col_ptr[2]);
        //c = strip.Color(Col_ptr[0], 0, 0);
        break;
      case 2:
        c = strip.Color(Col_ptr[0], Col_ptr[1], Col_ptr[2]);
        //c = strip.Color(0, 0, Col_ptr[2]);
        break;
      default:
        break;
    }
    k++;
    if (k >= 3) k = 0;
    restart_pat = false;
  }
  if (even == true)
  {
    for (int i = 0; i < strip.numPixels(); i = i + 3) {
      strip.setPixelColor(i + q, c);  //turn every third pixel on
    }
    strip.show();
    even = false;
  }
  else
  {
    for (int i = 0; i < strip.numPixels(); i = i + 3) {
      strip.setPixelColor(i + q, 0);      //turn every third pixel off
    }
    even = true;
    q++;
    if (q >= 3)
    {
      q = 0;
      j++;
      if (j >= 10)
      {
        j = 0;
      }
    }
  }
}
/*
//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
*/
void theaterChaseRainbow(void)
{
  static int j = 0;
  static int q = 0;
  static bool even = true;
  
  if (restart_pat == true)
  {
    j = 0;
    q = 0;
    even = true;
    restart_pat = false;
  }
  if (even == true)
  {
    for (int i = 0; i < strip.numPixels(); i = i + 3) {
      strip.setPixelColor(i + q, Wheel( (i+j) % 255));  //turn every third pixel on
    }
    strip.show();
    even = false;
  }
  else
  {
    for (int i = 0; i < strip.numPixels(); i = i + 3) {
      strip.setPixelColor(i + q, 0);      //turn every third pixel off
    }
    even = true;
    q++;
    if (q >= 3)
    {
      q = 0;
      j++;
      if (j >= 256)
      {
        j = 0;
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
