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
    delay(200);

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

void stream_data_to_excel(float* tempData, float* freqData, int N) {
  Serial.println("Time (s),Temperature (C),Frequency (Hz),Magnitude");

  for (int i = 0; i < N; i++) {
    float time = i;
    float temperature = tempData[i];
    float frequency = (i < N / 2) ? (float)i * SampleRate / N : 0.0;
    float magnitude = (i < N / 2) ? freqData[i] : 0.0;

    Serial.print(time, 2); Serial.print(",");
    Serial.print(temperature, 2); Serial.print(",");
    Serial.print(frequency, 4); Serial.print(",");
    Serial.println(magnitude, 4);
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
    delay(2000);
    
    collect_temperature_data(); 

    send_data_to_pc();


}

void loop()
{        
   
}
