---
# 📘 **DHT11 + Buzzer + Bluetooth (ESP32 FreeRTOS)**

Program ini membaca **sensor DHT11**, mengontrol **buzzer** berdasarkan suhu, dan mengirimkan data melalui **Bluetooth Classic**.
Proyek menggunakan **FreeRTOS** dengan dua task yang berjalan di core berbeda pada ESP32.
---

## 🚀 **Fitur Utama**

- Membaca **suhu** dan **kelembapan** dari sensor DHT11
- Mengaktifkan **buzzer otomatis** jika suhu di atas 40°C
- Mengirim data ke perangkat (Android/PC) via **Bluetooth Serial**
- Menggunakan **FreeRTOS multitasking** untuk meningkatkan performa
- Task untuk sensor dan Bluetooth dipisah ke core yang berbeda

---

## 🧩 **Hardware yang Dibutuhkan**

| Komponen        | Keterangan                  |
| --------------- | --------------------------- |
| ESP32 Dev Board | WROOM/WROVER                |
| Sensor DHT11    | Pembacaan suhu & kelembapan |
| Buzzer aktif    | Output alarm                |
| Kabel jumper    | Penghubung                  |

---

## 🔌 **Skema Wiring**

| ESP32   | DHT11 | Buzzer |
| ------- | ----- | ------ |
| 3.3V    | VCC   | —      |
| GND     | GND   | GND    |
| GPIO 32 | DATA  | —      |
| GPIO 33 | —     | +      |

> Buzzer yang digunakan adalah buzzer aktif (tidak perlu PWM).

---

## 🧠 **Arsitektur FreeRTOS**

Program terdiri dari dua task:

### **1. Task DHT11 + Buzzer (core 0)**

- Membaca suhu & kelembapan
- Menampilkan data ke Serial
- Menyalakan buzzer jika suhu > 40°C
- Delay 3 detik

### **2. Task Bluetooth (core 1)**

- Mengirimkan data melalui Bluetooth Classic
- Mengirim setiap 5 detik

Memisahkan task ke core berbeda meningkatkan stabilitas.

---

## 📡 **Cara Menghubungkan Bluetooth**

1. Nyalakan ESP32
2. Cari perangkat Bluetooth bernama:
   **`ESP32_Slave`**
3. Pairing (PIN default biasanya **1234** atau **0000**)
4. Gunakan aplikasi seperti:

   - Serial Bluetooth Terminal (Android)
   - Bluetooth Serial Monitor (PC)

Data yang dikirim berupa:

```
Suhu: 28.3 °C, Kelembapan: 70 %
```

---

## 🧾 **Penjelasan Logika Utama**

### **Buzzer**

Buzzer akan ON jika:

```
temperature > 40°C
```

### **Bluetooth**

Mengirim data setiap 5 detik dalam format:

```
"Suhu: xx °C, Kelembapan: xx %"
```

---

## 📂 **Struktur Program**

- `dhtBuzzerTask()` → Pembacaan sensor & kontrol buzzer
- `bluetoothTask()` → Pengiriman data via BT
- `FreeRTOS` digunakan untuk multitasking
- `loop()` kosong karena task berjalan di background

---

## 🛠️ **Library yang Digunakan**

Pastikan library berikut sudah terinstal:

- **Adafruit Unified Sensor**
- **DHT sensor library**
- **BluetoothSerial (default ESP32)**

---

## ▶️ **Cara Menggunakan**

1. Upload program ke ESP32
2. Periksa Serial Monitor (115200)
3. Pairing Bluetooth
4. Buka aplikasi Serial Bluetooth
5. Lihat data suhu & kelembapan
6. Buzzer akan menyala otomatis jika suhu terlalu tinggi

---

## 📜 **Lisensi**

Bebas digunakan untuk keperluan pembelajaran, riset, dan proyek IoT.

---
