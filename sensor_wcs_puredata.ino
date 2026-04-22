long data;
long value1 = 0;
unsigned long times1 = 0;
unsigned long times2 = 0;
int bpm = 0;

const int sensorPin = A0;
const int numRead = 20;

int minVal = 0;
int maxVal = 1023;
int threshold = 5;

bool debugText = false;   // true = moniteur série, false = Pure Data

void setup() {
  Serial.begin(9600);
}

void loop() {
  data = analogRead(sensorPin);

  long senseSum = 0;
  for (int k = 0; k < numRead; k++) {
    senseSum += analogRead(sensorPin);
  }

  long senseAve = senseSum / numRead;

  // Calcul BPM conservé
  BPM(senseAve);

  // Anti-bruit plus léger
  static long lastSent = 0;
  if (abs(senseAve - lastSent) > threshold) {
    lastSent = senseAve;
  }

  // Limite la plage utile
  long clipped = constrain(lastSent, minVal, maxVal);

  // Conversion 0-1023 -> 0-255 pour Pure Data
  byte outByte = map(clipped, minVal, maxVal, 0, 255);

  if (debugText) {
    // Mode texte pour le moniteur série
    Serial.print("senseAve = ");
    Serial.print(senseAve);
    Serial.print(" | outByte = ");
    Serial.print(outByte);
    Serial.print(" | bpm = ");
    Serial.println(bpm);
  } else {
    // Mode binaire pour Pure Data
    Serial.write(outByte);
  }

  delay(10);
}

long BPM(long value) {
  if (value1 > value) {
    value1 -= 1;
  }
  else if (value1 < value) {
    times2 = millis();

    if (times2 > times1) {
      int newBpm = 60000 / (times2 - times1);

      if (newBpm <= 180 && newBpm > 0) {
        bpm = newBpm;
        times1 = times2;
      }
    }

    value1 = value;
  }

  return value1;
}
