#include <stdio.h>
#include <esp_api.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "quickjs.h"
#include "quickjs-libc.h"
#include "quickjspp.hpp"

const uint32_t qjsc_test_size = 129;

const uint8_t qjsc_test[129] = {
        0x01, 0x06, 0x0e, 0x63, 0x6f, 0x6e, 0x73, 0x6f,
        0x6c, 0x65, 0x06, 0x6c, 0x6f, 0x67, 0x34, 0x47,
        0x65, 0x74, 0x20, 0x66, 0x72, 0x65, 0x65, 0x20,
        0x68, 0x65, 0x61, 0x70, 0x20, 0x66, 0x72, 0x6f,
        0x6d, 0x20, 0x45, 0x53, 0x50, 0x33, 0x32, 0x3a,
        0x20, 0x06, 0x45, 0x53, 0x50, 0x16, 0x67, 0x65,
        0x74, 0x46, 0x72, 0x65, 0x65, 0x48, 0x65, 0x61,
        0x70, 0x0e, 0x74, 0x65, 0x73, 0x74, 0x2e, 0x6a,
        0x73, 0x0e, 0x00, 0x06, 0x00, 0xa0, 0x01, 0x00,
        0x01, 0x00, 0x06, 0x00, 0x00, 0x29, 0x01, 0xa2,
        0x01, 0x00, 0x00, 0x00, 0x38, 0xd1, 0x00, 0x00,
        0x00, 0x42, 0xd2, 0x00, 0x00, 0x00, 0x04, 0xd3,
        0x00, 0x00, 0x00, 0x42, 0x5b, 0x00, 0x00, 0x00,
        0x38, 0xd4, 0x00, 0x00, 0x00, 0x42, 0xd5, 0x00,
        0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x01, 0x00,
        0x24, 0x01, 0x00, 0xcc, 0x28, 0xac, 0x03, 0x01,
        0x00,
};


extern "C" void app_main(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());


    JSRuntime *rt;
    JSContext *ctx;
    rt = JS_NewRuntime();
    js_std_init_handlers(rt);
    ctx = JS_NewContextRaw(rt);
    JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
    JS_AddIntrinsicBaseObjects(ctx);
    JS_AddIntrinsicDate(ctx);
    JS_AddIntrinsicEval(ctx);
    JS_AddIntrinsicStringNormalize(ctx);
    JS_AddIntrinsicRegExp(ctx);
    JS_AddIntrinsicJSON(ctx);
    JS_AddIntrinsicProxy(ctx);
    JS_AddIntrinsicMapSet(ctx);
    JS_AddIntrinsicTypedArrays(ctx);
    JS_AddIntrinsicPromise(ctx);
    esp_api_init(ctx);

    printf("Free heap after QJS modules added: %d bytes\n", esp_get_minimum_free_heap_size());

    js_std_add_helpers(ctx, 0, NULL);
    js_std_eval_binary(ctx, qjsc_test, qjsc_test_size, 0);

    JSMemoryUsage usage{};
    JS_ComputeMemoryUsage(rt, &usage);
    JS_DumpMemoryUsage(stdout, &usage, rt);

    printf("Free heap after QJS binary added: %d bytes\n", esp_get_minimum_free_heap_size());

    js_std_loop(ctx);

    printf("Free heap after QJS loop started: %d bytes\n", esp_get_minimum_free_heap_size());


    printf("Free heap after QJS freed: %d bytes\n", esp_get_minimum_free_heap_size());

    vTaskDelay(portMAX_DELAY);
}