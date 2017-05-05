/**
 * @file
 *
 * @date 21.02.13
 * @author Pavel Cherstvov
 */

/*#define MY_IRQ_NR         10*/
#define SPI_CHANNEL_NR  0

#ifndef OMAP3_SPI_POLL_H_
#define OMAP3_SPI_POLL_H_

#define CM_ICLKEN_PER       0x48005010
#define CM_FCLKEN_PER       0x48005000

#define CM_ICLKEN_WKUP      0x48004C10
#define CM_FCLKEN_WKUP      0x48004C00

#define OMAP37X_GPIO6_BASE  0x49058000
#define GPIO6_OE            OMAP37X_GPIO6_BASE + 0x034

#define GPIO6_SYSCONFIG     OMAP37X_GPIO6_BASE + 0x010
#define GPIO6_SYSSTATUS     OMAP37X_GPIO6_BASE + 0x014

#define GPIO6_CLEARDATAOUT  OMAP37X_GPIO6_BASE + 0x090
#define GPIO6_SETDATAOUT    OMAP37X_GPIO6_BASE + 0x094

#define CM_FCLKEN1_CORE             0x48004A00  /* EN_MCSPI1 18b */
#define CM_ICLKEN1_CORE             0x48004A10  /* EN_MCSPI1 18b */

#define OMAP37X_MCSPI1_BASE     0x48098000

#define MCSPI1_IRQSTATUS        (OMAP37X_MCSPI1_BASE + 0x18)            /* TX0_EMPTY 0b, RX0_EMPTY 2b */
#define MCSPI1_SYSSTATUS        (OMAP37X_MCSPI1_BASE + 0x14)            /* RESETDONE 0b */
#define MCSPI1_MODULCTRL        (OMAP37X_MCSPI1_BASE + 0x28)            /* MS 2b, SINGLE 0b */
#define MCSPI1_SYSCONFIG        (OMAP37X_MCSPI1_BASE + 0x10)            /* SOFTRESET 1b */

/* x = [0,3] for McSPI1 */
#define MCSPI1_CHxCONF(x)   (OMAP37X_MCSPI1_BASE + 0x2C + (0x14 * (x)))     /* IS 18b, DPE1 17b, DPE0 16b, TRM 13:12b, WL 11:7b, EPOL 6b, CLKD 5:2b */
#define MCSPI1_CHxCTRL(x)   (OMAP37X_MCSPI1_BASE + 0x34 + (0x14 * (x)))     /* EN 0b */
#define MCSPI1_TX(x)        (OMAP37X_MCSPI1_BASE + 0x38 + (0x14 * (x)))     /* TX 31:0b */
#define MCSPI1_RX(x)        (OMAP37X_MCSPI1_BASE + 0x3C + (0x14 * (x)))     /* RX 31:0b */

#define MCSPI_SYSCONFIG_SOFTRESET   (1 << 1)
#define MCSPI_SYSSTATUS_RESETDONE   (1 << 0)
#define MCSPI_IRQSTATUS_TXEMPTY(x)  (1 << (0 + (x) * 4))
#define MCSPI_IRQSTATUS_RXFULL(x)   (1 << (2 + (x) * 4))

#define GPIO6_SYSCONFIG_SOFTRESET   (1 << 1)
#define GPIO6_SYSSTATUS_RESETDONE   (1 << 0)

#define REG_CLEAR_BIT(addr, bit_nr) REG_ANDIN(addr, ~(1 << bit_nr));
#define REG_SET_BIT(addr, bit_nr)   REG_ORIN(addr, (1 << bit_nr));

#endif /* OMAP3_SPI_POLL_H_ */
