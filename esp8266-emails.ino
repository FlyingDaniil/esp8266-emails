#include <ESP8266WiFi.h>
const char* ssid = "ssid";          // имя wifi сети
const char* password = "password";  // пароль
char server[] = "mail.smtp2go.com"; // SMTP сервер
#define ASCII_username ASCIIusername //ascii логин
#define ASCII_password ASCIIpassword //ascii пароль
//адрес отправителя и получателя на строке 71, 76 и 87, 88

WiFiClient espClient;

void setup() {
  Serial.begin(9600);
  delay(10);
  //подключаемся к wifi
  Serial.print("Connecting To: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)  {
    delay(500);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //отправляем письмо
  byte ret = sendEmail();
}

void loop() {
}

byte sendEmail() {
  //пробуем подключиться к серверу
  if (espClient.connect(server, 2525) == 1)  {
    Serial.println(F("connected"));
  }
  else {
    Serial.println(F("connection failed"));
    return 0;
  }
  //проверяем ответ
  if (!emailResp()) {
    return 0;
  }
  Serial.println(F("Sending EHLO"));
  espClient.println("EHLO www.example.com");
  if (!emailResp()) {
    return 0;
  }
  
  //отправка логина и пароля
  Serial.println(F("Sending auth login"));
  espClient.println("AUTH LOGIN");
  if (!emailResp()) {
    return 0;
  }
  Serial.println(F("Sending User"));
  espClient.println(ASCII_username);
  if (!emailResp()) {
    return 0;
  }
  Serial.println(F("Sending Password"));
  espClient.println("ASCII_password");
  if (!emailResp()) {
    return 0;
  }

  //отправка письма
  Serial.println(F("Sending From"));
  espClient.println(F("MAIL From: test@gmail.com"));//адрес отправителя
  if (!emailResp()) {
    return 0;
  }
  Serial.println(F("Sending To"));
  espClient.println(F("RCPT To: test@yandex.by"));//адрес получателя
  if (!emailResp()) {
    return 0;
  }
  Serial.println(F("Sending DATA"));
  espClient.println(F("DATA"));
  if (!emailResp()) {
    return 0;
  }
  Serial.println(F("Sending email"));
  //отправка письма
  espClient.println(F("To:  test@yandex.by")); //адрес получателя
  espClient.println(F("From: test@gmail.com"));//адрес отправителя
  espClient.println(F("Subject: ESP8266 test e-mail\r\n")); //тема письма
  espClient.println(F("This is is a test e-mail sent from ESP8266.\n"));//первая строка письма
  espClient.println(F("Second line of the test e-mail."));//вторая строка письма
  espClient.println(F("Third line of the test e-mail."));//третья строка письма
  espClient.println(F("."));

  //получение ответа
  if (!emailResp()) {
    return 0;
  }
  //выход
  Serial.println(F("Sending QUIT"));
  espClient.println(F("QUIT"));
  if (!emailResp()) {
    return 0;
  }
  espClient.stop();
  Serial.println(F("disconnected"));
  return 1;
}

//функция проверки ответа
byte emailResp() {
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient.available()) {
    delay(1);
    loopCount++;
    // Wait for 20 seconds and if nothing is received, stop.
    if (loopCount > 20000) {
      espClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = espClient.peek();
  while (espClient.available()) {
    readByte = espClient.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4') {
    return 0;
  }
  return 1;
}
