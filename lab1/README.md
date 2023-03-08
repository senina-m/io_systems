# Лабораторная работа 1

**Название:** "Разработка драйверов символьных устройств"

**Цель работы:** 

Получить знания и навыки разработки драйверов символьных устройств для операционной системы Linux.

## Описание функциональности драйвера

При записи в файл символьного устройства текста типа "2+3" должен запоминаться результат операции (5 для примера). Должны поддерживаться операции сложения, вычитания, умножения и деления. Последовательность полученных результатов с момента загрузки модуля ядра должна выводится при чтении файла /proc/var2 в консоль пользователя.

При чтении из файла символьного устройства в кольцевой буфер ядра должен осуществляться вывод тех же данных, которые выводятся при чтении файла /proc/var2.

## Инструкция по сборке

Сборка

`make`

Загрузка модуля в ядро

`sudo insmod ch_drv.ko`

Выгрузка модуля из ядра

`sudo rmmod ch_drv.ko`

Очистка

`make clean`

## Инструкция пользователя

Передать пример на рассчет:

`echo "2+3" > /dev/var2`

Получить список результатов примеров в консоль пользователя:

`cat /proc/var2`

Получить список результатов примеров в колцевой буфер ядра:

`cat /dev/var2`

## Примеры использования

```
natalia@natalia-VirtualBox:~/io_labs/io_systems/lab1$ sudo insmod ch_drv.ko
natalia@natalia-VirtualBox:~/io_labs/io_systems/lab1$ echo "2+3" > /dev/var2
natalia@natalia-VirtualBox:~/io_labs/io_systems/lab1$ echo "3-6" > /dev/var2
natalia@natalia-VirtualBox:~/io_labs/io_systems/lab1$ cat /proc/var2
5-3
```

```
natalia@natalia-VirtualBox:~/io_labs/io_systems/lab1$ cat /dev/var2
natalia@natalia-VirtualBox:~/io_labs/io_systems/lab1$ dmesg
...
[19377.923091] Hello!
[19377.926375] /proc/var2 created
[19394.396821] Driver: open()
[19394.396841] Driver: write()
[19394.396842] Writen buffer 2+3
               \xb4\xb4\xff\xff\x1f\xb1\x14\x97\xff\xff\xff\xff
[19394.396847] Driver: close()
[19408.749360] Driver: open()
[19408.749373] Driver: write()
[19408.749374] Writen buffer 3-6
               \xb4\xb4\xff\xff\x1f\xb1\x14\x97\xff\xff\xff\xff
[19408.749445] Driver: close()
[19415.171743] Procfile read
[19415.171793] Procfile read
[19457.785777] Driver: open()
[19457.785786] Driver: read()
[19457.785786] '5' 
[19457.785787] '\x00' 
[19457.785788] '-' 
[19457.785788] '3' 
[19457.785788] '\x00' 
[19457.785793] Driver: close()
```
