// include the library code:
#include <LiquidCrystal.h>
#include <Time.h>
#include <TimeAlarms.h>

// Global pin constants
const int buzzerPin = 9;
const int ledPin = 13;
const int buttonPin = 7;

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// State of the press button
int buttonState = 0;
int lastButtonState = 0;

const int songLength = 7;
char startNotes[] = "abcdefg";
char endNotes[] = "gfedcba";
char startBreakNotes[] = "agagaga";
char endBreakNotes[] = "gagagag";
int beats[] = {4,4,2,2,1,1,1};
int tempo = 100;

struct PomoStats {
  int numPomos;
  int pomoTime;
  int pomoBlockLength;
  int pomoBlockBreakTime;
};

struct State {
  bool inPomo;
  int timeLeft;
  int startTime;
  int endTime;
};

// 25min = 1,500,000ms
// 5min = 300,000ms
PomoStats pomoStats;
State state;

void
playStartTune()
{
  int i, duration;
  for (i = 0; i < songLength; i++) {
      duration = beats[i] * tempo;  // length of note/rest in ms

      if (startNotes[i] == ' ') {
        delay(duration);
      } else {
        tone(buzzerPin, frequency(startNotes[i]), duration);
        delay(duration);
      }
      delay(tempo / 10); // brief pause between notes
    }
}

void
playEndTune()
{
  int i, duration;
  for (i = 0; i < songLength; i++) {
      duration = beats[i] * tempo;  // length of note/rest in ms

      if (endNotes[i] == ' ') {
        delay(duration);
      } else {
        tone(buzzerPin, frequency(endNotes[i]), duration);
        delay(duration);
      }
      delay(tempo / 10); // brief pause between notes
    }
}

void
playStartBreakTune()
{
  int i, duration;
  for (i = 0; i < songLength; i++) {
      duration = beats[i] * tempo;  // length of note/rest in ms

      if (startBreakNotes[i] == ' ') {
        delay(duration);
      } else {
        tone(buzzerPin, frequency(startBreakNotes[i]), duration);
        delay(duration);
      }
      delay(tempo / 10); // brief pause between notes
    }
}

void
playEndBreakTune()
{
  int i, duration;
  for (i = 0; i < songLength; i++) {
      duration = beats[i] * tempo;  // length of note/rest in ms

      if (endBreakNotes[i] == ' ') {
        delay(duration);
      } else {
        tone(buzzerPin, frequency(endBreakNotes[i]), duration);
        delay(duration);
      }
      delay(tempo / 10); // brief pause between notes
    }
}

void 
setup()
{
  // Set up serial output
  Serial.begin(9600);
  
  // Configure GPIO
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Configure the LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("POMO V1");
  
  // Initialize pomo state
  state.inPomo = false;
  state.timeLeft = 0;
  pomoStats.numPomos = 0;
  pomoStats.pomoTime = 4; // 1500
  pomoStats.pomoBlockLength = 1000;
  pomoStats.pomoBlockBreakTime = 3; // 300
}

void 
endPomoBreak()
{
  playEndBreakTune();
  state.inPomo = false;
}

void
startPomoBreak()
{
  playStartBreakTune();
  state.timeLeft = pomoStats.pomoBlockBreakTime;
  state.startTime = millis() / 1000;
  state.endTime = state.startTime + state.timeLeft; 
}

bool 
stopPomo()
{
  Serial.println("STOP POMO");
  
  playEndTune();
  pomoStats.numPomos++;
  Serial.println(pomoStats.numPomos);
  if (pomoStats.numPomos % pomoStats.pomoBlockLength != 0) {
    state.inPomo = false;
    return false;
  } else {
    return true;
  } 
}

void 
startPomo()
{
  playStartTune();
  state.inPomo = true;
  state.timeLeft = pomoStats.pomoTime;
  state.startTime = millis() / 1000;
  state.endTime = state.startTime + state.timeLeft;
}

void 
clearTime()
{
  lcd.setCursor(6, 1);
   lcd.print("Time: ");
  lcd.setCursor(11, 1);
  lcd.print("     ");
}

void printNumber()
{
  lcd.setCursor(0, 1);
  lcd.print("NUM=");
  lcd.print(pomoStats.numPomos);
}

void 
loop()
{
  lcd.setCursor(8, 0);
  lcd.print("[PAUSED]");
  printNumber();
  clearTime();
    
  buttonState = digitalRead(buttonPin);
//  Serial.println(buttonState);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (state.inPomo == false && buttonState == LOW) {
    lastButtonState = buttonState;
    lcd.setCursor(8, 0);
    lcd.print("[*LIVE*]");
    printNumber();
    clearTime();
    digitalWrite(ledPin, HIGH);
    
    Serial.println("START POMO");

    startPomo();
    int current = state.startTime;
    int timeElapsed = 0;
    int endTime = state.endTime;
    while (current < endTime) { // wait for the time to expire
      delay(100); 
      timeElapsed = (millis() / 1000) - state.startTime;
      lcd.setCursor(11, 1);
      lcd.print(state.timeLeft - timeElapsed);
      lcd.print("s");
      
      current = millis() / 1000;
    }
    digitalWrite(ledPin, LOW);

    bool takeBreak = stopPomo();    
    if (takeBreak) {
      lcd.setCursor(0, 1);
      lcd.print("NUM=");
      lcd.print(pomoStats.numPomos);
      lcd.setCursor(8, 0);
      lcd.print("[Zzz..]");
      clearTime();
      
      startPomoBreak();
      int current = state.startTime;
      int timeElapsed = 0;
      int endTime = state.endTime;

      while (current < endTime) {
        delay(100);
        timeElapsed = (millis() / 1000) - state.startTime;
        lcd.setCursor(11, 1);
        lcd.print(state.timeLeft - timeElapsed);
        lcd.print("s");
      
        current = millis() / 1000;
      }
      endPomoBreak();
    }
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
