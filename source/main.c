/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"


#include "usb_device_class.h"
#include "usb_device_msc.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"

#include "main.h"

#include "fsl_device_registers.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "MKL26Z4.h"

#include "fsl_clock.h"

void BOARD_InitHardware(void);
usb_msc_struct_t USB_DeviceStruct;

usb_status_t USB_DeviceMscCallback(class_handle_t handle, uint32_t event, void *param){
	usb_status_t error = kStatus_USB_Success;
	usb_device_lba_information_struct_t *lbaInformationStructure;
	usb_device_lba_app_struct_t *lbaData;

	switch(event){
		case kUSB_DeviceMscEventReadResponse:
		case kUSB_DeviceMscEventWriteResponse:
			lbaData = (usb_device_lba_app_struct_t*)param;
			break;
		case kUSB_DeviceMscEventReadRequest:
			lbaData = (usb_device_lba_app_struct_t*)param;
			lbaData->buffer = USB_DeviceStruct.storageDisk;
			break;
		case kUSB_DeviceMscEventWriteRequest:
			lbaData = (usb_device_lba_app_struct_t*)param;
			lbaData->buffer = USB_DeviceStruct.storageDisk;
			break;
		case kUSB_DeviceMscEventGetLbaInformation:
			lbaInformationStructure = (usb_device_lba_information_struct_t *)param;
			lbaInformationStructure->lengthOfEachLba = LENGTH_OF_EACH_LBA;
			lbaInformationStructure->totalLbaNumberSupports = TOTAL_LOGICAL_ADDRESS_BLOCKS_NORMAL;
			lbaInformationStructure->logicalUnitNumberSupported = LOGICAL_UNIT_SUPPORTED;
			lbaInformationStructure->bulkInBufferSize = DISK_SIZE_NORMAL;
			lbaInformationStructure->bulkOutBufferSize = DISK_SIZE_NORMAL;
			break;
		case kUSB_DeviceMscEventFormatComplete:
		case kUSB_DeviceMscEventRemovalRequest:
		default:
			kStatus_USB_NotSupported;
			break;
	}

	return error;
}

usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param){
	usb_status_t error = kStatus_USB_Error;
	uint16_t *temp16 = (uint16_t *)param;
	uint8_t *temp8 = (uint8_t *)param;
	switch (event){
		case kUSB_DeviceEventBusReset:
			USB_DeviceStruct.attach = 0;
			error = kStatus_USB_Success;
			break;
		case kUSB_DeviceEventSetConfiguration:
			if (param)
			{
				USB_DeviceStruct.attach = 1;
				USB_DeviceStruct.currentConfiguration = *temp8;
			}
			break;
		case kUSB_DeviceEventSetInterface:
			break;
		case kUSB_DeviceEventGetConfiguration:
			if (param)
			{
				*temp8 = USB_DeviceStruct.currentConfiguration;
				error = kStatus_USB_Success;
			}
			break;
		case kUSB_DeviceEventGetInterface:
			if (param)
			{
				uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
				if (interface < 1u)
				{
					*temp16 = (*temp16 & 0xFF00U);
					error = kStatus_USB_Success;
				}
				else
				{
					error = kStatus_USB_InvalidRequest;
				}
			}
			break;
		case kUSB_DeviceEventGetDeviceDescriptor:
			if (param)
			{
				error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
			}
			break;
		case kUSB_DeviceEventGetConfigurationDescriptor:
			if (param)
			{
				error = USB_DeviceGetConfigurationDescriptor(handle,
															 (usb_device_get_configuration_descriptor_struct_t *)param);
			}
			break;
		case kUSB_DeviceEventGetStringDescriptor:
			if (param)
			{
				error = USB_DeviceGetStringDescriptor(handle, (usb_device_get_string_descriptor_struct_t *)param);
			}
			break;
		default:
			break;
	}
	return error;
}

usb_device_class_config_struct_t USB_ClassConfig[] = {{
		USB_DeviceMscCallback, 0, &g_UsbDeviceMscConfig
}};

usb_device_class_config_list_struct_t USB_ClassList = {
		USB_ClassConfig, USB_DeviceCallback, 1u
};

void USB0_IRQHandler(void){
	USB_DeviceKhciIsrFunction(USB_DeviceStruct.deviceHandle);
}

void USB_DeviceApplicationInit(void){
	SystemCoreClockUpdate();
	CLOCK_EnableUsbfs0Clock(kCLOCK_UsbSrcPll0, CLOCK_GetFreq(kCLOCK_PllFllSelClk));

	USB_DeviceStruct.speed = USB_SPEED_FULL;
	USB_DeviceStruct.attach = 0;
	USB_DeviceStruct.mscHandle = (class_handle_t)NULL;
	USB_DeviceStruct.deviceHandle = NULL;

	if(USB_DeviceClassInit(kUSB_ControllerKhci0,&USB_ClassList, &USB_DeviceStruct.deviceHandle) != kStatus_USB_Success){
		// reset
	}

	NVIC_SetPriority(USB0_IRQn,USB_DEVICE_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ(USB0_IRQn);

	USB_DeviceRun(USB_DeviceStruct.deviceHandle);
}

int main(void){
	BOARD_InitPins();
	BOARD_BootClockRUN();

	USB_DeviceApplicationInit();

	while(1){
		USB_DeviceKhciTaskFunction(USB_DeviceStruct.deviceHandle);
	}

	return 0;
}
