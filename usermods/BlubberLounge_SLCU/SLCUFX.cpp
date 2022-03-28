#include "SLCUFX.h"

void SLCUFX::Laser::setIsOn(bool isOn)
{
    this->isOn = isOn;
    this->isHigh = this->reverse ? !isOn : isOn;
}

// default effect: no effect just static laser, manuel mode
void SLCUFX::fx_default() {
    // do nothing.
    return;
}

// test effect: test effects here before finally giving them a name and seperate method
void SLCUFX::fx_test()
{
    if ((millis() - fx_lastTime) > fx_speed) 
    {
    fx_lastTime = millis();
    curLaser = getNextLaser(curLaser);
    
    laserList[getPrevLaser(curLaser)].setIsOn(false);
    laserList[curLaser].setIsOn(true);


    // updates laser
    this->fx_update();
    }
}

void SLCUFX::fx_update() 
{
    for (uint8_t i=0; i < this->totalLaser; i++)
    {
    digitalWrite(laserList[i].pin, laserList[i].isHigh);
    }
}

/**
 * utility method
 * turn off all lasers interally and phisically 
 * 
 */
void SLCUFX::fx_clear() 
{
    for (uint8_t i=0; i < SIZEP(laserList); i++)
    {
    laserList[i].setIsOn(false);
    digitalWrite(laserList[i].pin, 0);
    }
}