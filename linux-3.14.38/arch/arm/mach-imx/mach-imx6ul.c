/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/can/platform/flexcan.h>
#include <linux/gpio.h>
#include <linux/irqchip.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/phy.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/mfd/syscon/imx6q-iomuxc-gpr.h>
#include <linux/pm_opp.h>
#include <linux/fec.h>
#include <linux/netdevice.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <linux/memblock.h>
#include "common.h"
#include "cpuidle.h"
struct proc_dir_entry *boardname_dir, *boardname_file;  


static void __init imx6ul_enet_clk_init(void)
{
	struct regmap *gpr;

	gpr = syscon_regmap_lookup_by_compatible("fsl,imx6ul-iomuxc-gpr");
	if (!IS_ERR(gpr))
		regmap_update_bits(gpr, IOMUXC_GPR1, IMX6UL_GPR1_ENET_CLK_DIR,
				   IMX6UL_GPR1_ENET_CLK_OUTPUT);
	else
		pr_err("failed to find fsl,imx6ul-iomux-gpr regmap\n");

}

static int ksz8081_phy_fixup(struct phy_device *dev)
{
	if (dev && dev->interface == PHY_INTERFACE_MODE_MII) {
		phy_write(dev, 0x1f, 0x8110);
		phy_write(dev, 0x16, 0x201);
	} else if (dev && dev->interface == PHY_INTERFACE_MODE_RMII) {
		phy_write(dev, 0x1f, 0x8190);
		phy_write(dev, 0x16, 0x202);
	}

	return 0;
}

#define PHY_ID_KSZ8081_MNRN60	0x00221560
#define PHY_ID_KSZ8081_MNRN61	0x00221561
static void __init imx6ul_enet_phy_init(void)
{
	phy_register_fixup_for_uid(PHY_ID_KSZ8081_MNRN60, 0xffffffff,	ksz8081_phy_fixup);
        phy_register_fixup_for_uid(PHY_ID_KSZ8081_MNRN61, 0xffffffff,	ksz8081_phy_fixup);
}

static int proc_show_ver(struct seq_file *file, void *v)
{
	int cnt = 0;
  
	if(of_machine_is_compatible("OKMX6UL-C"))
		seq_printf(file, "boardname: %s\n", "OKMX6UL-C");    
	else if(of_machine_is_compatible("OKMX6UL-C2"))
		seq_printf(file, "boardname: %s\n", "OKMX6UL-C2");   
	else if(of_machine_is_compatible("OKMX6UL-C3"))
		seq_printf(file, "boardname: %s\n", "OKMX6UL-C3");  
	
	return cnt; 
}
static int proc_boardname_open(struct inode *inode, struct file *file)
{
	single_open(file, proc_show_ver, NULL);  /*将proc_show_ver 和 seq_file文件关联起来*/
	return 0;
}

static struct file_operations boardname_fops = {
	.owner   = THIS_MODULE,
	.open    = proc_boardname_open, 
	.read    = seq_read,     /*定义在linux/seq_file.c*/
	.llseek  = seq_lseek,
	.release = single_release,  /*定义在linux/seq_file.c*/
};

int proc_init(void)
{
	boardname_file = proc_create("boardname", S_IFREG|S_IRUGO, NULL, &boardname_fops);
   	if(boardname_file)
   		return 0;
	else
   		return -ENOMEM;
}

static inline void imx6ul_enet_init(void)
{
	imx6ul_enet_clk_init();
	imx6ul_enet_phy_init();
	imx6_enet_mac_init("fsl,imx6ul-fec");
}

static void __init imx6ul_init_machine(void)
{
	struct device *parent;
	void __iomem *iomux;
	struct device_node *np;

	mxc_arch_reset_init_dt();

	parent = imx_soc_device_init();
	if (parent == NULL)
		pr_warn("failed to initialize soc device\n");

	of_platform_populate(NULL, of_default_bus_match_table,
					NULL, parent);

	imx6ul_enet_init();
	imx_anatop_init();
	imx6ul_pm_init();

	np = of_find_compatible_node(NULL,NULL,"fsl,imx6ul-iomuxc");
        iomux = of_iomap(np, 0);
        writel_relaxed(0x2,iomux+0x650);
        writel_relaxed(0x3,iomux+0x658);
	proc_init();
}

static void __init imx6ul_init_irq(void)
{
	imx_init_revision_from_anatop();
	imx_src_init();
	imx_gpc_init();
	irqchip_init();
}

static const char *imx6ul_dt_compat[] __initdata = {
	"fsl,imx6ul",
	NULL,
};

static void __init imx6ul_init_late(void)
{
	platform_device_register_simple("imx6q-cpufreq", -1, NULL, 0);

	imx6ul_cpuidle_init();
}

static void __init imx6ul_map_io(void)
{
	debug_ll_io_init();
	imx6_pm_map_io();
#ifdef CONFIG_CPU_FREQ
	imx_busfreq_map_io();
#endif
}

#ifdef CONFIG_MX6_CLK_FOR_BOOTUI_TRANS
static void imx6ul_init_reserve(void)
{
	phys_addr_t base, size;

	/*
	 * Frame buffer base address.
	 * It is same as CONFIG_FB_BASE in Uboot.
	 */
	base = 0x87b00000;

	/*
	 * Reserved display memory size.
	 * It should be bigger than 3 x framer buffer size.
	 * For 1080P 32 bpp, 1920*1080*4*3 = 0x017BB000.
	 */
	size = 0x01000000;

	memblock_reserve(base, size);
	memblock_remove(base, size);
}
#endif 

DT_MACHINE_START(IMX6UL, "Freescale i.MX6 UltraLite (Device Tree)")
	.map_io		= imx6ul_map_io,
	.init_irq	= imx6ul_init_irq,
	.init_machine	= imx6ul_init_machine,
	.init_late	= imx6ul_init_late,
	.dt_compat	= imx6ul_dt_compat,
	.restart	= mxc_restart,
#ifdef CONFIG_MX6_CLK_FOR_BOOTUI_TRANS
	.reserve        = imx6ul_init_reserve,
#endif

MACHINE_END
