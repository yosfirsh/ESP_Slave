#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BluetoothSerial.h>

// ===============================
// Konfigurasi Pin & Tipe Sensor
// ===============================
#define DHTPIN 32     // Pin data untuk sensor DHT11
#define DHTTYPE DHT11 // Tipe sensor DHT yang digunakan

#define BuzzerPin 33 // Pin output untuk buzzer

// Membuat objek untuk sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

// Membuat objek Bluetooth Classic
BluetoothSerial SerialBT;

// Handle untuk dua task FreeRTOS
TaskHandle_t dhtBuzzerTaskHandle = NULL;
TaskHandle_t bluetoothTaskHandle = NULL;

// Deklarasi fungsi task
void dhtBuzzerTask(void *parameter);
void bluetoothTask(void *parameter);

void setup()
{
  // ===============================
  // Inisialisasi Serial & Sensor
  // ===============================
  Serial.begin(115200);       // Serial monitor
  dht.begin();                // Memulai sensor DHT11
  pinMode(BuzzerPin, OUTPUT); // Set pin buzzer sebagai output

  // ===============================
  // Inisialisasi Bluetooth Classic
  // ===============================
  SerialBT.begin("ESP32_Slave"); // Nama perangkat BT
  Serial.println("Menunggu koneksi Bluetooth...");

  // Tunggu sampai ada device yang menghubungkan
  while (!SerialBT.hasClient())
  {
    delay(1000);
  }
  Serial.println("Bluetooth Connected!");

  // ===============================
  // Membuat Task untuk Sensor & Buzzer (Core 0)
  // ===============================
  xTaskCreatePinnedToCore(
      dhtBuzzerTask,        // Fungsi task
      "DHT & Buzzer Task",  // Nama task
      10000,                // Ukuran stack
      NULL,                 // Parameter
      1,                    // Prioritas
      &dhtBuzzerTaskHandle, // Task handle
      0                     // Core tempat task dijalankan
  );

  // ===============================
  // Membuat Task untuk Bluetooth TX (Core 1)
  // ===============================
  xTaskCreatePinnedToCore(
      bluetoothTask,
      "Bluetooth Task",
      10000,
      NULL,
      1,
      &bluetoothTaskHandle,
      1 // Dipisah ke core lain agar proses lebih stabil
  );
}

void loop()
{
  // Loop kosong — semua pekerjaan dilakukan oleh task FreeRTOS
}

// ===========================================================
// Task 1: Membaca DHT11 dan Mengontrol Buzzer
// ===========================================================
void dhtBuzzerTask(void *parameter)
{
  while (true)
  {
    // Membaca suhu & kelembapan
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Cek jika pembacaan gagal
    if (isnan(temperature) || isnan(humidity))
    {
      Serial.println("Gagal membaca dari sensor DHT!");
    }
    else
    {
      // Tampilkan ke serial monitor
      Serial.print("Suhu: ");
      Serial.print(temperature);
      Serial.print(" °C | Kelembapan: ");
      Serial.print(humidity);
      Serial.println(" %");

      // Jika suhu lebih dari 40°C, buzzer aktif
      if (temperature > 40)
      {
        digitalWrite(BuzzerPin, HIGH);
      }
      else
      {
        digitalWrite(BuzzerPin, LOW);
      }
    }

    // Delay agar tidak terlalu sering membaca sensor
    vTaskDelay(3000 / portTICK_PERIOD_MS); // 3 detik
  }
}

// ===========================================================
// Task 2: Mengirimkan Data via Bluetooth Setiap Interval
// ===========================================================
void bluetoothTask(void *parameter)
{
  while (true)
  {
    // Membaca suhu dan kelembapan
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Format string untuk dikirim
    String data = "Suhu: " + String(temperature) + " °C, "
                                                   "Kelembapan: " +
                  String(humidity) + " %";

    // Kirim melalui Bluetooth
    SerialBT.println(data);

    // Delay pengiriman data
    vTaskDelay(5000 / portTICK_PERIOD_MS); // 5 detik
  }
}
