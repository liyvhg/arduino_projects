#ifndef  _BLEND_H
#define _BLEND_H

#include <boards.h>
#include <lib_aci.h>
#include <aci_setup.h>
#include <SPI.h>

#include <avr/sleep.h>
#include <avr/interrupt.h>

/* Put the nRF8001 setup in the RAM of the nRF8001.*/
#include "vportal_services.h"
/* Include the services_lock.h to put the setup in the OTP memory of the nRF8001.
This would mean that the setup cannot be changed once put in.
However this removes the need to do the setup of the nRF8001 on every reset.*/

#if defined(BLEND_MICRO)
#define DEFAULT_REQN    6
#define DEFAULT_RDYN    7
#endif

void ble_begin();
void ble_write(unsigned char data);
void ble_write_bytes(unsigned char *data, unsigned char len);
void ble_do_events();
int ble_read();
unsigned char ble_available();
unsigned char ble_connected(void);
void ble_set_pins(uint8_t reqn, uint8_t rdyn);
unsigned char ble_busy();

#endif

