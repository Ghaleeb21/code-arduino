// Define pin connections
int sensor_pins[] = {A0, A1, A2, A3, A4};  // Analog pins for moisture sensors
int relay_pump_pin = 3;                // Digital pin for pump relay control
int relay_uv_pin = 4;                  // Digital pin for UV light relay control

// Variables
int moisture_value;
int moisture_values[4];                // Array to store moisture levels from each sensor
int number_of_sensors = 4;             // Number of sensors

unsigned long uv_on_duration = 68400000;  // 19 hours in milliseconds (on time)
unsigned long uv_off_duration = 18000000; // 5 hours in milliseconds (off time)
unsigned long previous_millis = 0;        // Stores the last time UV light state changed
bool uv_light_on = false;                 // Tracks UV light state

void setup() {
  pinMode(relay_pump_pin, OUTPUT);     // Set pump relay pin as output
  pinMode(relay_uv_pin, OUTPUT);       // Set UV light relay pin as output
  Serial.begin(9600);                  // Start serial communication
  Serial.println("Automated Irrigation System with UV Light 19 Hours On, 5 Hours Off...");
}

void loop() {
  unsigned long current_millis = millis(); // Get the current time

  int total_moisture = 0;              // Variable to store the sum of all moisture values

  // Read moisture sensor values
  for (int i = 0; i < number_of_sensors; i++) {
    moisture_values[i] = analogRead(sensor_pins[i]);
    moisture_values[i] = map(moisture_values[i], 550, 10, 0, 100);  // Map sensor value to percentage
    moisture_values[i] = constrain(moisture_values[i], 0, 100);     // Ensure value is between 0-100%
    total_moisture += moisture_values[i];                           // Add each sensor value to total
  }

  // Calculate average moisture level
  moisture_value = total_moisture / number_of_sensors;

  Serial.print("Average Moisture Level: ");
  Serial.print(moisture_value);
  Serial.println("%");

  // Control the pump based on average moisture level
  if (moisture_value <= 1) {
    delay(300);                        // 0.3 seconds delay
    digitalWrite(relay_pump_pin, LOW); // Turn on pump (assuming LOW activates the pump)
    Serial.println("Pump ON");
  } else {
    digitalWrite(relay_pump_pin, HIGH); // Turn off pump
    Serial.println("Pump OFF");
  }

  // Control UV light for 19 hours on and 5 hours off
  if (uv_light_on) {
    // If UV light is on, check if 19 hours have passed to turn it off
    if (current_millis - previous_millis >= uv_on_duration) {
      digitalWrite(relay_uv_pin, HIGH); // Turn off UV light (assuming active-high relay)
      uv_light_on = false;              // Update UV light state
      previous_millis = current_millis; // Reset the timer
      Serial.println("UV Light OFF");
    }
  } else {
    // If UV light is off, check if 5 hours have passed to turn it on
    if (current_millis - previous_millis >= uv_off_duration) {
      digitalWrite(relay_uv_pin, LOW);  // Turn on UV light (assuming active-low relay)
      uv_light_on = true;               // Update UV light state
      previous_millis = current_millis; // Reset the timer
      Serial.println("UV Light ON");
    }
  }

  delay(1000);  // Wait for 1 second before the next reading
}
