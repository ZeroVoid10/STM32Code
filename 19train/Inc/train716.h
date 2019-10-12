#ifndef __TRAIN716_H__
#define __TRAIN716_H__

#ifdef __cplusplus
extern "C" {
#endif

void flashLightLED(void);
void keyLightLED(void);
void transmitHello(void);
void keyTransmitHello(void);
void uprintf(char *fmt, ...);
void uprintfTest(void);
  
#ifdef __cplusplus
}
#endif

#endif /* __TRAIN716_H__ */