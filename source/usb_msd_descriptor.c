/*
 * usb_msd_descriptor.c
 *
 *  Created on: 17/04/2017
 *      Author: Otavio
 */

#include "usb.h"
#include "usb_msd_descriptor.h"
#include "main.h"

#define DEVICE_VERSION_MAJOR		0x01u
#define DEVICE_VERSION_MINOR		0x00u

#define STRING_SERIAL_NUMBER		0x00u
#define	STRING_MANUFACTURER_NUMBER	0x01u
#define STRING_PRODUCT_NUMBER		0x02u

#define DESCRIPTOR_SIZE_DEVICE		0x12u
#define DESCRIPTOR_SIZE_CONF		0x0020u

extern usb_status_t USB_DeviceCallback(usb_device_handle deviceHandle, uint32_t event, void *param);

uint8_t MSD_CurrentConfigure = 0;
uint8_t MSD_Interface;

usb_device_endpoint_struct_t g_UsbDeviceMscEndpoints[USB_MSC_ENDPOINT_COUNT] = {
    /* msc bulk in endpoint */
    {
        1u | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT), USB_ENDPOINT_BULK,
        512u,
    },
    /* msc bulk out endpoint */
    {
        2u | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT), USB_ENDPOINT_BULK,
        512u,
    }};
/* msc interface information */
usb_device_interface_struct_t g_UsbDeviceMscInterface[] = {{
    /* The alternate setting of the interface */
    0U,
    {
        2u,  /* Endpoint count */
        g_UsbDeviceMscEndpoints, /* Endpoints handle */
    },
    NULL
}};

usb_device_interfaces_struct_t g_UsbDeviceMscInterfaces[USB_MSC_INTERFACE_COUNT] = {
    {
    		0x08,	// MSD class
			0x06,	// subclass
			0x50,	// protocol
			0,		/* The interface number of the msc */
        g_UsbDeviceMscInterface, /* Interfaces handle */
        sizeof(g_UsbDeviceMscInterface) / sizeof(usb_device_interfaces_struct_t),
    },
};

usb_device_interface_list_t g_UsbDeviceMscInterfaceList[USB_CONFIGURE_COUNT] = {
    {
        1u,						  /* The interface count of the msc */
        g_UsbDeviceMscInterfaces, /* The interfaces handle */
    },
};

usb_device_class_struct_t g_UsbDeviceMscConfig = {
    g_UsbDeviceMscInterfaceList, /* The interface list of the msc */
    kUSB_DeviceClassTypeMsc,     /* The msc class type */
    1u,					         /* The configuration count */
};

uint8_t MSD_DeviceDescriptor[] = {
		0x12,		// descriptor length
		0x01,
		0x00,
		0x02,

		0x00,
		0x00,
		0x00,
		0x40,

		0xC9, 0x1F, // Vendor ID
		0x90, 0x00, // Product ID
		DEVICE_VERSION_MINOR, DEVICE_VERSION_MAJOR,
		0x01,		// manufacturer string
		0x02,		// product string
		0x00,		// serial number string

		0x01		// 1 configuration for USB device
};

uint8_t MSD_ConfigrationDescriptor[] = {
		0x09,	// size
		0x02,	// configuration descriptor
		0x20,	// full size low
		0x00,	// full size high
		0x01,	// 1 interface for bootloader
		0x01,	// value to select this config

		0x00,	// configuration string index
		0b11000000,
		/* Configuration characteristics
			 D7: Reserved (set to one)
			 D6: Self-powered
			 D5: Remote Wakeup
			 D4...0: Reserved (reset to zero)
		*/
		0x32,	// max power to 50u = 100mA
		// interface descriptor
		0x09,
		0x04,
		0x00,	// number of this interface setting
		0x00,	// value for alternate setting
		0x02,	// 2 endpoints

		0x08,	// MSD class
		0x06,	// subclass
		0x50,	// protocol
		0x00,	// interface string index

		// IN endpoint
		0x07,	// endpoint descriptor size
		0x05,	// endpoint descriptor
		0b1000001,
		0x03,	// interrupt mode
		0x00,	// endpoint buffer low
		0x04,	// endpoint buffer high
		0x01,	// polling interval

		// OUT endpoint
		0x07,	// endpoint descriptor size
		0x05,	// endpoint descriptor
		0b0000010,
		0x03,	// interrupt mode
		0x00,	// endpoint buffer low
		0x04,	// endpoint buffer high
		0x01,	// polling interval
};

uint8_t MSD_Strings[0][] = {
		// Serial Number String
		0x04,	// string size
		0x03,	// string descriptor
		0x09,	// SN low
		0x04	// SN high
};
uint8_t MSD_Strings[1][] = {
		// Manufacturer string
		0x12,	// string size
		0x03,	// string descriptor
		'A',
		0x00,
		'R',
		0x00,
		'M',
		0x00,
		'd',
		0x00,
		'u',
		0x00,
		'i',
		0x00,
		'n',
		0x00,
		'o',
		0x00
};
uint8_t MSD_Strings[2][] = {
		// Product string
		0x16,	// string size
		0x03,	// string descriptor
		'B',
		0x00,
		'o',
		0x00,
		'o',
		0x00,
		't',
		0x00,
		'l',
		0x00,
		'o',
		0x00,
		'a',
		0x00,
		'd',
		0x00,
		'e',
		0x00,
		'r',
		0x00
};

uint8_t MSD_StringsLength[] = {
		sizeof(MSD_Strings[0]),sizeof(MSD_Strings[1]),sizeof(MSD_Strings[2])
};

usb_language_t deviceLanguage[] = {{
		MSD_Strings, MSD_StringsLength, (uint16_t)0x0409U	// english
}};

usb_language_list_t deviceLanguageList = {	// only english support
		MSD_Strings[0], sizeof(MSD_Strings[0]), deviceLanguage, 1u
};

usb_status_t USB_DeviceGetDescriptor(usb_device_handle handle, usb_setup_struct_t *setup, uint32_t *length, uint8_t **buffer){
	usb_status_t error = kStatus_USB_Success;
	uint8_t descriptorType = (uint8_t)((setup->wValue & 0xFF00) >> 8u);
	uint8_t descriptorIndex = (uint8_t)((setup->wValue & 0x00FF));

	if(setup->bRequest != USB_REQUEST_STANDARD_GET_DESCRIPTOR)
		return kStatus_USB_InvalidRequest;

	switch(descriptorType){
		case USB_DESCRIPTOR_TYPE_STRING:
			if(descriptorIndex == 0){
				*buffer = (uint8_t*)deviceLanguageList.languageString;
				*length = deviceLanguageList.stringLength;
			}else{
				// only one language, if out of bound return error
				if(setup->wIndex != deviceLanguageList.languageList[0].languageId){
					return kStatus_USB_InvalidRequest;
				}else{
					*buffer = (uint8_t*)deviceLanguageList.languageList[0].string[0];
					*length = deviceLanguageList.languageList[0].length[0];
				}
			}
			break;
		case USB_DESCRIPTOR_TYPE_DEVICE:
			*buffer = MSD_DeviceDescriptor;
			*length = DESCRIPTOR_SIZE_DEVICE;
			break;
		case USB_DESCRIPTOR_TYPE_CONFIGURE:
			*buffer = MSD_ConfigrationDescriptor;
			*length = DESCRIPTOR_SIZE_CONF;
			break;
		default:
			error = kStatus_USB_InvalidRequest;
			break;
	}

	return error;
}

usb_status_t USB_DeviceSetConfigure(usb_device_handle handle, uint8_t configure){
	if(!configure)
		return kStatus_USB_Error;

	MSD_CurrentConfigure = configure;
	return USB_DeviceCallback(handle, kUSB_DeviceEventSetConfiguration, &configure);
}

usb_status_t USB_DeviceGetConfigure(usb_device_handle handle, uint8_t *configure)
{
    *configure = MSD_CurrentConfigure;
    return kStatus_USB_Success;
}

usb_status_t USB_DeviceSetInterface(usb_device_handle handle, uint8_t interface, uint8_t alternateSetting)
{
	if(interface != 0)
		return kStatus_USB_InvalidRequest;

    MSD_Interface = alternateSetting;
    return USB_DeviceCallback(handle, kUSB_DeviceEventSetInterface, &interface);
}

usb_status_t USB_DeviceGetInterface(usb_device_handle handle, uint8_t interface, uint8_t *alternateSetting)
{
    *alternateSetting = MSD_Interface;
    return kStatus_USB_Success;
}
