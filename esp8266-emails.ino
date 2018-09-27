#include <ESP8266WiFi.h>
const char* ssid = "ssid";          // имя wifi сети
const char* password = "password";  // пароль
char server[] = "mail.smtp2go.com"; // SMTP сервер
#define ASCII_username ASCIIusername //ascii логин
#define ASCII_password ASCIIpassword //ascii пароль
//адрес отправителя и получателя на строке 83, 88 и 99, 101

WiFiClient espClient;
void setup()
{
  Serial.begin(9600);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  byte ret = sendEmail();
}

void loop()
{

}

byte sendEmail()
{
  if (espClient.connect(server, 2525) == 1)
  {
    Serial.println(F("connected"));
  }
  else
  {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!emailResp())
    return 0;
  //
  Serial.println(F("Sending EHLO"));
  espClient.println("EHLO www.example.com");
  if (!emailResp())
    return 0;
  //
  /*Serial.println(F("Sending TTLS"));
    espClient.println("STARTTLS");
    if (!emailResp())
    return 0;*/
  //
  Serial.println(F("Sending auth login"));
  espClient.println("AUTH LOGIN");
  if (!emailResp())
    return 0;
  //
  Serial.println(F("Sending User"));
  // Change this to your base64, ASCII encoded username
  espClient.println(ASCII_username); //base64, ASCII encoded Username
  if (!emailResp())
    return 0;
  //
  Serial.println(F("Sending Password"));
  // change to your base64, ASCII encoded password
  /*
    For example, if your password is "testpassword" (excluding the quotes),
    it would be encoded as dGVzdHBhc3N3b3Jk
  */
  espClient.println("ASCII_password");
  if (!emailResp())
    return 0;
  //
  Serial.println(F("Sending From"));
  // change to sender email address
  espClient.println(F("MAIL From: test@gmail.com"));
  if (!emailResp())
    return 0;
  // change to recipient address
  Serial.println(F("Sending To"));
  espClient.println(F("RCPT To: test@yandex.by"));
  if (!emailResp())
    return 0;
  //
  Serial.println(F("Sending DATA"));
  espClient.println(F("DATA"));
  if (!emailResp())
    return 0;
  Serial.println(F("Sending email"));
  // change to recipient address
  espClient.println(F("To:  test@yandex.by"));
  // change to your address
  espClient.println(F("From: test@gmail.com"));
  espClient.println(F("Subject: ESP8266 test e-mail\r\n"));
  espClient.println(F("This is is a test e-mail sent from ESP8266.\n"));
  espClient.println(F("Second line of the test e-mail."));
  espClient.println(F("Third line of the test e-mail."));
  //
  espClient.println(F("."));
  if (!emailResp())
    return 0;
  //
  Serial.println(F("Sending QUIT"));
  espClient.println(F("QUIT"));
  if (!emailResp())
    return 0;
  //
  espClient.stop();
  Serial.println(F("disconnected"));
  return 1;
}

byte emailResp()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient.available())
  {
    delay(1);
    loopCount++;
    // Wait for 20 seconds and if nothing is received, stop.
    if (loopCount > 20000)
    {
      espClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = espClient.peek();
  while (espClient.available())
  {
    readByte = espClient.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4')
  {
    //  efail();
    return 0;
  }
  return 1;
}
