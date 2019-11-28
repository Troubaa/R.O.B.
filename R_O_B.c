//
// Created by Noah on 2019-11-26.
//

#include "competition.h"
#include "R_O_B.h"


//functions
void generator();
void movement(int time);
void gpsmovement(int time);
void search(int time);
void sensors();
void shields();
void weapons();

typedef enum{
    false, true, init
} bool;

//setupROB - Robot setup
void setupROB(void){                                                    //setupROB
    AddSensor(0, SENSOR_RADAR, 0, 10, 100);             //RIGHT SENSOR      0
    AddSensor(1, SENSOR_RADAR, 350, 10, 100);           //LEFT SENSOR       1
    AddSensor(2, SENSOR_RANGE, 0, 0, 0);                //FRONT RANGE       2
    AddSensor(3, SENSOR_RANGE, 120, 0, 0);              //REAR RANGE        3
}//setupROB end------------------------------------------------------------------|

//ROB_AI - Robot Actions
void ROB_AI(int time){                                              //ROB_AI
    movement(time);
}//ROB_AI end--------------------------------------------------------------------|

void generator(){                                                     //generator

}//end generator-----------------------------------------------------------------|

void movement(int time){                                                       //movement
    int isSearch = true;

    if (isSearch == true){            //if search
        search(time);
    }//if search

}//end movement-----------------------------------------------------------------|

void search(int time){                                                          //search
    GPS_INFO gps;                                     //structure of gps values             x, y, heading
    int rand;                                         //random number generator
    int timeHold;                                 //time at certain point grabbed
    int rRadar, lRadar, fRange, bRange;               //values for the radars
    int isTRight;                              //if turning right
    int isTLeft;                              //if turning Left

    rRadar = GetSensorData(0);
    lRadar = GetSensorData(1);
    fRange = GetSensorData(2);
    bRange = GetSensorData(3);

    //initialize values
    if(time < 100)
        timeHold = false;

    //if front sensor reads something and isnt holding a time value turn right
    if (fRange > 1  && bRange == 125){
        timeHold = time;
        isTRight = true;
        SetStatusMessage("RIGHT TURN");
    }//if

    //turn right for predetermined time
    if(isTRight == true){
        SetMotorSpeeds(100,70);
        if (time >= (timeHold + 5)) {
            isTRight = false;
            timeHold =0;
        }//if
        SetStatusMessage("HOLDING RIGHT TURN");
    }//if

    //if front sensor reads something and isnt holding a time value turn right
    if (bRange > 1  && fRange == 125){
        timeHold = time;
        isTLeft = true;
        SetStatusMessage("LEFT TURN");
    }//if

    //turn right for predetermined time
    if(isTLeft == true){
        SetMotorSpeeds(70,100);
        if (time >= (timeHold + 30)) {
            isTLeft = false;
            timeHold = 0;
        }//if
        SetStatusMessage("HOLDING LEFT TURN");
    }//if

    //sensors read nothing = randomly move around
    if(fRange == 125 && bRange == 125 && timeHold == false){
        rand = GetRandomNumber(1);
        switch(rand) {
            case 0:
                SetMotorSpeeds(70,100);
                break;
            case 1:
                SetMotorSpeeds(100,70);
                break;
        }//switch
    }//if
}//end search-------------------------------------------------------------------|

void gpsmovement(int time){                                         //gpsmovement

}//end gpsmovement--------------------------------------------------------------|

void sensors(){                                                        //sensors

}//end sensors------------------------------------------------------------------|

void shields(){                                                        //shields

}//end shields------------------------------------------------------------------|

void weapons(){                                                        //weapons

}//end weapons------------------------------------------------------------------|