#ifndef SI4703_LOW_H
#define SI4703_LOW_H

#include "si4703_exception.h"
#include "si4703_registers.h"

class Si4703_low {
public:
    Si4703_low (const char* i2c_dev, int i2c_address, int _rst_pin, int _en_pin ,int irq_pin);
    ~Si4703_low ();

    int read_registers(SI4703_REGISTERS_t &registers);
    int write_registers(SI4703_REGISTERS_t &registers);

private:
    int fd = 0;
    int i2c_address = 0;
    int _rst_pin = 0;
    int _en_pin = 0;
    int irq_pin = 0;

    inline void check_state() {
        if ( fd <= 0 || i2c_address <= 0 ) {
            raise_Si4703_exception("Si4703_low not initialized\n");
        }
    }

    void gpio_fs_write(std::string const &filename, std::string const &value);
    void gpio_fs_setup(int pin, std::string const &direction);
    void gpio_fs_unsetup(int pin);
    void gpio_fs_set_value(int pin, int value);

    void get_i2c_bus();


};

#endif //SI4703_LOW_H