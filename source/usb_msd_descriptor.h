/*
 * usb_msd_descriptor.h
 *
 *  Created on: 17/04/2017
 *      Author: Otavio
 */

#ifndef SOURCE_USB_MSD_DESCRIPTOR_H_
#define SOURCE_USB_MSD_DESCRIPTOR_H_

extern usb_device_class_struct_t g_UsbDeviceMscConfig;

usb_status_t USB_DeviceGetDescriptor(usb_device_handle handle, usb_setup_struct_t *setup, uint32_t *length, uint8_t **buffer);
usb_status_t USB_DeviceSetConfigure(usb_device_handle handle, uint8_t configure);
usb_status_t USB_DeviceGetConfigure(usb_device_handle handle, uint8_t *configure);
usb_status_t USB_DeviceSetInterface(usb_device_handle handle, uint8_t interface, uint8_t alternateSetting);
usb_status_t USB_DeviceGetInterface(usb_device_handle handle, uint8_t interface, uint8_t *alternateSetting);

#endif /* SOURCE_USB_MSD_DESCRIPTOR_H_ */
