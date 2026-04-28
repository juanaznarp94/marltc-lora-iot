```bash
ssh-keygen -t ed25519 -C "your_email@example.com"
cat ~/.ssh/id_ed25519.pub
```


```bash
sudo apt update
sudo apt install -y git build-essential
```

The Raspberry Pi 5 uses the RP1 chip for I/O, which requires specific entries in the firmware configuration to release the UART from the Bluetooth controller.
```bash
sudo nano /boot/firmware/config.txt
```

```bash
    enable_uart=1
    dtparam=uart0=on
    dtoverlay=disable-bt
    dtparam=spi=on
    dtoverlay=spi0-0cs
```

Enable Serial via raspi-config

Run the configuration tool:
```Bash
    sudo raspi-config
```
Navigate to Interface Options > Serial Port.
Login Shell? Select No.
- Hardware Port Enabled? Select Yes.
- Reboot the Raspberry Pi.

Verification steps: 
- pinctrl get 14,15
- ls -l /dev/ttyAMA0


# Build instructions (to e updated)

```bash
    cd ~/git/marltc-lora-iot
    mkdir -p build
    cd build
    cmake ..
    make -j4
```