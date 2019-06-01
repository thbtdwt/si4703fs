#include <iostream>
#include <fstream>
#include <sstream> 
#include <thread>
#include <chrono>

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/fcntl.h> 
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <errno.h>

#include "si4703_high.h"
#include "si4703_trace.h"


Si4703_high::Si4703_high(): si4703_registers(), si4703_low("/dev/i2c-1", 0x10, 17, 4, 18) {

    get_registers();
    si4703_registers.reg[SI4703_TEST1_INDEX].test1.bits.XOSCEN = 1;
    set_registers();
    std::this_thread::sleep_for (std::chrono::milliseconds(500));

    si4703_registers.reg[SI4703_POWERCFG_INDEX].powercfg.reg = 0;
    si4703_registers.reg[SI4703_POWERCFG_INDEX].powercfg.bits.ENABLE = 1;
    si4703_registers.reg[SI4703_POWERCFG_INDEX].powercfg.bits.DMUTE = 1;
    si4703_registers.reg[SI4703_POWERCFG_INDEX].powercfg.bits.DSMUTE = 1;
    si4703_registers.reg[SI4703_SYSCONFIG1_INDEX].sysconfig1.bits.RDS = 1;
    si4703_registers.reg[SI4703_SYSCONFIG1_INDEX].sysconfig1.bits.DE = 1;
    si4703_registers.reg[SI4703_SYSCONFIG2_INDEX].sysconfig2.bits.SPACE = 1;
    set_registers();
    std::this_thread::sleep_for (std::chrono::milliseconds(150));
    get_registers();
}

Si4703_high::~Si4703_high() {
}


void Si4703_high::set_volume(uint16_t volume) {

    if (volume > 0xF) {
        volume = 0xF;
    }

    get_registers();
    si4703_registers.reg[SI4703_SYSCONFIG2_INDEX].sysconfig2.bits.VOLUME = volume;
    set_registers();
}

uint16_t Si4703_high::get_volume() {

    get_registers();
    return si4703_registers.reg[SI4703_SYSCONFIG2_INDEX].sysconfig2.bits.VOLUME;
}


void Si4703_high::set_channel(float freq) {
    
    uint16_t chan = ((freq - 87.5) / 0.1);

    get_registers();
    si4703_registers.reg[SI4703_CHANNEL_INDEX].channel.bits.CHAN = chan;
    si4703_registers.reg[SI4703_CHANNEL_INDEX].channel.bits.TUNE = 1;
    set_registers();

    wait_for([this]{ 
        get_registers();
        printf("STC %d\n", si4703_registers.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.STC);
        return (si4703_registers.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.STC == 1);
        },60, 5);

    si4703_registers.reg[SI4703_CHANNEL_INDEX].channel.bits.TUNE = 0;
    set_registers();
    
    wait_for([this]{ 
        get_registers();
        printf("STC %d\n", si4703_registers.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.STC);
        return (si4703_registers.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.STC == 0);
        },10);
}

float Si4703_high::get_channel() {
    
    get_registers();
    uint16_t chan = si4703_registers.reg[SI4703_READCHAN_INDEX].readchan.bits.READCHAN;
    return chan * 0.1 + 87.5;
}

float Si4703_high::seek_up() {
    return seek(UP);
}

float Si4703_high::seek_down() {
    return seek(DOWN);
}

void Si4703_high::display_registers(bool get) {
    if (get) {
        get_registers();
    }
    printf("Si4703 registers:\n");
    for (int i=0; i < SI4703_NB_REGS; i++ ){ 
        printf("0x%0x: 0x%04x\n", i, si4703_registers.reg[i].value);
    }
}

/* Private part */

void Si4703_high::set_registers() {
    si4703_low.write_registers(si4703_registers);
}

void Si4703_high::get_registers() {
    si4703_low.read_registers(si4703_registers);
}

void Si4703_high::wait_for(std::function<bool(void)> f, int wait_ms, int resolution_ms) {
    
    printf("=> %d\n", wait_ms/resolution_ms);

    for (int i = 0; i < (wait_ms/resolution_ms); i++) {
        if ( f() ) {
            return;
        }
        std::this_thread::sleep_for (std::chrono::milliseconds(resolution_ms));
    }
    printf("Time out!\n");
}

float Si4703_high::seek(seek_t direction) {
    get_registers();
    si4703_registers.reg[SI4703_POWERCFG_INDEX].powercfg.bits.SKMODE = 0;
    si4703_registers.reg[SI4703_POWERCFG_INDEX].powercfg.bits.SKMODE = direction;
    si4703_registers.reg[SI4703_POWERCFG_INDEX].powercfg.bits.SEEK = 1;
    set_registers();

    wait_for([this]{ 
        get_registers();
        printf("STC %d\n", si4703_registers.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.STC);
        return (si4703_registers.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.STC == 1);
        },5000, 100);

    get_registers();
    bool seek_failed = si4703_registers.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.SFBL;

    si4703_registers.reg[SI4703_POWERCFG_INDEX].powercfg.bits.SEEK = 0;
    set_registers();

    wait_for([this]{ 
        get_registers();
        printf("STC %d\n", si4703_registers.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.STC);
        return (si4703_registers.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.STC == 0);
        },10);

    if (seek_failed)
        return 0;

    return get_channel();
}


/*

template <class Rep, class Period, class Rep2, class Period2>
static void wait_for(const std::chrono::duration<Rep,Period>& wait,
                    const std::chrono::duration<Rep2,Period2>& resolution) {



      printf("%s %s\n", std::to_string(wait.count()).c_str(),
                        std::to_string(resolution.count()).c_str());
  }
*/

//#if 0

int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv;


    Si4703_high si;

    //wait_for(std::chrono::seconds(1),std::chrono::milliseconds(10000));

    //si.display_registers(true);
    si.set_volume(1);
    //si.display_registers(true);
    si.set_channel(102.5);
    //si.display_registers(true);
    printf("Channel %f\n",si.get_channel());

    std::this_thread::sleep_for (std::chrono::seconds(5));

    printf("Seek up, channel %f\n", si.seek_up());
    std::this_thread::sleep_for (std::chrono::seconds(2));
    printf("Seek up, channel %f\n", si.seek_up());
    std::this_thread::sleep_for (std::chrono::seconds(2));
    printf("Seek down, channel %f\n", si.seek_down());

    for (int i=0; i < 10; i++) {
        printf("Seek up, channel %f\n", si.seek_up());
        std::this_thread::sleep_for (std::chrono::seconds(2));
    }

    std::this_thread::sleep_for (std::chrono::seconds(5));

    
    
/*
  
    test_regs.reg[SI4703_POWERCFG_INDEX].powercfg.bits.DSMUTE = 1;
    test_regs.reg[SI4703_CHANNEL_INDEX].channel.bits.CHAN = 0x96;
    test_regs.reg[SI4703_CHANNEL_INDEX].channel.bits.TUNE = 1;

    si.write_registers(test_regs);
    display_registers(test_regs);


    for (int i=0; i < 10; i++) {
        si.read_registers(test_regs);

        if ( test_regs.reg[SI4703_STATUSRSSI_INDEX].statusrssi.bits.STC ) {
            printf("STC ok\n");
            break;
        }


        display_registers(test_regs);
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }

    test_regs.reg[SI4703_CHANNEL_INDEX].channel.bits.TUNE = 0;
    si.write_registers(test_regs);

    std::this_thread::sleep_for (std::chrono::seconds(1));

    si.read_registers(test_regs);
    printf("READCHAN 0%x\n", test_regs.reg[SI4703_READCHAN_INDEX].readchan.bits.READCHAN);





    std::this_thread::sleep_for (std::chrono::seconds(10));



    si.read_registers(test_regs);

    printf("=>\n");
    for (int i=0; i < SI4703_NB_REGS; i++ ){ 
        printf("0x%0x: 0x%04x\n", i, test_regs.reg[i].value);
    }

    printf("=> 0x%04x\n", test_regs.reg[SI4703_TEST1_INDEX].test1.reg);
    printf("=> 0x%04x\n", test_regs.reg[SI4703_POWERCFG_INDEX].powercfg.reg);
*/
    

    return 0;
}

//#endif