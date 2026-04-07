# Project UTS SBM

### Anggota Kelompok:
1. Garjita Adicandra (24/535339/TK/59377)
2. M. Nabil Fitriansyah Boernama (24/545232/TK/60628)

This is a Midterm Project (Ujian Tengah Semester - UTS) for the SBM course. The project is an embedded system built around the STM32F401CCUX microcontroller using STM32CubeIDE and the HAL library. 

It implements a state machine with 4 distinct operating modes involving a DHT22 temperature and humidity sensor, a potentiometer, an SSD1306 OLED display, 8 LEDs, and 2 push buttons.

## Hardware Components
- **Microcontroller**: STM32F401CCUX (STM32 Black Pill)
- **Display**: SSD1306 OLED Display (I2C)
- **Sensor**: DHT22 (Temperature & Humidity)
- **Analog Input**: Potentiometer
- **Outputs**: 8x LEDs
- **Inputs**: 2x Push Buttons

## Pin Configuration (Pinout)

| Component | MCU Pin | Peripheral / Notes |
|---|---|---|
| **Button 1** | `PA0` | GPIO Input (Changes state) |
| **Button 2** | `PA1` | EXTI1 Interrupt |
| **Potentiometer** | `PA3` | ADC1 Channel 3 |
| **DHT22** | `PA4` | GPIO Output (Bit-banging) |
| **OLED SCL** | `PB10` | I2C2 SCL |
| **OLED SDA** | `PB3` | I2C2 SDA |
| **LED 1** | `PB9` | Output |
| **LED 2** | `PB7` | Output |
| **LED 3** | `PB5` | Output |
| **LED 4** | `PA15` | Output |
| **LED 5** | `PA11` | Output |
| **LED 6** | `PA9` | Output |
| **LED 7** | `PB15` | Output |
| **LED 8** | `PB13` | Output |

## Features & State Machine

The system uses `Button 1` or a virtual button to cycle through 4 different states (State 0 to 3). At any point, pressing `Button 2` triggers an external interrupt that forces all 8 LEDs to turn on for 5 seconds. The system is also integrated with **STM32CubeMonitor** for real-time dashboarding and control.

### State 0: Running LED
* Deretan 8 LED menyala satu per satu dari kiri ke kanan dan kembali mulai dari kiri.
* State LED ditampilkan di dalam STM32CubeMonitor dengan warna merah sebagai penanda mati dan warna hijau sebagai penanda hidup.
* Menampilkan state pada oled display.

### State 1: Gauge Animation (Kreativitas NIM)
* LED Padam (Kreativitas yang dilakukan adalah menyalakan LED berdasarkan increment dimana 8 LED akan membagi setiap angka terakhir NIM menjadi 8 bagian yang mana ketika angka tersebut tercapai maka LED akan menyala).
* Menampilkan sebuah Chart di STM32CubeMonitor berupa grafik cacahan increment, dengan cacahan pada NIM pertama hingga mencapai angka 28 dan cacahan NIM kedua hingga mencapai angka 77.
* State LED ditampilkan di dalam STM32CubeMonitor dengan warna merah sebagai penanda mati dan warna hijau sebagai penanda hidup.
* Menampilkan state pada oled display.

### State 2: ADC / Potensiometer
* Nilai tegangan yang menjadi output dari potensiometer akan secara incremental menyalakan LED dari kiri hingga kanan jika diputar penuh.
* State LED ditampilkan di dalam STM32CubeMonitor dengan warna merah sebagai penanda mati dan warna hijau sebagai penanda hidup.
* Nilai tegangan output dari potensiometer akan ditampilkan dalam gauge di STM32CubeMonitor.
* Menampilkan state pada oled display.

### State 3: DHT22 Sensor Suhu & Kelembapan
* Pembacaan suhu dan kelembapan udara oleh DHT22.
* Menampilkan suhu dan kelembapan saat ini dengan LED berdasarkan threshold yang telah ditentukan berdasarkan suhu dan kelembapan normal.
* Menampilkan state, suhu, dan kelembapan udara saat ini di oled display.
* Menampilkan suhu dan kelembapan udara saat ini di STM32CubeMonitor.

### Interrupt & Control
* **Interrupt**: Menyalakan semua LED selama 5 detik. Menampilkan status interrupt pada oled display.
* **Button 1**: Menambah variabel state dengan 1 dengan tujuan untuk lanjut ke state berikutnya.
* **Button 2**: Menjadi trigger untuk interrupt.
* **Button Virtual**: Melalui STM32CubeMonitor dapat dilakukan perubahan state seperti button 1 dan interrupt seperti button 2.
* **Write Panel**: Melalui STM32CubeMonitor dapat dilakukan overwrite nilai dari variabel yang diberi tipe volatile dan dipilih pada variabel node.

## Development Environment
- **IDE**: STM32CubeIDE
- **Firmware Package**: STM32Cube FW_F4
- **Language**: C

## How to Build & Flash
1. Open the `.project` or `.cproject` file using STM32CubeIDE.
2. Ensure you have the `Core` and `Drivers` directories present.
3. Build the project using `Project -> Build Project` (or the hammer icon).
4. Connect the STM32 to your PC using an ST-Link (or equivalent).
5. Flash using `Run -> Debug` or `Run -> Run`.
