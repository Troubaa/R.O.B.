//
// Created by Noah on 2019-11-26.
//

#include "competition.h"
#include "R_O_B.h"

typedef enum{
    false, true
} bool;

//functions
void determine(bool* isdef, bool* isAtt, bool* isSearch, bool* isStart, int time);
void generator (bool isdef, bool isAtt, bool isSearch, bool isStart);
void movement (bool time);
void gpsmovement (bool time);
void heading(float hWant, GPS_INFO gps);
void searchbool (bool time);
void sensors();
void shields();
void weapons();

//setupROB - Robot setup
void setupROB(void){                                                    //setupROB
    AddSensor(0, SENSOR_RADAR, 0, 10, 100);             //RIGHT SENSOR      0
    AddSensor(1, SENSOR_RADAR, 350, 10, 100);           //LEFT SENSOR       1
    AddSensor(2, SENSOR_RANGE, 0, 0, 0);                //FRONT RANGE       2
    AddSensor(3, SENSOR_RANGE, 120, 0, 0);              //REAR RANGE        3
}//setupROB end------------------------------------------------------------------|

//ROB_AI - Robot Actions
void ROB_AI (int time) {//ROB_AI
    bool isSearch;           //if in searching mode
    bool isAttack;           //if in attacking mode
    bool isDefense;          //if in defense
    bool isStart;            //if it is the Start of the Game.\

    if (isStart == false)
        isDefense = true;   //for testing

    determine(&isDefense, &isAttack, &isSearch, &isStart, time);
    generator(isDefense, isAttack, isSearch, isStart);
}//ROB_AI end--------------------------------------------------------------------|

void generator (bool isDefense,bool isAttack,bool isSearch,bool isStart){                                                     //generator
    bool sysChange;              //System change priorities

    SYSTEM priDefSys[4] = {SYSTEM_SHIELDS, SYSTEM_SENSORS, SYSTEM_LASERS, SYSTEM_MISSILES};                  //System priorities 0=shields 1=sensors 2=lasers 3=missiles
    SYSTEM priStartSys[2] = {SYSTEM_SHIELDS, SYSTEM_SENSORS};                                                //System priorities 0=shields 1=sensors 2=lasers 3=missiles
    SYSTEM priAttSys[4] = {SYSTEM_MISSILES, SYSTEM_SENSORS, SYSTEM_LASERS, SYSTEM_SHIELDS};                  //System priorities 0=shields 1=sensors 2=lasers 3=missiles
    SYSTEM priSearchSys[4] = {SYSTEM_SHIELDS, SYSTEM_SENSORS, SYSTEM_MISSILES, SYSTEM_LASERS };              //System priorities 0=shields 1=sensors 2=lasers 3=missiles


    //if in Start mode
    if(isStart == true) {
        SetSystemChargeRate(SYSTEM_SHIELDS, 1200);
        SetSystemChargeRate(SYSTEM_LASERS, 175);
        SetSystemChargeRate(SYSTEM_MISSILES, 0);
        sysChange = SetSystemChargePriorites(priStartSys);
        SetStatusMessage("Start Mode");
    }//if

    //if in Defense mode
    if(isDefense == true) {
        SetSystemChargeRate(SYSTEM_SHIELDS, 1000);
        SetSystemChargeRate(SYSTEM_LASERS, 200);
        SetSystemChargeRate(SYSTEM_MISSILES, 200);
        sysChange = SetSystemChargePriorites(priDefSys);
        SetStatusMessage("Defense Mode");
    }//if

    //if in Attack mode
    if(isAttack == true) {
        SetSystemChargeRate(SYSTEM_SHIELDS, 500);
        SetSystemChargeRate(SYSTEM_MISSILES, 500);
        SetSystemChargeRate(SYSTEM_LASERS, 400);
        sysChange = SetSystemChargePriorites(priAttSys);
        SetStatusMessage("Attack Mode");
    }//if

    //if in Search mode
    if(isSearch == true) {
        //if Rockets and lasers full
        if (GetSystemEnergy(SYSTEM_MISSILES) == 100 && GetSystemEnergy(SYSTEM_LASERS) == 50){
            SetSystemChargeRate(SYSTEM_SHIELDS, 1400);
            SetSystemChargeRate(SYSTEM_LASERS, 0);
            SetSystemChargeRate(SYSTEM_MISSILES, 0);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode R & L");
        }//if
        //if lasers full
        if (GetSystemEnergy(SYSTEM_MISSILES) == 100 && GetSystemEnergy(SYSTEM_LASERS) == 50){
            SetSystemChargeRate(SYSTEM_SHIELDS, 1000);
            SetSystemChargeRate(SYSTEM_LASERS, 0);
            SetSystemChargeRate(SYSTEM_MISSILES, 400);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode R & L");
        }//if
        //if rockets full
        if (GetSystemEnergy(SYSTEM_MISSILES) == 100) {
            SetSystemChargeRate(SYSTEM_SHIELDS, 1000);
            SetSystemChargeRate(SYSTEM_LASERS, 400);
            SetSystemChargeRate(SYSTEM_MISSILES, 0);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode R");
        }//if
        //if shields full
        if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000) {
            SetSystemChargeRate(SYSTEM_SHIELDS, 0);
            SetSystemChargeRate(SYSTEM_LASERS, 480);
            SetSystemChargeRate(SYSTEM_MISSILES, 920);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode R");
        }//if
        //if shields full and rockets full
        if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000 && GetSystemEnergy(SYSTEM_MISSILES) == 100) {
            SetSystemChargeRate(SYSTEM_SHIELDS, 0);
            SetSystemChargeRate(SYSTEM_LASERS, 1400);
            SetSystemChargeRate(SYSTEM_MISSILES, 0);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode R");
        }//if

    }//if

}//end generator-----------------------------------------------------------------|

void determine (bool* isDefense,bool* isAttack,bool* isSearch, bool* isStart, int time){
    bool rRadar, lRadar, fRange, bRange;               //values for the radars

    //get sensor data to determine what state we are in
    rRadar = GetSensorData(0);
    lRadar = GetSensorData(1);
    fRange = GetSensorData(2);
    bRange = GetSensorData(3);

    //if start
    if (time <= 250) {
        *isStart = true;
        *isAttack = false;
        *isSearch = false;
        *isDefense = false;
    }//if
    //The end of the start when shields full
    if (*isStart == true) {
        if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000) {
            *isStart = false;
        }//if
    }//is

    //if we want to defend
    if (rRadar == false && lRadar == false && fRange == false && GetSystemEnergy(SYSTEM_SHIELDS) <= 500) {
        *isAttack = false;
        *isSearch = false;
        *isDefense = true;
    }//if

    //if we want to attack
    if (rRadar == true || lRadar == true) {
        *isAttack = true;
        *isSearch = false;
        *isDefense = false;
    }//if

    //if we want to defend
    if (rRadar == false && lRadar == false) {
        *isAttack = false;
        *isSearch = true;
        *isDefense = false;
    }//if

}//end determine----------------------------------------------------------------|

void movement(bool time){                                               //movement
    char i =0;

}//end movement----------------------------------------------------------------|

void search(bool time){                                                  //search


}//end search-------------------------------------------------------------------|


void sensors(){                                                        //sensors

}//end sensors------------------------------------------------------------------|

void shields(){                                                        //shields

}//end shields------------------------------------------------------------------|

void weapons(){                                                        //weapons

}//end weapons------------------------------------------------------------------|


//------------------------FOR FUTURE EDITING------------------------------------|

void gpsmovementbool (bool time){                                         //gpsmovement
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