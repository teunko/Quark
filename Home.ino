////                                  ////
///             Home Menu            ///
//                                  //
float radius = 55; // Replace with your actual radius
float angleDegrees = 180; // Replace with your actual angle in degrees
std::string homeMenuItems[] = {"Calendar", "Tasks", "Games", "Clock", "Reader", "Measure", "Settings"};
int selectedApp = -1;
int selectedX = 0;
int selectedY = 0;

void homeLoop() {
  updateHomeInputs();
  updateAnims();
  radialMenu();
  displayFrame();
}

void advanceIconAnims() {
  if (step30FPS) {
    animFrames[2]++; // Advance Selector Animation
    if (animFrames[2] > 30) {animFrames[2] = 1;} 
    animFrames[3]++; // Advance Settings Icon Anim
    if (animFrames[3] > 28) {animFrames[3] = 1;}
    if (halfFrame) { // These Animations run on halves (15FPS)
      halfFrame = false;
      animFrames[4]++; // Advance Calendar Icon Anim
      if (animFrames[4] > 13) {animFrames[4] = 1;}
      animFrames[5]++; // Advance Calendar Icon Anim
      if (animFrames[5] > 15) {animFrames[5] = 1;}
      animFrames[6]++; // Advance Games Icon Anim
      if (animFrames[6] > 27) {animFrames[6] = 1;}
    }
    else {halfFrame = true;}
  }
}

void radialMenu() {
  advanceIconAnims();

  float angleRadians = angleDegrees * PI / 180.0;

  int l = 0;
  while (l<(sizeof(homeMenuItems) / sizeof(homeMenuItems[0]))) {
    // Convert From Polar To Cartesian
    float lAngle = angleRadians + (l*2*PI/(sizeof(homeMenuItems) / sizeof(homeMenuItems[0])));
    float lx = radius * cos(lAngle) + 129;
    float ly = radius * sin(lAngle) + 63;
    if (lx < 150) {
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.setDrawColor(0);
      u8g2.drawDisc(lx, ly, 15);
      u8g2.setDrawColor(1);
      // Draw Icons
      if (l == 6) {u8g2.drawXBMP(lx-12,ly-12,25,25,settingsIconallArray[animFrames[3]]);}
      else if (l == 0) {u8g2.drawXBMP(lx-12,ly-12,25,25,animCalendarallArray[animFrames[4]]);}
      else if (l == 1) {u8g2.drawXBMP(lx-12,ly-12,25,25,animTasksallArray[animFrames[5]]);}
      else if (l == 2) {u8g2.drawXBMP(lx-12,ly-12,25,25,gamesIconallArray[animFrames[6]]);}
      else {u8g2.drawXBMP(lx-12,ly-12,25,25,iconsallArray[l]);}

      u8g2.drawCircle(lx, ly, 15);
    }
    // Draw Icon Label
    if (lx < 110) {
      u8g2.drawStr(lx-18-u8g2.getStrWidth(homeMenuItems[l].c_str()),ly+2,homeMenuItems[l].c_str());
    }

    // Draw Selector
    if (40 < ly && ly < 88 && lx < 128) {  //verify
      u8g2.drawXBMP(lx-22,ly-22,45,45,spinDotsallArray[animFrames[2]-1]);
      selectedApp = l;
      selectedX = lx;
      selectedY = ly;
    }
    l++;
  }

}

void updateHomeInputs() {
  if (sleeping) {
    int I = 0;
    while (sleeping) {
      largeClock(1,I);
      I++;
    }
  }

  // Update the Bounce objects
  aButton.update();
  bButton.update();
  if (aButton.fell()) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setDrawColor(0);
    u8g2.drawDisc(selectedX, selectedY, 15);
    u8g2.setDrawColor(1);
    u8g2.drawXBMP(selectedX-12,selectedY-12,25,25,iconsallArray[selectedApp]);
    u8g2.drawCircle(selectedX, selectedY, 15);

    u8g2.drawStr(0,8,("S: "+String(selectedApp)).c_str());

    u8g2.sendBuffer();

    int valueZ = 15;
    int targetX = 64;
    int targetY = 64;
    int targetZ = 150;
    //App Launch Animation
    while (selectedX != targetX || selectedY != targetY || valueZ != targetZ) {
      // Move selectedX towards targetX
      if (selectedX < targetX) {
        selectedX++;
        delay(10);
      } 
      else if (selectedX > targetX) {
        selectedX--;
        delay(10);
      }

      // Move selectedY towards targetY
      if (selectedY < targetY) {
        selectedY++;
        delay(10);
      } else if (selectedY > targetY) {
        selectedY--;
        delay(10);
      }

      // Move valueZ towards targetZ
      if (valueZ < targetZ) {
        valueZ++;
      } else if (valueZ > targetZ) {
        valueZ--;
      }

      u8g2.clearBuffer();
      if (selectedApp == 6) {u8g2.drawXBMP(selectedX-12,selectedY-12,25,25,settingsIconallArray[animFrames[3]]);}
      else if (selectedApp == 0) {u8g2.drawXBMP(selectedX-12,selectedY-12,25,25,animCalendarallArray[animFrames[4]]);}
      else if (selectedApp == 1) {u8g2.drawXBMP(selectedX-12,selectedY-12,25,25,animTasksallArray[animFrames[5]]);}
      else if (selectedApp == 2) {u8g2.drawXBMP(selectedX-12,selectedY-12,25,25,gamesIconallArray[animFrames[6]]);}
      else {u8g2.drawXBMP(selectedX-12,selectedY-12,25,25,iconsallArray[selectedApp]);}
      
      u8g2.drawCircle(selectedX, selectedY, valueZ);
      u8g2.sendBuffer();
    }

    //Launch the Selected App
    switch (selectedApp) {
      case 0: //Calendar
        animFrames[1] = 0;
        while (!exitCalendarApp) {
          calendarLoop();
        }
        animFrames[1] = 1;
        exitCalendarApp = false;
        break;

      case 1: //Tasks
        animFrames[1] = 0;
        while (1) {
          //tasksLoop();
        }
        animFrames[1] = 1;
        break;

      case 2: //Games
        animFrames[1] = 0;
        while (!exitGamesApp) {
          gamesLoop();
        }
        exitGamesApp = false;
        animFrames[1] = 1;
        break;
      
      case 3: //Clock
        animFrames[1] = 0;
        while (1) {
          //clockLoop();
        }
        animFrames[1] = 1;
        break;

      case 4: //Reader
        animFrames[1] = 0;
        while (1) {
          //readerLoop();
        }
        animFrames[1] = 1;
        break;

      case 5: //Measure
        animFrames[1] = 0;
        while (1) {
          //measureLoop();
        }
        animFrames[1] = 1;
        break;

      case 6: //Settings
        animFrames[1] = 0;
        settingsCurrentState = SETTINGS_INTRO;
        breakCraneIntro = false;
        settingsEncoderCount = 0;
        while (!exitSettingsApp) {
          settingsLoop();
        }
        animFrames[1] = 1;
        exitSettingsApp = false;
        break;

      default:
        break;

    }
  }

  if (bButton.fell()) {
    // Code to be executed when B button is pressed
  }


  //Refresh Encoder Counts
  int sEncoder_change = sEncoder.getCount();
  int lEncoder_change = lEncoder.getCount();
  if (sEncoder_change) {
    Serial.println(sEncoder_change);
    if ((sEncoder_change < 0)) {
      angleDegrees += 2.5;
      animFrames[1]++;
      if (animFrames[1] > 12) {animFrames[1] = 1;}
    }
    else if (sEncoder_change > 0) {
      angleDegrees -= 2.5;
      animFrames[1]--;
      if (animFrames[1] < 1) {animFrames[1] = 12;}
    }
    sEncoder.setCount(0);
  }
}