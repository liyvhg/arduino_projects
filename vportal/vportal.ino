
#include <SPI.h>
#include <boards.h>
#include "blend.h"
#include "Boards.h"

// pin modes
#define ANALOG                  0x02 // analog pin in analogInput mode
#define PWM                     0x03 // digital pin in PWM output mode

byte pin_mode[TOTAL_PINS];
byte pin_state[TOTAL_PINS];
byte pin_pwm[TOTAL_PINS];

void setup() {
    Serial.begin(57600);
    Serial.println("Setup");

    /* Default all to digital input */
    for (int pin = 0; pin < TOTAL_PINS; pin++) {
        // Set pin to input with internal pull up
        pinMode(pin, INPUT);
        digitalWrite(pin, HIGH);

        // Save pin mode and state
        pin_mode[pin] = INPUT;
        pin_state[pin] = LOW;
    }

    // Init. and start BLE library.
    ble_begin();
}

static byte buf_len = 0;

void ble_write_string(byte *bytes, uint8_t len)
{
    if (buf_len + len > 20) {
        for (int j = 0; j < 15000; j++)
            ble_do_events();

        buf_len = 0;
    }

    for (int j = 0; j < len; j++) {
        ble_write(bytes[j]);
        buf_len++;
    }

    if (buf_len == 20) {
        for (int j = 0; j < 15000; j++)
            ble_do_events();

        buf_len = 0;
    }
}

byte reportDigitalInput()
{
    if (!ble_connected())
        return 0;

    static byte pin = 0;
    byte report = 0;

    if (!IS_PIN_DIGITAL(pin)) {
        pin++;
        if (pin >= TOTAL_PINS)
            pin = 0;
        return 0;
    }

    if (pin_mode[pin] == INPUT) {
        byte current_state = digitalRead(pin);

        if (pin_state[pin] != current_state) {
            pin_state[pin] = current_state;
            byte buf[] = {'G', pin, INPUT, current_state};
            ble_write_string(buf, 4);

            report = 1;
        }
    }

    pin++;
    if (pin >= TOTAL_PINS)
        pin = 0;

    return report;
}

byte queryDone = false;

void loop()
{
    while(ble_available()) {
        byte cmd;
        cmd = ble_read();
        Serial.write(cmd);

        // send out any outstanding data
        ble_do_events();
        buf_len = 0;

        return; // only do this task in this loop
    }

    // No input data, no commands, process analog data
    if (!ble_connected()) {
        queryDone = false; // reset query state
    }

    if (queryDone) { // only report data after the query state
        byte input_data_pending = reportDigitalInput();
        if (input_data_pending) {
            ble_do_events();
            buf_len = 0;

            return; // only do this task in this loop
        }

        ble_do_events();
        buf_len = 0;

        return;
    }

    ble_do_events();
    buf_len = 0;
}

