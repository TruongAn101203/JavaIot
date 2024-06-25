#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <Servo.h>

#define UDP_PORT 4210

// UDP
WiFiUDP UDP;
unsigned char incomingByte = 119;
unsigned char pre;

//Servo
Servo s1;

int RL_EN = 3; // pulse sensor
int R_PWM = 4; // control rotation direction
int L_PWM = 5; // control rotation direction
int servoPin = 14; // control steering angle
unsigned long time1 = 0;

// long map(long x, long in_min, long in_max, long out_min, long out_max) {
//   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }

void wifi_connect(){
  //WiFiManager, Local initialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  //wm.resetSettings();

  // IPAddress staticIP(172, 20, 10, 120);
  // IPAddress gateway(0, 0, 0, 0);
  // IPAddress subnet(255, 255, 255, 240);
  // wm.setSTAStaticIPConfig(staticIP, gateway, subnet);

  // if (digitalRead(buttonPin) == LOW)
  // {
  //   wm.resetSettings();
  //   Serial.println("RESET :)");
  // }

  bool res;
  //res = wm.autoConnect(); // auto-generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("ESP_Rhyster","password"); // password-protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {
    // if you get here you have connected to the WiFi
    Serial.println("Connected...yeey :)");
  }

  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

}

uint8_t map_V1(uint8_t value)
{
  uint8_t mp[] = {255, 219, 182, 146, 109, 73, 36, 0, 36, 73, 109, 146, 182, 219, 255};
  return mp[value];
}

uint8_t map_V2(uint8_t value)
{
  uint8_t mp[] = {0, 13, 26, 39, 52, 65, 78, 90, 103, 116, 129, 142, 155, 168, 180};
  return mp[value];
}

void Run_up(byte speed){
  if(speed < 7 || speed > 14)
    return;
  digitalWrite(R_PWM, HIGH);
  digitalWrite(L_PWM, LOW); //clockwise
  analogWrite(RL_EN, map_V1(speed)); 
}

void Run_down(byte speed){
  if(speed < 0 || speed > 7)
    return;
  digitalWrite(R_PWM, LOW);
  digitalWrite(L_PWM, HIGH);  //counter-clockwise
  analogWrite(RL_EN, map_V1(speed)); 
}

void Stop(){
  digitalWrite(R_PWM, LOW);
  digitalWrite(L_PWM, LOW);
  analogWrite(RL_EN, 0); 
}

void Angle_L_R(byte angle){
  if(angle < 0 || angle > 14)
    return;
  s1.write(180 - map_V2(angle));
}

uint8_t decode(uint8_t Vin, _Bool Mode)
{
  return (Mode == 1) ? (Vin >> 4) : (Vin & 15);
}

void control(byte value){
  // Retrieve values from the packet
      uint8_t V1 = decode(value, 0);
      uint8_t V2 = decode(value, 1);
      // Use the values as needed
      // Serial.print("Param1: ");
      // Serial.println(param1);
      // Serial.print("Param2: ");
      // Serial.println(param2);

      if(V1 > 7){
        Run_up(V1);
        Serial.print("Up: ");
        Serial.println(map_V1(V1));
      }
      else{
        if(V1 < 7){
          Run_down(V1);
          Serial.print("Down: ");
          Serial.println(map_V1(V1));
        }
        else{
          Serial.print("Stop: ");
          Stop();
        }
      }
      if(V2 != 7){
          Serial.print("Turn: ");
          Serial.println(map_V2(V2));
        V1 = V1 * 0.7;
      }

      Angle_L_R(V2);
      
      // if(param1 > 100){
      //   run_up(map(param1, 100, 200, 0, 255));
      //   Serial.print("up: ");
      //   Serial.println(map(param1, 100, 200, 0, 255));
      // }
      // else{
      //   if(param1 < 100){
      //     run_dow(map(param1, 100, 0, 0, 255));
      //     Serial.print("dow: ");
      //     Serial.println(map(param1, 100, 0, 0, 255));
      //   }
      //   else{
      //     Serial.print("top: ");
      //     stop();
      //   }
      // }
      // angle_L_R(map(param2, 0, 100, 0, 180));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  // Connect wifi
  wifi_connect();
  // servo set pin
  s1.attach(servoPin);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RL_EN, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  
  digitalWrite(R_PWM, LOW);
  digitalWrite(L_PWM, LOW); //STOP

  // Begin listening to UDP port
  UDP.begin(UDP_PORT);

  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT); // attaches the servo on GIO2 to the servo object

}

void loop() {
  // put your main code here, to run repeatedly:

  int packetSize = UDP.parsePacket();

    if(packetSize)
    {
      incomingByte = UDP.read();
    }

    if(pre != incomingByte)
    {
      // Send data to uart
      Serial.printf("UDP content: %u - %c\n",incomingByte, incomingByte);
      control(incomingByte);

      pre = incomingByte;
    }

  // if (packetSize)
  // {
  //     byte packet[2]; // Using byte to store 2 smaller integers
  //     UDP.read(packet, 2);
  //     control(packet);
  // }

  // if ((unsigned long) (millis() - time1) > 100 )
  // {
  //   if ( digitalRead(LED_BUILTIN) == LOW )
  //   {
  //     digitalWrite(LED_BUILTIN, HIGH);
  //   }
  //   else
  //   {
  //     digitalWrite(LED_BUILTIN, LOW );
  //   }
  //   time1 = millis();
  // }
}
