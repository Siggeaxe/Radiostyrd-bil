# Remote-Controlled Hand-Gesture Operated Car!

This project showcases a wirelessly controlled car operated through a glove-mounted remote that translates hand movements into driving commands.

### How It Works  
The glove remote determines the user’s hand orientation (attitude) using an MPU6050 Inertial Measurement Unit (IMU), which consists of a gyroscope and accelerometer. The accelerometer measures Earth's gravity to estimate the glove’s tilt angle using trigonometry. These calculated angles are then processed by an Atmega328p microcontroller, which converts them into control signals.  

The control signals are wirelessly transmitted via Bluetooth to the car, where they are received and interpreted to adjust motor speed and direction. The car’s propulsion system consists of an H-bridge motor driver, which regulates power to the four independent wheel motors, allowing smooth and precise movement.  

| The controller circuit mounted on a prototype board with a red LED indicating connection to the car | The controller circuit mounted on a glove |  
| --- | --- |  
| <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Kontroller.jpg" width="78%"></div> | <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Kontroller_handske.jpg" width="70%"></div> |  

| The car with the circuit connected on prototype boards, powered by eight AA batteries, mounted on a plastic chassis |  
| --- |  
| <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Bil2.jpg" width="50%"></div> |  

| Circuit diagram of the car | Circuit diagram of the controller |  
| --- | --- |  
| <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Motorstyrning_kretsschema.png" width="78%"></div> | <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Styrning_kretsschema.png" width="80%"></div> |  
