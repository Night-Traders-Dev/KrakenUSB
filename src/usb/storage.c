#include "storage.h"
#include "bsp/board_api.h"
#include "tusb.h"

// Static variables
static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

// Initialize storage functionality
void storage_init(void) {
    board_init();
    tud_init(BOARD_TUD_RHPORT);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }
}

// Run storage tasks (e.g., USB and LED blinking)
void storage_task(void) {
    tud_task();          // TinyUSB device task
    led_blinking_task(); // LED blinking task
    cdc_task();          // USB CDC task
}

// USB CDC callback implementations and other helpers
void tud_mount_cb(void) {
    blink_interval_ms = BLINK_MOUNTED;
}

void tud_umount_cb(void) {
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

void tud_suspend_cb(bool remote_wakeup_en) {
    (void)remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

void tud_resume_cb(void) {
    blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

// Internal functions (e.g., blinking task and CDC task)
static void led_blinking_task(void) {
    static uint32_t start_ms = 0;
    static bool led_state = false;

    if (board_millis() - start_ms < blink_interval_ms) return;
    start_ms += blink_interval_ms;

    board_led_write(led_state);
    led_state = !led_state; // Toggle LED state
}

static void cdc_task(void) {
    if (tud_cdc_available()) {
        char buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));

        tud_cdc_write(buf, count);
        tud_cdc_write_flush();
    }
}
