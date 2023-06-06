#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <gpio.h>
#include <serial.h>

int main() {

  char *commandList[] = {"AT", "ATI", "AT+COPS?", "AT+COPS=?"},
       serialFilePath[] = "/dev/serial0", *output;

  int serialPort, moduleStatus;
  unsigned long idx;
  time_t pwrKeyTime;

  moduleStatus = checkPinVal(27);
  if (0 > moduleStatus) {
    printf("Error checkPinVal()\n");
    return 1;
  } else if (moduleStatus == 0) {
    printf("Module is off\nPowering on\n");
    if (0 != modemPowerToggle(17)) {
      printf("Error toggling modem power\n");
      return 1;
    }
    pwrKeyTime = time(NULL);
    while (!checkPinVal(27)) {
      sleep(1);
      if (time(NULL) - pwrKeyTime > 10) {
        printf("Modem power on takes longer than 10s\nAborting\n");
        return 1;
      }
    }
  }

  serialPort = openSerialPort(serialFilePath);
  if (serialPort < 0) {
    printf("Error openSerialPort()\n");
    return 1;
  }

  output = malloc(1);
  for (idx = 0; idx < sizeof(commandList) / sizeof(*commandList); idx++) {
    if (0 != querySerialPort(&output, serialPort, commandList[idx])) {
      printf("Error querySerialPort()\n");
      close(serialPort);
      free(output);
      return 1;
    }
    printf("%s\n", output);
  }

  close(serialPort);

  free(output);
  return 0;
}
