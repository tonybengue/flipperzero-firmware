#include "valuemutex.h"
#include <string.h>

bool init_mutex(ValueMutex* valuemutex, void* value, size_t size) {
    // mutex without name,
    // no attributes (unfortunatly robust mutex is not supported by FreeRTOS),
    // with dynamic memory allocation
    const osMutexAttr_t value_mutext_attr = {
        .name = NULL, .attr_bits = 0, .cb_mem = NULL, .cb_size = 0U};

    valuemutex->mutex = osMutexNew(&value_mutext_attr);
    if(valuemutex->mutex == NULL) return false;

    valuemutex->value = value;
    valuemutex->size = size;

    return true;
}

bool delete_mutex(ValueMutex* valuemutex) {
    if(osMutexAcquire(valuemutex->mutex, osWaitForever) == osOK) {
        return osMutexDelete(valuemutex->mutex) == osOK;
    } else {
        return false;
    }
}

void* acquire_mutex(ValueMutex* valuemutex, uint32_t timeout) {
    if(osMutexAcquire(valuemutex->mutex, timeout) == osOK) {
        return valuemutex->value;
    } else {
        return NULL;
    }
}

bool release_mutex(ValueMutex* valuemutex, void* value) {
    if(value != valuemutex->value) return false;

    if(osMutexRelease(valuemutex->mutex) != osOK) return false;

    return true;
}

bool read_mutex(ValueMutex* valuemutex, void* data, size_t len, uint32_t timeout) {
    void* value = acquire_mutex(valuemutex, timeout);
    if(value == NULL || len > valuemutex->size) return false;
    memcpy(data, value, len > 0 ? len : valuemutex->size);
    if(!release_mutex(valuemutex, value)) return false;

    return true;
}

bool write_mutex(ValueMutex* valuemutex, void* data, size_t len, uint32_t timeout) {
    void* value = acquire_mutex(valuemutex, timeout);
    if(value == NULL || len > valuemutex->size) return false;
    memcpy(value, data, len > 0 ? len : valuemutex->size);
    if(!release_mutex(valuemutex, value)) return false;

    return true;
}