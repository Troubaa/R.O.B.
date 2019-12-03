//
// Created by Noah Sweetnam and Lorenzo Vignuzzi on 2019-11-26.
//

#include "competition.h"
#include "R_O_B.h"
#include <stdlib.h>

typedef enum{
    false, true
} bool;

//functions
void determine(bool* isdef, bool* isAtt, bool* isSearch, bool* isStart, int timer);
void generator (bool isdef, bool isAtt, bool isSearch, bool isStart);
void movement (int timer, bool isSearch, bool isAttack, bool isDefense, bool isStart, GPS_INFO gps);
int location (GPS_INFO gps);
int heading(GPS_INFO gps);
void searchbool (bool time);
void sensors();
void shields();
void weapons();
bool IsOnTrack(int dest_head, int curr_head);

//setupROB - Robot setup
void setupROB(void){                                                    //setupROB
    AddSensor(0, SENSOR_RADAR, 0, 30, 100);             //RIGHT SENSOR      0
    AddSensor(1, SENSOR_RADAR, 330, 30, 100);           //LEFT SENSOR       1
    AddSensor(2, SENSOR_RANGE, 0, 0, 0);                //FRONT RANGE       2
    AddSensor(3, SENSOR_RANGE, 90, 0, 0);              //REAR RANGE        3
}//setupROB end------------------------------------------------------------------|
//ROB_AI - Robot Actions
void ROB_AI (int time) {//ROB_AI
    static bool isSearch;           //if in searching mode
    static bool isAttack;           //if in attacking mode
    static bool isDefense;          //if in defense
    static bool isStart;            //if it is the Start of the Game.
    static GPS_INFO gps;            //GPS info.

    static int timer;

    determine(&isDefense, &isAttack, &isSearch, &isStart, timer);
    generator(isDefense, isAttack, isSearch, isStart);
    if(isSearch == true)
        GetGPSInfo(&gps);        //Retrieves GPS info
    movement(timer, isSearch, isAttack, isDefense, isStart, gps);
    timer += 1;
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
        //if lasers full
        if (GetSystemEnergy(SYSTEM_LASERS) == 50){
            SetSystemChargeRate(SYSTEM_SHIELDS, 1000);
            SetSystemChargeRate(SYSTEM_LASERS, 0);
            SetSystemChargeRate(SYSTEM_MISSILES, 400);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode L");
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
            SetStatusMessage("Search Mode S");
        }//if
        //if Rockets and lasers full
        if (GetSystemEnergy(SYSTEM_MISSILES) == 100 && GetSystemEnergy(SYSTEM_LASERS) == 50){
            SetSystemChargeRate(SYSTEM_SHIELDS, 1400);
            SetSystemChargeRate(SYSTEM_LASERS, 0);
            SetSystemChargeRate(SYSTEM_MISSILES, 0);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode R & L");
        }//if
        //if shields full and rockets full
        if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000 && GetSystemEnergy(SYSTEM_MISSILES) == 100) {
            SetSystemChargeRate(SYSTEM_SHIELDS, 0);
            SetSystemChargeRate(SYSTEM_LASERS, 1400);
            SetSystemChargeRate(SYSTEM_MISSILES, 0);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode S & R");
        }//if
        //if shields full and laser full
        if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000 && GetSystemEnergy(SYSTEM_LASERS) == 50) {
            SetSystemChargeRate(SYSTEM_SHIELDS, 0);
            SetSystemChargeRate(SYSTEM_LASERS, 0);
            SetSystemChargeRate(SYSTEM_MISSILES, 1400);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode S & L");
        }//if
        //if none
        if (GetSystemEnergy(SYSTEM_SHIELDS) != 1000 && GetSystemEnergy(SYSTEM_LASERS) != 50 && GetSystemEnergy(SYSTEM_MISSILES) != 100) {
            SetSystemChargeRate(SYSTEM_SHIELDS, 1000);
            SetSystemChargeRate(SYSTEM_LASERS, 350);
            SetSystemChargeRate(SYSTEM_MISSILES, 50);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode none");
        }//if
        if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000 && GetSystemEnergy(SYSTEM_LASERS) == 50 && GetSystemEnergy(SYSTEM_MISSILES) == 100) {
            SetSystemChargeRate(SYSTEM_SHIELDS, 1000);
            SetSystemChargeRate(SYSTEM_LASERS, 350);
            SetSystemChargeRate(SYSTEM_MISSILES, 50);
            sysChange = SetSystemChargePriorites(priSearchSys);
            SetStatusMessage("Search Mode all");
        }//if

    }//if

}//end generator-----------------------------------------------------------------|

void determine (bool* isDefense,bool* isAttack,bool* isSearch, bool* isStart, int timer){
    bool rRadar, lRadar, fRange, bRange;               //values for the radars

    //get sensor data to determine what state we are in
    rRadar = GetSensorData(0);
    lRadar = GetSensorData(1);
    fRange = GetSensorData(2);
    bRange = GetSensorData(3);


    //The end of the start when shields full
/*    if (*isStart == true) {
        if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000) {
            *isStart = false;
        }//if
    }//is */
    if(GetSystemEnergy(SYSTEM_SHIELDS) == 1000) {
        *isStart = false;
    }

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

    //if start
    if (timer <= 50 && rRadar == false && lRadar == false) {
        *isStart = true;
        *isAttack = false;
        *isSearch = false;
        *isDefense = false;
    }//if

}//end determine----------------------------------------------------------------|

void movement(int timer, bool isSearch, bool isAttack, bool isDefense, bool isStart, GPS_INFO gps){                                               //movement
    char i =0;

    if(isStart) {
        SetMotorSpeeds(-100, 100);
    }

    if(isSearch) { // search mode
        int currentHeading = (int) gps.heading; //simplicity
   //     if (timer % 3 == 0) {
            if (IsOnTrack(heading(gps), currentHeading) == false) { //check if robot needs to turn
                if (currentHeading - heading(gps) < -180 || (currentHeading - heading(gps) > 0 &&
                                                             currentHeading - heading(gps) <
                                                             180)) { //if robot needs to turn counter clockwise
                    SetMotorSpeeds(-100, 100);
                } else {
                    SetMotorSpeeds(100, -100);
                } //end if
//                SetStatusMessage("I want to turn!");
            } else {
//                SetStatusMessage("No turning for me!");
                SetMotorSpeeds(100, 100);
            }// end if
 //       }//if

    } else if(isAttack) {
        SetStatusMessage("Attack mode!");
        if (GetSensorData(2) > 50 && GetSensorData(2) < 125) {
            SetMotorSpeeds(100, 100);
        } else if(GetSensorData(2) < 50) {
            weapons();
            SetMotorSpeeds(-100, -100);
        } else if(GetSensorData(1) == 1) {
            SetMotorSpeeds(40, 100);
        } else if(GetSensorData(0) == 0) {
            SetMotorSpeeds(100,40);
        } else {
            SetMotorSpeeds(100, 100);
//            SetStatusMessage("No target found...");
        }


    } else if(isDefense) {
        if (GetSensorData(0) == 0 && GetSensorData(1) == 0 && IsTurboOn() == 0 && GetSensorData(2) == 125){
            SetMotorSpeeds(100, 100);
            TurboBoost();
        } else {
                SetMotorSpeeds(100,-100);
            }


    } /*else if(isStart) {
        if(GetSensorData(2) == 125)
            SetMotorSpeeds(100, 100);
        else
            SetMotorSpeeds(100, -100);

    }*/

}//end movement----------------------------------------------------------------|

void search(bool time){
        //search


}//end search-------------------------------------------------------------------|


void sensors(){                                                        //sensors

}//end sensors------------------------------------------------------------------|

void shields(){                                                        //shields

}//end shields------------------------------------------------------------------|

void weapons(){//weapons
    if(GetSystemEnergy(SYSTEM_MISSILES) == 100)
        FireWeapon(WEAPON_MISSILE, 0);

}//end weapons------------------------------------------------------------------|


//------------------------QUADRANT LOCATIONS------------------------------------|
//                         |                                  |                 |
//          9              |                8                 |             7   |
//                         |                                  |                 |
//------------------------------------------------------------------------------|
//                         |                                  |                 |
//          6              |                5                 |             4   |
//                         |                                  |                 |
//------------------------------------------------------------------------------|
//                         |                                  |                 |
//          3              |                2                 |             1   |
//                         |                                  |                 |
//------------------------------------------------------------------------------|
int location(GPS_INFO gps){                                         //location

        //inside zone 1
        if (gps.x >= 275 && gps.y <= 100){

            return 1;
        }//if

        //inside zone 2
        if (gps.x >= 100 && gps.x <= 275 && gps.y <= 100){

            return 2;
    }//if

    //inside zone 3
    if (gps.x <= 100 && gps.y <= 100){

        return 3;
    }//if

    //inside zone 4
    if (gps.x >= 275 && gps.y >= 100 && gps.y <= 275){

        return 4;
    }//if

    //inside zone 5 if no other zone returns at end of function

    //inside zone 6
    if(gps.x <= 100 && gps.y >= 100 && gps.y <=275){

        return 6;
    }//if

    //inside zone 7
    if (gps.x >= 275 && gps.y >= 275){

        return 7;
    }//if

    //inside zone 8
    if (gps.x >= 100 && gps.x <= 275 && gps.y >= 275){

        return 8;
    }//if

    //inside zone 9
    if (gps.x <= 100 && gps.y >= 275){

        return 9;
    }//if


    return 5; //centre zone
}//end location-------------------------------------------------------------|

//heading - this function will grab the wanted search heading
//          to turn towards.
int heading(GPS_INFO gps){//determining search
    if (location(gps) == 1) {
        return 225;
    } else if(location(gps) == 2) {
        return 270;
    } else if(location(gps) == 3) {
        return 315;
    } else if(location(gps) == 4) {
        return 0;
    } else if(location(gps) == 6) {
        return 180;
    } else if(location(gps) == 7) {
        return 135;
    } else if(location(gps) == 8) {
        return 90;
    } else if(location(gps) == 9) {
        return 45;
    } else
        return -1; //if centre quadrant

/*    float head, gHead;

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
    }//if */
}//end heading-----------------------------------------------------------------|

//IsOnTrack: checks if robot is on heading during search
bool IsOnTrack(int dest_head, int curr_head) {
    if (abs(curr_head - dest_head - 180) > 20 || abs(curr_head - dest_head) - 180 < -20 ) {
        if(dest_head > 0)
            return false;
        else
            return true;
    } else {
        return true;
    }//if
}//end IsOnTrack---------------------------------------------------------------|
