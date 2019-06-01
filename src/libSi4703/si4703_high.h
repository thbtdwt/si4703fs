#ifndef SI4703_HIGH_H
#define SI4703_HIGH_H

#include <functional>
#include <chrono>

#include "si4703_exception.h"
#include "si4703_registers.h"
#include "si4703_low.h"

class Si4703_high {
public:
    Si4703_high ();
    ~Si4703_high ();

    void set_volume(uint16_t volume);
    uint16_t get_volume();

    void set_channel(float freq);
    float get_channel();

    float seek_up();
    float seek_down();



    void display_registers(bool get=false);

private:

    SI4703_REGISTERS_t si4703_registers;
    Si4703_low si4703_low;

    enum seek_t {
        DOWN = 0,
        UP
    };
    float seek(seek_t direction);

    void set_registers();
    void get_registers(); 
    void wait_for(std::function<bool(void)> f, int wait_ms, int resolution_ms=1);
};

#endif //SI4703_HIGH_H