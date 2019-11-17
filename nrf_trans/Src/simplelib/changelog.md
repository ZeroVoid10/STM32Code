# SimpleLib/STM32Lib CHANGE LOG

## Version 0.2.1

- 串口相关
  - 部分解耦USART与CMD功能(均在使用IDLE 的DMA)
- CAN相关
  - 添加CAN所有接收回调函数
- NRF
  - 修复NRF通信宽度问题
  - 添加部分nrf_handle功能, 后期完全移植使用handle操作
  - 完善NRF 通信协议部分运行代码

### BUG or TODO

- TODO: NRF IRQ部分代码需要手动添加到HAL库GPIO回调问题
