#include "sdk/console.h"

#include "DummyDev.h"

int Builtin_ns::DummyDev::load(Interface_ns::addr_t begin_addr, uint64_t len, uint8_t *buffer)  {
    LOG_INFO("Load  DummyDev %s at dev addr %016lx len %016lx", devDesc, begin_addr, len);
    // --- Dump bytes to load
    STDOUT_ACQUIRE_LOCK;
    for(size_t i = 0; i < len; i++) {
        if(i % 8 == 0) printf("\n%s[INFO ]", STYLE_TEXT_BLUE);
        printf("%02x ", buffer[i]);
        printf("%s", STYLE_RST);
    }
    printf("\n");
    STDOUT_RELEASE_LOCK;
    return Interface_ns::FB_SUCCESS;
}

int Builtin_ns::DummyDev::store(Interface_ns::addr_t begin_addr, uint64_t len, const uint8_t *buffer)  {
    LOG_INFO("Store DummyDev %s at dev addr %016lx len %016lx", devDesc, begin_addr, len);
    // --- Dump bytes to store
    STDOUT_ACQUIRE_LOCK;
    for(size_t i = 0; i < len; i++) {
        if(i % 8 == 0) printf("\n%s[INFO ]", STYLE_TEXT_BLUE);
        printf("%02x ", buffer[i]);
        printf("%s", STYLE_RST);
    }
    printf("\n");
    STDOUT_RELEASE_LOCK;
    return Interface_ns::FB_SUCCESS;
}

Builtin_ns::DummyDev::DummyDev(const char *desc) : devDesc(desc ? desc : "default") {}

