/*
  USB Driver for Sierra Wireless

  Copyright (C) 2006, 2007, 2008  Kevin Lloyd <klloyd@sierrawireless.com>
 
  Copyright (C) 2008, 2009 Elina Pasheva, Matthew Safar, Rory Filer
                           <linux@sierrawireless.com>

  IMPORTANT DISCLAIMER: This driver is not commercially supported by
  Sierra Wireless. Use at your own risk.

  This driver is free software; you can redistribute it and/or modify
  it under the terms of Version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  Portions based on the option driver by Matthias Urlichs <smurf@smurf.noris.de>
  Whom based his on the Keyspan driver by Hugh Blemings <hugh@blemings.org>

  Back ported to kernel 2.6.21
*/
/* Uncomment to log function calls */
/* #define DEBUG  */
#define DRIVER_VERSION "v.1.6.2"
#define DRIVER_AUTHOR "Kevin Lloyd, Elina Pasheva, Matthew Safar, Rory Filer"
#define DRIVER_DESC "USB Driver for Sierra Wireless USB modems"

#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/errno.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>

#define SWIMS_USB_REQUEST_SetPower	0x00
#define SWIMS_USB_REQUEST_SetNmea	0x07
#define SWIMS_USB_REQUEST_SetMode	0x0B
#define SWIMS_SET_MODE_Modem		0x0001

/* per port private data */
#define N_IN_URB	8
#define N_OUT_URB	64
#define IN_BUFLEN	4096

#define MAX_TRANSFER		(PAGE_SIZE - 512)
/* MAX_TRANSFER is chosen so that the VM is not stressed by
   allocations > PAGE_SIZE and the number of packets in a page
   is an integer 512 is the largest possible packet on EHCI */

static int debug;
static int nmea;
static int truinstall = 1;

enum devicetype {
	DEVICE_MODEM =		0,
	DEVICE_INSTALLER =	1,
};

/* list of interface numbers - used for constructing interface blacklists */
struct list {
	const u32 listlen; /* number of interface numbers on list */
	const u8  *list;   /* pointer to the array holding the numbers */
};

/* static device type specific data */
struct sierra_device_static_info {
	const enum devicetype	dev_type;
	const struct list	iface_blacklist;
};

static int sierra_set_power_state(struct usb_device *udev, __u16 swiState)
{
	int result;
	dev_dbg(&udev->dev, "%s\n", __func__);
	result = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			SWIMS_USB_REQUEST_SetPower,	/* __u8 request      */
			USB_TYPE_VENDOR,		/* __u8 request type */
			swiState,			/* __u16 value       */
			0,				/* __u16 index       */
			NULL,				/* void *data        */
			0,				/* __u16 size 	     */
			USB_CTRL_SET_TIMEOUT);		/* int timeout 	     */
	return result;
}

static int sierra_set_ms_mode(struct usb_device *udev, __u16 eSWocMode)
{
	int result;
	dev_dbg(&udev->dev, "%s\n", "DEVICE MODE SWITCH");
	result = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			SWIMS_USB_REQUEST_SetMode,	/* __u8 request      */
			USB_TYPE_VENDOR,		/* __u8 request type */
			eSWocMode,			/* __u16 value       */
			0x0000,				/* __u16 index       */
			NULL,				/* void *data        */
			0,				/* __u16 size 	     */
			USB_CTRL_SET_TIMEOUT);		/* int timeout       */
	return result;
}

static int sierra_vsc_set_nmea(struct usb_device *udev, __u16 enable)
{
	int result;
	dev_dbg(&udev->dev, "%s\n", __func__);
	result = usb_control_msg(udev, usb_sndctrlpipe(udev, 0),
			SWIMS_USB_REQUEST_SetNmea,	/* __u8 request      */
			USB_TYPE_VENDOR,		/* __u8 request type */
			enable,				/* __u16 value       */
			0x0000,				/* __u16 index       */
			NULL,				/* void *data        */
			0,				/* __u16 size 	     */
			USB_CTRL_SET_TIMEOUT);		/* int timeout       */
	return result;
}

static int sierra_calc_num_ports(struct usb_serial *serial)
{
	int num_ports = 0;
	u8 ifnum, numendpoints;
	
	dev_dbg(&serial->dev->dev, "%s\n", __func__);
	
	ifnum = serial->interface->cur_altsetting->desc.bInterfaceNumber;
	numendpoints = serial->interface->cur_altsetting->desc.bNumEndpoints;
	
	/* Dummy interface present on some SKUs should be ignored */
	if (ifnum == 0x99)
		num_ports = 0;
	else if (numendpoints <= 3)
		num_ports = 1;
	else
		num_ports = (numendpoints-1)/2;
	return num_ports;
}

static int is_blacklisted( const u8 ifnum, const struct list *blacklist )
{
	const u8  *list;
	int i;
	
	if (blacklist) {
		list = blacklist->list;
		
		for (i=0; i < blacklist->listlen; i++) {
			if (list[i] == ifnum)
				return 1;
		}
	}
	return 0;
}

static int sierra_calc_interface(struct usb_serial *serial)
{
	int interface;
	struct usb_interface *p_interface;
	struct usb_host_interface *p_host_interface;
	dev_dbg(&serial->dev->dev, "%s\n", __func__);

	/* Get the interface structure pointer from the serial struct */
	p_interface = serial->interface;

	/* Get a pointer to the host interface structure */
	p_host_interface = p_interface->cur_altsetting;

	/* read the interface descriptor for this active altsetting
	 * to find out the interface number we are on
	*/
	interface = p_host_interface->desc.bInterfaceNumber;

	return interface;
}

static int sierra_probe(struct usb_serial *serial,
			const struct usb_device_id *id)
{
	const struct sierra_device_static_info * info;	
	int result = 0;
	struct usb_device *udev;
	u8 ifnum, ifclass; 

	udev = serial->dev;
	dev_dbg(&udev->dev, "%s\n", __func__);

	/* Check TRU-Install first */
	info = (const struct sierra_device_static_info *)id->driver_info;
	ifclass = serial->interface->cur_altsetting->desc.bInterfaceClass;
	if (ifclass == USB_CLASS_MASS_STORAGE) {
		/* If TRU-Install support is enabled, force to modem mode */
		if (truinstall && info && info->dev_type == DEVICE_INSTALLER) {
			dev_dbg(&udev->dev, "%s\n", "FOUND TRU-INSTALL DEVICE");
			result = sierra_set_ms_mode(udev, SWIMS_SET_MODE_Modem);
		}
		return -ENODEV;
	}
	
	ifnum = sierra_calc_interface(serial);
	if (info && is_blacklisted(ifnum, &info->iface_blacklist)) {
		dev_dbg(&serial->dev->dev,
			"Ignoring blacklisted interface #%d\n", ifnum);
		return -ENODEV;
	}
	
	/*
	 * If this interface supports more than 1 alternate
	 * select the 2nd one
	 */
	if (serial->interface->num_altsetting == 2) {
		dev_dbg(&udev->dev, "Selecting alt setting for interface %d\n",
			ifnum);
		/* We know the alternate setting is 1 for the MC8785 */
		usb_set_interface(udev, ifnum, 1);
	}
	/* Be careful here, The ifnum, ifclass etc. might be incorrect, because
	 * of the usb_set_interface call. (all obtained using 
	 * serial->interface->cur_altsetting that was changed by that call)
	 */
	
	return result;
}

static const struct sierra_device_static_info tru_inst_info = {
	.dev_type = DEVICE_INSTALLER,
};

static const u8 direct_ip_non_serial_ifaces[] = { 7, 8, 9, 10, 11 };
static const struct sierra_device_static_info direct_ip_interface_blacklist = {
	.dev_type = DEVICE_MODEM,
	.iface_blacklist = {
		.listlen = ARRAY_SIZE( direct_ip_non_serial_ifaces ),
		.list = direct_ip_non_serial_ifaces,
	},
};

static struct usb_device_id id_table [] = {
	{ USB_DEVICE(0x1199, 0x0017) },	/* Sierra Wireless EM5625 */
	{ USB_DEVICE(0x1199, 0x0018) },	/* Sierra Wireless MC5720 */
	{ USB_DEVICE(0x1199, 0x0218) },	/* Sierra Wireless MC5720 */
	{ USB_DEVICE(0x03f0, 0x1b1d) }, /* HP ev2200 a.k.a MC5720 */
	{ USB_DEVICE(0x1199, 0x0020) },	/* Sierra Wireless MC5725 */
	{ USB_DEVICE(0x1199, 0x0024) },	/* Sierra Wireless MC5727 */
	{ USB_DEVICE(0x1199, 0x0220) },	/* Sierra Wireless MC5725 */
	{ USB_DEVICE(0x1199, 0x0019) },	/* Sierra Wireless AirCard 595 */
	{ USB_DEVICE(0x1199, 0x0021) },	/* Sierra Wireless AirCard 597E */
	{ USB_DEVICE(0x1199, 0x0120) },	/* Sierra Wireless USB Dongle 595U */
	 /* Sierra Wireless C597 */
	{ USB_DEVICE(0x1199, 0x0023) },
	 /* Sierra Wireless Device */
	{ USB_DEVICE(0x1199, 0x0025) },
	{ USB_DEVICE(0x1199, 0x0026) }, /* Sierra Wireless Device */
	{ USB_DEVICE(0x1199, 0x0027) }, /* Sierra Wireless Device */
	{ USB_DEVICE(0x1199, 0x0028) }, /* Sierra Wireless Device */

	{ USB_DEVICE(0x1199, 0x6802) },	/* Sierra Wireless MC8755 */
	{ USB_DEVICE(0x1199, 0x6804) },	/* Sierra Wireless MC8755 */
	{ USB_DEVICE(0x1199, 0x6803) },	/* Sierra Wireless MC8765 */
	{ USB_DEVICE(0x1199, 0x6812) },	/* Sierra Wireless MC8775 & AC 875U */
	{ USB_DEVICE(0x1199, 0x6813) },	/* Sierra Wireless MC8775 (Lenovo) */
	{ USB_DEVICE(0x1199, 0x6815) },	/* Sierra Wireless MC8775 */
	{ USB_DEVICE(0x03f0, 0x1e1d) },	/* HP hs2300 a.k.a MC8775 */
	{ USB_DEVICE(0x1199, 0x6820) },	/* Sierra Wireless AirCard 875 */
	{ USB_DEVICE(0x1199, 0x6821) },	/* Sierra Wireless AirCard 875U */
	{ USB_DEVICE(0x1199, 0x6832) },	/* Sierra Wireless MC8780 */
	{ USB_DEVICE(0x1199, 0x6833) },	/* Sierra Wireless MC8781 */
	{ USB_DEVICE(0x1199, 0x683A) },	/* Sierra Wireless MC8785 */
	{ USB_DEVICE(0x1199, 0x683B) },	/* Sierra Wireless MC8785 Composite */
	/* Sierra Wireless MC8790, MC8791, MC8792 Composite */
	{ USB_DEVICE(0x1199, 0x683C) },	
	{ USB_DEVICE(0x1199, 0x683D) },	/* Sierra Wireless MC8791 Composite */
	/* Sierra Wireless MC8790, MC8791, MC8792 */
	{ USB_DEVICE(0x1199, 0x683E) },
	{ USB_DEVICE(0x1199, 0x6850) },	/* Sierra Wireless AirCard 880 */
	{ USB_DEVICE(0x1199, 0x6851) },	/* Sierra Wireless AirCard 881 */
	{ USB_DEVICE(0x1199, 0x6852) },	/* Sierra Wireless AirCard 880 E */
	{ USB_DEVICE(0x1199, 0x6853) },	/* Sierra Wireless AirCard 881 E */
	{ USB_DEVICE(0x1199, 0x6855) },	/* Sierra Wireless AirCard 880 U */
	{ USB_DEVICE(0x1199, 0x6856) },	/* Sierra Wireless AirCard 881 U */
	{ USB_DEVICE(0x1199, 0x6859) },	/* Sierra Wireless AirCard 885 E */
	{ USB_DEVICE(0x1199, 0x685A) },	/* Sierra Wireless AirCard 885 E */
	/* Sierra Wireless C885 */
	{ USB_DEVICE(0x1199, 0x6880)},
	/* Sierra Wireless Device */
	{ USB_DEVICE(0x1199, 0x6890)},
	/* Sierra Wireless Device */
	{ USB_DEVICE(0x1199, 0x6891)},
	/* Sierra Wireless Device */
	{ USB_DEVICE(0x1199, 0x6892)},

	{ USB_DEVICE(0x1199, 0x0112) }, /* Sierra Wireless AirCard 580 */
	{ USB_DEVICE(0x0F3D, 0x0112) }, /* Airprime/Sierra PC 5220 */

	{ USB_DEVICE(0x1199, 0x0FFF),
		.driver_info = (kernel_ulong_t)&tru_inst_info
	},
	
	{ USB_DEVICE(0x1199, 0x68A3), 	/* Sierra Wireless Direct IP modems */
		.driver_info = (kernel_ulong_t)&direct_ip_interface_blacklist 
	},
	
	{ }
};
MODULE_DEVICE_TABLE(usb, id_table);

static struct usb_driver sierra_driver = {
	.name       = "sierra",
	.probe      = usb_serial_probe,
	.disconnect = usb_serial_disconnect,
	.id_table   = id_table,
};

struct sierra_port_private {
	spinlock_t lock;	/* lock the structure */
	int outstanding_urbs;	/* number of out urbs in flight */

	/* Input endpoints and buffers for this port */
	struct urb *in_urbs[N_IN_URB];

	/* Settings for the port */
	int rts_state;	/* Handshaking pins (outputs) */
	int dtr_state;
	int cts_state;	/* Handshaking pins (inputs) */
	int dsr_state;
	int dcd_state;
	int ri_state;
};

static int sierra_send_setup(struct usb_serial_port *port)
{
	struct usb_serial *serial = port->serial;
	struct sierra_port_private *portdata;
	__u16 interface = 0;

	dev_dbg(&port->dev, "%s\n", __func__);

	portdata = usb_get_serial_port_data(port);

	if (port->tty) {

		int val = 0;
		if (portdata->dtr_state)
			val |= 0x01;
		if (portdata->rts_state)
			val |= 0x02;
			
		/* If composite device then properly report interface */
		if (serial->num_ports == 1) {
			interface = sierra_calc_interface(serial);
			/* Control message is send only to interfaces with 
			 * interrupt_in endpoints
			 */
			if(port->interrupt_in_urb) {
				/* send control message */
				return usb_control_msg(serial->dev,
					usb_rcvctrlpipe(serial->dev, 0),
					0x22,0x21,val,interface,NULL,0,
					USB_CTRL_SET_TIMEOUT);
			}
		}

		/* Otherwise the need to do non-composite mapping */
		else {
			if (port->bulk_out_endpointAddress == 2)
				interface = 0;
			else if (port->bulk_out_endpointAddress == 4)
				interface = 1;
			else if (port->bulk_out_endpointAddress == 5)
				interface = 2;

			return usb_control_msg(serial->dev,
				usb_rcvctrlpipe(serial->dev, 0),
				0x22,0x21,val,interface,NULL,0,
				USB_CTRL_SET_TIMEOUT);
		}
	}

	return 0;
}

static void sierra_set_termios(struct usb_serial_port *port,
			struct ktermios *old_termios)
{
	dev_dbg(&port->dev, "%s\n", __func__);
	sierra_send_setup(port);
}

static int sierra_tiocmget(struct usb_serial_port *port, struct file *file)
{
	unsigned int value;
	struct sierra_port_private *portdata;

	portdata = usb_get_serial_port_data(port);

	value = ((portdata->rts_state) ? TIOCM_RTS : 0) |
		((portdata->dtr_state) ? TIOCM_DTR : 0) |
		((portdata->cts_state) ? TIOCM_CTS : 0) |
		((portdata->dsr_state) ? TIOCM_DSR : 0) |
		((portdata->dcd_state) ? TIOCM_CAR : 0) |
		((portdata->ri_state) ? TIOCM_RNG : 0);

	return value;
}

static int sierra_tiocmset(struct usb_serial_port *port, struct file *file,
			unsigned int set, unsigned int clear)
{
	struct sierra_port_private *portdata;

	portdata = usb_get_serial_port_data(port);

	if (set & TIOCM_RTS)
		portdata->rts_state = 1;
	if (set & TIOCM_DTR)
		portdata->dtr_state = 1;

	if (clear & TIOCM_RTS)
		portdata->rts_state = 0;
	if (clear & TIOCM_DTR)
		portdata->dtr_state = 0;
	return sierra_send_setup(port);
}
static void sierra_release_urb(struct urb *urb)
{
	struct usb_serial_port *port;
	if (urb) {
		port =  urb->context;
		dev_dbg(&port->dev, "%s: %p\n", __func__, urb); 
		if (urb->transfer_buffer)
			kfree(urb->transfer_buffer);
		usb_free_urb(urb);
	}
}

static void sierra_outdat_callback(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	struct sierra_port_private *portdata = usb_get_serial_port_data(port);
	int status = urb->status;
	unsigned long flags;

	dev_dbg(&port->dev, "%s - port %d\n", __func__, port->number);

	/* free up the transfer buffer, as usb_free_urb() does not do this */
	kfree(urb->transfer_buffer);

	if (status)
		dev_dbg(&port->dev, "%s - nonzero write bulk status "
		    "received: %d\n", __func__, status);

	spin_lock_irqsave(&portdata->lock, flags);
	--portdata->outstanding_urbs;
	spin_unlock_irqrestore(&portdata->lock, flags);

	usb_serial_port_softint(port);
}

/* Write */
static int sierra_write(struct usb_serial_port *port,
			const unsigned char *buf, int count)
{
	struct sierra_port_private *portdata = usb_get_serial_port_data(port);
	struct usb_serial *serial = port->serial;
	unsigned long flags;
	unsigned char *buffer;
	struct urb *urb;
	size_t writesize = min((size_t)count, (size_t)MAX_TRANSFER);
	int retval = 0;

	/* verify that we actually have some data to write */
	if (count == 0)
		return 0;

	portdata = usb_get_serial_port_data(port);

	dev_dbg(&port->dev, "%s: write (%d bytes)\n", __func__, writesize);

	spin_lock_irqsave(&portdata->lock, flags);
	if (portdata->outstanding_urbs > N_OUT_URB) {
		spin_unlock_irqrestore(&portdata->lock, flags);
		dev_dbg(&port->dev, "%s - write limit hit\n", __func__);
		return 0;
	}
	portdata->outstanding_urbs++;
	spin_unlock_irqrestore(&portdata->lock, flags);

	buffer = kmalloc(writesize, GFP_ATOMIC);
	if (!buffer) {
		dev_err(&port->dev, "out of memory\n");
		retval = -ENOMEM;
		goto error_no_buffer;
	}

	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb) {
		dev_err(&port->dev, "no more free urbs\n");
		retval = -ENOMEM;
		goto error_no_urb;
	}

	memcpy(buffer, buf, writesize); 

	usb_serial_debug_data(debug, &port->dev, __func__, writesize, buffer);

	usb_fill_bulk_urb(urb, serial->dev,
			  usb_sndbulkpipe(serial->dev,
					  port->bulk_out_endpointAddress),
			  buffer, writesize, sierra_outdat_callback, port);

        /* Handle the need to send a zero length packet */
        urb->transfer_flags |= URB_ZERO_PACKET;

	/* send it down the pipe */
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval) {
		dev_err(&port->dev, "%s - usb_submit_urb(write bulk) failed "
			"with status = %d\n", __func__, retval);
		goto error;
	}

	/* we are done with this urb, so let the host driver
	 * really free it when it is finished with it */
	usb_free_urb(urb);
	
	return writesize;
error:
	usb_free_urb(urb);
error_no_urb:
	kfree(buffer);
error_no_buffer:
	spin_lock_irqsave(&portdata->lock, flags);
	--portdata->outstanding_urbs;
	spin_unlock_irqrestore(&portdata->lock, flags);
	return retval;
}

static void sierra_indat_callback(struct urb *urb)
{
	int err;
	int endpoint;
	struct usb_serial_port *port;
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	int status = urb->status;

	endpoint = usb_pipeendpoint(urb->pipe);
	port =  urb->context;
	
	dev_dbg(&port->dev, "%s: %p\n", __func__, urb); 

	if (status) {
		dev_dbg(&port->dev, "%s: nonzero status: %d on"
			" endpoint %02x\n", __func__, status, endpoint);
	} else {
		tty = port->tty;
		if (urb->actual_length) {
			tty_buffer_request_room(tty, urb->actual_length);
			tty_insert_flip_string(tty, data, urb->actual_length);
			tty_flip_buffer_push(tty);
			usb_serial_debug_data(debug, &port->dev, __func__, 
				urb->actual_length, data);
		} else {
			dev_dbg(&port->dev, "%s: empty read urb"
				" received\n", __func__);
		}
	}

	/* Resubmit urb so we continue receiving */
	if (port->open_count && status != -ESHUTDOWN) {
		err = usb_submit_urb(urb, GFP_ATOMIC);
		if (err)
			dev_err(&port->dev, "resubmit read urb failed."
				"(%d)\n", err);
	}
	
	return;
}

static void sierra_instat_callback(struct urb *urb)
{
	int err;
	int status = urb->status;
	struct usb_serial_port *port =  urb->context;
	struct sierra_port_private *portdata = usb_get_serial_port_data(port);
	struct usb_serial *serial = port->serial;

	dev_dbg(&port->dev, "%s: urb %p port %p has data %p\n", __func__,
		urb, port, portdata);

	if (status == 0) {
		struct usb_ctrlrequest *req_pkt =
				(struct usb_ctrlrequest *)urb->transfer_buffer;

		if (!req_pkt) {
			dev_dbg(&port->dev, "%s: NULL req_pkt\n",
				__func__);
			return;
		}
		if ((req_pkt->bRequestType == 0xA1) &&
				(req_pkt->bRequest == 0x20)) {
			int old_dcd_state;
			unsigned char signals = *((unsigned char *)
					urb->transfer_buffer +
					sizeof(struct usb_ctrlrequest));

			dev_dbg(&port->dev, "%s: signal x%x\n", __func__,
				signals);

			old_dcd_state = portdata->dcd_state;
			portdata->cts_state = 1;
			portdata->dcd_state = ((signals & 0x01) ? 1 : 0);
			portdata->dsr_state = ((signals & 0x02) ? 1 : 0);
			portdata->ri_state = ((signals & 0x08) ? 1 : 0);

			if (port->tty && !C_CLOCAL(port->tty) &&
					old_dcd_state && !portdata->dcd_state)
				tty_hangup(port->tty);
		} else {
			dev_dbg(&port->dev, "%s: type %x req %x\n",
				__func__, req_pkt->bRequestType,
				req_pkt->bRequest);
		}
	} else
		dev_dbg(&port->dev, "%s: error %d\n", __func__, status);

	/* Resubmit urb so we continue receiving IRQ data */
	if (port->open_count && status != -ESHUTDOWN) {
		urb->dev = serial->dev;
		err = usb_submit_urb(urb, GFP_ATOMIC);
		if (err)
			dev_err(&port->dev, "%s: resubmit intr urb "
				"failed. (%d)\n", __func__, err);
	}
}

static int sierra_write_room(struct usb_serial_port *port)
{
	struct sierra_port_private *portdata = usb_get_serial_port_data(port);
	unsigned long flags;

	dev_dbg(&port->dev, "%s - port %d\n", __func__, port->number);

	/* try to give a good number back based on if we have any free urbs at
	 * this point in time */
	spin_lock_irqsave(&portdata->lock, flags);
	if (portdata->outstanding_urbs > N_OUT_URB * 2 / 3) {
		spin_unlock_irqrestore(&portdata->lock, flags);
		dev_dbg(&port->dev, "%s - write limit hit\n", __func__);
		return 0;
	}
	spin_unlock_irqrestore(&portdata->lock, flags);

	return 2048;
}


static struct urb *sierra_setup_urb(struct usb_serial *serial, int endpoint,
					int dir, void *ctx, int len,
					usb_complete_t callback)
{
	struct urb	*urb;
	u8		*buf;
	
	if (endpoint == -1)
		return NULL;

	urb = usb_alloc_urb( 0, GFP_KERNEL );
	if (urb == NULL) {
		dev_dbg(&serial->dev->dev, "%s: alloc for endpoint %d failed\n", 
			__func__, endpoint);
		return NULL;
	}
	
	buf = kmalloc(len, GFP_KERNEL);
	if (buf)
	{
		/* Fill URB using supplied data */
		usb_fill_bulk_urb(urb, serial->dev,
			usb_sndbulkpipe(serial->dev, endpoint) | dir,
			buf, len, callback, ctx);

		/* debug */
		dev_dbg(&serial->dev->dev,"%s %c u:%p d:%p\n", __func__, 
				dir == USB_DIR_IN?'i':'o', urb, buf );
	} else {
		dev_dbg(&serial->dev->dev,"%s %c u:%p d:%p\n", __func__, 
				dir == USB_DIR_IN?'i':'o', urb, buf );

		sierra_release_urb(urb);
		urb = NULL;
	}
	
	return urb;
}
static int sierra_open(struct usb_serial_port *port, struct file *filp)
{
	struct sierra_port_private *portdata;
	struct usb_serial *serial = port->serial;
	int i, err;
	struct urb *urb;

	portdata = usb_get_serial_port_data(port);

	dev_dbg(&port->dev, "%s\n", __func__);

	/* Set some sane defaults */
	portdata->rts_state = 1;
	portdata->dtr_state = 1;
	
	if (port->tty) {
		port->tty->low_latency = 1;
	}

	spin_lock_init(&portdata->lock);
	
	/* Reset low level data toggle and start reading from endpoints */
	for (i = 0; i < N_IN_URB; i++) {
		urb = sierra_setup_urb(serial,
					port->bulk_in_endpointAddress,
					USB_DIR_IN,
					port,
					IN_BUFLEN,
					sierra_indat_callback);
		portdata->in_urbs[i] = urb;
		if (!urb)
			continue;
		
		/*
		 * make sure endpoint data toggle is synchronized with the
		 * device
		 */
		usb_clear_halt(urb->dev, urb->pipe);
		
		err = usb_submit_urb(urb, GFP_ATOMIC);
		if (err) {
			dev_err(&port->dev, "%s: submit urb %d failed (%d) %d\n",
				__func__, i, err,
				urb->transfer_buffer_length);
			portdata->in_urbs[i] = NULL;
			sierra_release_urb(urb);
		}
	}

	/* Start up the interrupt endpoint if we have one */
	if(port->interrupt_in_urb) {
		port->interrupt_in_urb->dev = port->serial->dev;
		err = usb_submit_urb(port->interrupt_in_urb, GFP_ATOMIC);
		if (err)
			dev_err(&port->dev, "%s: submit irq_in urb failed %d\n",
				__func__, err);
	}
	
	sierra_send_setup(port);

	return 0;
}

static void sierra_close(struct usb_serial_port *port, struct file *filp)
{
	int i;
	struct usb_serial *serial = port->serial;
	struct sierra_port_private *portdata;

	dev_dbg(&port->dev, "%s\n", __func__);
	portdata = usb_get_serial_port_data(port);

	portdata->rts_state = 0;
	portdata->dtr_state = 0;

	if (serial->dev) {
		sierra_send_setup(port);

		/* Stop reading/writing urbs */
		for (i = 0; i < N_IN_URB; i++) {
			usb_kill_urb(portdata->in_urbs[i]);
			sierra_release_urb(portdata->in_urbs[i]);
			portdata->in_urbs[i] = NULL;
		}
	}
	usb_kill_urb(port->interrupt_in_urb);
	port->tty = NULL;
}

static int sierra_startup(struct usb_serial *serial)
{
	struct usb_serial_port *port;
	struct sierra_port_private *portdata;
	int i;

	dev_dbg(&serial->dev->dev, "%s\n", __func__);

	/* Set Device mode to D0 */
	sierra_set_power_state(serial->dev, 0x0000);

	/* Check NMEA and set */
	if (nmea)
		sierra_vsc_set_nmea(serial->dev, 1);

	/* Now setup per port private data */
	for (i = 0; i < serial->num_ports; i++) {
		port = serial->port[i];
		portdata = kzalloc(sizeof(*portdata), GFP_KERNEL);
		if (!portdata) {
			dev_dbg(&port->dev, "%s: kmalloc for "
				"sierra_port_private (%d) failed!\n",
				__func__, i);
			return -ENOMEM;
		}
		/* Set the port private data pointer */
		usb_set_serial_port_data(port, portdata);
	}

	return 0;
}

static void sierra_shutdown(struct usb_serial *serial)
{
	int i;
	struct usb_serial_port *port;
	struct sierra_port_private *portdata;

	dev_dbg(&serial->dev->dev, "%s\n", __func__);

	for (i = 0; i < serial->num_ports; ++i) {
		port = serial->port[i];
		if (!port)
			continue;
		portdata = usb_get_serial_port_data(port);
		if (!portdata)
			continue;
		kfree(portdata);
		usb_set_serial_port_data(port, NULL);
	}
}

static struct usb_serial_driver sierra_device = {
	.driver = {
		.owner =	THIS_MODULE,
		.name =		"sierra",
	},
	.description       = "Sierra USB modem",
	.id_table          = id_table,
	.usb_driver        = &sierra_driver,
	.num_interrupt_in  = NUM_DONT_CARE,
	.num_bulk_in       = NUM_DONT_CARE,
	.num_bulk_out      = NUM_DONT_CARE,
	.calc_num_ports	   = sierra_calc_num_ports,
	.probe		   = sierra_probe,
	.open              = sierra_open,
	.close             = sierra_close,
	.write             = sierra_write,
	.write_room        = sierra_write_room,
	.set_termios       = sierra_set_termios,
	.tiocmget          = sierra_tiocmget,
	.tiocmset          = sierra_tiocmset,
	.attach            = sierra_startup,
	.shutdown          = sierra_shutdown,
	.read_int_callback = sierra_instat_callback,
};

/* Functions used by new usb-serial code. */
static int __init sierra_init(void)
{
	int retval;
	retval = usb_serial_register(&sierra_device);
	if (retval)
		goto failed_device_register;


	retval = usb_register(&sierra_driver);
	if (retval)
		goto failed_driver_register;

	info(DRIVER_DESC ": " DRIVER_VERSION);

	return 0;

failed_driver_register:
	usb_serial_deregister(&sierra_device);
failed_device_register:
	return retval;
}

static void __exit sierra_exit(void)
{
	usb_deregister(&sierra_driver);
	usb_serial_deregister(&sierra_device);
}

module_init(sierra_init);
module_exit(sierra_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL");

module_param(truinstall, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(truinstall, "TRU-Install support");

module_param(nmea, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(nmea, "NMEA streaming");

module_param(debug, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Debug messages");
