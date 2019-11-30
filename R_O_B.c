//
// Created by Noah on 2019-11-26.
//

#include "competition.h"
#include "R_O_B.h"


//functions
void generator();
void movement(int time);
void gpsmovement(int time);
void heading(float hWant, GPS_INFO gps);
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
void ROB_AI(int time){                                                   //ROB_AI
    movement(time);

}//ROB_AI end--------------------------------------------------------------------|

void generator(){                                                     //generator

}//end generator-----------------------------------------------------------------|

void movement(int time){                                               //movement
    char i =0;

}//end movement----------------------------------------------------------------|

void search(int time){                                                  //search
    int i;
    float turn;
    int rand;                                         //random number generator
    int rRadar, lRadar, fRange, bRange;               //values for the radars
    int isTRight;                              //if turning right
    int isTLeft;                              //if turning Left

    rRadar = GetSensorData(0);
    lRadar = GetSensorData(1);
    fRange = GetSensorData(2);
    bRange = GetSensorData(3);


    turn = gpsmovement(time);

}//end search-------------------------------------------------------------------|

void gpsmovement(int time){                                         //gpsmovement
    GPS_INFO gps;
    float turn = 0;

    //get gps info
    GetGPSInfo(&gps);

    //inside zone 1 move to heading 135
    if (gps.x >= 275 && gps.y <= 100){
        heading(135, gps);
    }//if

    //inside zone 2 move to heading 90
    if (gps.x >= 100 && gps.x <= 275 && gps.y <= 100){
        heading(90, gps);
    }//if

    //inside zone 3 move to heading 45
    if (gps.x <= 100 && gps.y <= 100){
        heading(45, gps);
    }//if

    //inside zone 4 move to heading 180
    if (gps.x >= 275 && gps.y >= 100 && gps.y <= 275){
        heading(180, gps);
    }//if

    //inside zone 5 do nothing

    //inside zone 6 move to heading 0/360
    if(gps.x <= 100 && gps.y >= 100 && gps.y <=275){
        heading(360, gps);
    }//if

    //inside zone 7 move to heading 225
    if (gps.x >= 275 && gps.y >= 275){
        heading(225, gps);
    }//if

    //inside zone 8 move to heading 270
    if (gps.x >= 100 && gps.x <= 275 && gps.y >= 275){
        heading(270, gps);
    }//if

    //inside zone 9 move to heading 315
    if (gps.x <= 100 && gps.y >= 275){
        heading(315, gps);
    }//if
}//end gpsmovement-------------------------------------------------------------|

//heading - this function will grab the wanted heading
//          to turn towards and turn the robot to the wanted heading
void heading(float hWant, GPS_INFO gps){                                  //heading
    float head, gHead;

    head = hWant - gps.heading;
    //turn left by head
    if(head >= 0 && head <=180){
        SetMotorSpeeds(100,0);
    }//if
    //turn right by 360 - head
    if(head > 180){
        SetMotorSpeeds(0,100);
    }//if
    //turn right by head
    if(head < 0 && head >= -180){
        SetMotorSpeeds(0,100);
    }//if
    //turn left by 360 + head
    if(head <= -180){
        SetMotorSpeeds(100,0);
    }//if
}//end heading-----------------------------------------------------------------|

void sensors(){                                                        //sensors

}//end sensors------------------------------------------------------------------|

void shields(){                                                        //shields

}//end shields------------------------------------------------------------------|

void weapons(){                                                        //weapons

}//end weapons------------------------------------------------------------------|