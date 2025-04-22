// Pin assignments
const int RPWM = 5;  // Right PWM pin for BTS7960
const int LPWM = 6;  // Left PWM pin for BTS7960
const int REN = 7;   // Right Enable pin
const int LEN = 8;   // Left Enable pin
const int supplyPin = A0;  // Analog pin to read LiPo voltage from the voltage divider

// Constants
const float voltageDividerRatio = (19.8 + 5.4) / 5.4; // R1 + R2 / R2
float rpm = 7;  // Input RPM (set manually for now, replace with sensor later)

// Functions for voltage calculations
float calculateFlowRate(float rpm) {
  float term1 = 0.2 * (rpm * rpm);
  float term2 = 0.6 * rpm;
  float constant = 3.5224;

  Serial.print("Term 1 (RPM^2): ");
  Serial.println(term1, 6);
  Serial.print("Term 2 (RPM): ");
  Serial.println(term2, 6);
  Serial.print("Constant: ");
  Serial.println(constant, 6);

  float flowRate = term1 + term2 + constant;
  Serial.print("Calculated Flow Rate: ");
  Serial.println(flowRate, 6);
  return flowRate;
}

float calculateVoltage(float flowRate) {
  return 0.0054 * (flowRate * flowRate) + 0.4961 * flowRate + 1.8575;
}

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(REN, OUTPUT);
  pinMode(LEN, OUTPUT);

  pinMode(A3, INPUT);    // Ensure A3 is input
  digitalWrite(A3, LOW); // Pull-down A3 to avoid floating

  digitalWrite(REN, HIGH);  // Enable Right channel
  digitalWrite(LEN, HIGH);  // Enable Left channel

  Serial.begin(9600);
}


void loop() {
  // Measure the supply voltage using the voltage divider
  int rawValue = analogRead(supplyPin);  // Read ADC value from A0
  float voltageOut = (rawValue / 1023.0) * 5.15*1;  // Convert ADC to voltage
  float supplyVoltage = voltageOut * voltageDividerRatio;  // Scale to actual LiPo voltage

  // Calculate flow rate and desired voltage
  float flowRate = calculateFlowRate(rpm);
  float desiredVoltage = calculateVoltage(flowRate);

  // Constrain desired voltage within the range (4V to 12V)
  desiredVoltage = constrain(desiredVoltage, 4.0, 12.6);

  // Calculate PWM duty cycle safely
  int dutyCycle = (desiredVoltage / max(supplyVoltage, 1.0)) * 255;
  dutyCycle = constrain(dutyCycle, 0, 255);

  // Apply PWM signal
  analogWrite(RPWM, dutyCycle);
  analogWrite(LPWM, 0);  

  // Debugging output
  Serial.print("Raw ADC Value: ");
  Serial.print(rawValue);
  Serial.print(", Voltage at A0: ");
  Serial.print(voltageOut);
  Serial.print(" V, LiPo Voltage: ");
  Serial.print(supplyVoltage);
  Serial.print(" V, RPM: ");
  Serial.print(rpm);
  Serial.print(", Flow Rate: ");
  Serial.print(flowRate);
  Serial.print(" L/min, Desired Voltage: ");
  Serial.print(desiredVoltage);
  Serial.print(" V, PWM Duty Cycle: ");
  Serial.print(dutyCycle);
  Serial.print(" (");
  Serial.print((dutyCycle / 255.0) * 100);
  Serial.println("%)");

  delay(10000);
}