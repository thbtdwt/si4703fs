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


int Si4703_low::read_registers(void) {
    //DEBUG_LOG("entrer\n");
    get_i2c_bus();

    unsigned char buffer[32];
    read(fd,buffer,32);
    //i2c_smbus_read_i2c_block_data(fd, 0x0A, 32, buffer);
    int i;
    for (i=0; i < 31; i=i+2 ){
		printf("%02x %02x\n", buffer[i], buffer[i+1]);
    }

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



int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv;

    {

    Si4703_low si("/dev/i2c-1", 0x10, 17, 4, 18);
    si.read_registers();

    }

    return 0;
}