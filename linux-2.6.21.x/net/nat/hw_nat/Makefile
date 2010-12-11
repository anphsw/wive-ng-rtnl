obj-$(CONFIG_RA_HW_NAT) += hw_nat.o
hw_nat-objs := ra_nat.o foe_fdb.o util.o
hw_nat-objs += hwnat_ioctl.o
ifeq ($(CONFIG_RA_HW_NAT_FULL_FEATURES),y)
hw_nat-objs += acl_policy.o ac_policy.o mtr_policy.o ac_ioctl.o acl_ioctl.o mtr_ioctl.o
endif
