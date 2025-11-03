#include "config.h"
#include <stdio.h>
#include <string.h>

#include "evSerial.h"
#include <unistd.h>

#include <sigslot/signal.hpp>
#include "lo2hi.pb.h"
#include "hi2lo.pb.h"

volatile bool sw_version_received = false;

void recvKeepAliveLo(KeepAliveLo s) {
  printf("Current Yeti SW Version: %s (Protocol %i.%0.2i)\n", s.sw_version_string, s.protocol_version_major, s.protocol_version_minor); 
  sw_version_received = true;
}


void help() {
  printf ("\nUsage: ./yeti_fwupdate /dev/ttyXXX firmware.bin\n\n");
  printf ("This tool uses stm32flash (version 0.6 and above) which needs to be installed.\n");
}

int main(int argc, char* argv[]) {
  printf ("Yeti ROM Bootloader Firmware Updater %i.%i\n", yeti_fwupdate_VERSION_MAJOR, yeti_fwupdate_VERSION_MINOR);
  if (argc != 3) {
    help();
    exit(0);
  }
  const char *device = argv[1];
  const char *filename = argv[2];

  evSerial* p = new evSerial();

  if (p->openDevice(device, 115200)) {
      //printf("Running\n");
      p->run();
      p->signalKeepAliveLo.connect(recvKeepAliveLo);
      while (true) {
        if (sw_version_received) break;
        usleep (100);
      }
      printf("\nRebooting Yeti in ROM Bootloader mode...\n");
      // send some dummy commands to make sure protocol is in sync
      p->setMaxCurrent(6.);
      p->setMaxCurrent(6.);
      // now reboot uC in boot loader mode
      p->firmwareUpdate(true);
      sleep (1);
      delete p;
        
      sleep(1);
      char cmd[1000];
      sprintf(cmd, "stm32flash -b 115200 %.100s -v -w %.100s -R", device, filename);
      //sprintf(cmd, "stm32flash -b115200 %.100s", device);
      printf ("Executing %s ...\n", cmd);
      system(cmd);

    //printf ("Joining\n");
  } else {
    printf ("Cannot open device \"%s\"\n", device);
    delete p;
  }
  return 0;
}



