# ESP32-Home-assistant

This is a voice assistant named Eden. This code represents the embedded part of the entire project, which involves three repositories. You can find these repositories at the following links:

* [ESP32-Home-assistant](https://github.com/periclesbgf/ESP32-Home-assistant) (this repository)
* [Home-Assistant-Server](https://github.com/periclesbgf/Home-Assistant-Server)
* [home-assistant-model](https://github.com/periclesbgf/home-assistant-model)

## Architecture

The project consists of three parts:

* Embedded system: Collects audio data and sends it to the server, also responds to server requests.
* Server: Receives data from the ESP32, converts it into a WAV file, and processes the audio.
* Activation word speech recognition: A classification model responsible for determining whether the spoken word is 'Eden'.

## Components

The following parts are required for this project:

* ESP32
* INMP441 Microphone
* DHT11 Temperature sensor (Optional)

