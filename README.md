# ArduinoProjects

Collection of Arduino Uno, ESP32 projects.

## Projects Included:

### ESP32 WiFi Car

This project features a ESP32 that can remotely control a small car via a webserver that features a joystick. Because of the variable delay between HTTP request and responses, the car's motors may not always react quick enough and it is possible that it could get "stuck" on a previous joystick command.

[Credits for joystick design](https://automatedhome.party/2017/07/15/wifi-controlled-car-with-a-self-hosted-htmljs-joystick-using-a-wemos-d1-miniesp8266/)

### ESP32-CAM SMTP Program

This project features a ESP32-CAM that can send a captured photograph by email to a desired recipient. At the time of testing, the pushbutton was intended to trigger the photograph capture and email, but there were issues with the pushbutton states not being recognized.

[Credits for program](https://randomnerdtutorials.com/esp32-cam-send-photos-email/)