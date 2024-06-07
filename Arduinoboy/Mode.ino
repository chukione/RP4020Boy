/**************************************************************************
 * Name:    Timothy Lamb                                                  *
 * Email:   trash80@gmail.com                                             *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
/* ***************************************************************************/
/* "Mode" Functions. Deals with changing the setup of arduino.              */
/* ***************************************************************************/

 /*
   setMode will check if the push button is depressed, If it is it will 
   increment the mode number and make sure its in the 
   range 0 to 4 by mod (%). It will then write the mode to memory,
   set the leds to display the mode, and switch the code over to the
   right function.
 */
 

void setMode()
{
  buttonDepressed = digitalRead(pinButtonMode);
  //u8x8_DrawString(u8g2.getU8x8(), 2, 2, "setmode");
  if(!memory[MEM_FORCE_MODE] && buttonDepressed) { //if the button is pressed
    memory[MEM_MODE]++;                           //increment the mode number
    if(memory[MEM_MODE] > (NUMBER_OF_MODES - 1)) memory[MEM_MODE]=0;  //if the mode is greater then 4 it will wrap back to 0
    #ifndef USE_DUE
    if(!memory[MEM_FORCE_MODE]) EEPROM.write(MEM_MODE, memory[MEM_MODE]); //write mode to eeprom if we arnt forcing a mode in the config
    #endif
    showSelectedMode();            //set the LEDS
    switchMode();
  }
}


 /*
   switchMode is only called from setMode. its responsible for
   linking the mode number to its corrisponding function, 
   and then calling that function. function. function.
 */
void switchMode()
{
  switch(memory[MEM_MODE])
  {
    case 0:
      #ifdef OLED
      #ifndef OLED_GFX
      u8x8.clearLine(5);
      u8x8.clearLine(6);
      u8x8.drawString(1,5,"LSDJ SlaveSync");
      #else
      #ifndef USE_PICO
      u8x8_ClearLine(u8g2.getU8x8(),5);
      u8x8_ClearLine(u8g2.getU8x8(),6);
      u8x8_DrawString(u8g2.getU8x8(),1,5,"LSDJ SlaveSync");
      #else
      u8g2.clearBuffer();
      u8g2.drawXBMP(17, 0, lsdj_width, lsdj_height, lsdj_bits);
      u8g2.sendBuffer();
      u8x8_ClearLine(u8g2.getU8x8(), 6);
      u8x8_ClearLine(u8g2.getU8x8(), 7);
      u8x8_DrawString(u8g2.getU8x8(), 4, 7, "SlaveSync");
      #endif
      #endif
      #endif
      modeLSDJSlaveSyncSetup();
      break;
    case 1:
      #ifdef OLED
      #ifndef OLED_GFX
      u8x8.clearLine(5);
      u8x8.clearLine(6);
      u8x8.drawString(1,5,"LSDJ MasterSync");
      #else
      u8x8_ClearLine(u8g2.getU8x8(),5);
      u8x8_ClearLine(u8g2.getU8x8(),6);
      u8x8_DrawString(u8g2.getU8x8(),1,5, "LSDJ MasterSync");
      #endif
      #endif
      modeLSDJMasterSyncSetup();
      break;
    case 2:
      #ifdef OLED
      #ifndef OLED_GFX
      u8x8.clearLine(5);
      u8x8.clearLine(6);
      u8x8.drawString(1,5,"LSDJ Keyboard");
      #else
      u8x8_ClearLine(u8g2.getU8x8(),5);
      u8x8_ClearLine(u8g2.getU8x8(),6);
      u8x8_DrawString(u8g2.getU8x8(),1,5,"LSDJ Keyboard");
      #endif
      #endif
      modeLSDJKeyboardSetup();
      break;
    case 3:
      #ifdef OLED
      #ifndef OLED_GFX
      u8x8.clearLine(5);
      u8x8.clearLine(6);
      u8x8.drawString(4,5,"NANOLOOP");
      #else
      #ifndef USE_PICO
      u8x8_ClearLine(u8g2.getU8x8(),5);
      u8x8_ClearLine(u8g2.getU8x8(),6);
      u8x8_DrawString(u8g2.getU8x8(),4,5,"NANOLOOP");
      #else
      u8g2.clearBuffer();
      u8g2.drawXBMP(28, 0, nanoloop_width, nanoloop_height, nanoloop_bits);
      u8g2.sendBuffer();
      u8x8_ClearLine(u8g2.getU8x8(), 6);
      u8x8_ClearLine(u8g2.getU8x8(), 7);
      u8x8_DrawString(u8g2.getU8x8(), 4, 7, "nanoloop");
      #endif
      #endif
      #endif
      modeNanoloopSetup();
      break;
    case 4:
      #ifdef OLED
      #ifndef OLED_GFX
      u8x8.clearLine(5);
      u8x8.clearLine(6);
      u8x8.drawString(6,5,"mGB");
      #else
      u8x8_ClearLine(u8g2.getU8x8(),5);
      u8x8_ClearLine(u8g2.getU8x8(),6);
      u8x8_DrawString(u8g2.getU8x8(),6,5,"mGB");
      #endif
      #endif
      modeMidiGbSetup();
      break;
    case 5:
      #ifdef OLED
      #ifndef OLED_GFX
      u8x8.clearLine(5);
      u8x8.clearLine(6);
      u8x8.drawString(4,5,"LSDJ Map");
      #else
      u8x8_ClearLine(u8g2.getU8x8(),5);
      u8x8_ClearLine(u8g2.getU8x8(),6);
      u8x8_DrawString(u8g2.getU8x8(),4,5,"LSDJ Map");
      #endif
      #endif
      modeLSDJMapSetup();
      break;
    case 6:
      #ifdef OLED
      #ifndef OLED_GFX
      u8x8.clearLine(5);
      u8x8.clearLine(6);
      u8x8.drawString(4,5,"MIDI OUT");
      #else
      u8x8_ClearLine(u8g2.getU8x8(),5);
      u8x8_ClearLine(u8g2.getU8x8(),6);
      u8x8_DrawString(u8g2.getU8x8(),4,5,"MIDI OUT");
      #endif
      #endif
      modeLSDJMidioutSetup();
      break;
  }
}


/* ***************************************************************************/
/* General Global Functions Used in more then one of the modes               */
/* ***************************************************************************/

 /*
   sequencerStart is called when either LSDJ has started to play in master mode, 
   or when a MIDI Start or continue command is received in lsdj slave mode.
   Basically it just resets some counters we use and sets a "start" flag.
 */
 
void sequencerStart()
{
  sequencerStarted = true; //Sequencer has started?
  countSyncPulse = 0;      //Used for status LED, counts 24 ticks (quarter notes)
  countSyncTime = 0;       //Used to count a custom amount of clock ticks (2/4/8) for sync effects
  countSyncLightTime=0;
  switchLight=0;
}

 /*
   sequencerStop is called when either LSDJ has stopped sending sync commands for
   some time in LSDJ Master mode, or when a MIDI Stop command is received in
   lsdj slave mode.
   Basically it just resets some counters we use and sets the "start" flag to false.
 */
void sequencerStop()
{
  midiSyncEffectsTime = false;//Turn off MIDI sync effects in LSDJ slave mode
  sequencerStarted = false;   //Sequencer has started?
  countSyncPulse = 0;         //Used for status LED, counts 24 ticks (quarter notes)
  countSyncTime = 0;          //Used to count a custom amount of clock ticks (2/4/8) for sync effects
  countSyncLightTime=0;
  switchLight=0;
  digitalWrite(pinLeds[0],LOW);
  digitalWrite(pinLeds[1],LOW);
  digitalWrite(pinLeds[2],LOW);
  digitalWrite(pinLeds[3],LOW);
  digitalWrite(pinLeds[memory[MEM_MODE]],HIGH);
}
