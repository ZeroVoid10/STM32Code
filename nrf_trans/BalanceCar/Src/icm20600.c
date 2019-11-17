#include "icm20600.h"
#include "i2c.h"
#include "usart.h"
#include "main.h"


ICM_Device_Setting ICM_Setting;

/*
下面几个个函数需用户自己实现。
*/

extern I2C_HandleTypeDef hi2c1;
uint8_t I2C_Write_Buffer(uint8_t slaveAddr, uint8_t writeAddr, uint8_t *pBuffer,uint16_t len);
uint8_t I2C_Read_Buffer(uint8_t slaveAddr,uint8_t readAddr,uint8_t *pBuffer,uint16_t len);
void I2C_Reset(void);
extern void Delay_Us(uint32_t nus);
#define I2C_OK  0x00


void i2c_error_deal();
inline void i2c_error_deal();
void I2C_Error_Check(uint8_t i2c_result);

static void i2c_write_byte(uint8_t write_addr,uint8_t data){
  if(I2C_Write_Buffer(ICM_ADDR,write_addr,&data,1)!=I2C_OK){
    i2c_error_deal();
  }
}

inline void i2c_error_deal(){
  static int i2c_error_count=0;
  i2c_error_count++;
  if(i2c_error_count>20){
    I2C_Reset();
    i2c_error_count=0;
  }
}

static uint8_t I2C_read_byte(uint8_t read_addr){
  uint8_t temp=0;
  if(I2C_Read_Buffer(ICM_ADDR,read_addr,&temp,1)!=I2C_OK){
    i2c_error_deal();
  }
  return temp;
}

inline void I2C_Error_Check(uint8_t i2c_result){
  if(i2c_result!=I2C_OK){
    i2c_error_deal();
  }
}
void ICM_Read_Raw(int16_t ac[],int16_t gy[]){
  
  uint8_t temp[6];
  uint8_t i2c_result;
  
  i2c_result=I2C_Read_Buffer(ICM_ADDR,ACCEL_XOUT_H,temp,6);
  I2C_Error_Check(i2c_result);
  ac[0]=(temp[0]<<8)|temp[1];
  ac[1]=(temp[2]<<8)|temp[3];
  ac[2]=(temp[4]<<8)|temp[5];
  
  i2c_result=I2C_Read_Buffer(ICM_ADDR,GYRO_XOUT_H,temp,6);
  I2C_Error_Check(i2c_result);
  gy[0]=(temp[0]<<8)|temp[1];
  gy[1]=(temp[2]<<8)|temp[3];
  gy[2]=(temp[4]<<8)|temp[5];	
}

void Gyroraw_to_Angle_Speed(int16_t *gy,float *angle_speed ){
  for(int i=0;i<3;++i){
    angle_speed[i]=ICM_Setting.gyro_range*gy[i]/32768.0f;    // 单位为dgree/s
  }
}

void Accraw_to_Acceleration(int16_t *ac,float *acceleration){
  for(int i=0;i<3;++i){
    acceleration[i]=ICM_Setting.accel_range*ac[i]/32768.0f;  // 单位为g
  }
}

void ICM_Init(){
  ICM_Setting.accel_range_setting=RANGE16G;
  ICM_Setting.accel_lpf_setting=0x01;//200hz
  ICM_Setting.gyro_range_setting=RANGE500;
  
INIT:
  
  //Delay_Us(1000);
  

  i2c_write_byte(PWR_MGMT_1,0x01);
  i2c_write_byte(SMPLRT_DIV, 0x00);
  i2c_write_byte(MPU_CONFIG, 0x02);  //之前延时为20ms(0x06，现在为3ms左右 0x02)
  
  i2c_write_byte(GYRO_CONFIG, ICM_Setting.gyro_range_setting);   //
  i2c_write_byte(ACCEL_CONFIG, ICM_Setting.accel_range_setting); 
  i2c_write_byte(ACCEL_CONFIG2, ICM_Setting.accel_lpf_setting); 
  
  switch(ICM_Setting.gyro_range_setting){
  case RANGE250:
    ICM_Setting.gyro_range=250.0f;
    break;
  case RANGE500:
    ICM_Setting.gyro_range=500.0f;
    break;
  case RANGE1000:
    ICM_Setting.gyro_range=1000.0f;
    break;
  case RANGE2000:
    ICM_Setting.gyro_range=2000.0f;
    break;
  }
  
  switch(ICM_Setting.accel_range_setting){
  case RANGE2G:
    ICM_Setting.accel_range=2;
    break;
  case RANGE4G:
    ICM_Setting.accel_range=4;
    break;	
  case RANGE8G:
    ICM_Setting.accel_range=8;
    break;	
  case RANGE16G:
    ICM_Setting.accel_range=16;
    break;		
  }
  
  uprintf("start icm20600 init\r\n");
  uint8_t dev_ID;
  dev_ID=I2C_read_byte(WHO_AM_I);
  
  if(dev_ID!=0x11){
    I2C_Reset();
    uprintf("icm20600 init failed!\r\n");
    goto INIT;
  }
  uprintf("icm20600 init ok\r\n");
}