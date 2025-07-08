////                                  ////
///         Tetrapolis  Game         ///
//                                  //
//Variables
int backgroundArray[4][20];
int setGameMode = 1;
int positionCount = 4;
int selectedRow = 0;
int selectedCol = 0;
int side = 0;
int blockBeingRemoved = -1;
int blockRemovalProgress = 0;
bool tetraDebug = false;
int saveNumSpaces = 0;
int timerMillis = 0;
int timeLimit = 120000 + millis(); // 2 minutes

int tetraRandomMillis = 0;

int maps[][10][10] = {
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0,-1,-1, 0, 0, 0, 0, 0,-1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0,-1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0,-1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0,-1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0,-1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0,-1, 0, 0, 0, 0, 0,-1,-1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0,-1, 0, 0,-1,-1, 0, 0,-1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0,-1,-1,-1,-1, 0, 0},
        {0, 0,-1,-1,-1,-1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0,-1, 0, 0,-1,-1, 0, 0,-1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    }
};

int generatedMap[10][10];
int gameMap[10][10];

// Functions
void initializeArray(int arr[10][10], int value) {
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
        arr[i][j] = value;
    }
  }
}

void setArrayEqual(int target[10][10], const int source[10][10]) {
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
        target[i][j] = source[i][j];
    }
  }
}

int countZeros(const int arr[10][10]) {
  int zeroCount = 0;
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      if (arr[i][j] == 0) {
        zeroCount++;
      }
    }
  }
  return zeroCount;
}

// Map generation for mode 1 (Shapes)
void generateMapMode1() {
  // Initialize variables
  int A = 0;
  int B = 0;
  // Initialize the Map Array
  initializeArray(generatedMap, 0);
  // Pick a random map
  int mapNumber = random(0,(sizeof(maps) / (sizeof(int) * 100)));
  // Set the generated map equal to an archetype
  setArrayEqual(generatedMap, maps[mapNumber]);
  // Count the empty spaces in the array
  int spaces = countZeros(generatedMap);
  saveNumSpaces = spaces;

  bool blankFound = false;
  int totalPlaced = 0;
  int typePlaced = 0;
  //Run until all spaces are filled
  while (totalPlaced < spaces) {
    // Pick a random block to place
    int currentBlock = random(1,4); // 0:Blank 1:Triangle, 2:Circle, 3:X
    while (typePlaced < 3) { //Ensure 3 blocks are placed
      while (!blankFound) { // Pick coordinates until an unoccupied one is found
        A = random(0,10);
        B = random(0,10);

        u8g2.clearBuffer();
        u8g2.drawXBMP(0,40,128,41,epd_bitmap_tetralogo00);
        // Print progress to screen
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.setCursor(64 -(u8g2.getStrWidth("Generating Map:      ")/2), 100);
        u8g2.print("Generating Map: ");
        u8g2.print(totalPlaced+1);
        u8g2.print("/");
        u8g2.print(spaces);
        u8g2.sendBuffer();

        if (generatedMap[A][B] == 0) {blankFound = true;}
      }
      blankFound = false;
      generatedMap[A][B] = currentBlock;
      totalPlaced++;
      typePlaced++;
    }
    typePlaced = 0;
  }
}

void displayMap(int dispArray[10][10], int X, int Y) {
  u8g2.setDrawColor(0);
  u8g2.drawBox(X,Y,100,100);
  u8g2.setDrawColor(1);
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      switch (dispArray[i][j]) {
        case -1: // Blockers
          u8g2.drawXBMP(X+(10*i),Y+(10*j),10,10,blockIcons_allArray[0]);
          break;
        case 0: // Blank
          break;
        case 1: // Triangle
          u8g2.drawXBMP(X+(10*i),Y+(10*j),10,10,blockIcons_allArray[1]);
          break;
        case 2: // Circle
          u8g2.drawXBMP(X+(10*i),Y+(10*j),10,10,blockIcons_allArray[2]);
          break;
        case 3: // X
          u8g2.drawXBMP(X+(10*i),Y+(10*j),10,10,blockIcons_allArray[3]);
          break;
        default:
          break;
      }
    }
  }
}

void gameOver() {
  tetraCurrentState = TETRA_GAMEOVER;
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(64 -(u8g2.getStrWidth("Game Over!")/2), 32);
  u8g2.print("Game Over!");
  u8g2.setCursor(64 -(u8g2.getStrWidth("(A) Try Again")/2), 80);
  u8g2.print("(A) Try Again");
  u8g2.setCursor(64 -(u8g2.getStrWidth("(B) Quit     ")/2), 88);
  u8g2.print("(B) Quit     ");
  u8g2.sendBuffer();
  breakLoop = false;
  while (!breakLoop) {
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(64 -(u8g2.getStrWidth("Game Over!")/2), 32);
    u8g2.print("Game Over!");
    u8g2.setCursor(64 -(u8g2.getStrWidth("(A) Try Again")/2), 80);
    u8g2.print("(A) Try Again");
    u8g2.setCursor(64 -(u8g2.getStrWidth("(B) Quit     ")/2), 88);
    u8g2.print("(B) Quit     ");
    u8g2.sendBuffer();
    updateTetraInputs();
  }
  positionCount = 4;
  selectedRow = 0;
  selectedCol = 0;
  side = 0;
  blockBeingRemoved = -1;
  blockRemovalProgress = 0;
  timerMillis = 0;
  timeLimit = 120000 + millis();
}

void gameWin() { 
  animFrames[8] = 0;
  for (int i=0;i<50;i++) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(64 -(u8g2.getStrWidth("Game Complete!")/2), 120);
    u8g2.print("Game Complete!");
    u8g2.drawXBMP(14,0,100,100,partyallArray[animFrames[8]]);
    animFrames[8]++;
    if (animFrames[8] > 27) {animFrames[8] = 0;}
    delay(100);
    u8g2.sendBuffer();
  }

  positionCount = 4;
  selectedRow = 0;
  selectedCol = 0;
  side = 0;
  blockBeingRemoved = -1;
  blockRemovalProgress = 0;
  timerMillis = 0;
  timerMillis = 0;
  timeLimit = 120000 + millis();
  tetraCurrentState = TETRA_LOGO;
}

void removeBlock() {
  bool dontRemove = false;
  int row = 0;
  int col = 0;
  // Bottom Side
  if (side == 0) {
    for (col = 9; col >= 0; --col) {
      if (gameMap[selectedRow][col] != 0) {
        row = selectedRow;
        break;
      }
    }

    if (blockBeingRemoved == -1) { // If no block set is being removed
      blockBeingRemoved = gameMap[row][col];
      blockRemovalProgress = 1;
    }
    else if (gameMap[row][col] != blockBeingRemoved && blockBeingRemoved != -1) { // If selected block isn't the same as the block set being removed, game over
      gameOver();
      dontRemove = true;
    }
    else { // Block is the same, progress 3
      blockRemovalProgress++;
      if (blockRemovalProgress >= 3) {
        blockRemovalProgress = 0;
        blockBeingRemoved = -1;
      }
    }
    if (gameMap[row][col] != -1 && !dontRemove) {
      gameMap[row][col] = 0;
    }
  }

  else if (side == 1) {
    for (row = 9; row >= 0; --row) {
      if (gameMap[row][selectedCol] != 0) {
        col = selectedCol;
        break;
      }
    }

    if (blockBeingRemoved == -1) { // If no block set is being removed
      blockBeingRemoved = gameMap[row][col];
      blockRemovalProgress = 1;
    }
    else if (gameMap[row][col] != blockBeingRemoved && blockBeingRemoved != -1) { // If selected block isn't the same as the block set being removed, game over
      gameOver();
      dontRemove = true;
    }
    else { // Block is the same, progress 3
      blockRemovalProgress++;
      if (blockRemovalProgress >= 3) {
        blockRemovalProgress = 0;
        blockBeingRemoved = -1;
      }
    }
    if (gameMap[row][col] != -1 && !dontRemove) {
      gameMap[row][col] = 0;
    }
  }

  else if (side == 2) {
    for (col = 0; col < 10; ++col) {
      if (gameMap[selectedRow][col] != 0) {
        row = selectedRow;
        break;
      }
    }

    if (blockBeingRemoved == -1) { // If no block set is being removed
      blockBeingRemoved = gameMap[row][col];
      blockRemovalProgress = 1;
    }
    else if (gameMap[row][col] != blockBeingRemoved && blockBeingRemoved != -1) { // If selected block isn't the same as the block set being removed, game over
      gameOver();
      dontRemove = true;
    }
    else { // Block is the same, progress 3
      blockRemovalProgress++;
      if (blockRemovalProgress >= 3) {
        blockRemovalProgress = 0;
        blockBeingRemoved = -1;
      }
    }
    if (gameMap[row][col] != -1 && !dontRemove) {
      gameMap[row][col] = 0;
    }
  }

  else if (side == 3) {
    for (row = 0; row < 10; ++row) {
      if (gameMap[row][selectedCol] != 0) {
        col = selectedCol;
        break;
      }
    }

    if (blockBeingRemoved == -1) { // If no block set is being removed
      blockBeingRemoved = gameMap[row][col];
      blockRemovalProgress = 1;
    }
    else if (gameMap[row][col] != blockBeingRemoved && blockBeingRemoved != -1) { // If selected block isn't the same as the block set being removed, game over
      gameOver();
      dontRemove = true;
    }
    else { // Block is the same, progress 3
      blockRemovalProgress++;
      if (blockRemovalProgress >= 3) {
        blockRemovalProgress = 0;
        blockBeingRemoved = -1;
      }
    }
    if (gameMap[row][col] != -1 && !dontRemove) {
      gameMap[row][col] = 0;
    }
  }


}

void updateTetraInputs() {
  switch (tetraCurrentState) {
    case TETRA_LOGO:
      // Update the Bounce objects
      aButton.update();
      bButton.update();
      if (aButton.fell()) {
        tetraRunOnce = true;
        switch (setGameMode) {
          case 1:
            tetraCurrentState = TETRA_MODE1;
            break;
          case 2:
            tetraCurrentState = TETRA_MODE2;
            break;
        }
      }

      if (bButton.fell()) {
        exitTetraApp = true;
      }


      //Refresh Encoder Counts
      sEncoder_change = sEncoder.getCount();
      lEncoder_change = lEncoder.getCount();
      if (sEncoder_change) {
        if ((sEncoder_change < 0)) {
          if (setGameMode == 1) {setGameMode = 2;}
          else {setGameMode = 1;}
        }
        else if (sEncoder_change > 0) {
          if (setGameMode == 1) {setGameMode = 2;}
          else {setGameMode = 1;}
        }
        sEncoder.setCount(0);
      }
      break;

    case TETRA_MODE1:
      // Update the Bounce objects
      aButton.update();
      bButton.update();
      if (aButton.fell()) {
        removeBlock();
      }

      if (bButton.fell()) {
        
      }


      //Refresh Encoder Counts
      sEncoder_change = sEncoder.getCount();
      lEncoder_change = lEncoder.getCount();
      if (sEncoder_change) {
        if ((sEncoder_change > 0)) {
          positionCount++;
          if (positionCount > 39) {positionCount = 0;}
        }
        else if (sEncoder_change < 0) {
          positionCount--;
          if (positionCount < 0) {positionCount = 39;}
        }
        sEncoder.setCount(0);
      }
      break;

    case TETRA_GAMEOVER:
      // Update the Bounce objects
      aButton.update();
      bButton.update();
      if (aButton.fell()) {
        setArrayEqual(gameMap,generatedMap);
        tetraCurrentState = TETRA_MODE1;
        tetraRunOnce = false;
        breakLoop = true;
      }

      if (bButton.fell()) {
        positionCount = 0;
        selectedRow = 0;
        selectedCol = 0;
        side = 0;
        blockBeingRemoved = -1;
        blockRemovalProgress = 0;
        tetraCurrentState = TETRA_LOGO;
        breakLoop = true;
      }
      break;

    default:
      break;
  }
}

void tetraGame() {
  // Game operations
  switch (tetraCurrentState) {
    case TETRA_LOGO: // Logo screen
      // Initial game setup to run once
      if (tetraRunOnce) {
        // Setup cool background with moving blocks
        for (int i = 0; i < 20; i++) {
          backgroundArray[1][i] = random(0,128); // X
          backgroundArray[2][i] = random(5,123); // Y
          backgroundArray[3][i] = random(0,4);   // Block
          backgroundArray[4][i] = random(1,4);   // Speed
        } 

        tetraRunOnce = false;
      }

      // Display cool blocks background
      if ((millis() - tetraRandomMillis) > 25) {
        for (int i = 0; i < 20; i++) {
          tetraRandomMillis = millis();
          backgroundArray[2][i] = backgroundArray[2][i] + backgroundArray[4][i];
          if (backgroundArray[2][i] > 133) {
            backgroundArray[1][i] = random(0,128);
            backgroundArray[2][i] = -5;
            backgroundArray[3][i] = random(0,4);
            backgroundArray[4][i] = random(1,4);
          }
        }
      }
      for (int i = 0; i < 20; i++) {
        u8g2.drawXBMP(backgroundArray[1][i]-5,backgroundArray[2][i]-5,10,10,blockIcons_allArray[backgroundArray[3][i]]);
      }

      // Display logo
      u8g2.setDrawColor(0);
      u8g2.drawBox(0,39,128,42);
      u8g2.setDrawColor(1);
      u8g2.drawXBMP(0,40,128,41,epd_bitmap_tetralogo00);
      
      // Draw Text
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.setCursor(64 -(u8g2.getStrWidth("Press Something!")/2), 100);
      u8g2.print("Press Something!");
      u8g2.setCursor(64 -(u8g2.getStrWidth("Start Mode  ")/2), 120);
      u8g2.print("Start Mode "); u8g2.print(setGameMode);
      break;
    
    case TETRA_MODE1:
      timerMillis = millis();
      if (timeLimit - timerMillis > 0) {
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.setCursor(64 -(u8g2.getStrWidth("0:00")/2), 8);
        u8g2.print(int((timeLimit - timerMillis)/60000));
        u8g2.print(":");
        u8g2.print(int(((timeLimit - timerMillis)/1000) - 60*((timeLimit - timerMillis)/60000)));
      }
      else {
        gameOver();
      }

      // Run setup once
      if (tetraRunOnce) {
        generateMapMode1();
        setArrayEqual(gameMap,generatedMap);
        timerMillis = 0;
        timeLimit = 120000 + millis();
        tetraRunOnce = false;
      }

      // Show the map
      displayMap(gameMap,14,14);

      // Show the player
      if (0<=positionCount && positionCount<=9) {
        u8g2.drawXBMP(14+(10*positionCount),115,10,10,playerIcon_allArray[0]);
        u8g2.drawXBMP(15+(10*positionCount),15,1,100,dottedLine0);
        u8g2.drawXBMP(22+(10*positionCount),15,1,100,dottedLine0);
        selectedRow = positionCount;
        selectedCol = -1;
        side = 0;  
      }
      else if (10<=positionCount && positionCount<=19) {
        u8g2.drawXBMP(115,114-(10*(positionCount-9)),10,10,playerIcon_allArray[1]);
        u8g2.drawXBMP(15,115-(10*(positionCount-9)),100,1,dottedLine1);
        u8g2.drawXBMP(15,122-(10*(positionCount-9)),100,1,dottedLine1);
        selectedRow = -1;
        selectedCol = 10-(positionCount-9);
        side = 1;
      }
      else if (20<=positionCount && positionCount<=29) {
        u8g2.drawXBMP(114-(10*(positionCount-19)),2,10,10,playerIcon_allArray[2]);
        u8g2.drawXBMP(115-(10*(positionCount-19)),12,1,100,dottedLine2);
        u8g2.drawXBMP(122-(10*(positionCount-19)),12,1,100,dottedLine2);
        selectedRow = 10-(positionCount-19);
        selectedCol = -1;
        side = 2;
      }
      else if (30<=positionCount && positionCount<=39) {
        u8g2.drawXBMP(4,4+(10*((positionCount-29))),10,10,playerIcon_allArray[3]);
        u8g2.drawXBMP(14,5+(10*((positionCount-29))),100,1,dottedLine3);
        u8g2.drawXBMP(14,12+(10*((positionCount-29))),100,1,dottedLine3);
        selectedRow = -1;
        selectedCol = positionCount-30;
        side = 3;
      }

      if (tetraDebug) {
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.setCursor(0,118);
        u8g2.print("Row ");u8g2.print(selectedRow);u8g2.print(" Col ");u8g2.print(selectedCol);u8g2.print(" Side ");u8g2.print(side);
      }

      // Removal progress indicator
      if (blockBeingRemoved != -1) {
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.setCursor(0,8);
        u8g2.print(blockRemovalProgress);
        u8g2.drawXBMP(6,0,10,10,blockIcons_allArray[blockBeingRemoved]);
      }

      // Check for Win
      if (countZeros(gameMap) == saveNumSpaces) {
        gameWin();
      }

      break;

    default:
      break;
  }
  
}

void tetraGameLoop() {
  tetraGame();
  updateTetraInputs();
  updateAnims();
  displayFrame();
}