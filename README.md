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
 Transformasi Karakter Rot13 (rot13_char)
 sebuah cipher substitusi sederhana yang menggeser huruf sebanyak 13 posisi dalam alfabet. Hanya huruf (a-z, A-Z) yang dipengaruhi, karakter selain huruf dikembalikan tanpa perubahan.
```
static char rot13_char(char c) {
    if (c >= 'a' && c <= 'z') {
        return ((c - 'a' + 13) % 26) + 'a';
    } else if (c >= 'A' && c <= 'Z') {
        return ((c - 'A' + 13) % 26) + 'A';
    }
    return c;
}

```
Transformasi String Rot13 (rot13_string)
Fungsi ini menerima string input, menerapkan transformasi rot13_char pada setiap karakter, dan menyimpan hasilnya dalam string output. Fungsi ini bekerja pada string dan menerapkan cipher ROT13 pada seluruh karakter.
```
static void rot13_string(const char *input, char *output) {
    int i;
    for (i = 0; input[i] != '\0'; i++) {
        output[i] = rot13_char(input[i]);
    }
    output[i] = '\0'; 
}
 ```
Mendapatkan Path Asli dari Path yang Dienkripsi (get_real_path)

Fungsi get_real_path menerjemahkan nama file yang telah di-mount secara virtual menggunakan ROT13 menjadi path asli di filesystem nyata.
```
static void get_real_path(const char *path, char *real_path) {
    char decrypted_path[4096];
    rot13_string(path, decrypted_path);
    snprintf(real_path, 4096, "%s%s", ROT13_DATA->root_dir, decrypted_path);
}

```
 Mengambil Atribut File atau Direktori (rot13_getattr)
  Fungsi ini mengambil atribut (misalnya, ukuran file, izin akses) dari file atau direktori. Fungsi ini mendekripsi path dengan get_real_path, kemudian memanggil lstat untuk mendapatkan atribut file yang sebenarnya.
```
static int rot13_getattr(const char *path, struct stat *stbuf,
                         struct fuse_file_info *fi) {
    (void) fi;
    int res;
    char real_path[4096];

    get_real_path(path, real_path);

    res = lstat(real_path, stbuf);
    if (res == -1) {
        return -errno;
    }
    return 0;
}
 
```
 Membaca Isi Direktori (rot13_readdir)
 Fungsi ini membaca isi dari sebuah direktori. Path direktori didekripsi terlebih dahulu, lalu direktori dibuka. Setiap entri direktori diubah namanya dengan cipher ROT13 sebelum ditambahkan ke dalam daftar direktori yang dikembalikan.
 ```
static int rot13_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi,
                         enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;
    (void) flags;

    char real_path[4096];
    get_real_path(path, real_path);

    DIR *dp = opendir(real_path);
    if (dp == NULL) {
        return -errno;
    }

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        char encrypted_name[256];
        rot13_string(de->d_name, encrypted_name);
        
        if (filler(buf, encrypted_name, &st, 0, 0)) {
            break;
        }
    }

    closedir(dp);
    return 0;
}
  ```
Membuka File (rot13_open)
Fungsi ini membuka file. Path file didekripsi terlebih dahulu, lalu file dibuka dengan flag yang ditentukan (fi->flags). Deskriptor file (res) disimpan di dalam fi->fh.
``` 
static int rot13_open(const char *path, struct fuse_file_info *fi) {
    int res;
    char real_path[4096];

    get_real_path(path, real_path);

    res = open(real_path, fi->flags);
    if (res == -1) {
        return -errno;
    }

    fi->fh = res;
    return 0;
}

```
Membaca Isi File (rot13_read)
Fungsi ini membaca data dari file. Fungsi ini membaca size byte dari file, dimulai dari offset yang diberikan, dan menyimpannya di dalam buf. Deskriptor file diambil dari fi->fh.
```
static int rot13_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {
    (void) path;
    int res = pread(fi->fh, buf, size, offset);
    if (res == -1) {
        res = -errno;
    }
    return res;
}
```
 Melepaskan File (rot13_release)
 : Fungsi ini melepaskan file setelah selesai digunakan. Fungsi ini menutup file menggunakan deskriptor file yang ada di fi->fh.
 ```
static int rot13_release(const char *path, struct fuse_file_info *fi) {
    (void) path;
    close(fi->fh);
    return 0;
}

```

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
