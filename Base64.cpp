#include "Base64.h"
#include "Base64_config.h"


const char PROGMEM b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

/* 'Private' declarations */
inline void a3_to_a4(unsigned char* a4, const unsigned char * a3);
inline void a4_to_a3(unsigned char* a3, const unsigned char * a4);
inline unsigned char b64_lookup(const char c);

int base64_encode(char *output, char *input, int inputLen) {
  int i = 0, j = 0;
  int encLen = 0;
  //unsigned char a3[3];
  //unsigned char a4[4];
  unsigned char* a4 = (unsigned char*)output;
  
  while(inputLen--) {
    a4[i++] = *(input++);
    if(i == 3) {
      a3_to_a4(a4, a4);

      for(i = 0; i < 4; i++) {
        output[encLen++] = pgm_read_byte(&b64_alphabet[a4[i]]);
      }
      i = 0;
      a4 += 4;
    }
  }

  if(i) {
    for(j = i; j < 3; j++) {
      a4[j] = '\0';
    }

    a3_to_a4(a4, a4);

    for(j = 0; j < i + 1; j++) {
      output[encLen++] = pgm_read_byte(&b64_alphabet[a4[j]]);
    }

    while((i++ < 3)) {
      output[encLen++] = '=';
    }
  }
  output[encLen] = '\0';
  return encLen;
}

int base64_decode(char * output, char * input, int inputLen) {
  int i = 0, j = 0;
  int decLen = 0;
  //unsigned char a3[3];
  //unsigned char a4[4];
  unsigned char* a4 = (unsigned char*)output;

  while (inputLen--) {
    if(*input == '=') {
      break;
    }

    a4[i++] = *(input++);
    if (i == 4) {
      for (i = 0; i <4; i++) {
        a4[i] = b64_lookup(a4[i]);
      }

      a4_to_a3(a4,a4);

      for (i = 0; i < 3; i++) {
        output[decLen++] = a4[i];
      }
      i = 0;
      a4 += 3;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) {
      a4[j] = '\0';
    }

    for (j = 0; j <4; j++) {
      a4[j] = b64_lookup(a4[j]);
    }

    a4_to_a3(a4,a4);

    for (j = 0; j < i - 1; j++) {
      output[decLen++] = a4[j];
    }
  }
  output[decLen] = '\0';
  return decLen;
}

int base64_enc_len(const int inputLen) {
  return (inputLen + 2 - ((inputLen + 2) % 3)) / 3 * 4;
}

int base64_dec_len(char * input, const int inputLen) {
  int i = 0;
  int numEq = 0;
  for(i = inputLen - 1; input[i] == '='; i--) {
    numEq++;
  }

  return ((6 * inputLen) / 8) - numEq;
}

inline void a3_to_a4(unsigned char * a4, const unsigned char * a3) {
  a4[3] = (a3[2] & 0x3f);
  a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
  a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
  a4[0] = (a3[0] & 0xfc) >> 2;
}

inline void a4_to_a3(unsigned char * a3, const unsigned char * a4) {
  a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
  a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
  a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

inline unsigned char b64_lookup(const char c) {
  if(c >='A' && c <='Z') return c - 'A';
  if(c >='a' && c <='z') return c - 71;
  if(c >='0' && c <='9') return c + 4;
  if(c == '+') return 62;
  if(c == '/') return 63;
  return -1;
}

/*
inline unsigned char b64_lookup(const char c) {
  int i;
  for(i = 0; i < 64; i++) {
    if (pgm_read_byte(&b64_alphabet[i]) == c) {
      return i;
    }
  }

  return -1;
}
*/

