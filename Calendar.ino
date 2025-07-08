////                                  ////
///       Calendar Application       ///
//                                  //
//  Notes:
//    - Reading/Writing from flash is broken
//      somewhere, not sure what problem is.
//      Should be updated to work with SD.

int          dayToday        = 12;
int          monthToday      = 3;
int          yearToday       = 2024;
volatile int selectedDay     = dayToday;
volatile int selectedMonth   = monthToday;
volatile int selectedYear    = yearToday;
enum CalendarAppState {HOME, DAY_VIEW, ADD_EVENT};
CalendarAppState calendarCurrentState = HOME;

bool testOnce = false;

void updateCalendarInputs() {
  aButton.update();
  bButton.update();  
  int sEncoder_change = sEncoder.getCount();
  int lEncoder_change = lEncoder.getCount();

  switch (calendarCurrentState) {
    case HOME:
      if (sEncoder_change) {
        if ((sEncoder_change < 0)) {
          selectedDay++;
          if (selectedDay > daysInMonth(selectedMonth, selectedYear)) {
            selectedDay = 1;
            selectedMonth++;
            animFrames[0] = 1;
            if (selectedMonth > 12) {
              selectedMonth = 1;
              selectedYear++;
            }
          }
        }
        else if (sEncoder_change > 0) {
          selectedDay--;
          if (selectedDay < 1) {
            animFrames[0] = 1;
            if (selectedMonth == 1) {
              selectedMonth = 12;
              selectedYear--;
            }
            else {
              selectedMonth--;
            }
            selectedDay = daysInMonth(selectedMonth, selectedYear);
          }
        }
        sEncoder.setCount(0);
      }
      if (lEncoder_change) {
        if (lEncoder_change < 0) {
          selectedMonth++;
          if (selectedMonth > 12) {
            selectedMonth = 1;
            selectedYear++;
          }
        }
        if (lEncoder_change > 0) {
          if (selectedMonth == 1) {
              selectedMonth = 12;
              selectedYear--;
          }
          else {
            selectedMonth--;
          }
        }
        if (selectedDay > daysInMonth(selectedMonth, selectedYear)) {
          selectedDay = daysInMonth(selectedMonth, selectedYear);
        }
        animFrames[0] = 1;
      }

      if (aButton.fell()) {
        calendarCurrentState = DAY_VIEW;
      }

      if (bButton.fell()) {
        exitCalendarApp = true;
        break;
      }
      break;

    case DAY_VIEW:
      if (aButton.fell()) { 
      }
      if (bButton.fell()) {
        calendarCurrentState = HOME;
        delay(250);
      }
      break;
  }
  
}

int getDayFromUnixTime(unsigned long unixTime) {
  return (unixTime / 86400L) % 31 + 1; // Assuming every month has 31 days
}

int getMonthFromUnixTime(unsigned long unixTime) {
  return ((unixTime / 2629743L) % 12) + 1; // Approximate calculation assuming a 30-day month
}

int getYearFromUnixTime(unsigned long unixTime) {
  return (unixTime / 31556926L) + 1970; // Approximate calculation assuming 365.25 days per year
}

void addEvent(unsigned long time, const String& name) {
  // Find the first available slot in the array
  for (int i = 0; i < MAX_CALENDAR_EVENTS; ++i) {
    if (calendarEvents[i].unixTime == 0) {
      // Found an empty slot, add the event
      calendarEvents[i].unixTime = time;
      calendarEvents[i].eventName = name;
      break;
    }
  }
  saveCalendarEventsToFile();
}

std::vector<int> findEventsOnDay(int day, int month, int year) {
  // Create a vector to store indices of events
  std::vector<int> eventIndices;

  // Iterate through events list
  for (int i = 0; i < MAX_CALENDAR_EVENTS; ++i) {
    // Extract day, month, and year from event's unix time
    int eventDay = getDayFromUnixTime(calendarEvents[i].unixTime);
    int eventMonth = getMonthFromUnixTime(calendarEvents[i].unixTime);
    int eventYear = getYearFromUnixTime(calendarEvents[i].unixTime);

    // Check if event occurs on the specified day, month, and year
    if (eventDay == day && eventMonth == month && eventYear == year) {
      // Add index of the event to the vector
      eventIndices.push_back(i);
    }
  }

  // Return the vector containing indices of events
  return eventIndices;
}

void saveCalendarEventsToFile() {
  File calendarEventsFile = SPIFFS.open("/events.txt", FILE_WRITE);
  if (!calendarEventsFile) {
    //Serial.println("Failed to open calendar events file for writing");
    return;
  }

  // Write each calendar event to file
  for (int i = 0; i < MAX_CALENDAR_EVENTS; i++) {
    calendarEventsFile.println(calendarEvents[i].unixTime);
    calendarEventsFile.println(calendarEvents[i].eventName);
  }

  calendarEventsFile.close();
  //Serial.println("Calendar events saved to file");
}

void loadCalendarEventsFromFile() {
  File calendarEventsFile = SPIFFS.open("/events.txt", FILE_READ);
  if (!calendarEventsFile) {
    //Serial.println("Failed to open calendar events file for reading");
    return;
  }

  // Read each calendar event from file
  for (int i = 0; i < MAX_CALENDAR_EVENTS; i++) {
    if (calendarEventsFile.available()) {
      calendarEvents[i].unixTime = calendarEventsFile.readStringUntil('\n').toInt();
      calendarEvents[i].eventName = calendarEventsFile.readStringUntil('\n');
    }
  }

  calendarEventsFile.close();
  //Serial.println("Calendar events loaded from file");
}

int zellersCongruence(int day, int month, int year) {
  String dayOfWeekString;
  int iso = 0;

  if (month < 3) {
    month += 12;
    year--;
  }
  int h = (day + 13 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
  iso = (h + 6) % 7;// + 1; //0 is sunday, 1 is monday, ... ,  6 is saturday

  return iso;
}

int daysInMonth(int month, int year) {
  if (month < 1 || month > 12) {
    // Invalid month
    return -1;
  }

  switch (month) {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
      return 31;
    case 4: case 6: case 9: case 11:
      return 30;
    case 2:
      // Check for leap year
      if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 29; // Leap year
      } else {
        return 28; // Non-leap year
      }
    default:
      return -1; // Invalid month
  }
}

String getMonthName(int month) {
  static const char* monthNames[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

  if (month < 1 || month > 12) {
    return "Invalid Month";
  }

  return String(monthNames[month - 1]);
}

void drawMonth () {
  int i = zellersCongruence(1, selectedMonth, selectedYear);
  int daysInSelectedMonth = daysInMonth(selectedMonth, selectedYear);
  int j = 0;
  int k = 1;

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(4, 10, (getMonthName(selectedMonth) + " " + String(selectedYear)).c_str());

  while (k <= daysInSelectedMonth) { // Draw the month day by day
    u8g2.drawStr((i*18)+4,(j*18)+30,String(k).c_str());

    if (k == dayToday && selectedMonth == monthToday && selectedYear == yearToday) {
      u8g2.drawCircle((i*18)+9,(j*18)+28, 9);
    }
    else {
      u8g2.drawRFrame((i*18)+1,(j*18)+20,18,18,5);
    }

    if (k == selectedDay) {
      u8g2.drawHLine((i*18)+14,(j*18)+33,3);
      u8g2.drawVLine((i*18)+14,(j*18)+33,3);
      u8g2.drawLine((i*18)+14,(j*18)+33,(i*18)+14+4,(j*18)+33+4);
    }

    k++;i++;
    if (i > 6) {i = 0; j++;}
  }
}

void loadCalendar() {
  switch (calendarCurrentState) {
    case HOME:
      drawMonth();
      break;
    
    case DAY_VIEW:
      drawMonth();
      u8g2.setDrawColor(0);
      u8g2.drawXBMP(0, 0, 128, 128, fuzzallArray[0]);
      u8g2.drawBox(24,3,78,118);
      u8g2.setDrawColor(1);
      u8g2.drawFrame(23,2,80,120);
      u8g2.drawXBMP(60,-2,25,25,iconsallArray[2]);
      String dayName = "";
      switch (zellersCongruence(selectedDay, selectedMonth, selectedYear)) {
        case 0:
          dayName = "Sunday,";
          break;
        case 1:
          dayName = "Monday,";
          break;
        case 2:
          dayName = "Tuesday,";
          break;
        case 3:
          dayName = "Wednesday,";
          break;
        case 4:
          dayName = "Thursday,";
          break;
        case 5:
          dayName = "Friday,";
          break;
        case 6:
          dayName = "Saturday,";
          break;
      }
      u8g2.drawStr(30,26,dayName.c_str());
      u8g2.drawStr(30,34,(String(selectedMonth) + "/" + String(selectedDay) + "/" + String(selectedYear)).c_str());
      u8g2.drawHLine(26,36,75);
      //Add menu script to load events on day
      std::vector<int> indices = findEventsOnDay(selectedDay, selectedMonth, selectedYear);
      int eventY = 0;
      for (int index : indices) {
        u8g2.drawStr(26, 37+(8*eventY), (calendarEvents[index].eventName).c_str());
        eventY++;
      }
      break;
  } 
}

void calendarLoop() {
  /*if (testOnce) {
    // Add some sample events
    addEvent(1710280728, "Meeting");
    addEvent(1710280800, "Appointment");

    // Save events to file
    saveCalendarEventsToFile();
    testOnce = false;

    std::vector<int> indices = findEventsOnDay(selectedDay, selectedMonth, selectedYear);
    int eventY = 0;
    for (int index : indices) {
      u8g2.drawStr(26, 37+(8*eventY), (calendarEvents[index].eventName).c_str());
      eventY++;
    }
    u8g2.sendBuffer();
    delay(5000);
  }*/

  updateCalendarInputs();
  updateAnims();
  loadCalendar();
  displayFrame();
}
