# Arduino-Coursework

Task 1:
All tasks completed successfully. Merge completed successfully, had trouble initially and had to unmerge and remerge. This was because within the workspace 
the tab was saying the merge had been completed successfully, however wihtin the actions bar, the test verification was not showing this. Therefore I unmerged the 
two branches and merged them again. After doing this it showed that the merge had been completed successfully wihtin actions through task_verification.
Along with this the test branch was deleted successfully.

Task 2:
All multiple choice questions answered via gaining understanding through the website. Along with this, connections for arduino, temp sensor and base shield 
were made.
Solutions:
a2_1_1     :'NTC Thermistor'
a2_1_2     :'A raw voltage value (0-1023)'
a2_1_3     :'To linearize the thermistor response using the Steinhart-Hart equation'
a2_1_4     :'To initialize communication between Arduino and Serial Monitor'
a2_1_5     :'To increase the sampling frequency'
a2_1_6     :'The resistance of the thermistor'
a2_1_7     :'The sensor will stop measuring temperature'
a2_1_8     :'Stores the computed temperature in Celsius'
a2_1_9     :'It represents the temperature coefficient of the thermistor'
a2_1_10    :'The temperature reading becomes unstable or returns an error'
a2_1_11    :'All of the above'
a2_1_12    :'Use an SD card module'
a2_1_13    :'Use Serial.print() with comma-separated values and save via Serial Monitor'
a2_1_14    :'False'
a2_1_15    :'Change `delay(100);` to `delay(2000);`'

Task 3:
3.1-3.4 code:
# include <math.h>

const int B = 4275000;            // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A0

const int SampleRate = 1;      
const int DurationSecs = 181;     
const int SampleCount = SampleRate * DurationSecs;    

float TempData [SampleCount];
float Magnitudes [SampleCount];

# if defined(ARDUINO_ARCH_AVR)
# define debug  Serial
# elif defined(ARDUINO_ARCH_SAMD) ||  defined(ARDUINO_ARCH_SAM)
# define debug  SerialUSB
# else
# define debug  Serial
# endif

#define ACTIVE 0
#define IDLE 1
#define POWER_DOWN 2

void collect_temperature_data(){
    Serial.println("Temperate data being collected for 3 mins...");

    for (int i = 0; i < SampleCount; i++) {
        int a = analogRead(pinTempSensor);

        float R = 1023.0/a-1.0;
        R = R0*R;

        float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet

        TempData[i] = temperature;


        Serial.print("sample");
        Serial.print(i);
        Serial.print(": ");
        Serial.print("temperature = ");
        Serial.print(temperature);
        Serial.println(" °C");

        delay(1000);
    }
        
    Serial.println("Temperature data collection finished.");

}

float* apply_dft(float* data, int N, float fs) {
    for(int k=0; k < N; k++) {
        float real = 0.0;
        float imag = 0.0;

        for (int n=0; n < N; n++) {
            float angle = 2.0 * PI *k * n / N;
            real += data[n] * cos(angle);
            imag -= data[n] * sin(angle);
        }

        Magnitudes[k] = sqrt((real * real) + (imag* imag));
    }

    return Magnitudes;
}

int decide_power_mode(float* magnitudes, int N, float fs) {
  float totalWeightedFreq = 0.0;
  float totalMagnitude = 0.0;

  for (int k = 1; k < N / 2; k++) { 
    float frequency = (float)k * fs / N;
    float magnitude = magnitudes[k];

    totalWeightedFreq += frequency * magnitude;
    totalMagnitude += magnitude;
  }

  float avgFreq = 0.0;
  if (totalMagnitude > 0.0) {
    avgFreq = totalWeightedFreq / totalMagnitude;
  }

  Serial.print("Average frequency = ");
  Serial.print(avgFreq, 4);
  Serial.println(" Hz");

  if (avgFreq > 0.5) {
    return ACTIVE;
  } else if (avgFreq > 0.1) {
    return IDLE;
  } else {
    return POWER_DOWN;
  }
}

void send_data_to_pc() {
   Serial.println("Time-Domain Data:");
    for (int i = 0; i < SampleCount; i++) {
        float time = i;
        float temp = TempData[i];

        Serial.print ("time= ");
        Serial.print(time);
        Serial.print(" s, ");
        Serial.print("Temperature = ");
        Serial.print(temp);
        Serial.println(" °C");

        delay(200);
    }

    float* dft_result = apply_dft(TempData, SampleCount, SampleRate);

    Serial.println("Frequency-Domain Data");

    for (int k = 0; k < SampleCount / 2; k++) {
        float frequency = (float)k * SampleRate / SampleCount;
        float magnitude = dft_result[k];

        Serial.print("Frequency= ");
        Serial.print(frequency);
        Serial.print(" Hz, ");
        Serial.print("Magnitude= ");
        Serial.print(magnitude);
        Serial.println(" °C"); 
        
        delay(200);

    }

  int mode = decide_power_mode(dft_result, SampleCount, SampleRate);
  if (mode == ACTIVE) {
    Serial.println("Mode Indicator: ACTIVE");
  } else if (mode == IDLE) {
    Serial.println("System Mode: IDLE");
  } else {
    Serial.println("Mode Indicator: POWER_DOWN");
  }

}


void setup()
{
    Serial.begin(9600);
    
    collect_temperature_data(); 

    send_data_to_pc();
}

void loop()
{        
   
}

Temperature, Time, Frequncy and Magnitude readings/calculations completed successfully. Along with this the correct power mode was chosen depending on the 
readings obtained. When doing this I had trouble with getting the data to come out clearly therefore had to add lots of delays. This lead to the data being 
recorded in the correct intervals and also stopped any formatting issues within the serial monitor.

Task 3.5:
I struggled with this significantly as all though I was able to connect with excel data streamer automattically I couldn't get the data and tables to format correctly. This resulted in the data being challenging to read on there. This is something that I spent a long time attempting and was unssuccessful with getting
the headers to format automatically.
Code:
void stream_data_to_excel(float* tempData, float* freqData, int N) 
  {
    Serial.println("Time (s),Temperature (C),Frequency (Hz),Magnitude");

    for (int i = 0; i < N / 2; i++) {
      float time = i;                                      
      float temperature = tempData[i];                     
      float frequency = (float)i * SampleRate / N;         
      float magnitude = freqData[i];                       

      Serial.print(time, 2); Serial.print(",");
      Serial.print(temperature, 2); Serial.print(",");
      Serial.print(frequency, 4); Serial.print(",");
      Serial.println(magnitude, 4);
    }

Task 3.6:
Due to having poor formatting on excel of my data as stated in the previous questions the graphs were unable to be produced. However I made the code for how I would have completed this task if I was able to format the tables successfully within excel.
Code:
import os
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

from google.colab import files
uploaded = files.upload()

filename = list(uploaded.keys())[0]  

data = pd.read_csv(filename)

print(data.head())  

plt.figure(figsize=(10, 4))
plt.plot(data['Time (s)'], data['Temperature (C)'], color='blue')
plt.title('Temperature vs Time')
plt.xlabel('Time (seconds)')
plt.ylabel('Temperature (°C)')
plt.grid(True)
plt.tight_layout()
plt.show()


plt.figure(figsize=(10, 4))
plt.plot(data['Frequency (Hz)'], data['Magnitude'], color='green')
plt.title('Frequency Spectrum of Temperature Data')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude')
plt.grid(True)
plt.tight_layout()
plt.show()

# Adjust the file path for your .csv file.
file_path = r"C:\Users\user\Downloads\threeminstemperature_F428226.csv.csv"


Task 4:
Code:
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

When firs producing this code I used large amounts of memory, which led to the code being very inefficient. Therfore, I reduced array sizes where I could and also reused arrays. For example I reused TempData[] and other arrays within this on various occassions, resulting in more optimised memory useage.
