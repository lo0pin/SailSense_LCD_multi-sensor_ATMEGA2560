#include testfile.h

void setup(){
  systemInit();
}

void loop(){
  updateButtons();
  updateSensors();
  updateNavigation();
  updateMenuSystem();
  renderDisplay();
  handleAlarms();
}
