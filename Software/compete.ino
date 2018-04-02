//             __  _________   ___  _______  
//            /  |/  / ____/  |__ \<  / __ \ 
//           / /|_/ / __/     __/ // / / / / 
//          / /  / / /___    / __// / /_/ /  
//        _///__/_/_____/   /____/_/\___//__ 
//       /_  __/__  ____ _____ ___     / __ \
//        / / / _ \/ __ `/ __ `__ \   / / / /
//       / / /  __/ /_/ / / / / / /  / /_/ / 
//      /_/  \___/\__,_/_/ /_/ /_/   \____/  
//
//            Don't be first. Be zeroth.
//       (c) Kyle, Shalini, Christopher, Joe

#include "DriveMotor.hpp"
#include "LaunchTube.hpp"
#include "TapeSensor.hpp"
#include "TipperMotor.hpp"

static LaunchTube upperTube(6, 11, 1); //solenoid pin, switch pin, initial ball count
static LaunchTube lowerTube(7, 12, 3);

static DriveMotor xAxisMotor(2, 3, 5, true, 16); //out1 pin, out2 pin, en pin, direction, switch pin
static DriveMotor yAxisMotor(0, 1, 4, false, 15, 17);

static TipperMotor tipper(8, 9, 10, false);

static TapeSensor tapeSensor(14, 15000); //phototransistor pin, minimum microsec over a line
static const uint32_t GATE_SENSOR_PIN = 18;

static uint32_t startMillis = 0;

static int greyCount = 0;
static int greenCount = 0;

void setup() {
  // Init serial object.
  Serial.begin(0); // Baud rate ignored. Teensy USB is always standard 12 Mbit/sec
  analogWriteResolution(10); // Real resolution at this frequency.
  pinMode(GATE_SENSOR_PIN, INPUT);
  Serial.println("Booted");

  yAxisMotor.notifyOnNextNegativeLimitPress([]() {
    startMillis = millis();     // Stop play at 2:10
    primaryPass();
    Serial.println("Started");
  });
}

static void primaryPass() {
  xAxisMotor.timedMove(-1023, 0.4*1000000, []() {
    yAxisMotor.moveToLimit(400, []() {  //go @ 500 power until we hit wall track switch
      tipper.down();
      xAxisMotor.move(-420);
      yAxisMotor.trackPositiveLimit(400);
      greyCount = 0;
      tapeSensor.observeGrey([]() {
        greyCount++;
                  
        if (greyCount == 1) { //hit first grey line
          xAxisMotor.timedMove(0, 0.8*1000000, []() {         
            xAxisMotor.move(-340);
          });
        } else if (greyCount == 2) { //hit second grey line
          xAxisMotor.stop();
          lowerTube.dumpBallsPresent([]() {
            xAxisMotor.move(-370);     
          });          
        } else if (greyCount == 3) {  //hit third grey line
          upperTube.dumpBallsPresent([]() {});
          tapeSensor.stopObserving();
          thirdGreyToGateAttempt();
        }
      });
    });
  });
}

static void thirdGreyToGateAttempt() {
  yAxisMotor.stopTrackingPositiveLimit(); // Stop hugging wall to avoid turning
  xAxisMotor.timedMove(0, 1.5*1000000, []() {
    tipper.up();
    xAxisMotor.move(-400); //not full speed so you don't slam into wall by the gate
    tapeSensor.observeBlack([]() {  //hit black line in front of gate
      tapeSensor.stopObserving();

      xAxisMotor.stop();

      if (gateOpen()) {
        // Sprint until we've won, then slowly drive through to the wall.
        // TODO, secondary gate check later?
        yAxisMotor.move(1023);
        greenCount = 0;
        tapeSensor.observeGreen([]() {
          greenCount++;
          if (greenCount == 2) {
            yAxisMotor.moveToLimit(300, []() {
              // We've won!
              disableAll();
            });
          }
        });
      } else {
        moveToReload();
      }
    });
  });
}

static void moveToReload() {
  // Timed sprint toward South wall
  yAxisMotor.timedMove(-1023, 0.4*1000000, []() {
    // Finish moving South
    yAxisMotor.moveToLimit(-500, []() {
      // Pull away from South wall to reduce friction
      yAxisMotor.timedMove(400, 0.6*1000000, [](){});

      // Timed sprint toward East wall
      xAxisMotor.timedMove(1023, 3.1*1000000, [](){ //~3.5
        // Finish moving East and stay there
        xAxisMotor.moveToLimit(500, []() {
          xAxisMotor.timedMove(1023, 0.05*1000000, [](){}); // back off just a bit

          // Go up and hit the button.
          yAxisMotor.trackPositiveLimit(600);
          // wait for reload, then continue
          lowerTube.notifyOnNextLoad([](){
            yAxisMotor.stopTrackingPositiveLimit();
            xAxisMotor.stopTrackingPositiveLimit();
            secondaryPass();
          });
        });
      });
    });
  });
}

static void secondaryPass() {
  // Reset against round wall
  yAxisMotor.timedMove(-400, 0.3*1000000, [](){});
  xAxisMotor.timedMove(-1023, 1.0*1000000, [](){
    yAxisMotor.moveToLimit(400, [](){
      tipper.down();
      yAxisMotor.trackPositiveLimit(400);
      xAxisMotor.move(-1023);

      greyCount = 0;
      tapeSensor.observeGrey([]() {
        greyCount++;
                  
        if (greyCount == 1) { //hit first grey line
          xAxisMotor.timedMove(0, 0.8*1000000, []() {         
            xAxisMotor.move(-340);
          });
        } else if (greyCount == 2) { //hit second grey line
          xAxisMotor.stop();
          lowerTube.dumpBallsPresent([]() {
            xAxisMotor.move(upperTube.hasBalls() ? -370 : -1023);
          });
        } else if (greyCount == 3) {  //hit third grey line
          if (upperTube.hasBalls()) {
            upperTube.dumpBallsPresent([]() {
              tapeSensor.stopObserving();
              thirdGreyToGateAttempt();
            });
          } else {
            tapeSensor.stopObserving();
            thirdGreyToGateAttempt();
          }
        }
      });
    });
  });
}

static bool gateOpen() {
  Serial.print("Gate says: "); Serial.println(analogRead(GATE_SENSOR_PIN));
  // return false;
  return analogRead(GATE_SENSOR_PIN) > 15;
}

static void disableAll() {
   Serial.println("Disabled");
   upperTube.disable();
   lowerTube.disable();
   tapeSensor.stopObserving();
   xAxisMotor.disable();
   yAxisMotor.disable();
   tipper.disable();
}

void loop() {
  if (startMillis != 0 && millis() > startMillis + (2*60 + 10) * 1000) {
    disableAll();
    startMillis = 0;
  }
  Serial.println("*"); delay(1000); 
}
