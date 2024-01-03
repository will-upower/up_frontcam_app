sudo rmmod cmemdrv
sudo insmod /usr/lib/modules/5.10.147-yocto-standard/extra/cmemdrv.ko bsize=0x20000000
sudo rmmod uio_pdrv_genirq
sudo insmod /usr/lib/modules/5.10.147-yocto-standard/kernel/drivers/uio/uio_pdrv_genirq.ko of_id="generic-uio"
