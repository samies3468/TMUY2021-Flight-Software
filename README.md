# İTÜ APİS AR-GE 10B | TÜRKSAT Model Uydu Yarışması 2021 – Uçuş Yazılımı ve Teknik Belgeler

Bu depo, İstanbul Teknik Üniversitesi APİS AR-GE 10B Takımı'nın **TÜRKSAT Model Uydu Yarışması 2021** kapsamında geliştirdiği uçuş yazılımı, otonom kontrol algoritması ve yarışma sürecinde hazırlanan teknik raporları içermektedir. 

---
## 🚀 Görev Senaryosu

Yarışma kapsamında geliştirilen Model Uydu’nun görevi, aşağıdaki görev adımlarını başarıyla ve otonom şekilde yerine getirmektir:

1. **Roketle fırlatma:** Model Uydu (taşıyıcı + görev yükü) yaklaşık **700 m** yüksekliğe roket ile taşınır.
2. **Pasif iniş:** Roketten ayrıldıktan sonra sistem, 10–14 m/s hızla pasif olarak **400 m** yüksekliğe kadar serbest düşüş yapar.
3. **Taşıyıcıdan ayrılma:** **400 m (+/-10 m)** irtifada, görev yükü taşıyıcıdan **otonom bir mekanizma** ile ayrılır. Eğer bu işlem başarısız olursa, yer istasyonu manuel ayrılma komutu gönderir.
4. **Aktif iniş başlatma:** Taşıyıcıdan ayrılan görev yükü, üzerinde bulunan **aktif iniş sistemi (auto-gyro)** ile 8–10 m/s hızla inişe başlar.
5. **Bonus Görev – İrtifa sabitleme:** **200 m (+/- 50 m)** yüksekliğe ulaşıldığında görev yükü **yaklaşık 10 saniye boyunca askıda kalır**, yani belirli bir irtifada sabitlenir.
6. **İnişin tamamlanması:** Sabitleme görevinden sonra aktif iniş devam eder ve görev yükü güvenli biçimde yere iner.
7. **Veri aktarımı ve görev takibi:**
   - Uçuş boyunca sıcaklık, basınç, yükseklik, ivme, konum, pil gerilimi ve yönelme gibi telemetri verileri **her saniye (1 Hz)** yer istasyonuna aktarılır.
   - Yer istasyonuna **canlı video görüntüsü** gönderilir.
   - Telemetri verileri ve video ayrıca **SD karta da kayıt edilir.**
   - İniş sonrası görev yükü, 1 dakika boyunca veri iletmeye devam eder.
8. **Kurtarma:** İnişten sonra görev yükünün yerini tespit etmek için **GPS**, **sesli ikaz (buzzer)** ve **renkli duman kapsülü** birlikte çalışır. Böylece görev yükü kolayca bulunabilir.

Görev, otonomluk, veri iletimi, yazılım güvenliği ve sistem entegrasyonu açısından çok disiplinli bir mühendislik yaklaşımı gerektirir.

---
## 📄 Teknik Raporlar

### `TMUY2021_56727_PDR_v1.1.pdf` – **Ön Tasarım Gözden Geçirme Raporu (Preliminary Design Review)**

Bu rapor, yarışmanın ilk aşaması olan **PDR (Preliminary Design Review)** kapsamında hazırlanmıştır. Proje başlangıcında takımın ortaya koyduğu sistem mimarisi, tasarım seçenekleri ve görev akış planları yer almaktadır.

**İçerik Başlıkları:**
- Görev Özeti ve Sistem Konsepti
- Sensör, İniş Kontrol, Elektrik, Haberleşme ve Uçuş Yazılımı Alt Sistemlerinin ön tasarımı
- 2 farklı iniş konfigürasyonunun (çift motorlu yatay & dört motorlu dikey) karşılaştırması
- Model uydu fiziksel tasarımı ve görev senaryosu
- Sistem isterleri listesi ve görev akış diyagramları

Bu belge, tasarım kararlarının mühendislik gerekçeleriyle nasıl şekillendirildiğini gösterir.

---

### `TMUY2021_56727_CDR_v1.1.pdf` – **Kritik Tasarım İnceleme Raporu (Critical Design Review)**

PDR sonrası yapılan tüm geliştirme ve değişiklikleri içeren **CDR (Critical Design Review)** raporudur. Yarışma jürisine teslim edilen en kapsamlı teknik dokümandır.

**İçerik Başlıkları:**
- Alt sistemlerin nihai hali: mekanik, elektrik, sensör, yazılım, yer istasyonu ve haberleşme
- Uçuşa hazır sistem konfigürasyonu
- Telemetri ve video aktarımı test senaryoları
- Sistem isterlerinin sağlandığını gösteren doğrulama matrisi
- Uçuş ve kurtarma operasyonu planı
- Sistem seviyesi test planları ve çevresel dayanıklılık analizleri

Bu rapor, model uydunun uçuş öncesi son teknik kontrolüdür.

---

## 🧠 Kaynak Kodlar

### `TMUY2021_56727_UCUSYAZILIMI.ino` – **Arduino Tabanlı Uçuş Kontrol Yazılımı (Gömülü Sistem)**

Bu dosya, görev yükünün üzerinde doğrudan çalışan, uçuşun her aşamasını otonom şekilde yöneten **Arduino tabanlı uçuş kontrol yazılımıdır**. TÜRKSAT Model Uydu Yarışması sistem isterlerine tam uyumlu olacak şekilde geliştirilmiştir.

Yazılım, tüm uçuş sürecini gerçek zamanlı olarak yönetir ve aşağıdaki temel işlevleri yerine getirir:

---

## ⚙️ Sistem Mimarisi

### 1. Başlatma ve Kalibrasyon
- Sistem açıldığında tüm sensör bileşenlerinin çalıştığı kontrol edilir.
- SD kart erişimi doğrulanır.
- Uçuş öncesi kalibrasyon için belirli bir bekleme süresi tanımlıdır.
- Barometrik basınç sensörü sıfır noktası alınır (basınç deniz seviyesi referansı).

### 2. Pasif İniş Takibi
- Uçuşun ilk aşamasında görev yükü taşıyıcıyla birlikte pasif iniştedir.
- Barometrik sensörden alınan yükseklik verisi her döngüde güncellenir.
- İrtifa 400 m’ye düştüğünde taşıyıcıdan ayrılma koşulu kontrol edilir.

---

## 🔄 Olay Yönetimi ve Durum Geçişleri

Yazılım, bir **durum makinesi (state machine)** mantığıyla çalışır. Her durum, aşağıdaki koşullarla birbirine bağlıdır:

| AŞAMA | KOŞUL | YAPILAN İŞLEM |
|-------|-------|----------------|
| 0 – Hazırlık | Sistem açıldı | Sensör kalibrasyonu, SD kart hazırlığı |
| 1 – Pasif iniş | İrtifa > 400 m | Telemetri yayını başlar |
| 2 – Ayrılma | İrtifa ≤ 400 m | Ayrılma mekanizması tetiklenir |
| 3 – Aktif iniş | Ayrılma tamamlandı | Motorlar devreye alınır |
| 4 – Askıda kalma | 200 m ± 50 m | İrtifa sabitleme algoritması uygulanır |
| 5 – İniş sonu | İrtifa < 20 m | Motorlar durdurulur, iniş tamamlanır |

---

## 📡 Telemetri Yayını

Yazılım, yarışma gereği olarak her saniye (1 Hz) güncellenen telemetri paketleri üretir ve gönderir. Paketler şunları içerir:

- Zaman damgası
- İrtifa (barometrik)
- Hız (hesaplanan)
- Pil voltajı
- GPS konumu
- Sıcaklık (isteğe bağlı)
- Yönelim (gyroscope verisi)
- Paket numarası (artımlı sayaç, yeniden başlatıldığında devam eder)

**Aynı veriler eş zamanlı olarak SD karta da yazılır.**

---

## 🧾 Veri Kaydı ve Yedekleme

- SD karta `.CSV` formatında telemetri verisi kaydedilir.
- Her paket, zaman damgası ile birlikte yazılır.
- Uçuş esnasında sistem yeniden başlasa dahi kaldığı yerden devam edecek şekilde **veri bütünlüğü sağlanır**.
- Yazılım, dosya açma/kapatma işlemlerini güvenli şekilde yönetir (`flush()` çağrılarıyla).

---

## 📌 Hata Yönetimi ve Güvenlik

- Sensörlerden veri alınamadığında yazılım hata bayrakları üretir.
- SD kart yazım hataları için yedek dosya açma mekanizması mevcuttur.
- Voltaj düşüşü gibi kritik durumlarda sistem pasif moda geçer.
- Motorlar, belirli koşullar dışında çalışmaz (örneğin, yer kontrolü yapılmamışsa).

---

## 🕒 Zamanlama ve Performans

- Kodda döngü süresi optimize edilmiştir (genellikle <100 ms).
- Zamanlayıcı olarak `millis()` kullanılır.
- 1 Hz telemetri yayını `modulo` zaman kontrolüyle sağlanır.
- ISR (interrupt) kullanılmaz; polling temelli yapı tercih edilmiştir.

---


---

### `TMUY2021_56727_OTONOMINISALGORITMASI.py` – **Otonom İniş Algoritması**

Bu Python dosyası, yarışma uydusunun otonom iniş sürecini yöneten uçuş kontrol algoritmasını içermektedir. Kod, `DroneKit` ve `MAVLink` protokolleri kullanılarak yazılmış olup, hem **saha testlerinde** hem de **yarışma uçuşu sırasında gerçek zamanlı olarak** görev yükü üzerinde çalıştırılmıştır.

---

## ⚙️ Genel Yapı ve Amaç

Yazılım, görev yükünün taşıyıcıdan ayrıldıktan sonra motorlu iniş sürecini otonom olarak yöneten bir kontrol algoritmasıdır. Ana işlevi, iniş boyunca belirli irtifalarda belirli görevleri yerine getirmek ve inişi kararlı biçimde sonlandırmaktır.

---

## 🔁 Uçuş Aşamaları ve Akış

Algoritma üç ana fazdan oluşur:

### 1. **Aktif İniş Başlatma (≈ 400 m)**
- Yazılım sürekli olarak irtifayı izler.
- Araç 320–360 m aralığına girdiğinde `GUIDED` moduna geçilir.
- Yön kontrolü ve motor manevraları başlatılır.

### 2. **Spiral İniş ve İrtifa Sabitleme (≈ 200 m)**
- İniş, yatay yönlendirme ve düşey hız kontrolü ile gerçekleştirilir.
- Yaklaşık 200 m irtifada görev yükü kısa süreli askıda kalır (bonus görev).
- Bu sabitleme, yön değiştirme ve hız azaltma manevraları ile sağlanır.

### 3. **İnişi Sonlandırma (≈ 30 m)**
- Belirli alt irtifa aralığında tüm hareketler durdurulur.
- `VehicleMode("LAND")` komutu ile uçuş yazılımı inişi tamamlar.
- Ardından sistem güvenli şekilde kapanır.

---

## 📡 Telemetri ve Konum İzleme

- Uçuş boyunca `vehicle.location.global_relative_frame.alt` ile irtifa izlenir.
- `condition_yaw()` fonksiyonu ile yön belirlenir.
- `send_ned_velocity()` fonksiyonu ile her eksendeki hareketler gönderilir.
- Bu komutlar, belirli aralıklarla (≈ 28 Hz) araca gönderilir.

---

## 🔐 Güvenlik ve Kontrol

- Her irtifa aralığı için ayrı güvenlik koşulları tanımlanmıştır.
- `VehicleMode("LAND")` yalnızca son aşamada tetiklenir.
- Hedef irtifalar arasında geçiş koşulları yazılım içinde açıkça tanımlıdır.
- Kod sonunda `vehicle.close()` komutuyla bağlantı güvenli şekilde kapatılır.

---



---


