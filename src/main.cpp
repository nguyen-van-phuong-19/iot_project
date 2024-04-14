//client
#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define WARTER_SENSOR A0

// Các chân kết nối với LoRa module
// Tần số hoạt động của module LoRa
#define RF95_FREQ 433.0

RH_RF95 rf95;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

void init_lora();
void send(const char *message);
uint8_t* receive();
void control_lcd(const char* data);
void control_servo(int pos_init, int pos_turn);
int read_warter_sensor();


void setup()
{
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  init_lora();
  myServo.attach(9); // Gắn servo vào chân số 9
  // Khởi động màn hình LCD
  lcd.init();
  // Bật đèn nền LCD
  lcd.backlight();

  control_lcd("Xin Chao!");

}

void loop()
{
  // Send a message to rf95_server
  static uint8_t data[] = "Hello I'm from lora!";
  static uint8_t* buf;
  send((const char*) data);
  buf = receive();
  delay(400);
}

void init_lora() {

  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  rf95.setTxPower(18, false);
  Serial.println("Init Done!!!!");
}

void send(const char *message) {
  rf95.send((uint8_t *)message, strlen(message));
  rf95.waitPacketSent();
  Serial.println("Message sent!");
}

uint8_t* receive() {
  static uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; // Bộ đệm tĩnh để lưu trữ dữ liệu nhận được

  if (rf95.available()) {
    // Đặt độ dài tối đa cho bộ đệm
    uint8_t len = RH_RF95_MAX_MESSAGE_LEN;

    // Nhận dữ liệu và trả về con trỏ nếu thành công
    if (rf95.recv(buf, &len)) {
      Serial.print("Received: ");
      Serial.println((char*)buf);
      return buf; // Trả về con trỏ đến bộ đệm
    } else {
      Serial.println("Receive failed");
    }
  }
  return nullptr; // Trả về nullptr nếu không có dữ liệu hoặc nhận thất bại
}



int read_warter_sensor(){
  return analogRead(WARTER_SENSOR);
}

void control_lcd(const char* data){
  // Đặt con trỏ tại vị trí đầu tiên của màn hình
  lcd.setCursor(0, 0);
  // In ra dòng chữ "Hello, World!"
  lcd.print(data);
}

void control_servo(int pos_init, int pos_turn){
    if(pos_turn > pos_init){
      for (int pos = pos_init; pos <= pos_turn; pos += 1) {
          myServo.write(pos); // Điều khiển servo xoay đến góc 'pos'
          delay(15); // Đợi 15ms để servo kịp xoay
        }
    } else {
      for (int pos = pos_init; pos >= pos_turn; pos -= 1) {
          myServo.write(pos); // Điều khiển servo xoay đến góc 'pos'
          delay(15); // Đợi 15ms để servo kịp xoay
        }
    }
}