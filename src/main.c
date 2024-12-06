#include "kraken_scheduler.h"
#include "storage.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>

#define PICO_DEFAULT_LED_PIN 25
#define EVENT_LED_ON_BIT (1 << 0)
#define EVENT_LED_OFF_BIT (1 << 1)

/* Global Event Group */
static EventGroupHandle_t led_event_group;

/* Runtime Stats Task */
void runtime_stats_task(void *parameters) {
    char stats_buffer[512];

    while (1) {
        /* Fetch and print task runtime stats */
        kraken_print_task_runtime_stats(stats_buffer, sizeof(stats_buffer));
        printf("Runtime Stats:\n%s\n", stats_buffer);

        vTaskDelay(pdMS_TO_TICKS(5000)); // Print every 5 seconds
    }
}

/* Storage Task */
void storage_task(void *parameters) {
    // Run the storage functionality in a loop
    while (1) {
        storage_task(); // Call the storage task from storage.c
        vTaskDelay(pdMS_TO_TICKS(10)); // Allow other tasks to run
    }
}

int main(void) {
    /* Initialize the Kraken log system */
    kraken_log("Initializing Kraken Machine Firmware...");

    /* Initialize storage functionality */
    storage_init();

    /* Create the Event Group */
    led_event_group = kraken_create_event_group();
    if (led_event_group == NULL) {
        kraken_assert(0, "Failed to create event group");
    }

    /* Create Tasks */
    kraken_create_task(runtime_stats_task, "Runtime Stats", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
    kraken_create_task(storage_task, "Storage Task", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);

    /* Start the Scheduler */
    kraken_start_scheduler();

    /* Should never reach here */
    kraken_assert(0, "Scheduler stopped unexpectedly");
    while (1);
}
