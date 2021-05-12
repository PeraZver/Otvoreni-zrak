# Otvoreni Zrak - Mjerna stanica
Ovo je dokumentacija firmwarea za mjerne stanice, izradjene u sklopu projekta [Otvoreni Zrak](https://zrak.mozemo.hr/).

Pero Krivic, Svibanj 2021
## Opis
### Mjerenje koncentracije pm2.5/pm10 cestica u Zagrebu
Senzor kvalitete zraka se spaja na kucnu WiFi mrezu. Mjerene PMx vrijednosti se postaju na udaljeni server za daljnju obradu.
Prilikom prvog koristenja, korisnik se mora spojiti na lokalnu mrezu  `Otvoreni Zrak`, izabrati vlastiti kucni WiFi, te unijeti korisnicku zaporku. Senzor ce unesene podatke pohraniti te koristiti prilikom sljedeceg pokusaja spajanja. Ukoliko se WiFi promijeni, potrebno je iskljuciti-ukljuciti senzor, te krenuti ispocetka sa lokalnom mrezom. Plava LEDica svijetli dok god senzor nije spojen na mrezu, i gasi se jednom kad se spoji. Nakon toga blinka jednom svakih 5 minuta, kada senzor provjerava kvalitetu WiFI veze. Ukoliko WiFi nije dobar, uredjaj se auto-resetira. 

Senzor PMS7003 dostavlja mjerene vrijednosti masene koncentracije PM 1.0, 2.5 i 10 cestica (μg/m3), te brojcanu koncentraciju PM 0.3, 0.5, 1.0, 2.5, 5 i 10 cestica (#/100 cm3), preciznosti ovisne o rasponu koncentracija. Komunikacija sa senzorom je putem UART sucelja, implementiranog `SoftwareSerial` bibliotekom.     

Mjereni podaci se ocitavaju svakih 60 sekundi i usrednjavaju preko 1 sat. Prosjecne vrijednosti se formatiraju u JSON format te salju na udaljeni "Zrak Je Nas" server putem HTTP POSTa. Primjer formatiranih vrijednosti je:
```
{
  "pm10": 1.23,
  "pm25": 1.23,
  "measure_time": 123456789,
  "data":{
      "ESP_id": 123456,
      "Mass": {
          "pm01": 1.23,
          "pm25": 1.23,
          "pm10": 1.23
        },
      "Num":  {
          "n0p3": 1.23,
          "n0p5": 1.23,
          "n1p0": 1.23,
          "n2p5": 1.23,
          "n5p0": 1.23,
          "n10p0": 1.23
        }
  }
}
```
### Verdict
Imamo studije koje su proucavale ovakve low cost senozre, i SPS30 nije prosao bas dobro. Naime, tocnost mu opada kod mjerenja PM10 cestica. Osim toga, moj test u Slavonskom Brodu pokazao je da su mjerene vrijednosti drasticno opale na otvorenom, uz obalu rijeke, na nekih 5°C. Stoga smo odlucili prijeci na Plantowerov PMS7003 senzor, za kojeg se prica po selu da je puno bolji. 

### Debug
Senzor emitira poruke i na serijski port. Dovoljno je samo prikljuciti ga preko USB kabela, i PC ce ga skuziti kao COM port (dev/tty na Linuxu). Na portu je vidljivo stanje senzora kao i mjerne vrijednosti (baudrate 115200, 8, N, 1). Aktiviranje komunikacije ce resetirati uredjaj, ali to ne ometa standardan rad sustava.

## HW
 * ESP8266 NodeMCU
 * Plantower PMS7003 + beakout board
 * 4-zilni flat kabel
 * Kuciste printano po 3D modelu s projekta [Klimerko](https://github.com/DesconBelgrade/Klimerko/tree/master/Klimerko_Printable_Case)

### Wiring
|Cable Wire|NodeMCU Pin|Function|
|----------|-----------|--------|
|Purple    |Vin        |Power   |
|Orange    |GND        |Ground  |
|Green     |D5         |RX      |
|Blue      |D6         |TX      |

## SW 
 * Arduino Core for ESP8266 (v2.7.4)
 * PlatformIO on VSCode (v5.1.1)

### 3rd Party Libs
 * [PMSerial v1.1.1](https://github.com/avaldebe/PMserial/) 
 * [WiFi Manager v0.16.0](https://github.com/tzapu/WiFiManager)
 * [ArduinoJSON v6.17.2](https://arduinojson.org/)
 * [NTPClient v3.1.0](https://github.com/arduino-libraries/NTPClient)

## To-Do
 - [x] Ocitavanje senzora
 - [x] Captive portal za spajanje na home WiFi
 - [x] Spremanje/citanje WiFi podataka iz EEPROMa
 - [x] Formatiranje u oku ugodan JSON
 - [x] Postanje podataka na udaljeni server
 - [ ] Mali WebServer za prikaz podataka u browseru *(optional)*
 - [x] OTA Update
 - [ ] Graf kretanja PMx vrijednosti na WebServeru *(optional)* 
 - [x] Prijeci na PMS7003 senzor
 - [x] Postati ChipID za razlicite stanice.
 - [x] Prijeci na Token-based model komunikacije
 - [x] Pohrana tokena u EEPROM
 - [x] Dohvacanje timestampa putem NTP-a
 - [x] Averaging svih mjerenih vrijednosti
 - [x] Lampica kao indikator za WiFi.

Pero, Travanj 2021