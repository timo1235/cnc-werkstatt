:Start
avrdude -F -V -q -pavr128db64 -catmelice_updi -e -Uflash:w:Bootloader_Terminal.hex:i -Ueeprom:w:CM_Estlcam_KlemmenadapterXL.eep:i -Ubootsize:w:0x02:m  -Uwdtcfg:w:0x09:m
pause
goto Start
