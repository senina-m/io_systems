#!/bin/bash

if [ "$(whoami)" != "root" ]; then
  sudo "$0" "$@"
  exit $?
fi

mkfs.vfat /dev/vramdisk1
mkfs.vfat /dev/vramdisk2
# mkfs.vfat /dev/vramdisk3
mkfs.vfat /dev/vramdisk5
mkfs.vfat /dev/vramdisk6

mkdir /mnt/vramdisk1
mkdir /mnt/vramdisk2
# mkdir /mnt/vramdisk3
mkdir /mnt/vramdisk5
mkdir /mnt/vramdisk6

mount /dev/vramdisk1 /mnt/vramdisk1
mount /dev/vramdisk2 /mnt/vramdisk2
# mount /dev/vramdisk3 /mnt/vramdisk3
mount /dev/vramdisk5 /mnt/vramdisk5
mount /dev/vramdisk6 /mnt/vramdisk6

function create_files() {
  dd if=/dev/urandom of=/mnt/vramdisk1/file bs=1M count=5
  dd if=/dev/urandom of=/mnt/vramdisk2/file bs=1M count=5
  # dd if=/dev/urandom of=/mnt/vramdisk3/file bs=1M count=5
  dd if=/dev/urandom of=/mnt/vramdisk5/file bs=1M count=5
  dd if=/dev/urandom of=/mnt/vramdisk6/file bs=1M count=5
}

function delete_files() {
  rm /mnt/vramdisk1/file
  rm /mnt/vramdisk2/file
  # rm /mnt/vramdisk3/file
  rm /mnt/vramdisk5/file
  rm /mnt/vramdisk6/file
}

create_files
  echo "Copying files within virtual disk"
  pv /mnt/vramdisk1/file > /mnt/vramdisk2/file
  pv /mnt/vramdisk2/file > /mnt/vramdisk2/file
  # pv /mnt/vramdisk3/file > /mnt/vramdisk5/file
  pv /mnt/vramdisk5/file > /mnt/vramdisk6/file
  pv /mnt/vramdisk6/file > /mnt/vramdisk1/file

delete_files

create_files
  echo "Copying files from virtual file to real disk"
  mkdir /tmp/io
  pv /mnt/vramdisk1/file > /tmp/io/testfile
  pv /mnt/vramdisk2/file > /tmp/io/testfile
  # pv /mnt/vramdisk3/file > /tmp/io/testfile
  pv /mnt/vramdisk5/file > /tmp/io/testfile
  pv /mnt/vramdisk6/file > /tmp/io/testfile
delete_files

umount /mnt/vramdisk1
umount /mnt/vramdisk2
# umount /mnt/vramdisk3
umount /mnt/vramdisk5
umount /mnt/vramdisk6

rmdir /mnt/vramdisk1
rmdir /mnt/vramdisk2
# rmdir /mnt/vramdisk3
rmdir /mnt/vramdisk5
rmdir /mnt/vramdisk6
