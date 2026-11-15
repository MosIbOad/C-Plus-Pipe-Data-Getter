# C-Plus-Pipe-Data-Getter

Windows İsimlendirilmiş Boru Hattı (**Named Pipe**) üzerinden veri okuyan ve gelen verileri **JSON** formatında çözümleyip işleyen C++ istemci (client) uygulaması.

---

## 📌 Proje Hakkında

Bu proje, Windows işletim sistemi üzerinde çalışan bir IPC (Inter-Process Communication / Süreçler Arası İletişim) istemcisidir. Uygulama, `\\.\pipe\MyPipe` adresindeki önceden tanımlanmış bir Named Pipe hattına bağlanır ve karşı taraftan (sunucu süreçten) gelen veri paketlerini dinler.

Gelen bayt akışı string formatına dönüştürüldükten sonra **nlohmann/json** kütüphanesi yardımıyla doğrulanır ve JSON içeriğindeki anahtar-değer (`key` - `value`) çiftleri ayıklanarak konsol ekranına basılır.

---

## ✨ Özellikler

- **Windows API Entegrasyonu:** Windows Named Pipe (`CreateFileA`, `ReadFile`, `CloseHandle`) mimarisi ile yerel süreçler arası hızlı veri iletişimi.
- **JSON Ayrıştırma:** Gömülü [nlohmann/json](https://github.com/nlohmann/json) kütüphanesi ile güvenli JSON doğrulama ve veri çıkarma.
- **Hata Yönetimi:**
  - Sunucuya bağlanılamadığı durumlarda hata kodu (`GetLastError()`) bildirimi.
  - Hatalı/bozuk veya eksik JSON verilerine karşı `json::exception` yakalama ve koruma.
  - Boş veri paketlerini algılama ve yönetme.
- **Sürekli Dinleme Döngüsü:** Bağlantı aktif olduğu sürece pipe üzerinden veri okuma.

---

## 🏗️ Proje Yapısı

```text
C-Plus-Pipe-Data-Getter/
├── .gitignore                      # Git takip dışı bırakma kuralları
├── README.md                       # Proje tanıtım ve kullanım kılavuzu
├── C-Plus-Pipe-Data-Getter.sln     # Visual Studio Solution dosyası
└── C-Plus-Pipe-Data-Getter/
    ├── C-Plus-Pipe-Data-Getter.cpp # Ana kaynak kod (Named Pipe istemcisi)
    ├── json.hpp                    # nlohmann/json kütüphanesi başlık dosyası
    ├── C-Plus-Pipe-Data-Getter.vcxproj
    ├── C-Plus-Pipe-Data-Getter.vcxproj.filters
    └── C-Plus-Pipe-Data-Getter.vcxproj.user
```

---

## 🚀 Beklenen Veri Formatı

Uygulama, gelen JSON paketinde en az `key` ve `value` alanlarının bulunmasını bekler:

```json
{
  "key": "sensor_temperature",
  "value": "24.5C"
}
```

Eğer gelen veri geçerli bir JSON değilse veya `key` / `value` alanlarından biri eksikse, uygulama hata mesajı basarak bir sonraki veriyi beklemeye devam eder.

---

## 🛠️ Gereksinimler

- **İşletim Sistemi:** Windows 10 / 11 veya Windows Server
- **Geliştirme Ortamı / Derleyici:** 
  - Visual Studio 2022 (MSVC v143 veya üzeri)
  - C++17 veya C++20 standardı desteği
  - Windows SDK (10.0 veya üzeri)

---

## ⚙️ Kurulum ve Derleme

1. Projeyi bilgisayarınıza klonlayın:
   ```bash
   git clone https://github.com/KULLANICI_ADI/C-Plus-Pipe-Data-Getter.git
   cd C-Plus-Pipe-Data-Getter
   ```

2. `C-Plus-Pipe-Data-Getter.sln` dosyasını Visual Studio ile açın.

3. Üst araç çubuğundan hedef yapılandırmayı seçin:
   - **Configuration:** `Debug` veya `Release`
   - **Platform:** `x64` (veya `Win32`)

4. Menüden **Build > Build Solution** (Ctrl + Shift + B) seçeneğine tıklayarak projeyi derleyin.

5. Derlenen çalıştırılabilir dosya `x64/Release/C-Plus-Pipe-Data-Getter.exe` veya `x64/Debug/` altında hazır olacaktır.

---

## 🧪 Nasıl Test Edilir?

Bu program bir **istemci (client)** olduğu için, çalıştırıldığında dinleyebileceği bir **sunucu (server)** Named Pipe hattının açık olması gerekir. Aksi takdirde uygulama `Baglanti basarisiz!` hatası verip kapanacaktır.

### Hızlı Test Sunucusu (Python Örneği)

Test etmek için aynı makinede aşağıdaki basit Python betiğini çalıştırabilirsiniz:

```python
# pipe_server.py
import time
import win32pipe
import win32file

PIPE_NAME = r'\\.\pipe\MyPipe'

print("Named Pipe sunucusu olusturuluyor...")
pipe = win32pipe.CreateNamedPipe(
    PIPE_NAME,
    win32pipe.PIPE_ACCESS_OUTBOUND,
    win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_READMODE_MESSAGE | win32pipe.PIPE_WAIT,
    1, 65536, 65536,
    0,
    None
)

print("İstemci baglantisi bekleniyor...")
win32pipe.ConnectNamedPipe(pipe, None)
print("İstemci baglandi! Veri gonderiliyor...")

messages = [
    '{"key": "user_id", "value": "12345"}',
    '{"key": "status", "value": "online"}',
    '{"key": "command", "value": "start_processing"}'
]

for msg in messages:
    time.sleep(2)
    print(f"Gonderiliyor: {msg}")
    win32file.WriteFile(pipe, msg.encode('utf-8'))

time.sleep(1)
win32file.CloseHandle(pipe)
print("Sunucu kapatildi.")
```

> **Not:** Python betiği için `pip install pywin32` gereklidir.

1. Önce Python test sunucusunu başlatın.
2. Ardından `C-Plus-Pipe-Data-Getter.exe` uygulamasını çalıştırın.
3. Konsolda verilerin ayrıştırıldığını göreceksiniz:
   ```text
   Baglandi! Veri bekleniyor...

   Okunan Veri: [{"key": "user_id", "value": "12345"}]

   Key: user_id | Value: 12345
   ```

---

## 📄 Lisans

Bu proje kişisel / eğitim amaçlı bir projedir. İhtiyaçlarınıza göre serbestçe geliştirebilir ve kullanabilirsiniz.
