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


#include "si4703_low.h"
#include "si4703_trace.h"


Si4703_low::Si4703_low (const char* i2c_dev, int i2c_address, int _rst_pin, int _en_pin ,int irq_pin){

    if ( i2c_dev == NULL || i2c_address == 0 ) {
        return; //-EINVAL;
    }
    this->i2c_address = i2c_address;
    this->_rst_pin = _rst_pin;
    this->_en_pin = _en_pin;
    this ->irq_pin = irq_pin;
    fd = open(i2c_dev, O_RDWR);
    if (fd < 0) {
        raise_Si4703_exception_c_style("open failed: '%m' (%d)\n", errno);
    }
    DEBUG_LOG("i2c_dev %s opened file=%d\n", i2c_dev, fd);

    if ( ioctl(fd, I2C_PEC, 1) < 0 ) { 
        raise_Si4703_exception_c_style("Failed to enable PEC: '%m' (%d)\n", errno);
    }

    gpio_fs_setup(_rst_pin, "out");
    gpio_fs_setup(_en_pin, "out");
    gpio_fs_setup(irq_pin, "in");
    gpio_fs_set_value(_rst_pin, 0);
    gpio_fs_set_value(_en_pin, 1);
    std::this_thread::sleep_for (std::chrono::seconds(1));
    gpio_fs_set_value(_rst_pin, 1);
    std::this_thread::sleep_for (std::chrono::seconds(1));
    
}


Si4703_low::~Si4703_low() {

    if ( fd > 0) {
        DEBUG_LOG("close file %d\n", fd);
        close(fd);
    }

    gpio_fs_set_value(_rst_pin, 0);
    gpio_fs_set_value(_en_pin, 0);
    std::this_thread::sleep_for (std::chrono::milliseconds(500));
    gpio_fs_unsetup(_rst_pin);
    gpio_fs_unsetup(_en_pin);
    gpio_fs_unsetup(irq_pin);
    
}


int Si4703_low::read_registers(SI4703_REGISTERS_t &registers) {
    uint16_t buffer[SI4703_NB_REGS];   
    
    get_i2c_bus();
    read(fd, (uint8_t*)buffer, sizeof(buffer));
    //i2c_smbus_read_i2c_block_data(fd, 0x0A, sizeof(buffer), (uint8_t*)buffer);
    /*for (int i=0; i < SI4703_NB_REGS; i++ ){
		printf("%04x\n", buffer[i]);
    }*/

    // Read starts at 0xA and wrap to 0x9, so the buffer has to be reorganised. 
    int register_index = 0xA;
    for (int i=0; i < SI4703_NB_REGS; i++ ){ 
        registers.reg[register_index & 0xF].value = (buffer[i] >> 8) | (buffer[i] << 8);
        //__builtin_bswap16(buffer[i]);
        register_index++;
    }
    return 0;
}

int Si4703_low::write_registers(SI4703_REGISTERS_t &registers) {
    uint16_t buffer[SI4703_NB_REGS];

    // Write starts at 0x2. The .
    int register_index = 0x2;
    for (int i=0; i < SI4703_NB_REGS; i++ ){

        buffer[i] =  (registers.reg[register_index & 0xF].value >> 8) | (registers.reg[register_index & 0xF].value << 8);
        //__builtin_bswap16(registers.reg[register_index & 0xF].value);
        register_index++;
    }
/*
    for (int i=0; i < SI4703_NB_REGS; i++ ){
		printf("%04x\n", buffer[i]);
    }
*/
    
    get_i2c_bus();
    write(fd, (uint8_t*)buffer, sizeof(buffer));
    //i2c_smbus_write_i2c_block_data(fd, 0x02, sizeof(buffer), (uint8_t*)buffer);
    /*for (int i=0; i < SI4703_NB_REGS; i++ ){
		printf("%04x\n", buffer[i]);
    }*/
    return 0;
}



/* Private part */

/*
void gpio_open_and_write(s char* filename, const char* value){
    int gpio_fd = open(filename, O_WRONLY);
    if ( 0 > gpio_fd) {
        raise_Si4703_exception_c_style("open(%s) failed: '%m' (%d)\n", filename, errno);
    } 
    if ( 0 != write(gpio_fd, value, std::strlen(value)) ) {
        raise_Si4703_exception_c_style("write(%s) failed: '%m' (%d)\n", filename, errno);
    }
    close(gpio_fd) ;
}
*/

void Si4703_low::gpio_fs_write(std::string const &filename, std::string const &value ){
    DEBUG_LOG("Write %s in %s\n", value.c_str(), filename.c_str());
    std::ofstream st(filename);
    if (!st.is_open()) {
        raise_Si4703_exception_c_style("ofstream(%s) failed: '%m' (%d)\n", filename.c_str(), errno);
    }
    st << value;
    st.flush();
}


void Si4703_low::gpio_fs_setup(int pin, std::string const &direction) {
    {
        std::string filename("/sys/class/gpio/export");
        gpio_fs_write(filename, std::to_string(pin));
    }
    std::this_thread::sleep_for (std::chrono::seconds(1));
    {
        std::ostringstream filename; 
        filename << "/sys/class/gpio/gpio" << pin << "/direction";
        gpio_fs_write(filename.str(), direction);
    }
}

void Si4703_low::gpio_fs_unsetup(int pin) {
    std::string filename("/sys/class/gpio/unexport");
    gpio_fs_write(filename, std::to_string(pin));
}

void Si4703_low::gpio_fs_set_value(int pin, int value) {
    std::ostringstream filename; 
    filename << "/sys/class/gpio/gpio" << pin << "/value"; 
    gpio_fs_write(filename.str(), std::to_string(value));
}

void Si4703_low::get_i2c_bus() {
    check_state();
    if (ioctl(fd, I2C_SLAVE, i2c_address) < 0) {
        raise_Si4703_exception_c_style("ioctl failed: '%m' (%d)\n", errno);
    }
}

#if 0

void display_registers(SI4703_REGISTERS_t &registers) {
    printf("Si4703 registers:\n");
    for (int i=0; i < SI4703_NB_REGS; i++ ){ 
        printf("0x%0x: 0x%04x\n", i, registers.reg[i].value);
    }
}



int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv;

    {

    Si4703_low si("/dev/i2c-1", 0x10, 17, 4, 18);
    
    SI4703_REGISTERS_t test_regs;
    si.read_registers(test_regs);

    //display_registers(test_regs);

    test_regs.reg[SI4703_TEST1_INDEX].test1.bits.XOSCEN = 1;

    si.write_registers(test_regs);

    std::this_thread::sleep_for (std::chrono::milliseconds(500));

    //display_registers(test_regs);

    test_regs.reg[SI4703_POWERCFG_INDEX].powercfg.reg = 0;
    test_regs.reg[SI4703_POWERCFG_INDEX].powercfg.bits.ENABLE = 1;
    test_regs.reg[SI4703_POWERCFG_INDEX].powercfg.bits.DMUTE = 1;

    test_regs.reg[SI4703_SYSCONFIG1_INDEX].sysconfig1.bits.RDS = 1;
    test_regs.reg[SI4703_SYSCONFIG1_INDEX].sysconfig1.bits.DE = 1;
    test_regs.reg[SI4703_SYSCONFIG2_INDEX].sysconfig2.bits.SPACE = 1;

    display_registers(test_regs);
    si.write_registers(test_regs);

    std::this_thread::sleep_for (std::chrono::milliseconds(150));

    si.read_registers(test_regs);
    display_registers(test_regs);

    test_regs.reg[SI4703_SYSCONFIG2_INDEX].sysconfig2.bits.VOLUME = 1;
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

/*

    si.read_registers(test_regs);

    printf("=>\n");
    for (int i=0; i < SI4703_NB_REGS; i++ ){ 
        printf("0x%0x: 0x%04x\n", i, test_regs.reg[i].value);
    }

    printf("=> 0x%04x\n", test_regs.reg[SI4703_TEST1_INDEX].test1.reg);
    printf("=> 0x%04x\n", test_regs.reg[SI4703_POWERCFG_INDEX].powercfg.reg);
*/
    }

    return 0;
}

#endif