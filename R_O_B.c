//
// Created by Noah Sweetnam and Lorenzo Vignuzzi on 2019-11-26.
//

#include "competition.h"
#include "R_O_B.h"

#define STARTTIMER 30           //Sets how long you are in the start mode
#define ATTACKFOLLOWDIST 75     //Sets the distance which we follow targeted robot in attack mode

//mode - for which mode we want to be in
typedef enum{
    Defence, Attack, Search, Start, Hit, Target
} mode;

//bool - for true of false
typedef enum{
    false, true
} bool;

//functions
void determine(mode* isMode, int timer, int bump);                  //This function determines which mode we will go into
void generator (mode isMode);                                       //This function sets what the power priorities, charge rates and changes which sensors are on
void movement (int timer, mode* isMode, GPS_INFO gps, int bump);    //This function changes movement patterns based on the mode
int location (GPS_INFO gps);                                        //This function returns what zone in the map we are in based on gps scan
int heading(GPS_INFO gps);                                          //This function returns returns the heading we want to move to based on what zone we are in
void weapons();                                                     //This function will fire weapons based on sensor readings
bool IsOnTrack(int dest_head, int curr_head);                       //This function will move the robot to the heading we want based on previous heading

//setupROB - This function will setup the sensors on our robot
void setupROB(void){                                                    //setupROB
    AddSensor(0, SENSOR_RADAR, 75, 45, 100);             //Right SENSOR       0
    AddSensor(1, SENSOR_RADAR, 350, 20, 100);            //Front SENSOR       1
    AddSensor(2, SENSOR_RANGE, 350, 0, 0);               //Left RANGE         2
    AddSensor(3, SENSOR_RANGE, 10, 0, 0);                //Right RANGE        3
}//setupROB end------------------------------------------------------------------|

//ROB_AI - This function will run all the robots AI systems
void ROB_AI (int time) {//ROB_AI
    mode isMode;                    //this tells what mode we are currently in
    static GPS_INFO gps;            //structure that holds x,y coordinates and our current heading
    static int bump;                //holds values for what out robot has hit
    static int timer;               //Our own made timer

    bump = GetBumpInfo();

    //determine what mode we are in
    determine(&isMode, timer, bump);
    //Generate power
    generator(isMode);
    //Move robot
    movement(timer, &isMode, gps, bump);
    //increase time
    //note: doesnt increases close to real time sec
    timer += 1;
}//ROB_AI end--------------------------------------------------------------------|

//generator - This function sets what the power priorities, charge rates and changes which sensors are on
//isMode - holds values for which mode we are currently in
void generator (mode isMode){                                                     //generator
    bool sysChange;              //System change priorities passes successfully

    //set system priorities based on mode
    SYSTEM priDefSys[4] = {SYSTEM_SHIELDS, SYSTEM_SENSORS, SYSTEM_LASERS, SYSTEM_MISSILES};
    SYSTEM priStartSys[4] = {SYSTEM_SHIELDS, SYSTEM_SENSORS, SYSTEM_MISSILES, SYSTEM_LASERS};
    SYSTEM priAttSys[4] = {SYSTEM_SHIELDS, SYSTEM_MISSILES, SYSTEM_SENSORS, SYSTEM_LASERS};
    SYSTEM priSearchSys[4] = {SYSTEM_SHIELDS, SYSTEM_SENSORS, SYSTEM_MISSILES, SYSTEM_LASERS };

    //if lasers full
    if (GetSystemEnergy(SYSTEM_LASERS) == 50){
        SetSystemChargeRate(SYSTEM_SHIELDS, 600);
        SetSystemChargeRate(SYSTEM_LASERS, 0);
        SetSystemChargeRate(SYSTEM_MISSILES, 600);
        //          SetStatusMessage("Search Mode L");
    }//if
    //if rockets full
    if (GetSystemEnergy(SYSTEM_MISSILES) == 100) {
        SetSystemChargeRate(SYSTEM_SHIELDS, 600);
        SetSystemChargeRate(SYSTEM_LASERS, 500);
        SetSystemChargeRate(SYSTEM_MISSILES, 0);
//            SetStatusMessage("Search Mode R");
    }//if
    //if shields full
    if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000) {
        SetSystemChargeRate(SYSTEM_SHIELDS, 0);
        SetSystemChargeRate(SYSTEM_LASERS, 500);
        SetSystemChargeRate(SYSTEM_MISSILES, 600);
//            SetStatusMessage("Search Mode S");
    }//if
    //if Rockets and lasers full
    if (GetSystemEnergy(SYSTEM_MISSILES) == 100 && GetSystemEnergy(SYSTEM_LASERS) == 50){
        SetSystemChargeRate(SYSTEM_SHIELDS, 600);
        SetSystemChargeRate(SYSTEM_LASERS, 0);
        SetSystemChargeRate(SYSTEM_MISSILES, 0);
//            SetStatusMessage("Search Mode R & L");
    }//if
    //if shields full and rockets full
    if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000 && GetSystemEnergy(SYSTEM_MISSILES) == 100) {
        SetSystemChargeRate(SYSTEM_SHIELDS, 0);
        SetSystemChargeRate(SYSTEM_LASERS, 500);
        SetSystemChargeRate(SYSTEM_MISSILES, 0);
//            SetStatusMessage("Search Mode S & R");
    }//if
    //if shields full and laser full
    if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000 && GetSystemEnergy(SYSTEM_LASERS) == 50) {
        SetSystemChargeRate(SYSTEM_SHIELDS, 0);
        SetSystemChargeRate(SYSTEM_LASERS, 0);
        SetSystemChargeRate(SYSTEM_MISSILES, 600);
//            SetStatusMessage("Search Mode S & L");
    }//if
    //if none
    if (GetSystemEnergy(SYSTEM_SHIELDS) != 1000 && GetSystemEnergy(SYSTEM_LASERS) != 50 && GetSystemEnergy(SYSTEM_MISSILES) != 100) {
        SetSystemChargeRate(SYSTEM_SHIELDS, 500);
        SetSystemChargeRate(SYSTEM_LASERS, 400);
        SetSystemChargeRate(SYSTEM_MISSILES, 500);
//            SetStatusMessage("Search Mode none");
    }//if
    if (GetSystemEnergy(SYSTEM_SHIELDS) == 1000 && GetSystemEnergy(SYSTEM_LASERS) == 50 && GetSystemEnergy(SYSTEM_MISSILES) == 100) {
        SetSystemChargeRate(SYSTEM_SHIELDS, 0);
        SetSystemChargeRate(SYSTEM_LASERS, 0);
        SetSystemChargeRate(SYSTEM_MISSILES, 0);
//            SetStatusMessage("Search Mode all");
    }//if

    //if in Start mode
    if(isMode == Start) {
        //update which sensors are on and off
        SetSensorStatus(0,1);
        SetSensorStatus(1,0);
        SetSensorStatus(2,0);
        SetSensorStatus(3,0);
        sysChange = SetSystemChargePriorites(priStartSys);
    }//if

    //if in Defense mode
    if(isMode == Defence) {
        //update which sensors are on and off
        SetSensorStatus(0,0);
        SetSensorStatus(1,1);
        SetSensorStatus(2,1);
        SetSensorStatus(3,1);
        sysChange = SetSystemChargePriorites(priDefSys);
    }//if

    //if in Attack mode
    if(isMode == Attack) {
        //update which sensors are on and off
        SetSensorStatus(0,0);
        SetSensorStatus(1,1);
        SetSensorStatus(2,1);
        SetSensorStatus(3,1);
        sysChange = SetSystemChargePriorites(priAttSys);
    }//if

    //if in Search mode
    if(isMode == Search) {
        //update which sensors are on and off
        SetSensorStatus(0,1);
        SetSensorStatus(1,1);
        SetSensorStatus(2,0);
        SetSensorStatus(3,0);
        sysChange = SetSystemChargePriorites(priSearchSys);
    }//if

}//end generator-----------------------------------------------------------------|

//determine - This function determines which mode we will go into
//isMode - which mode the robot is in
//timer - holds around real time in sec
//bump - holds the values for what the robot bumped into
void determine (mode* isMode, int timer, int bump){
    bool rRadar, lRadar, fRange, bRange;               //if radar reads a value
    static bool isSearch = false;                      //true if we are searching
    static int searchTime = 0;                         //counter for how long to search
    static int HitTurnTime;                            //counter for how long to turn when hit
    bool GotBumped;                                    //true if we hit something

    //get sensor data to determine what state we are in
    rRadar = GetSensorData(0);
    lRadar = GetSensorData(1);

    //if start of match charge shields before searching
    if (timer <= STARTTIMER) {
        *isMode = Start;
    }//if

    //get out of start
    if (timer >= STARTTIMER && *isMode == Start) {
        *isMode = Search;
    }//if

    //if target on the right
    if ((GetSensorData(0) == 1 || isSearch == true)) {
        *isMode = Target;
        searchTime++;
        isSearch = true;
        if (searchTime >= 5)
        {
            isSearch = false;
            searchTime = 0;
        }//if
    }//if target

    //if we got hit by lasers, missles or another robot
    if(bump == 0x04 || bump == 0x08 || bump == 0x02) {
        GotBumped = true;
    } else
        GotBumped = false;
    //if we want to Search
    if (lRadar == false && *isMode != Target && *isMode != Hit) {
        //if hit in search mode turn around to find them
        if (GotBumped == true) {
            *isMode = Hit;
            HitTurnTime = timer;
            HitTurnTime += 10;
        } else if (HitTurnTime > timer)
            *isMode = Hit;
        else
            *isMode = Search;
    }//else
    //if we want to attack
    if (lRadar == true) {
        *isMode = Attack;

        //if target triggered reset values
        isSearch = false;
        searchTime = 0;
    }//if
}//end determine----------------------------------------------------------------|

//This function changes movement patterns based on the mode
//isMode - which mode the robot is in
//timer - holds around real time in sec
//gps - this holds the coordinates for our robot
//bump - holds the values for what the robot bumped into
void movement(int timer, mode* isMode, GPS_INFO gps, int bump){                                               //movement
    static bool isBumpWall = false;         //if we hit a wall
    static bool isInc = true;               //if our value needs to increase
    static int mid = 0;                     //number determines what to subtract motor speeds by
    static int searchM1 = 50;               //search motor 1
    static int searchM2 = 100;              //search motor 2
    bool cant_turn;                         //if our robot doesnt have enoughb power to remove

    //if start
    if(*isMode == Start) {
        SetMotorSpeeds(-100,100);
    }

    //if search
    if(*isMode == Search) { // search mode
        //drive if wall is not hit
        if (isInc == true) {
            if (mid != 50) {
                searchM1++;
                searchM2--;
                mid++;
            }
            else{
                isInc = false;
            }
        }
        if (isInc == false) {
            if (mid != 0) {
                searchM1--;
                searchM2++;
                mid--;
            }
            else{
                isInc = true;
            }
        }
        SetMotorSpeeds(searchM1, searchM2);

        //if hit wall
        if(bump == 0x01)
            isBumpWall = true;


        //bumpwall
        if (isBumpWall == true){
            cant_turn = GetGPSInfo(&gps);
            //no energy to turn
            if (cant_turn == false)
                isBumpWall = false;
            if(IsOnTrack(heading(gps),gps.heading))
                isBumpWall = false;
            else
                SetMotorSpeeds(-100, 100);
        }//bumpwall

    }//if search

    //if target
    if(*isMode == Target) {
        SetMotorSpeeds(100, -100);
    }//if target

    //if attack
    if(*isMode == Attack) {
        char string[20];
        sprintf(string, "%d %d", 100 * GetSensorData(2)/ GetSensorData(3), 100 * GetSensorData(3)/ GetSensorData(2));
        SetStatusMessage(string);
        if (GetSensorData(2) > ATTACKFOLLOWDIST && GetSensorData(3) > ATTACKFOLLOWDIST) {
            SetMotorSpeeds(100, 100);
        }//if
        else
            SetMotorSpeeds( 100 * GetSensorData(2)/ GetSensorData(3),   100 * GetSensorData(3)/ GetSensorData(2));
        if(GetSensorData(2) < ATTACKFOLLOWDIST && GetSensorData(3) < ATTACKFOLLOWDIST) {
            SetMotorSpeeds(-100, -100);
        }//if
        else
            SetMotorSpeeds( 100 * GetSensorData(2)/ GetSensorData(3),   100 * GetSensorData(3)/ GetSensorData(2));
    //fire weapons
        weapons();
    }//if attack

    //if defense
    if(*isMode == Defence) {
        if (GetSensorData(0) == 0 && GetSensorData(1) == 0 && IsTurboOn() == 0 && GetSensorData(2) == 125) {
            SetMotorSpeeds(100, 100);
            TurboBoost();
        } else {
            SetMotorSpeeds(100, -100);
        }//if defense
    }//if defense

    if(*isMode == Hit) {
        SetMotorSpeeds(-100,100);
    }//if hit

}//end movement----------------------------------------------------------------|

//weapons - when called, fires missiles and/or lasers under
//          the proper conditions
void weapons(){//weapons
    int fRadar, lRange, rRange;               //values for the radars
    int genStruct;                            //simplifies process of checking generator structure

    //get sensor data to determine what state we are in
    fRadar = GetSensorData(1);
    lRange = GetSensorData(2);
    rRange = GetSensorData(3);
    genStruct = GetGeneratorStructure();
    //genStruct = GetGeneratorStructure();

    //fire rockets when charge is full and on target
    if (fRadar == 1 && lRange != 125 && rRange != 125 && GetSystemEnergy(SYSTEM_MISSILES) == 100)
    {
        FireWeapon(WEAPON_MISSILE, 0);
    }//fire rockets

    //fire lasers when charge is full and on target
    if (fRadar == 1 && lRange != 125 && rRange != 125 && GetSystemEnergy(SYSTEM_LASERS) == 50)
    {
        FireWeapon(WEAPON_LASER, 0);
    }//fire lasers

    //damaged robot fire less powerful
    if (genStruct <= 300 && fRadar == 1 && lRange != 125 && rRange != 125 && GetSystemEnergy(SYSTEM_LASERS) == 25)
    {
        FireWeapon(WEAPON_LASER, 0);
    }//fire lasers
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
//location - this function finds the current
//           quadrant of the robot
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

    return 5; //centre zone if nothing else returns
}//end location-------------------------------------------------------------|

//heading - this function will grab the wanted search heading
//          to turn towards, depending on the map quadrant it's in.
int heading(GPS_INFO gps){
    if (location(gps) == 1) {
        return 135;
    } else if(location(gps) == 2) {
        return 90;
    } else if(location(gps) == 3) {
        return 45;
    } else if(location(gps) == 4) {
        return 180;
    } else if(location(gps) == 6) {
        return 0;
    } else if(location(gps) == 7) {
        return 225;
    } else if(location(gps) == 8) {
        return 270;
    } else if(location(gps) == 9) {
        return 315;
    } else
        return -1; //if in centre quadrant, does not need to turn
}//end heading-----------------------------------------------------------------|

//IsOnTrack: checks if robot is on heading during search
bool IsOnTrack(int dest_head, int curr_head) {
if (dest_head - curr_head >= -20 && dest_head - curr_head <= 20) // if heading is +- 20 of intended heading
    return true;
else
    return false;
}//end IsOnTrack---------------------------------------------------------------|
