#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "AES.h"
#include "Base64.h"
#include "random.h"

#define MAX_MESSAGE_SIZE    (16*1024)
#define TESTS_BASE64_COUNT    (2*1024)
#define TESTS_AES_COUNT     (2*512)

char get_rnd_b64(void)
{
  const char b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
    return b64_alphabet[getrnd()&0x3F];
}

int32_t test_AES(const uint32_t tests_count)
{
  uint8_t message[MAX_MESSAGE_SIZE];
  uint8_t out[MAX_MESSAGE_SIZE];
  uint8_t cipher[MAX_MESSAGE_SIZE+16];
    int32_t tests_failed = 0;
    float old_perc = 0;
    
    // Our AES key. Note that is the same that is used on the Node-Js side but as hex bytes.
  const byte key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
  
    AES aes;
    
    aes.set_key((byte*)key , N_BLOCK);  // Get the globally defined key
    
    byte iv_encode[N_BLOCK] = { 0 };
    byte iv_decode[N_BLOCK] = { 0 };
    //iv_decode[0] = 1;
    
    printf("Running %d AES Tests :\n", tests_count);
    for ( uint32_t i=0; i<tests_count; i++ ) {
    int32_t count = (int32_t)getrnd() << 8;
    int32_t size;
    count += getrnd();
    count %= MAX_MESSAGE_SIZE;
    
    const float perc = 100.0*(i+1)/tests_count;
    if ( perc-old_perc>5 ) {
      old_perc = perc;
      printf("\t%3.2f%% done.\n", perc);
    }
    
    //count = ((count+N_BLOCK-1)/N_BLOCK) * N_BLOCK;
    for ( uint32_t j=0; j<count; j++ )
      //message[j] = (byte)get_rnd_b64();//getrnd();
      message[j] = getrnd();
    
    //printf("### [%u] iv_encode : ", N_BLOCK); aes.printArray((byte*)iv_encode, N_BLOCK);
    //printf("### [%u] iv_decode : ", N_BLOCK); aes.printArray((byte*)iv_decode, N_BLOCK);
    
    //printf("### [%u] plain     : ", count); aes.printArray((byte*)message, count);
    
    aes.do_aes_encrypt((byte*)message, count+1 , cipher, (byte*)key, 128, iv_encode);
    size = aes.get_size();
    //printf("### [%u] cipher    : ", size); aes.printArray(cipher, size);
    
    aes.do_aes_decrypt(cipher, size, (byte*)out, (byte*)key, 128, iv_decode);
    size = aes.get_size();
    //printf("### [%u] decoded   : ", size); aes.printArray((byte*)out, size);
    
    if ( /*size != count  ||*/ memcmp(message, out, count) ) {
      printf("Test #%06u Failed %u -> %u\n", i, count, aes.get_size());
      tests_failed += 1;
    }
  }
  
  if ( old_perc<100 ) printf("\t%3.2f%% done.\n", 100.0);
  printf("\n\n\tTests Failed: %u\n", tests_failed);
  
    return tests_failed;
}

int32_t test_base64(const uint32_t tests_count)
{
  uint8_t message[MAX_MESSAGE_SIZE];
  uint8_t out[MAX_MESSAGE_SIZE];
  uint8_t b64[2*MAX_MESSAGE_SIZE];
  int32_t tests_failed = 0;
  float old_perc = 0;
  
  printf("Running %d Base64 Tests :\n", tests_count);
  for ( uint32_t i=0; i<tests_count; i++ ) {
    uint32_t count = (uint32_t)getrnd() << 8;
    int32_t size;
    count += getrnd();
    count %= MAX_MESSAGE_SIZE;
    
    const float perc = 100.0*(i+1)/tests_count;
    if ( perc-old_perc>5 ) {
      old_perc = perc;
      printf("\t%3.2f%% done.\n", perc);
    }
    
    for ( uint32_t j=0; j<count; j++ )
      message[j] = getrnd();
      
    size = base64_encode((char*)b64, (char*)message, count);
    size = base64_decode((char*)out, (char*)b64, size);
    
    tests_failed += ( size != count  || memcmp(message, out, count) );
  }
  
  if ( old_perc<100 ) printf("\t%3.2f%% done.\n", 100.0);
  printf("\n\n\tTest Failed: %u\n", tests_failed);
    
  return tests_failed;
}

int main(int argc, char* argv[])
{
  test_base64(TESTS_BASE64_COUNT);
  
  test_AES(TESTS_AES_COUNT);
  
  return 0;
}
