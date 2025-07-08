////                                  ////
///       Settings Application       ///
//                                  //

void updateSettingsInputs() {
  aButton.update();
  bButton.update();  
  int sEncoder_change = sEncoder.getCount();
  int lEncoder_change = lEncoder.getCount();

  switch (settingsCurrentState) {
    case SETTINGS_INTRO:
      if (sEncoder_change) {
        if ((sEncoder_change < 0)) {
          settingsEncoderCount++;
          breakCraneIntro = true;
        }
        else if (sEncoder_change > 0) {
          settingsEncoderCount--;
          if (settingsEncoderCount < 0) {settingsEncoderCount = 0;}
        }
        sEncoder.setCount(0);
      }
      if (lEncoder_change) {
        if (lEncoder_change < 0) {
          //settingsEncoderCount++;
          //settingsCurrentState = SETTINGS_HOME;
        }
        if (lEncoder_change > 0) {
          //settingsEncoderCount--;
          //if (settingsEncoderCount < 0) {settingsEncoderCount = 0;}
        }
      }

      if (aButton.fell()) {
        // Do Nothing
      }

      if (bButton.fell()) {
        exitSettingsApp = true;
        break;
      }
      break;
    case SETTINGS_HOME:
      if (sEncoder_change) {
        if ((sEncoder_change < 0)) {
          settingsEncoderCount++;
        }
        else if (sEncoder_change > 0) {
          settingsEncoderCount--;
          if (settingsEncoderCount < 0) {settingsEncoderCount = 0;}
        }
        sEncoder.setCount(0);
      }
      if (lEncoder_change) {
        if (lEncoder_change < 0) {
          settingsEncoderCount++;
        }
        if (lEncoder_change > 0) {
          settingsEncoderCount--;
          if (settingsEncoderCount < 0) {settingsEncoderCount = 0;}
        }
      }

      if (aButton.fell()) {
        
      }

      if (bButton.fell()) {
        exitSettingsApp = true;
        break;
      }
      break;

    case BRIGHTNESS:
      break;
    default:
      break;

  }
  
}

void advanceSettingsAnims() {
  if (step30FPS) { // These Animations run at 30FPS
    
    if (halfFrame) { // These Animations run on halves (15FPS)
      halfFrame = false;
      
      if (quarterFrame) { // These Animations run on quarters (7.5FPS)
        quarterFrame = false;
        animFrames[7]++; // Advance Crane Anim
        if (animFrames[7] > 16) {animFrames[7] = 1;}
      }
      else {quarterFrame = true;}
    }
    else {halfFrame = true;}
  }
}

void settingsMenu() {
  switch (settingsCurrentState) {
    case SETTINGS_INTRO:
      if (!breakCraneIntro) { // Play Intro Animation While Waiting For Scroll
        advanceSettingsAnims();
        u8g2.drawXBMP(0,0,128,128,craneallArray[animFrames[7]-1]);
      }
      else {
        if (settingsEncoderCount < 96) { // Allow movement of crane
          u8g2.drawXBMP(0,0,128,128,epd_bitmap_craneMiddle);
          u8g2.drawVLine(47,18,96-settingsEncoderCount);
          u8g2.drawXBMP(41,115-settingsEncoderCount,13,13,upTabupTab00);
          u8g2.drawHLine(0,128-settingsEncoderCount,128);
          u8g2.drawPixel((4 * cos(settingsEncoderCount) + 108),(4 * sin(settingsEncoderCount) + 69));
          u8g2.setDrawColor(0);
          if (settingsEncoderCount > 2) {u8g2.drawBox(0,129-settingsEncoderCount,128,settingsEncoderCount);}
          u8g2.setDrawColor(1);
          u8g2.drawXBMP(0,129-settingsEncoderCount,128,96,settingsArtsettingsArt00);
        }
        else { //Crane has gone to limit, finish animation and open menu
          while (settingsEncoderCount < 128) {
            u8g2.drawXBMP(0,0,128,128,epd_bitmap_craneMiddle);
            u8g2.drawXBMP(41,115-settingsEncoderCount,13,13,upTabupTab00);
            u8g2.drawHLine(0,128-settingsEncoderCount,128);
            u8g2.setDrawColor(0);
            u8g2.drawBox(0,129-settingsEncoderCount,128,settingsEncoderCount);
            u8g2.setDrawColor(1);
            u8g2.drawXBMP(0,129-settingsEncoderCount,128,96,settingsArtsettingsArt00);
            settingsEncoderCount++;
            delay(10);
            u8g2.sendBuffer();
            u8g2.clearBuffer();
          }
          u8g2.clearBuffer();
          settingsEncoderCount = 0;
          settingsCurrentState = SETTINGS_HOME;
        }
      }
      break;

    case SETTINGS_HOME:
      if (settingsEncoderCount < 96) {u8g2.drawXBMP(0,-settingsEncoderCount,128,96,settingsArtsettingsArt00);}

      break;
  }
}

void settingsLoop() {
  updateSettingsInputs();
  updateAnims();
  settingsMenu();

  displayFrame();
}