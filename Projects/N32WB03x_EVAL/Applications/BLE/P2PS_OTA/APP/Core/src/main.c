/**
 * @file main.c
 * @brief Main function for the BLE Beacon OTA application
 * /
 * This file is part of the N32WB03x_EVAL project and implements the main function for the BLE Beacon OTA application. It initializes the necessary peripherals, sets up the BLE stack, and starts the main application loop. */

#include "main.h"
#include "n32wb03x.h"
#include "app_log.h"

 int main(void)
 {
     APP_Init();
     while (1)
     {
        APP_Process();
     }
 }