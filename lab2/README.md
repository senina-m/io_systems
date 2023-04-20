# Лабораторная работа 2

**Название:** "Разработка драйверов блочного устройств"

**Цель работы:** 

Получить знания и навыки разработки драйверов блочных устройств для операционной системы Linux.

## Описание функциональности драйвера
Реализовать драйвер блочного устройства, создающего виртуальный диск в RAM со следующими разделами:

Два первичных и один расширенный разделы с размерами 10Мбайт, 20Мбайт и 20Мбайт соответственно. Расширенный раздел должен быть разделен на два логических с размерами
по 10Мбайт каждый.

## Инструкция по сборке

Сборка

`make`

Загрузка модуля в ядро

`sudo insmod lab2.ko`

Выгрузка модуля из ядра

`sudo rmmod lab2.ko`

Очистка

`make clean`

## Инструкция пользователя

С помощью утилиты `sudo fdisk -x` можно посмотреть таблицу разделов нашего диска: 

```
marsen@mint:~/itmo/io_systems/lab2$ sudo fdisk -x


Disk /dev/sda: 25 GiB, 26843545600 bytes, 52428800 sectors
Disk model: VBOX HARDDISK   
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: gpt
Disk identifier: DD438A50-1482-4418-8826-21445E9CF38A
First LBA: 34
Last LBA: 52428766
Alternative LBA: 52428799
Partition entries LBA: 2
Allocated partition entries: 128

Device       Start      End  Sectors Type-UUID                            UUID                                 Name                 Attrs
/dev/sda1     2048     4095     2048 21686148-6449-6E6F-744E-656564454649 158B026A-75DB-4018-B1DA-4D4EC2120394                      
/dev/sda2     4096  1054719  1050624 C12A7328-F81F-11D2-BA4B-00A0C93EC93B 22122E8F-F604-4EA4-9BAD-08B731810A37 EFI System Partition 
/dev/sda3  1054720 52426751 51372032 0FC63DAF-8483-4772-8E79-3D69D8477DE4 60A68EBC-BF46-4324-B472-8774B39416D3                      


Disk /dev/vramdisk: 50 MiB, 52428800 bytes, 102400 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x36e5756d

Device         Boot Start    End Sectors Id Type     Start-C/H/S End-C/H/S Attrs
/dev/vramdisk1          1  20479   20479 83 Linux          0/0/2  159/3/32 
/dev/vramdisk2      20480  61439   40960 83 Linux          0/4/1 159/11/32 
/dev/vramdisk3      61440 102399   40960  5 Extended      0/12/1 159/19/32 
/dev/vramdisk5      61441  81919   20479 83 Linux         0/1/57  159/2/33 
/dev/vramdisk6      81921 102399   20479 83 Linux         0/2/34 159/50/49
```
Чтобы посмотреть время загрузки и выгрузки данных с диска на его же разделы или на диск компьютера можно воспользоваться скриптом `time.sh`
```
marsen@mint:~/itmo/io_systems/lab2$ sudo ./time.sh
mkfs.fat 4.2 (2021-01-31)
mkfs.fat 4.2 (2021-01-31)
mkfs.fat 4.2 (2021-01-31)
mkfs.fat 4.2 (2021-01-31)
5+0 records in
5+0 records out
5242880 bytes (5,2 MB, 5,0 MiB) copied, 0,0248917 s, 211 MB/s
5+0 records in
5+0 records out
5242880 bytes (5,2 MB, 5,0 MiB) copied, 0,023448 s, 224 MB/s
5+0 records in
5+0 records out
5242880 bytes (5,2 MB, 5,0 MiB) copied, 0,0212351 s, 247 MB/s
5+0 records in
5+0 records out
5242880 bytes (5,2 MB, 5,0 MiB) copied, 0,018871 s, 278 MB/s
Copying files within virtual disk
5,00MiB 0:00:00 [ 667MiB/s] [==========================================>] 100%            
pv: input file is output file: /mnt/vramdisk2/file
5,00MiB 0:00:00 [ 732MiB/s] [==========================================>] 100%            
5,00MiB 0:00:00 [ 773MiB/s] [==========================================>] 100%            
5+0 records in
5+0 records out
5242880 bytes (5,2 MB, 5,0 MiB) copied, 0,019487 s, 269 MB/s
5+0 records in
5+0 records out
5242880 bytes (5,2 MB, 5,0 MiB) copied, 0,0180665 s, 290 MB/s
5+0 records in
5+0 records out
5242880 bytes (5,2 MB, 5,0 MiB) copied, 0,0196116 s, 267 MB/s
5+0 records in
5+0 records out
5242880 bytes (5,2 MB, 5,0 MiB) copied, 0,0187785 s, 279 MB/s
Copying files from virtual file to real disk
mkdir: cannot create directory ‘/tmp/io’: File exists
5,00MiB 0:00:00 [1,24GiB/s] [==========================================>] 100%            
5,00MiB 0:00:00 [1,09GiB/s] [==========================================>] 100%            
5,00MiB 0:00:00 [1,31GiB/s] [==========================================>] 100%            
5,00MiB 0:00:00 [1,12GiB/s] [==========================================>] 100%            
```