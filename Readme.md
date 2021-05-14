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
### Debug
Senzor emitira poruke i na serijski port. Dovoljno je samo prikljuciti ga preko USB kabela, i PC ce ga skuziti kao COM port (dev/tty na Linuxu). Na portu je vidljivo stanje senzora kao i mjerne vrijednosti (baudrate 115200, 8, N, 1). Aktiviranje komunikacije ce resetirati uredjaj, ali to ne ometa standardan rad sustava.

### Klimerko
Postoji jedan jako slican open-source projekt za IoT mjerenje kvalitete zraka, imena [Klimerko](https://klimerko.org). Iz tog projekta preuzeli smo 3D dizajn kucista te ga modificirali (dodali nas logotip). Iako korisitimo identicne komponente (izuzev senzora BME280), dizajn nije preuzet s tog projekta. Takodjer, firmware je znatno drugaciji, kao i sucelje prema serveru te web-sizajn. 

## HW
 * ESP8266 NodeMCU
 * Plantower PMS7003 + beakout board
 * 4-zilni flat kabel
 * Kuciste printano po 3D modelu s projekta [Klimerko](https://github.com/DesconBelgrade/Klimerko/tree/master/Klimerko_Printable_Case)

Zapravo, najbolje je kupiti senzor s ukljucenim kabelom i breakout plocicom, poput [ovoga](https://www.aliexpress.com/item/33041320350.html?spm=a2g0o.productlist.0.0.16451628vJGQLo&algo_pvid=9e941e09-aa4c-4c5a-9300-3834d01949af&algo_expid=9e941e09-aa4c-4c5a-9300-3834d01949af-20&btsid=0bb0623616209892738276079e458b&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_). Jedna strana kabela se naravno prikljuci na plocicu, a druga se klijestima odsijece. Zice se oprezno zaleme za NodeMCU plocicu po nize navedenom planu. Detaljnije upute o montazi dostupne su i na [videu](https://youtu.be/D9VHvuHtT14?t=420) s projekta Klimerko. 

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