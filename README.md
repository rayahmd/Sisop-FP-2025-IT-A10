# Final Project Sistem Operasi IT

## Peraturan
1. Waktu pengerjaan dimulai hari Kamis (19 Juni 2025) setelah soal dibagikan hingga hari Rabu (25 Juni 2025) pukul 23.59 WIB.
2. Praktikan diharapkan membuat laporan penjelasan dan penyelesaian soal dalam bentuk Readme(github).
3. Format nama repository github “Sisop-FP-2025-IT-[Kelas][Kelompok]” (contoh:Sisop-FP-2025-IT-A01).
4. Setelah pengerjaan selesai, seluruh source code dan semua script bash, awk, dan file yang berisi cron job ditaruh di github masing - masing kelompok, dan link github dikumpulkan pada form yang disediakan. Pastikan github di setting ke publik.
5. Commit terakhir maksimal 10 menit setelah waktu pengerjaan berakhir. Jika melewati maka akan dinilai berdasarkan commit terakhir.
6. Jika tidak ada pengumuman perubahan soal oleh asisten, maka soal dianggap dapat diselesaikan.
7. Jika ditemukan soal yang tidak dapat diselesaikan, harap menuliskannya pada Readme beserta permasalahan yang ditemukan.
8. Praktikan tidak diperbolehkan menanyakan jawaban dari soal yang diberikan kepada asisten maupun praktikan dari kelompok lainnya.
9. Jika ditemukan indikasi kecurangan dalam bentuk apapun di pengerjaan soal final project, maka nilai dianggap 0.
10. Pengerjaan soal final project sesuai dengan modul yang telah diajarkan.

## Kelompok 10

Nama | NRP
--- | ---
Fika Arka Nuriyah | 5027241071
Zahra Hafizhah | 5027241121
Raya Ahmad Syarif | 5027241041
Hafiz Ramadhan | 5027241096

## Deskripsi Soal
Buatlah FUSE file system yang mengenkripsi nama file menggunakan ROT13. Saat direktori asli di-mount, semua nama file dan folder yang terlihat di dalamnya harus berubah. Misalnya, dokumen.txt menjadi qbxhzra.txt. File system harus bisa menerjemahkan kembali nama yang terenkripsi ini agar file tetap bisa diakses.



### Catatan
Pengerjaan menggunakan library FUSE3 (Filesystem in Userspace). Enkripsi dilakukan hanya pada nama file dan folder, bukan isi file. Enkripsi yang digunakan adalah ROT13 (shift 13 huruf alfabet). Saat sistem dijalankan, nama file di direktori asli (misalnya percobaan/) akan tampil dalam bentuk terenkripsi di direktori mountpoint (misalnya mountdir/), tetapi tetap bisa diakses seperti biasa. Semua perubahan hanya terjadi secara virtual ketika sistem dijalankan, dan akan hilang saat unmount.

Struktur repository:
```
Sisop-FP-2025-IT-A10/
├── assets
├── README.md
└── testcase26.c        
```

## Pengerjaan

> ROT13

**Teori**

ROT13 adalah algoritma cipher substitusi monoalfabetik yang termasuk dalam keluarga Caesar Cipher. Setiap huruf dalam alfabet diganti dengan huruf 13 posisi setelahnya dalam alfabet. Karena jumlah huruf dalam alfabet Latin adalah 26, maka ROT13 bersifat involutif (jika dilakukan dua kali akan mengembalikan ke teks asli).

ROT13 sangat ringan dan cepat karena hanya memanipulasi karakter berdasarkan posisi ASCII, tanpa kunci eksternal.

ROT13 tidak aman untuk data sensitif, tapi cukup efektif untuk obfuscation (penyamaran) seperti pada nama file.
**Solusi**
```
static void rot13_string(const char *input, char *output);
static void get_real_path(const char *path, char *real_path);

```
Fungsi get_real_path menerjemahkan nama file yang telah di-mount secara virtual menggunakan ROT13 menjadi path asli di filesystem nyata.

> FUSE

**Teori**

Sistem file berbasis FUSE (Filesystem in Userspace) dapat digunakan untuk mengintersep aktivitas file secara real-time, termasuk mendeteksi pola enkripsi yang mencurigakan seperti pada serangan ransomware. Dengan memanfaatkan posisi FUSE di user space, sistem dapat memantau dan memproses metadata dan isi file secara fleksibel, tanpa harus memodifikasi kernel. 
Jurnal ini menjelaskan bagaimana komponen sistem berkas seperti Virtual Filesystem Switch (VFS) dapat dipindahkan dari kernel ke userspace menggunakan FUSE. Dengan FUSE, pengembang dapat membuat sistem berkas baru tanpa akses kernel, lebih aman, dan portabel. Salah satu implementasi populer adalah EncFS, yang memungkinkan enkripsi file. Konsep ini sejalan dengan proyek kamu, karena kamu mengenkripsi nama file menggunakan ROT13 di lapisan userspace menggunakan FUSE.

**Solusi**
```
static int rot13_readdir(const char *path, void *buf, fuse_fill_dir_t filler, ...);
static int rot13_open(const char *path, struct fuse_file_info *fi);
static int rot13_getattr(const char *path, struct stat *stbuf, ...);
```
...

**Video Menjalankan Program**  
[Video Demo](https://drive.google.com/file/d/1SqOQzBEGqVrlrP4K7dwshtyNt9GWL9_V/view?usp=sharing)



## Daftar Pustaka

Kurnia, A., Prasetyo, R., & Lestari, D. (2025). Analisis Performa Beaufort Cipher dan ROT13 dalam Proses Enkripsi dan Dekripsi pada Data Teks. Jurnal Media Informatika, 10(1), 45–52. https://ejournal.sisfokomtek.org/index.php/jumin/article/view/5363/3353

Faber, Ł., & Boryczko, K. (2013). Portable userspace virtual filesystem switch. Computer Science, 14(3), 345–360. https://doi.org/10.7494/csci.2013.14.3.345
