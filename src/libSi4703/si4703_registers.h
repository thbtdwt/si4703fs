#ifndef SI4703_REGISTERS_H
#define SI4703_REGISTERS_H

#include <cstdint>

typedef union {
    uint16_t reg;
    struct {
        unsigned MFGID:12;
	    unsigned PN:4;
	} bits;
} SI4703_DEVICEID_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned FIRMWARE:6;
        unsigned DEV:4;
	    unsigned REV:6;
	} bits;
} SI4703_CHIPID_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned ENABLE:1;
        unsigned UNUSED_3:5;
        unsigned DISABLE:1;
        unsigned UNUSED_2:1;
        unsigned SEEK:1;
	    unsigned SEEKUP:1;
        unsigned SKMODE:1;
        unsigned RDSM:1;
	    unsigned UNUSED_1:1;
        unsigned MONO:1;
        unsigned DMUTE:1;
	    unsigned DSMUTE:1;
	} bits;
} SI4703_POWERCFG_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned CHAN:10;
        unsigned UNUSED:5;
	    unsigned TUNE:1;
	} bits;
} SI4703_CHANNEL_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned GPIO1:2;
        unsigned GPIO2:2;
        unsigned GPIO3:2;
        unsigned BLNDAD:2;
        unsigned UNUSED_2:2;
        unsigned AGCD:1;
        unsigned DE:1;
        unsigned RDS:1;
        unsigned UNUSED_1:1;
	    unsigned STCIEN:1;
	    unsigned RDSIEN:1;
	} bits;
} SI4703_SYSCONFIG1_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned VOLUME:4;
        unsigned SPACE:2;
	    unsigned BAND:2;
	    unsigned SEEKTH:8;
	} bits;
} SI4703_SYSCONFIG2_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned SKCNT:4;
        unsigned SKSNR:4;
        unsigned VOLEXT:1;
        unsigned UNUSED:3;
	    unsigned SMUTEA:2;
	    unsigned SMUTER:2;
	} bits;
} SI4703_SYSCONFIG3_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned UNUSED:14;
	    unsigned AHIZEN:1;
	    unsigned XOSCEN:1;
	} bits;
} SI4703_TEST1_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned UNUSED:16;
	} bits;
} SI4703_TEST2_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned UNUSED:16;
	} bits;
} SI4703_BOOTCONFIG_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned RSSI:8;
        unsigned ST:1;
        unsigned BLERA:2;
        unsigned RDSS:1;
        unsigned AFCRL:1;
        unsigned SFBL:1;
	    unsigned STC:1;
	    unsigned RDSR:1;
	} bits;
} SI4703_STATUSRSSI_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned READCHAN:10;
        unsigned BLERD:2;
	    unsigned BLERC:2;
	    unsigned BLERB:2;
	} bits;
} SI4703_READCHAN_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned RDSA:16;
	} bits;
} SI4703_RDSA_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned RDSB:16;
	} bits;
} SI4703_RDSB_t;

typedef union {
    uint16_t reg;
    struct {
        unsigned RDSC:16;
	} bits;
} SI4703_RDSC_t;


typedef union {
    uint16_t reg;
    struct {
        unsigned RDSD:16;
	} bits;
} SI4703_RDSD_t;


#define SI4703_NB_REGS 16

typedef enum {
    SI4703_DEVICEID_INDEX = 0,
    SI4703_CHIPID_INDEX,
    SI4703_POWERCFG_INDEX,
    SI4703_CHANNEL_INDEX,
    SI4703_SYSCONFIG1_INDEX,
    SI4703_SYSCONFIG2_INDEX,
    SI4703_SYSCONFIG3_INDEX,
    SI4703_TEST1_INDEX,
    SI4703_TEST2_INDEX,
    SI4703_BOOTCONFIG_INDEX,
    SI4703_STATUSRSSI_INDEX,
    SI4703_READCHAN_INDEX,
    SI4703_RDSA_INDEX,
    SI4703_RDSB_INDEX,
    SI4703_RDSC_INDEX,
    SI4703_RDSD_INDEX
} SI4703_INDEX_t;

typedef union {
    uint16_t value;
    SI4703_DEVICEID_t deviceid;
    SI4703_CHIPID_t chipid;
    SI4703_POWERCFG_t powercfg;
    SI4703_CHANNEL_t channel;
    SI4703_SYSCONFIG1_t sysconfig1;
    SI4703_SYSCONFIG2_t sysconfig2;
    SI4703_SYSCONFIG3_t sysconfig3;
    SI4703_TEST1_t test1;
    SI4703_TEST2_t test2;
    SI4703_BOOTCONFIG_t bootconfig;
    SI4703_STATUSRSSI_t statusrssi;
    SI4703_READCHAN_t readchan;
    SI4703_RDSA_t rdsa;
    SI4703_RDSB_t rdsb;
    SI4703_RDSC_t rdsc;
    SI4703_RDSD_t rdsd;
} SI4703_REGISTER_t;

typedef struct {
    SI4703_REGISTER_t reg[SI4703_NB_REGS];
} SI4703_REGISTERS_t;

#endif //SI4703_REGISTERS_H