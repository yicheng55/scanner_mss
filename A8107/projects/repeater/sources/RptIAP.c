#include "main.h"
#include "flash.h"
#include "RptIAP.h"

pFunction Jump_To_Application;
pFunction Jump_To_Bootloader;
uint32_t JumpAddress;

void HardFault_ReStartApp(void)
{
		if (((*(__IO uint32_t*)RestartAppAddress) & 0x2FFE0000 ) == 0x20000000) {
				printf("Execute user Application Program\r\n\n");

				// Jump to App code
				JumpAddress = *(__IO uint32_t*) (RestartAppAddress + 4);
				Jump_To_Application = (pFunction) JumpAddress;

				// Initialize the App program stack pointer
				__set_MSP(*(__IO uint32_t*) RestartAppAddress);
				Jump_To_Application();
		} else {
				printf("no user Application Program\r\n\n");
		}
}

void IAP_JumpTo_APP(void)
{
		if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000) {
				printf("Execute user Application Program\r\n\n");

				// Jump to App code
				JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
				Jump_To_Application = (pFunction) JumpAddress;

				// Initialize the App program stack pointer
				__set_MSP(*(__IO uint32_t*) ApplicationAddress);
				Jump_To_Application();
		} else {
				printf("no user Application Program\r\n\n");
		}
}

void APP_JumpTo_IAP(void)
{
		printf("APP_JumpTo_IAP\r\n");
		if (((*(__IO uint32_t*)BootloaderAddress) & 0x2FFE0000 ) == 0x20000000) {
				printf("Execute user Bootloader Program\r\n\n");

				// Jump to App code
				JumpAddress = *(__IO uint32_t*) (BootloaderAddress + 4);
				Jump_To_Bootloader = (pFunction) JumpAddress;

				// Initialize the App program stack pointer
				__set_MSP(*(__IO uint32_t*) BootloaderAddress);
				Jump_To_Bootloader();
		} else {
				printf("no user Bootloader Program\r\n\n");
		}
}
