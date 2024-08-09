The aim of my project is to design an artificial intelligence supported product production management platform using STM32 and ESP32 boards. In this platform, an interface was designed to enable the user to log in to the system and after the login process, an interface was designed that provides product production information and product time information according to the user's selection from the products in the current product production list. The product production information received temporally was transmitted via UART (Serial) communication over ESP32. In addition, using the Gemini API key, a future production forecast was made with the temporal product production data of the past years and compared with the current production data.
STM32 PINOUT & CONFIGURATION section:
![image](https://github.com/user-attachments/assets/20db74ae-ae51-43fc-9b57-a71b31c87d77)
On the Esp32 side the connections are made with pins G17 and G16.

