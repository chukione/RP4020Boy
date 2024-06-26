void modeProgrammer()
{
  while(sysexProgrammingConnected || sysexProgrammingMode) {
    checkProgrammerConnected();
    if (serial->available()) checkForProgrammerSysex(serial->read());
    #ifndef USE_PICO
    updateProgrammerLeds();
    #endif
    setMode();
    usbMidiUpdate();
  }
  #ifndef USE_PICO
  showSelectedMode();
  #else
  ShowSelectedModeOled();
  #endif
  switchMode();
}

void setProgrammerConnected()
{
  sysexProgrammerLastResponse = millis();
  if(!sysexProgrammingConnected) {
    programmerSendSettings();
  }
  sysexProgrammingConnected = 1;
}

void checkProgrammerConnected()
{
  programmerSendConnectRequest();
  programmerCheckTimeout();
}

void programmerSendSettings()
{
  sysexData[0] = 0xF0;
  sysexData[1] = sysexManufacturerId;
  sysexData[2] = 0x40;
  memcpy(&sysexData[3], memory, MEM_MAX+1);
  sysexData[MEM_MAX+3] = 0xF7;
  serial->write(sysexData, MEM_MAX+4);
#if defined(USE_TEENSY) || defined(USE_PICO)
  usbMIDI.sendSysEx(MEM_MAX+4, sysexData);
#endif
}

void setProgrammerRequestConnect()
{
  uint8_t data[4] = {0xF0,sysexManufacturerId,65,0xF7};
  serial->write(data, 4);
#if defined(USE_TEENSY) || defined(USE_PICO)
  usbMIDI.sendSysEx(4, data);
#endif
}

void setProgrammerMemorySave()
{
  byte offset = 2;
  for(byte m=4;m<MEM_MAX;m++) {
    memory[m] = sysexData[offset];
    offset++;
  }
  saveMemory();
  loadMemory();
  programmerSendSettings();
}

void setProgrammerRestoreMemory()
{
  initMemory(1);
  programmerSendSettings();
}

void programmerCheckTimeout()
{
  if(sysexProgrammingConnected && millis() > (sysexProgrammerLastResponse+sysexProgrammerWaitTime)) {
    //programmer timeout!
    sysexProgrammingConnected = 0;
    sysexProgrammingMode      = 0;
  }
  if(sysexProgrammingMode && millis() > (sysexProgrammerLastResponse+sysexProgrammerWaitTime)) {
    //programmer timeout!
    sysexProgrammingConnected = 0;
    sysexProgrammingMode      = 0;
  }
}

void programmerSendConnectRequest()
{
  if(millis() > (sysexProgrammerLastSent+sysexProgrammerCallTime)) {
    uint8_t data[6] = {0xF0, sysexManufacturerId, 0x7F, defaultMemoryMap[MEM_VERSION_FIRST], defaultMemoryMap[MEM_VERSION_SECOND], 0xF7};
    serial->write(data, 6);
#if defined(USE_TEENSY) || defined(USE_PICO)
    usbMIDI.sendSysEx(6, data);
#endif
    sysexProgrammerLastSent = millis();
  }
}

boolean checkSysexChecksum()
{
  byte checksum = sysexData[(sysexPosition - 1)];
  byte checkdata= 0;
  if(checksum) {
    for(int x=2;x!=(sysexPosition - 2);x++) {
      checkdata += sysexData[x];
    }
    if(checkdata & 0x80) checkdata -= 0x7F;
    if(checkdata == checksum) {
      return true;
    }
  }
  return true;
}


void clearSysexBuffer()
{
  for(int x=0;x!=sysexPosition;x++) {
    sysexData[x]= 0;
  }
  sysexPosition = 0;
}

void setMode(byte mode)
{
  memory[MEM_MODE] = mode;
  #if !defined(USE_DUE) && !defined(USE_PICO)
  EEPROM.write(MEM_MODE, memory[MEM_MODE]);
  #endif
  #ifndef USE_PICO
  showSelectedMode();
  #else
  ShowSelectedModeOled();
  #endif
  switchMode();
}

void sendMode()
{
  uint8_t data[4] = {0xF0, sysexManufacturerId, memory[MEM_MODE], 0xF7};
  serial->write(data, 4);
#if defined(USE_TEENSY) || defined(USE_PICO) 
  usbMIDI.sendSysEx(4, data);
#endif
}

void setMidioutDelay(byte a,byte b,byte c,byte d)
{
  memory[MEM_MIDIOUT_BIT_DELAY] = a;
  memory[MEM_MIDIOUT_BIT_DELAY+1] = b;
  memory[MEM_MIDIOUT_BYTE_DELAY] = c;
  memory[MEM_MIDIOUT_BYTE_DELAY+1] = d;
  saveMemory();
  changeTasks();
}

void getSysexData()
{
  if(sysexData[0] == 0x69 && checkSysexChecksum()) {
    //sysex good, do stuff
    sysexPosition = 0;
    if(sysexProgrammingMode) {
      if(sysexData[1] == 64
      && sysexData[2] == defaultMemoryMap[MEM_VERSION_FIRST]
      && sysexData[3] == defaultMemoryMap[MEM_VERSION_SECOND]) {
        //serial connected to programmer
        setProgrammerRequestConnect();
      }
      if(sysexData[1] == 66
      && sysexData[2] == defaultMemoryMap[MEM_VERSION_FIRST]
      && sysexData[3] == defaultMemoryMap[MEM_VERSION_SECOND]) {
        //serial connected to programmer
        setProgrammerConnected();
      }
      if(sysexData[1] == 70) {
        //save states
        setProgrammerMemorySave();
      }
      if(sysexData[1] == 71) {
        //save states
        setProgrammerRestoreMemory();
      }
    }
    if(sysexData[1] == 72) {
      sysexProgrammingMode = true;
      sysexProgrammerLastResponse = millis();
      modeProgrammer();
    }
    if(sysexData[1] == 73) {
      sendMode();
    }
    if(sysexData[1] == 74) {
      setMode(sysexData[2]);
    }
    if(sysexData[1] == 75) {
      setMidioutDelay(sysexData[2],sysexData[3],sysexData[4],sysexData[5]);
    }
  }
  clearSysexBuffer();
}



boolean checkForProgrammerSysex(byte sin)
{
    if(sin == 0xF0) {
        sysexReceiveMode = true;
        sysexPosition= 0;
        return true;
    } else if (sin == 0xF7 && sysexReceiveMode) {
        sysexReceiveMode = false;
        getSysexData();
        sysexPosition= 0;
        return true;
    } else if (sysexReceiveMode == true) {
        sysexData[sysexPosition] = sin;
        sysexPosition++;
        if(sysexPosition > longestSysexMessage) {
            clearSysexBuffer();
            sysexReceiveMode = false;
        }
        return true;
    }
    return false;
}


void blinkSelectedLight(int led)
{
  #ifndef USE_PICO
      if(!blinkSwitch[led]) digitalWrite(pinLeds[led],HIGH);
      blinkSwitch[led]=1;
      blinkSwitchTime[led]=0;
  #endif
}
