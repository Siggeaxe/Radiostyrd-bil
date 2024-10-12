# Radiostyrd bil!
Ett projektarbete där vi byggde en radiostyrd bil som styrs med handrörelser.

Handens lutning mäts med en så kallad "Inertial Measurement Unit" av modellen MPU6050, som består av ett gyroskop och en accelerometer. Med hjälp av accelerometerns mätning av jordens gravitation kan handens vinkel beräknas med trigonometri. Mikrokontrollern av modellen Atmega328p konverterar sedan vinkeln till styrsignaler som skickas till bilen via en Bluetooth-modul, där en H-brygga driver bilens fyra motorer.

| Kretschema av bilen | kretschema av kontrollern  | 
| ---           | ---           |                                                                                                                                                   
| <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Motorstyrning_kretsschema.png" width="78%"></div> | <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Styrning_kretsschema.png" width="80%"></div> |


| Kontrollerkretsen monterad på en prototypbräda med en röd LED som indikerar anslutning till bilen | Prototypbräda monterat på en handske  | 
| ---           | ---           |                                                                                                                                                   
| <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Kontroller.jpg" width="78%"></div> | <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Kontroller_handske.jpg" width="70%"></div> |

| Bilen med kretsen kopplad på prototypbrädor driven av 8 stycken AA-batterier, monterat på ett plast-chassi |
| ---           |
| <div align="center"><img src="https://github.com/Siggeaxe/Radiostyrd-bil/blob/main/Bilder/Bil2.jpg" width="50%"></div> |


