#include "pin_manager.h"

#define FX_SLCU_COUNT 2

#define FX_SLCU_DEFAULT   0
#define FX_SLCU_AROUND    1


class SLCUFX
{
    typedef uint16_t (SLCUFX::*mode_ptr)(void);

    static SLCUFX* instance;

    public:
        typedef struct Laser {
            uint8_t id;
            uint8_t pin = -1;
            bool isOn = false;    // member for internal computations
            bool isHigh = false;  // physical pin output
            bool reverse = true;  // reverse pin output

            /* setter & getter */
            void setIsOn(bool isOn);
            void setPin(uint8_t pin) 
            {
                if (pinManager.allocatePin(pin, true, PinOwner::UM_BLUBBERLOUNGE_SLCU)) 
                {
                    this->pin = pin;
                    pinMode(this->pin, true);
                    digitalWrite(this->pin, this->isOn);
                } else {
                    this->pin = -1;
                }
            };


            /* constructor */
            Laser()
            {
                Laser(-1);
            }

            Laser(uint8_t pin)
            {
            Laser(pin, true);
            }
            
            Laser(uint8_t pin, bool reverse)
            {
                this->setPin(pin);
                this->reverse = reverse;
            }
        } laser;


        /* constructor */
        SLCUFX() {
            SLCUFX::instance = this;

            mode[FX_SLCU_DEFAULT] = &SLCUFX::fx_default;
        }
        
        /* FX */
        uint16_t
            fx_default(void),
            fx_test(void),
            fx_arround(void),
        /* FX utillity / helper */
            fx_update(void),
            fx_clear(void);

    private:
        mode_ptr mode[FX_SLCU_COUNT];
        
        /* utillity methods */
        static uint8_t getNextLaser(uint8_t id, uint8_t totalLaser) 
        {
            return (++id % (totalLaser+1));
        }

        static uint8_t getPrevLaser(uint8_t id, uint8_t totalLaser) 
        {
            return(--id % (totalLaser+1));
        }
};