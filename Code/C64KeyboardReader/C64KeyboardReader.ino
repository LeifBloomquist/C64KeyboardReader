// Arduino Micro Commodore Keyboard Reader + HID Device

#include <HID.h>
#include <Keyboard.h>

#define PIN_INPUT_RESTORE 12 // Connector Pin 3

// Using the Simon Inns layout (see Docs/600px-C64_Keyboard_Schematics_PNG.png)

#define PIN_INPUT_COLA   2   // Connector Pin 12
#define PIN_INPUT_COLB   3   // Connector Pin 11
#define PIN_INPUT_COLC   4   // Connector Pin 10
#define PIN_INPUT_COLD   5   // Connector Pin 9
#define PIN_INPUT_COLE   6   // Connector Pin 8
#define PIN_INPUT_COLF   7   // Connector Pin 7
#define PIN_INPUT_COLG   8   // Connector Pin 6
#define PIN_INPUT_COLH   9   // Connector Pin 5

#define PIN_OUTPUT_ROW0 10   // Connector Pin 20
#define PIN_OUTPUT_ROW1 11   // Connector Pin 19 
#define PIN_OUTPUT_ROW2 A5   // Connector Pin 18
#define PIN_OUTPUT_ROW3 A4   // Connector Pin 17
#define PIN_OUTPUT_ROW4 A3   // Connector Pin 16
#define PIN_OUTPUT_ROW5 A2   // Connector Pin 15
#define PIN_OUTPUT_ROW6 A1   // Connector Pin 14
#define PIN_OUTPUT_ROW7 A0   // Connector Pin 13

#define PIN_OUTPUT_LED   13

// Connector pin 1 to GND
// Connector pin 2 is key (no pin)
// Connector pin 4 is +5V

#define NUM_KEYS      65 // To include restore key as virtual key #64
#define COMMODORE_KEY 5
#define RESTORE_KEY   64

// Array of characters
// Refer to https://www.arduino.cc/en/Reference/KeyboardModifiers for modifier keys

char keycodes[NUM_KEYS] = { '1', '`', KEY_LEFT_CTRL, KEY_ESC, ' ', KEY_LEFT_GUI , 'q', '2', // COLA
                            '3', 'w', 'a', KEY_LEFT_SHIFT, 'z', 's', 'e', '4',              // COLB
                            '5', 'r', 'd', 'x', 'c', 'f', 't', '6',                         // COLC
                            '7', 'y', 'g', 'v', 'b', 'h', 'u', '8',                         // COLD
                            '9', 'i', 'j', 'n', 'm', 'k', 'o', '0',                         // COLE
                            '+', 'p', 'l', ',', '.', ':', '@', '-',                         // COLF
                            '&', '*', ';', '/', KEY_RIGHT_ALT, '=', '^', KEY_HOME,          // COLG 
                            KEY_BACKSPACE, KEY_RETURN, KEY_RIGHT_ARROW, KEY_DOWN_ARROW,     // COLH
                            KEY_F1, KEY_F3, KEY_F5, KEY_F7, 
                            ' ' };                                                          // RESTORE 

bool keys[NUM_KEYS] = { 0 };       // Current state of keyboard matrix
bool last_keys[NUM_KEYS] = { 0 };  // Previous state of keyboard matrix, for edge detection

void setup()
{
  // Set pin directions
  pinMode(PIN_INPUT_RESTORE, INPUT_PULLUP);
  pinMode(PIN_INPUT_COLA, INPUT_PULLUP);
  pinMode(PIN_INPUT_COLB, INPUT_PULLUP);
  pinMode(PIN_INPUT_COLC, INPUT_PULLUP);
  pinMode(PIN_INPUT_COLD, INPUT_PULLUP);
  pinMode(PIN_INPUT_COLE, INPUT_PULLUP);
  pinMode(PIN_INPUT_COLF, INPUT_PULLUP);
  pinMode(PIN_INPUT_COLG, INPUT_PULLUP);
  pinMode(PIN_INPUT_COLH, INPUT_PULLUP);

  pinMode(PIN_OUTPUT_ROW0, OUTPUT);
  pinMode(PIN_OUTPUT_ROW1, OUTPUT);
  pinMode(PIN_OUTPUT_ROW2, OUTPUT);
  pinMode(PIN_OUTPUT_ROW3, OUTPUT);
  pinMode(PIN_OUTPUT_ROW4, OUTPUT);
  pinMode(PIN_OUTPUT_ROW5, OUTPUT);
  pinMode(PIN_OUTPUT_ROW6, OUTPUT);
  pinMode(PIN_OUTPUT_ROW7, OUTPUT);

  pinMode(PIN_OUTPUT_LED,  OUTPUT);

  // Set default output states
  digitalWrite(PIN_OUTPUT_ROW0, HIGH);
  digitalWrite(PIN_OUTPUT_ROW1, HIGH);
  digitalWrite(PIN_OUTPUT_ROW2, HIGH);
  digitalWrite(PIN_OUTPUT_ROW3, HIGH);
  digitalWrite(PIN_OUTPUT_ROW4, HIGH);
  digitalWrite(PIN_OUTPUT_ROW5, HIGH);
  digitalWrite(PIN_OUTPUT_ROW6, HIGH);
  digitalWrite(PIN_OUTPUT_ROW7, HIGH);

  digitalWrite(PIN_OUTPUT_LED, LOW);

  Keyboard.begin();
  Keyboard.releaseAll();
}

void loop()
{
  // 1. Scan
  Scan();

  // 2. Commodore key is handled specially
  if (keys[COMMODORE_KEY] == 0) 
  {
 //    HandleCommodoreKey();   
  }
  else if (keys[RESTORE_KEY] == 0)
  {
     HandleRestoreKey();
  }
  else
  {
    // 2. Edge detection for normal key handling
    for (int k = 0; k < NUM_KEYS; k++)
    {
      if (k == RESTORE_KEY) continue;  // Ignore, handled above

      if (KeyPressed(k))   // Falling edge
      {
        Keyboard.press(keycodes[k]);   
        digitalWrite(PIN_OUTPUT_LED, HIGH);
      }

      if (KeyReleased(k))  // Rising edge
      {
        Keyboard.release(keycodes[k]);
      }
    }
  }
                                                                                              
  // 3. Store key state for next iteration
  for (int k = 0; k < NUM_KEYS; k++)
  {
    last_keys[k] = keys[k];
  }

  // 4. Debounce - a simple delay seems to suffice	 
  delay(10); 

  digitalWrite(PIN_OUTPUT_LED, LOW);
}

void Scan()
{
  for (int row = 0; row <= 7; row++)
  {
    // Set current row to LOW, rest to HIGH

    digitalWrite(PIN_OUTPUT_ROW0, (row == 0) ? LOW : HIGH);
    digitalWrite(PIN_OUTPUT_ROW1, (row == 1) ? LOW : HIGH);
    digitalWrite(PIN_OUTPUT_ROW2, (row == 2) ? LOW : HIGH);
    digitalWrite(PIN_OUTPUT_ROW3, (row == 3) ? LOW : HIGH);
    digitalWrite(PIN_OUTPUT_ROW4, (row == 4) ? LOW : HIGH);
    digitalWrite(PIN_OUTPUT_ROW5, (row == 5) ? LOW : HIGH);
    digitalWrite(PIN_OUTPUT_ROW6, (row == 6) ? LOW : HIGH);
    digitalWrite(PIN_OUTPUT_ROW7, (row == 7) ? LOW : HIGH);

    int offset = (row * 8);

    keys[offset + 0] = digitalRead(PIN_INPUT_COLA);
    keys[offset + 1] = digitalRead(PIN_INPUT_COLB);
    keys[offset + 2] = digitalRead(PIN_INPUT_COLC);
    keys[offset + 3] = digitalRead(PIN_INPUT_COLD);
    keys[offset + 4] = digitalRead(PIN_INPUT_COLE);
    keys[offset + 5] = digitalRead(PIN_INPUT_COLF);
    keys[offset + 6] = digitalRead(PIN_INPUT_COLG);
    keys[offset + 7] = digitalRead(PIN_INPUT_COLH);
  }

  keys[RESTORE_KEY] = digitalRead(PIN_INPUT_RESTORE);
}

bool KeyPressed(int k)
{
  return ((last_keys[k] == 1) && (keys[k] == 0));
}

bool KeyReleased(int k)
{
  return ((last_keys[k] == 0) && (keys[k] == 1));
}

// Special functions with Commodore Key
void HandleCommodoreKey()
{
  // A fun macro (Commodore + Q)
  if (KeyPressed(6))
  {
     Keyboard.print("My hovercraft is full of eels\n");
     keys[COMMODORE_KEY] = 1;   // Suppress release event
     return;
  }

  // TODO options for switching keyboard layouts, etc.

}

// Special functions with Restore
void HandleRestoreKey()
{
    Keyboard.releaseAll();
}