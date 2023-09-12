<a name="readme-top"></a>
<hr>
<br />
<div align="center">
  <a href="https://github.com/BelalHassan7/Autonomous_Aeroponic">
  </a>
<h3 align="center">Autonomous Aeroponic</h3>
  <p align="center">
    <br />
    <a href="https://github.com/BelalHassan7/Autonomous_Aeroponic"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/BelalHassan7/Autonomous_Aeroponic">View Demo</a>
    ·
    <a href="https://github.com/BelalHassan7/Autonomous_Aeroponic/issues">Report Bug</a>
    ·
    <a href="https://github.com/BelalHassan7/Autonomous_Aeroponic/issues">Request Feature</a>
  </p>
</div>
<hr>


<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#About-The-Project">About The Project</a>
      <ul>
        <li><a href="#Built-With">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#Functionss">Functions</a>
    </li>
    <li><a href="#Contact">Contact</a></li>
  </ol>
</details>

## About The Project
This project is an Autonomous Aeroponic controlled by an ESP32. It is made to be user-friendly and, therefore, connected to a mobile application that containes all the 
required growth conditions (temperature, lighting, and watering time) for any plant. The user could choose the plant either by using the camera or by selecting it manually
from the list provided. When the user chooses the plant, the data of the plant (growth conditions) will be sent to the ESP32 through the firebase, and the ESP32 will start
monitoring the growth conditions of the plant. In addition, the ESP32 will send the temperature and the led status to the mobile application through the firebase, providing
the user with the information needed without checking the plant.

### Built With

* [![arduino][arduino.js]][arduino-url]
* [![esp][esp.js]][esp-url]
* [![fb][fb.js]][fb-url]
<p align="right">(<a href="#readme-top">back to top</a>)

## Functions

* `int getTemp()` returns air temperature.

* `void sendTemp()` sends temperature to the firebase every "freq2" seconds.

* `int getLedStatus()` returns the LEDs status (wheter turned off or on).

* `void water()` controls water flow for irrigation

* `void torgb(int wavelength, struct light* rgb)` turns wavelength into rgb value.

* `void lightControl()` controls the LEDs' lighting.

* `void off()` turns off all the temperature adjustment system's components.

* `bool cold()` checks if the temperature is cold.

* `void heat(int targetTemp)` heats untill the "targetTemp."

* `bool hot()` checks if the temperature is hot.

* `void cool(int targetTemp)` cools until the "targetTemp."

* `void adjustTemp()` adjusts the temperature of the air.
<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Contact

Belal - belal.34327@gmail.com

Project Link: [https://github.com/BelalHassan7/Autonomous_Aeroponic](https://github.com/BelalHassan7/Autonomous_Aeroponic.git)
<p align="right">(<a href="#readme-top">back to top</a>)</p>

[arduino.js]: https://img.shields.io/badge/Arduino-00878F?logo=arduino&logoColor=fff&style=flat
[arduino-url]: https://www.arduino.cc
[esp.js]: https://img.shields.io/badge/Espressif-E7352C?logo=espressif&logoColor=fff&style=flat
[esp-url]: https://www.espressif.com
[fb.js]: https://img.shields.io/badge/Firebase-FFCA28?logo=firebase&logoColor=000&style=flat
[fb-url]: https://firebase.google.com
