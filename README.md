# AQUAPUNO_ETH
AQUAPUNO ethernet version- AQUAPONICS command from web

Deskripsi
---------
AQUAPUNO adalah system terpadu AQUAPONIK dibantu dengan microcontroller arduino.
menggunakan arduino UNO sebagai alat mengukur sensor dan menyalakan/mematikan relay
(alat elektronik) seperti pompa atau pengatur pupuk.

Cara Kerja
----------
Alat ini menggunakan ethernet shield sebagai alat komunikasi dengan komputer utama.
komputer utama menggunakan web2py webserver untuk mengambil data dari arduino ini melalui protokol
http seperti web browser.
Juga dapat melakukan interupsi command melalui http (misal: perlu menyalakan pompa segera)
Arduino ini juga sekaligus berfungi sebagai timer menyalakan relay dengan interval waktu tertentu.
Sensor yang digunakan adalah 2 x moisture sensor , temperature sensor dan sensor cahaya.
menggunakan 4 relay untuk on/off

Software ini bergantung juga pada webduino
  https://github.com/sirleech/Webduino
  atau
  https://github.com/irzaip/Webduino (clone)


Setting
-------
ada beberapa tempat setting yang harus di sesuaikan dengan butuhan

Pin sensor DHT
  #define DHTPIN A0     // what pin we're connected to
  #define LHTPIN A5     

Mac address
  static uint8_t mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };

IP address
  static uint8_t ip[] = { 192, 168, 30, 93 };

Timer otomatis relay dan deskripsi
  RelayTimer Relay1(4, 3000, 4000, "Pompa Air");
  RelayTimer Relay2(5, 3000, 3500, "Oksigen ikan");
  RelayTimer Relay3(6, 3000, 5500, "Makan ikan");
  RelayTimer Relay4(7, 3000, 6500, "Pompa Pupuk");


contoh Query ke arduino
-----------------------

http://host/parsedCmd?RLY1=ON
http://host/parsedCmd?GETINFO=1

