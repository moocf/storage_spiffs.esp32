#include <stdio.h>
#include <string.h>
#include <esp_spiffs.h>
#include <mbedtls/md5.h>
#include "macros.h"


static esp_err_t read_hello_txt() {
  printf("- Open file /spiffs/hello.txt\n");
  FILE *f = fopen("/spiffs/hello.txt", "r");
  if (f == NULL) {
    printf("Cannot open file!\n");
    return ESP_FAIL;
  }
  char buff[64];
  memset(buff, 0, sizeof(buff));
  printf("- Read file contents\n");
  fread(buff, 1, sizeof(buff), f);
  printf(buff);
  printf("\n");
  printf("- Close file\n");
  fclose(f);
  return ESP_OK;
}


static esp_err_t md5_alice_txt() {
  printf("- Open file /spiffs/story/alice.txt\n");
  FILE *f = fopen("/spiffs/story/alice.txt", "r");
  if (f == NULL) {
    printf("Cannot open file!\n");
    return ESP_FAIL;
  }
  #define MD5_MAX_LEN 16
  uint8_t buff[64];
  mbedtls_md5_context ctx;
  uint8_t digest[MD5_MAX_LEN];
  printf("- Initialize MD5 context\n");
  mbedtls_md5_init(&ctx);
  printf("- Start MD5 ret\n");
  mbedtls_md5_starts_ret(&ctx);
  size_t read;
  do {
    read = fread(buff, 1, sizeof(buff), f);
    mbedtls_md5_update_ret(&ctx, buff, read);
  } while(read == sizeof(buff));
  printf("- Finish MD5 ret\n");
  mbedtls_md5_finish_ret(&ctx, digest);
  printf("- Generate digest string (hex)\n");
  char digest_str[MD5_MAX_LEN * 2];
  for (int i=0; i<MD5_MAX_LEN; i++)
    sprintf(&digest_str[i<<1], "%02x", digest[i]);
  printf("MD5 digest = %s\n", digest_str);
  printf("- Close file");
  fclose(f);
  return ESP_OK;
}


void app_main() {
  printf("- Mount SPIFFS as VFS\n");
  printf("(VFS enables access though stdio)\n");
  esp_vfs_spiffs_conf_t config = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = false,
  };
  ERETV( esp_vfs_spiffs_register(&config) );
  printf("- Get SPIFFS info total, used`bytes\n");
  size_t total, used;
  ERETV( esp_spiffs_info(NULL, &total, &used) );
  printf("Total = %d, Used = %d\n", total, used);
  ERETV( read_hello_txt() );
  ERETV( md5_alice_txt() );
}
