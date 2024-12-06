#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h> // For uint32_t
#include <stdbool.h> // For bool

// Blink pattern durations
enum {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

// Function declarations
void storage_init(void);
void storage_task(void);
void tud_mount_cb(void);
void tud_umount_cb(void);
void tud_suspend_cb(bool remote_wakeup_en);
void tud_resume_cb(void);

#endif // STORAGE_H
