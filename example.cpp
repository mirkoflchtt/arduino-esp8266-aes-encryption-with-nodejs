#include <stdio.h>
#include <stdint.h>
#include <string>

#include "AES.h"
#include "Base64.h"
#include "random.h"

#define TO_C_STRING(str)		(str).c_str()
#define MSG_SIZE				    (512)

typedef std::string String;


// Generate a random initialization vector
void gen_iv(byte  *iv) {
  for ( int32_t i = 0 ; i < N_BLOCK ; i++ ) {
    iv[i]= (byte) getrnd();
  }
}

const char* dump_hex(const byte* data, const size_t size)
{
	static String str;
	static const char* const lut = "0123456789abcdef";
	char hex[3];
	hex[2] = '\0';
	
	str = String("0x");
	for ( size_t i=0; i<size; i++ ) {
		hex[0]  = lut[data[i]>>4];
		hex[1]  = lut[data[i]&0xF];
		str += String(hex);
	}
	return (const char*)TO_C_STRING(str);
}

uint32_t pad_b64(char* msg, uint32_t size)
{
  AES aes;
	const uint32_t pad_size = aes.calc_size_n_pad(size);
	for ( ; size<pad_size; size++ )
		msg[size] = '=';
	
	return pad_size;
}

uint32_t encode_msg(
			char* dst,
			const uint8_t* src, const uint32_t src_size,
			const uint8_t key[N_BLOCK],
			uint8_t* iv)
{
  byte cipher[MSG_SIZE];
  //char decoded[MSG_SIZE];
  uint32_t size;
  
  AES aes;
  
  printf("#####  Encoding  ########################################\n");
  printf("[%u] key [HEX]   : %s\n", N_BLOCK, dump_hex(key, N_BLOCK));
  if ( iv )
  printf("[%u] iv  [HEX]   : %s\n", N_BLOCK, dump_hex(iv, N_BLOCK));
  
  aes.set_key((byte*)key , N_BLOCK);  // Get the globally defined key
  
  // Print the IV
  if ( iv ) {
	size = base64_encode(dst, (char*)iv, N_BLOCK);
	printf("[%u] IV (B64)       : %s\n", size, dst);
  }
  printf("[%u] Message        : %s\n", src_size, src);
  
  size = base64_encode(dst, (char*)src, src_size);
  printf("[%u] Message (B64)  : %s\n", size, dst);
  
  // For sanity check purpose
  /*uint32_t s = base64_decode(decoded , dst , size);
  printf("\nSanity Check!!\n");
  printf("[%u] Decoded        : %s\n", s, decoded);*/

  size = pad_b64((char*)dst, size);
  
  // Encrypt! With AES128, our key and IV, CBC and pkcs7 padding
  if ( iv )  
    aes.do_aes_encrypt((byte*)dst, size+1, cipher, (byte*)key, 128, iv);
    //aes.do_aes_encrypt((byte*)src, src_size , cipher, (byte*)key, 128, iv);
  else
	aes.do_aes_encrypt((byte*)dst, size+1, cipher, (byte*)key, 128);
  printf("[%u] Cypher         : %s\n", aes.get_size(), dump_hex(cipher, aes.get_size()));
  
  return base64_encode(dst, (char*)cipher, aes.get_size());
}



uint32_t decode_msg(
			uint8_t* dst,
			const char* src,
			const byte key[N_BLOCK],
			byte* iv)
{
  byte cipher[MSG_SIZE];
  uint32_t size;
  
  AES aes;
  
  printf("#####  Decoding  ########################################\n");
  printf("[%u] key [HEX]   : %s\n", N_BLOCK, dump_hex(key, N_BLOCK));
  if ( iv )
  printf("[%u] iv  [HEX]   : %s\n", N_BLOCK, dump_hex(iv, N_BLOCK));
  
  aes.set_key((byte*)key , N_BLOCK);  // Get the globally defined key
  
  // Print the IV
  if ( iv ) {
	size = base64_encode((char*)dst, (char*)iv, N_BLOCK);
	printf("[%u] IV (B64)       : %s\n", size, (char*)dst);
  }
  
  size = base64_decode((char*)cipher , (char*)src , strlen(src));
  printf("[%u] Cypher         : %s\n", size, dump_hex(cipher, size));
  
  // Decrypt! With AES128, our key and IV, CBC and pkcs7 padding    
  if ( iv )
	aes.do_aes_decrypt(cipher, size, (byte*)cipher, (byte*)key, 128, iv);
  else
    aes.do_aes_decrypt(cipher, size, (byte*)cipher, (byte*)key, 128);
  //cipher[75] = '='; cipher[76] = '\0';
  size  = (uint32_t)strlen((char*)cipher);
  printf("[%u] Message Decrypted : %s\n", size, cipher);
  
  return base64_decode((char*)dst , (char*)cipher, aes.get_size());
}

int main( int argc, char* argv[] )
{
  // Our AES key. Note that is the same that is used on the Node-Js side but as hex bytes.
  const byte key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
    
  // The unitialized Initialization vector
  byte encode_iv[N_BLOCK], decode_iv[N_BLOCK];
  //byte *encode_iv = NULL, *decode_iv = NULL;
  
  // Our message to encrypt. Static for this example.
  const char* msg[] = { "{\"data\":{\"value\":300}, \"SEQN\":700 , \"msg\":\"IT WORKS!!\" }",
						"{\"data\":{\"value\":415}, \"SEQN\":633 , \"msg\":\"Updated Temperature!\" }"};
  
  char encoded[MSG_SIZE];
  uint8_t decoded[MSG_SIZE];
  
  // Generate a random IV
  if ( encode_iv ) {
	gen_iv(encode_iv);                  
	memcpy(decode_iv, encode_iv, sizeof(encode_iv));
  }
  
  for ( int32_t i=0; i<2; i++ ) {
	uint32_t size;
	
	size = encode_msg(encoded, (uint8_t*)msg[i], strlen(msg[i]), key, encode_iv);
	printf("\n[%u] Encoded Message: %s\n\n", size, encoded);
	
	size = decode_msg(decoded, encoded, key, decode_iv);
	printf("\n[%u] Decoded Message: %s\n\n", size, (char*)decoded);
	
    if ( 0==memcmp(msg[i], (char*)decoded, strlen(msg[i])) ) {

    }	
  }
  
  return 0;
}
