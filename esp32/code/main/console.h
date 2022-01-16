#ifndef MENU_CONSOLE_H
#define MENU_CONSOLE_H
#include <thread>
#include "esp_pthread.h"
#include "esp_task_wdt.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include <cstring>
#include <iostream>
class MenuConsole
{
public:
   MenuConsole(uart_port_t port_num);

   void setBaud(int baud) {}
   void setStopBits(uart_stop_bits_t bits) {}
   void setParity(uart_parity_t parity) {}

   static void *exec(void*)
   {
      bool needPrompt = true;
      char *line_p;
      std::string line_str;
      uint16_t dt[7];
      
      while (true)
      {
         if (needPrompt)
         {
            std::cout << (const char*)"\n";
            needPrompt = false;
//            m_currentEntry->PrintEntry();
            line_p = linenoise((const char*)"$: ");
            std::cout << std::endl;
            linenoiseHistoryAdd(line_p);
         }
         needPrompt = true;
         std::cout << "|" << line_p;
         std::this_thread::sleep_for(std::chrono::milliseconds(1));
         esp_task_wdt_reset();
      }
   };


private:
   uart_port_t m_portNum;
};
#endif

