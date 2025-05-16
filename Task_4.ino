#include <math.h>

const int B = 4275000;            // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A0

const int initialSampleRate = 1;
const int SampleDuration = 60;

const float fluctuationThreshold = 0.5;

enum PowerMode { ACTIVE, IDLE, POWER_DOWN };
PowerMode currentMode = ACTIVE; 

float TempData[60];
float Magnitudes[30];
float lastTemp[10];
int SampleRate = initialSampleRate;
int SampleCount = SampleRate * SampleDuration;

void setup() {
  Serial.begin(9600);
  pinMode(pinTempSensor, INPUT);
  collect_temperature_data();
  adjust_sampling_rate();
}

void loop() {

  collect_temperature_data();
  send_data_to_pc();
  adjust_sampling_rate();
  
  delay(1000);
}


void collect_temperature_data() {
  static int index = 0;
  float temperature = 0;
  for (int i = 0; i < SampleCount; i++) {
    int a = analogRead(pinTempSensor);

    float R = 1023.0/a-1.0;
    R = R0*R;

    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; 

    TempData[i] = temperature;

  }

  if (index > 0) {
    lastTemp[index % 10] = TempData[index] - TempData[index - 1];
  }
  
  index = (index + 1) % 100;
}

void apply_dft(float* data, int N) {
  for (int k = 0; k < N / 2; k++) {
    float real = 0.0;
    float imag = 0.0;
    for (int n = 0; n < N; n++) {
      float angle = 2.0 * PI * k * n / N;
      real += data[n] * cos(angle);
      imag -= data[n] * sin(angle);
    }
    Magnitudes[k] = sqrt(real * real + imag * imag);
  }
}

void send_data_to_pc() {
  Serial.println("Time-Domain Data:");

  for (int i = 0; i < 60; i++) {
    Serial.print("Time = ");
    Serial.print(i);
    Serial.print("s, Temperature = ");
    Serial.print(TempData[i]);
    Serial.println(" °C");
    delay(1000);
  }
  
  Serial.println("Frequency-Domain Data:");

  apply_dft(TempData, 60);
  
  for (int i = 0; i < 30; i++) {
    float frequency = (float)i * SampleRate / 60;
    float magnitude = Magnitudes[i];
    Serial.print("Frequency = ");
    Serial.print(frequency);
    Serial.print(" Hz, Magnitude = ");
    Serial.println(magnitude);
    delay(200);
  }
}


void adjust_sampling_rate() {

  float avgTempChange = 0;
  for (int i = 0; i < 10; i++) {
    avgTempChange += abs(lastTemp[i]);
  }
  avgTempChange /= 10;
  
  if (avgTempChange > fluctuationThreshold) {
    currentMode = ACTIVE;
    SampleRate = max(2, SampleRate);
  } else if (avgTempChange > fluctuationThreshold / 2) {
    currentMode = IDLE;
    SampleRate = max(1, SampleRate - 1);
  } else {
    currentMode = POWER_DOWN;
    SampleRate = 1;
  }

  Serial.print("Current Power Mode: ");
  switch (currentMode) {
    case ACTIVE: Serial.println("ACTIVE"); break;
    case IDLE: Serial.println("IDLE"); break;
    case POWER_DOWN: Serial.println("POWER_DOWN"); break;
  }
  Serial.print("Sample Rate: ");
  Serial.println(SampleRate);
}

