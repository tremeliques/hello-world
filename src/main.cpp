
#include <Arduino.h>
#include <nvs_flash.h>
#include <nvs.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  esp_err_t err = nvs_flash_init();
  nvs_flash_erase();
}

void loop() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      // NVS partition was truncated and needs to be erased
      // Retry nvs_flash_init
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
  }
  ESP_ERROR_CHECK( err );

  // Open
    Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else {
        Serial.printf("Done\n");

        // Read
        Serial.printf("Reading restart counter from NVS ... ");
        int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
        switch (err) {
            case ESP_OK:
                Serial.printf("Done\n");
                Serial.printf("Restart counter = %d\n", restart_counter);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                Serial.printf("The value is not initialized yet!\n");
                break;
            default :
                Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        // Write
        Serial.printf("Updating restart counter in NVS ... ");
        restart_counter++;
        err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        Serial.printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        Serial.printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        Serial.printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);
    }

    //Olá Mundo
    Serial.printf("\n");

    // Restart module
    for (int i = 10; i >= 0; i--) {
        Serial.printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    Serial.printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}