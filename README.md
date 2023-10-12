# LED Cluster Controller using ESP8266

## Overview

This project provides a way to control clusters of LEDs through a web interface. The system is based on the ESP8266 WiFi module and uses WebSockets for real-time communication between the web interface and the ESP8266.

## Features

- Control up to 3 LED clusters independently.
- Change color (RGB) and intensity for each cluster.
- Real-time updates using WebSockets.
- Simple web interface that can be accessed through any browser.

## Prerequisites

- Arduino IDE with ESP8266 board support installed
- ESP8266 WiFi Module
- WS2811 LEDs
- Basic knowledge of HTML and JavaScript

## Hardware Setup

- Connect the data pin of the WS2811 LEDs to the D5 pin on the ESP8266.
- Power the WS2811 LEDs using an external 5V supply.

> **Note:** Make sure to connect the ground of the ESP8266 to the ground of the external 5V supply.

## Software Setup

1. Open the Arduino IDE, go to File -> Open and select the `.ino` file from this repository.
2. Modify the `ssid` and `password` variables with your WiFi credentials.
3. Modify the `staticIP`, `gateway`, and `subnet` variables according to your network configuration.
4. Compile and upload the code to your ESP8266.

## Web Interface Setup

1. Open the `index.html` file in a web browser or host it on a web server.
2. The web interface should automatically connect to the ESP8266 via WebSocket.
3. Use the sliders to control the RGB color and intensity of each LED cluster.

## Usage

- **Cluster 1-3:** Use the sliders to set the Red, Green, Blue, and Intensity values for each LED cluster.

## Contributing

If you'd like to contribute, please fork the repository and use a feature branch. Pull requests are warmly welcome.

## Authors

- Raffi Tchakerian  
- Joanne Hayek

---

# The Air Between Us

## Setup Manual

1. Connect to Linksys with your computer. Password: `1q2w3e4r5T`
2. Open VNC Viewer
3. Connect VNC Viewer to the IP address: `192.168.0.129:5900`
4. When asked for username and password (in the pop-up window “authenticate VNC Server”)
   use: (username: pi) (password: electronica)
5. Open Visual Studio Code from Raspberry Pi’s desktop
6. In Visual Studio Code, open the folder ‘THE AIR BETWEEN US’ (from the desktop)
7. Right-click on `STEP 1.html`, select ‘open in live server’
8. In the browser, position the circles over each plant, and scale their diameters to cover the
   leaves. Adjust the ring diameter and thickness. Choose the right websocket. Once done with
   the first, you can click again on the video to add a second. etc.
9. Once done, hit the ‘finalize’ button. It will save the JSON file to the Download folder.
10. Make sure the JSON file is named: “the_air_between_us-setup_layout.json”
11. Drag the file from downloads to the Raspberry Pi folder ‘THE AIR BETWEEN US’
12. Open `STEP2-reversed.html` in live server
13. Now you’re all set to test the interaction.
14. Adjust the orientation of the LED disks based on the interaction
15. On the screen, adjust the slider values to enhance the interaction experience
16. Take note of the values selected.
17. Go back to Visual Studio Code, and insert these values in the slider html tags (lines 36 to 43),
    in `value=“…”`
18. Save the file.
19. You’re good to go!
20. You can disconnect the computer from Raspberry Pi and keep it running
