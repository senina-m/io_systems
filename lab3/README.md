# Лабораторная работа 3

**Название:** "Разработка драйверов сетевых устройств"

**Цель работы:** 

Получить знания и навыки разработки драйверов сетевых интерфейсов для операционной системы Linux.

## Описание функциональности драйвера
1. Драйвер должен создавать виртуальный сетевой интерфейс в ОС Linux.
2. Созданный сетевой интерфейс должен перехватывать пакеты родительского интерфейса (eth0 или другого).
3. Сетевой интерфейс должен перехватывать:
    Пакеты протокола IPv4, адресуемые конкретному IP. Вывести IP адреса отправителя и получателя.
    Состояние разбора пакетов необходимо выводить в файл в директории /proc
4. Должна иметься возможность просмотра статистики работы созданного интерфейса.

## Инструкция по сборке

Сборка

`make`

Загрузка модуля в ядро

`sudo insmod virt_net_if.ko`

Выгрузка модуля из ядра

`sudo rmmod virt_net_if`

Очистка

`make clean`

## Инструкция пользователя

Выполнить после загрузки модуля команду `ifconfig` и проверить, что на устройстве появился новый интерфейс:
```
lab3: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.56.101  netmask 255.255.255.0  broadcast 192.168.56.255
        inet6 fe80::92b:bf1c:4827:40dd  prefixlen 64  scopeid 0x20<link>
        ether 08:00:27:2e:fc:6a  txqueuelen 1000  (Ethernet)
        RX packets 18  bytes 1609 (1.6 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 17  bytes 2574 (2.5 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

Запомнить указанный в интерфейсе IP адрес и "пингануть" его с другой машины командой `ping 192.168.56.101` (для IP адресса из примера)

Так как пингуется этот IP адрес UDP дейтаграммами, которые посылаются на сетевом уровне как IP пакеты. Наш драйвер должен будет их отфильтровать. 
И их будет видно в файле `/proc/var2`. Его содержимое можно увидеть командой `cat /proc/var2`:

Пример вывода:
```
marsen@mint:~/itmo/io_systems/lab3$ cat /proc/var2
Captured IP packet, saddr: 0.0.0.0
daddr: 255.255.255.255
Captured IP packet, saddr: 192.168.56.100
daddr: 255.255.255.255
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.22
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.22
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.101
daddr: 224.0.0.251
Captured IP packet, saddr: 192.168.56.1
daddr: 192.168.56.101
```
