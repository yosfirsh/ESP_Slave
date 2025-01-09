#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BluetoothSerial.h>

#define DHTPIN 32     // Pin yang digunakan untuk sensor DHT11
#define DHTTYPE DHT11 // Jenis sensor DHT

#define BuzzerPin 33 // Pin yang digunakan untuk buzzer

DHT dht(DHTPIN, DHTTYPE);

// Membuat objek BluetoothSerial
BluetoothSerial SerialBT;

// Task handle
TaskHandle_t dhtBuzzerTaskHandle = NULL;
TaskHandle_t bluetoothTaskHandle = NULL;

// Deklarasi fungsi dhtBuzzerTask dan bluetoothTask
void dhtBuzzerTask(void *parameter);
void bluetoothTask(void *parameter);

void setup()
{
  // Mulai serial monitor
  Serial.begin(115200);
  // Inisialisasi sensor DHT11
  dht.begin();

  // Inisialisasi pin untuk buzzer
  pinMode(BuzzerPin, OUTPUT);

  // Inisialisasi Bluetooth Serial
  SerialBT.begin("ESP32_Slave"); // Nama perangkat Bluetooth Classic

  // Menunggu koneksi Bluetooth
  Serial.println("Menunggu koneksi Bluetooth...");
  while (!SerialBT.hasClient())
  {
    delay(1000);
  }
  Serial.println("Bluetooth Connected!");

  // Membuat task untuk membaca sensor DHT11 dan mengendalikan buzzer
  xTaskCreatePinnedToCore(
      dhtBuzzerTask,        // Nama fungsi task
      "DHT & Buzzer Task",  // Nama task
      10000,                // Ukuran stack task
      NULL,                 // Parameter untuk task (tidak ada)
      1,                    // Prioritas task
      &dhtBuzzerTaskHandle, // Task handle untuk referensi
      0                     // Core tempat task akan dijalankan (0 atau 1)
  );

  // Membuat task untuk mengirimkan data Bluetooth
  xTaskCreatePinnedToCore(
      bluetoothTask,        // Nama fungsi task
      "Bluetooth Task",     // Nama task
      10000,                // Ukuran stack task
      NULL,                 // Parameter untuk task (tidak ada)
      1,                    // Prioritas task
      &bluetoothTaskHandle, // Task handle untuk referensi
      1                     // Core tempat task akan dijalankan (0 atau 1)
  );
}

void loop()
{
  // Loop utama kosong karena task sudah menangani semuanya
}

// Fungsi untuk membaca DHT11 dan mengecek suhu untuk buzzer
void dhtBuzzerTask(void *parameter)
{
  while (true)
  {
    // Membaca suhu dalam Celsius
    float temperature = dht.readTemperature();

    // Membaca kelembapan
    float humidity = dht.readHumidity();

    // Periksa jika ada pembacaan yang gagal
    if (isnan(temperature) || isnan(humidity))
    {
      Serial.println("Gagal membaca dari sensor DHT!");
    }
    else
    {
      // Tampilkan hasil pembacaan di Serial Monitor
      Serial.print("Suhu: ");
      Serial.print(temperature);
      Serial.print(" °C ");
      Serial.print("Kelembapan: ");
      Serial.print(humidity);
      Serial.println(" %");

      // Periksa suhu dan aktifkan buzzer jika suhu lebih dari 40
      if (temperature > 40)
      {
        digitalWrite(BuzzerPin, HIGH); // Menyalakan buzzer
      }
      else
      {
        digitalWrite(BuzzerPin, LOW); // Mematikan buzzer
      }
    }

    // Delay untuk menghindari pembacaan terlalu cepat
    vTaskDelay(3000 / portTICK_PERIOD_MS); // Delay 3 detik
  }
}

// Fungsi untuk mengirimkan data suhu dan kelembapan melalui Bluetooth
void bluetoothTask(void *parameter)
{
  while (true)
  {
    // Membaca suhu dalam Celsius
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Kirim data suhu dan kelembapan melalui Bluetooth Serial
    String data = "Suhu: " + String(temperature) + " °C, Kelembapan: " + String(humidity) + " %";
    SerialBT.println(data);

    // Delay untuk menghindari pengiriman terlalu cepat
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Delay 60 detik
  }
}
