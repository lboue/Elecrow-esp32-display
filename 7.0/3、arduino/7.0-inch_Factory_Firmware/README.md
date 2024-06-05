# Flash 7.0-inch board with factory firmware

## With ESP Tool

To flash the board with ESP Tool:
```bash
esptool.py -b 460800 --before default_reset --after no_reset --chip esp32s3 \
  write_flash --flash_size detect --flash_freq 80m \
    0xe000 elecrow/boot_app0.bin \
    0x0 elecrow/LvglWidgets-LVGL-7.0.ino.bootloader.bin \
    0x8000 elecrow/LvglWidgets-LVGL-7.0.ino.partitions.bin \
    0x10000 elecrow/LvglWidgets-LVGL-7.0.ino.bin
```
