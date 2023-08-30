LED Cluster Controller using ESP8266
Overview

This project provides a way to control clusters of LEDs through a web interface. The system is based on the ESP8266 WiFi module and uses WebSockets for real-time communication between the web interface and the ESP8266.
Features

    Control up to 3 LED clusters independently.
    Change color (RGB) and intensity for each cluster.
    Real-time updates using WebSockets.
    Simple web interface that can be accessed through any browser.

Prerequisites

    Arduino IDE with ESP8266 board support installed
    ESP8266 WiFi Module
    WS2811 LEDs
    Basic knowledge of HTML and JavaScript

Hardware Setup

    Connect the data pin of the WS2811 LEDs to the D5 pin on the ESP8266.
    Power the WS2811 LEDs using an external 5V supply.

Note: Make sure to connect the ground of the ESP8266 to the ground of the external 5V supply.
Software Setup

    Open the Arduino IDE, go to File -> Open and select the .ino file from this repository.
    Modify the ssid and password variables with your WiFi credentials.
    Modify the staticIP, gateway, and subnet variables according to your network configuration.
    Compile and upload the code to your ESP8266.

Web Interface Setup

    Open the index.html file in a web browser or host it on a web server.
    The web interface should automatically connect to the ESP8266 via WebSocket.
    Use the sliders to control the RGB color and intensity of each LED cluster.

Usage

    Cluster 1-3: Use the sliders to set the Red, Green, Blue, and Intensity values for each LED cluster.

Contributing

If you'd like to contribute, please fork the repository and use a feature branch. Pull requests are warmly welcome.
Authors

    Raffi Tchakerian