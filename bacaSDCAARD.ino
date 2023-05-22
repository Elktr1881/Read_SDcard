#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

int clust = 0;
int nClust = 2;

File myFile;

String arr[500];
String buffer;
String ssid, pass;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial2.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}

void getClust(int num) {
  int mulai = nClust * (num - 1) + 1;
  int selesai = nClust * num;
  myFile = SD.open("BMI.csv", FILE_READ);
  int i = 0, br = 0, cl = 0;
  StaticJsonDocument<1000> doc;
  JsonArray data = doc.createNestedArray("data");

  if (myFile) {
    while (myFile.available()) {
      br++;
      buffer = myFile.readStringUntil('\n');
      if ((br >= mulai) and (br <= selesai)) {
        int i1 = buffer.indexOf(';');
        int i2 = buffer.indexOf(';', i1 + 1);
        int i3 = buffer.indexOf(';', i2 + 1);
        int i4 = buffer.indexOf(';', i3 + 1);
        int i5 = buffer.indexOf(';', i4 + 1);
        int i6 = buffer.indexOf(';', i5 + 1);
        int i7 = buffer.indexOf(';', i6 + 1);
        String id = buffer.substring(0, i1);
        String nama = buffer.substring(i1 + 1, i2);
        String nisn = buffer.substring(i2 + 1, i3);
        String tgl = buffer.substring(i3 + 1, i4);
        String tinggi = buffer.substring(i4 + 1, i5);
        String berat = buffer.substring(i5 + 1, i6);
        String bmi = buffer.substring(i6 + 1, i7);

        int b1 = tgl.indexOf(' ');
        int b2 = tgl.indexOf(' ', b1 + 1);

        String tglNu = tgl.substring(b1 + 1, b2);

        int c1 = tglNu.indexOf('/');
        int c2 = tglNu.indexOf('/', c1 + 1);

        String tanggal = tglNu.substring(0, c1);
        String bulan = tglNu.substring(c1 + 1, c2);
        String tahun = tglNu.substring(c2 + 1);

        tglNu = tahun + "-" + bulan + "-" + tanggal;

        tinggi.replace(" cm", "");
        berat.replace(" kg", "");

        StaticJsonDocument<200> doc2;
        doc2["nisn"] = nisn;
        doc2["berat"] = berat;
        doc2["tinggi"] = tinggi;
        doc2["bmi"] = bmi;
        doc2["tanggal_ukur"] = tglNu;
        //      serializeJson(doc2, Serial);
        data.add(doc2);
        if (br == selesai) break;
      }
    }
    myFile.close();
  }
  serializeJsonPretty(doc, Serial);
  Serial2.print("~");
  serializeJson(doc, Serial2);
  Serial2.print("!");
  //  Serial.println(br);
  //  delay(10000);
}

int getNData() {
  myFile = SD.open("BMI.csv", FILE_READ);
  int i = 0, br = 0, cl = 0;

  if (myFile) {
    while (myFile.available()) {
      br++;
      buffer = myFile.readStringUntil('\n');
    }
    myFile.close();
  }
  return br;
}

void getWifi() {
  myFile = SD.open("wifi.txt", FILE_READ);
  int i = 0, br = 0, cl = 0;

  if (myFile) {
    while (myFile.available()) {
      br++;
      buffer = myFile.readStringUntil('\n');
      if (br == 1) ssid = buffer;
      if (br == 2) pass = buffer;
      //      Serial.println(buffer);
    }
    myFile.close();
  }
  Serial.print("SSID : "); Serial.println(ssid);
  Serial.print("PASS : "); Serial.println(pass);
}

void loop() {
  
  getWifi();
  
  int cc = getNData() / nClust;
  if ((getNData() % nClust) > 0) cc += 1;

  while (Serial2.available() >= 0) {
    Serial.println("SEND~" + ssid + "!" + pass + "@");
    Serial2.println("SEND~" + ssid + "!" + pass + "@");
    String dataku = Serial2.readString();
    dataku.replace("\r\n", "");
    Serial.println(dataku);
    if (dataku == "OK") {
      break;
    }
    delay(300);
  }
  for (int i = 1; i <= cc; i++) {
    while (Serial2.available() >= 0) {
      getClust(i);
      String dataku = Serial2.readString();
      dataku.replace("\r\n", "");
      Serial.println(dataku);
      if (dataku == "NEXT") {
        break;
      }
      delay(1000);
    }
    delay(1000);
  }
  Serial.println("DONE=============");
  delay(10000);

}
