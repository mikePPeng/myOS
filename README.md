# myOS
This is a minimal Real Time Operating System(RTOS) for ARM Cortex M3/M4 processor.

Impelemented Features:

* Disable and Enable Interrupts
* Task
* Software Timer
* IPC
* Memory Management

# Intergration Steps
* Create Bare Metal Project

  In this manual, bare metal project is created using [*STM32CubeIDE*](https://www.st.com/en/development-tools/stm32cubeide.html). You can download and install the software by yourself, or using other IDEs.

  + Create STM32 Project

    1. Open [*STM32CubeIDE*](https://www.st.com/en/development-tools/stm32cubeide.html), go to **File -> New -> STM32 Project**.

    ![1](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/1.png)

    2. Select MCU, in this manual [*STM32F429IGTx*](https://www.st.com/content/st_com/en/products/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus/stm32-high-performance-mcus/stm32f4-series/stm32f429-439/stm32f429ig.html) is selected. Go to **next**. On the first time of launch, some network transmission may be presented.

    ![2](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/2.png)

    3. Enter project name, then go to **next**. 

    ![3](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/3.png)
    
    4. Choose firmware packages, and check **Copy only the necessary library files**, then **Finish**.
    
    ![4](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/4.png)
    
    5. Now STM32 project is successfully created, and *.ioc* file is automatically opened where customized settings can be configured. In this manual default settings is used and no modification in *.ioc* file is required. 
    
    ![5](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/5.png)
    
  + Build and Debug Project

    1. Right click on the project, choose **Build Project**.

    ![6](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/6.png)

    2. Right click on the project, choose **Debug As -> Debug Configurations...**.

    ![7](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/7.png)

    3. Right click on **STM32 Cortex-M C/C++ Application**, choose **New Configuration**.

    ![8](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/8.png)

    4. The configuration corresponding to the created project will be automatically generated. Go to **Debugger** and select debugger according to your board then **Apply**. In this case *SEGGER J-LINK* is selected.

    ![9](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/9.png)

    5. Go to **Main -> Search Project** and select *.elf* file generated during building process.

    ![10](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/10.png)

    ![11](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/11.png)

    6. Click **Apply** and connect your board to your PC, then **Debug**. Now the program is downloaded into your board.

    ![12](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/12.png)

* Intergrate myOS

  1. Since we are in a RTOS environment, **PendSV_Handler** and **SysTick_Handler** have to be implemented by myOS. Go to *stm32f4xx_it.c* file, add ```__attribute__((weak))``` before **PendSV_Handler** and **SysTick_Handler**. These two handlers are implemented in ***kernel_src/system_exception.c***.

  ![13](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/13.png)

  2. Everything that myOS provides is organized in folder ***kernel_src*** and ***kernel_inc***. Add ***kernel_src*** to project path ***Core/Src*** and ***kernel_inc*** to ***Core/Inc***.

     Right click on ***Core/Src***, go to **New -> Folder**.

     ![14](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/14.png)

     Choose **Advanced -> Link to alternate location (Linked Folder)**, then choose the path of ***kernel_src*** and **Finish**. Now ***kernel_src*** is linked to your project. <span style='color:red'>OR</span>, you can simply copy folder ***kernel_src*** to path ***Core/Src*** in file explorer.

     ![15](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/15.png)

     ![16](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/16.png)
  
  3. Similarly, add ***kernel_inc*** to project path ***Core/Inc***. <span style='color:red'>OR</span>, you can simply copy folder ***kernel_inc*** to path ***Core/Inc*** in file explorer.
  
     ![17](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/17.png)
  
  4. <span style='color:red'>NOTE: This step is not required if you just copy folder ***kernel_src***, ***kernel_inc*** to project path ***Core/Src***, ***Core/Inc*** in file explorer during above steps.</span>
  
     Add include path to the project. Right click on project, go to **properties -> C\C++ General -> Path and Symbols -> Includes -> Add...**. Choose the path one level above ***kernel_inc***  and add to include directories (in the source code, header files are included in ```#include "kernel_inc/xxx.h"```manner). Click **Apply and Close**.
  
     ![18](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/18.png)
  
  5. <span style='color:red'>NOTE: This step is not required but highly recommanded.</span>
  
     In myOS, all log information is printed by USARTs. If you want to see these logs, you have to re-visit some previous steps.
  
     In this case, USART1 is used to print log information, so we need to configure USART1. Open *.ioc* file, find **A->Z** and **USART1**. Change **Mode** from ***disable*** to ***Asynchronous***, then check the settings of USART1, like BaudRate, etc..
  
     ![19](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/19.png)
  
     After everything  is checked, right click on the *.ioc* file, and click **Generate Code**.
  
     ![20](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/20.png)
  
     Now in the *main.c*, we can see the initialize function of USART1, which is called in ```int main(void)```.
  
     ![21](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/21.png)
  
     Also, we need to implement ```_write()``` to redirect C standard function```printf()``` to USART1:
  
     ```c
     __attribute__((used)) int _write(int file, char *ptr, int len)
     {
         HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
         return len;
     }
     ```
  
     ![22](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/22.png)
  
     <span style='color:red'>NOTE: If you configure USART in this step and generate USART related codes, you may need to check **PendSV_Handler** and **SysTick_Handler** in *stm32f4xx_it.c*, since they might be over-written by code generator. Add ```__attribute__((weak))``` again if they are over-written.</span>
  
  6. In folder ***kernel_src/sample_code***, some sample code is provided. You can try these sample codes and paly with them. In the *main.c* file, simply comment or delete the infinite loop generated by IDE, and call the entry function of sample codes:
  
     ![23](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/23.png)
  
  7. If you configured USART for log output, you can see these logs using USART tools:
  
     ![24](/Users/pengyuheng/Documents/mike/stm32cube_workspace/myOS/figures/24.png)



That's all about myOS manual. Your advices are most welcome!