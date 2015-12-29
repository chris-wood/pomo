// include the library code:
#include <LiquidCrystal.h>
#include <SimpleTimer.h>

// Global pin constants
const int buzzerPin = 9;
const int ledPin = 13;
const int buttonPin = 7;

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// State of the press button
int buttonState = 0;

const int songLength = 7;
char startNotes[] = "abcdefg";
char endNotes[] = "gfedcba";
char startBreakNotes[] = "agagaga";
char endBreakNotes[] = "gagagag";
int beats[] = {4,4,2,2,1,1,1};
int tempo = 150;

struct PomoStats {
  int numPomos;
  int pomoTime;
  int pomoBlockLength;
  int pomoBlockBreakTime;
};

struct State {
  bool inPomo;
  int timerId;
}

// 
SimpleTimer timer;

// 25min = 1,500,000ms
// 5min = 300,000ms
PomoStats pomoStats = {0, 1500000, 4, 300000};
PomoState state = {false, 0};

void
playTune(int songlength, char notes[songlength])
{
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
}

void 
setup()
{
  // Set up serial output
  Serial.begin(9600);
  
  // Configure GPIO
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Configure the LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  lcd.print("pomodoro timer V0.1");
  
  // Initialize pomo state
  state.inPomo = false;
}

void 
endPomoBreak()
{
  playTune(songlength, endBreakNotes);
  state.inPomo = false;
}

void 
stopPomo()
{
  playTune(songlength, stopNotes);
  pomoStats.numPomos++;
  if (pomoStats.numPomos % pomoStats.pomoBlockLength == 0) {
    state.inPomo = false;
  } else {
    playTune(songlength, startBreakNotes);
    state.timerid = timer.setTimeout(pomoStats.pomoBlockBreakTime, endPomoBreak);
  }
  
}

void 
startPomo()
{
  state.inPomo = true;
  state.timerid = timer.setTimeout(pomoStats.pomoTime, stopPomo);
  playTune(songlength, startNotes);
}

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
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (state.inPomo == false && buttonState == HIGH) {
    digitalWrite(ledPin, HIGH);
    startPomo();
  } else if (state.inPomo == false) { 
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
