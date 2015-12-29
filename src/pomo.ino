// include the library code:
#include <LiquidCrystal.h>

// Global pin constants
const int buzzerPin = 9;
const int ledPin = 13;
const int buttonPin = 7;

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// State of the press button
int buttonState = 0;

const int songLength = 6;
char startNotes[] = "abcdefg";
char endNotes[] = "gfedcba";
int beats[] = {4,4,2,2,1,1,1};
int tempo = 150;

void setup()
{
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  lcd.begin(16, 2);
  lcd.print("pomodoro timer V0.1");
}

int played = 0;

void 
loop()
{
  int i, duration;

  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);

  buttonState = digitalRead(buttonPin);
  Serial.print(buttonState);
  Serial.print(" ");
  Serial.println(played);

  if (buttonState == 0 && played == 1) {
    played = 0;
  }

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (played == 0 && buttonState == HIGH) {
    
    // turn LED on
    digitalWrite(ledPin, HIGH);
    played = 1;

    for (i = 0; i < songLength; i++) {
      duration = beats[i] * tempo;  // length of note/rest in ms

      if (notes[i] == ' ') {
        delay(duration);
      } else {
        tone(buzzerPin, frequency(notes[i]), duration);
        delay(duration);
      }
      delay(tempo / 10); // brief pause between notes
    }
  } else { // turn LED off
    digitalWrite(ledPin, LOW);
  }
}


int 
frequency(char note)
{
  int i;
  const int numNotes = 8;  // number of notes we're storing

  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};
  
  for (i = 0; i < numNotes; i++) {
    if (names[i] == note) {
      return(frequencies[i]);
    }
  }
  return(0);
}
