////                                  ////
///        Games  Application        ///
//                                  //

// Tetrapolis Game
bool exitTetraApp = false;
bool tetraRunOnce = true;
enum TetraAppState {TETRA_LOGO, TETRA_HOME,TETRA_MODE1, TETRA_MODE2, TETRA_GAMEOVER};
TetraAppState tetraCurrentState = TETRA_LOGO;

void gamesLoop() {
  if (!exitTetraApp) {
    tetraGameLoop();
  }
  else {
    exitGamesApp = true;
  }
}