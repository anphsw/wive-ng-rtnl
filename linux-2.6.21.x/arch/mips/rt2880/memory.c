/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     memory setup for Ralink RT2880 solution
 *
 *  Copyright 2007 Ralink Inc. (bruce_chang@ralinktech.com.tw)
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2007 Bruce Chang
 *
 * Initial Release
 *
 *
 *
 **************************************************************************
 */


#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/ioport.h>
#include <asm/bootinfo.h>
#include <asm/page.h>
#include <asm/system.h>
#include <linux/irq.h>
#include <asm/irq_cpu.h>
#include <asm/rt2880/prom.h>

//#define DEBUG
#define RAM_SIZE        CONFIG_RALINK_RAM_SIZE*1024*1024

//no need
#if 0
#if defined(CONFIG_RT2880_ASIC) || defined(CONFIG_RT2880_FPGA)
#define RAM_FIRST       0x08000400  /* Leave room for interrupt vectors */
#define RAM_END         (0x08000000 + RAM_SIZE)
#else
#define RAM_FIRST       0x00000400  /* Leave room for interrupt vectors */
#define RAM_END         (0x00000000 + RAM_SIZE)
#endif
struct resource rt2880_res_ram = {
        .name = "RAM",
        .start = 0,
        .end = RAM_SIZE,
        .flags = IORESOURCE_MEM
};
#endif

spinlock_t rtlmem_lock = SPIN_LOCK_UNLOCKED;
unsigned long detect_ram_sequence[4];

enum surfboard_memtypes {
	surfboard_dontuse,
	surfboard_rom,
	surfboard_ram,
};

#ifdef DEBUG
static char *mtypes[3] = {
	"Dont use memory",
	"Used ROM memory",
	"Free RAM memory",
};
#endif

/* References to section boundaries */
extern char _end;

#ifdef DEBUG
struct prom_pmemblock mdesc[PROM_MAX_PMEMBLOCKS];
struct prom_pmemblock * __init prom_getmdesc(void)
{
	char *env_str;
	unsigned int ramsize, rambase;

	env_str = prom_getenv("ramsize");
	if (!env_str) {
		ramsize = RAM_SIZE;
		prom_printf("maximum ramsize = %d MBytes\n", CONFIG_RALINK_RAM_SIZE );
	} else {
		prom_printf("maximum ramsize = %s\n", env_str);
		ramsize = simple_strtol(env_str, NULL, 0);
	}

	env_str = prom_getenv("rambase");
	if (!env_str) {
#if defined(CONFIG_RT2880_ASIC) || defined(CONFIG_RT2880_FPGA)
		prom_printf("rambase not set, set to default (0x08000000)\n");
		rambase = 0x08000000;
#else
		prom_printf("rambase not set, set to default (0x00000000)\n");
		rambase = 0x00000000;
#endif 
	} else {
#ifdef DEBUG
		prom_printf("rambase = %s\n", env_str);
#endif
		rambase = simple_strtol(env_str, NULL, 0);
	}

	memset(mdesc, 0, sizeof(mdesc));

	mdesc[0].type = surfboard_ram;
	mdesc[0].base = rambase;
	mdesc[0].size = ramsize;

	return &mdesc[0];
}
#endif

void __init prom_meminit(void)
{
#ifdef DEBUG
    struct prom_pmemblock *p;
    struct prom_pmemblock *psave;
#endif
#ifdef CONFIG_RAM_SIZE_AUTO
    unsigned long mem, memsize, reg_mem, mempos, memmeg;
    unsigned long before, offset;
    unsigned long flags;
    unsigned short save_dword;

       spin_lock_irqsave(&rtlmem_lock, flags);
       //Maximum RAM for autodetect
       reg_mem = 64;
       //FIRST PASS RAM capacity
       for(memmeg=8;memmeg<reg_mem;memmeg+=8){
	    mempos = 0xa0000000L + memmeg * 0x100000;
	    save_dword = *(volatile unsigned short *)mempos;

	    *(volatile unsigned short *)mempos = (unsigned short)0xABCD;
	    if (*(volatile unsigned short *)mempos != (unsigned short)0xABCD){
		*(volatile unsigned short *)mempos = save_dword;
		break;
	    }

	    *(volatile unsigned short *)mempos = (unsigned short)0xDCBA;
	    if (*(volatile unsigned short *)mempos != (unsigned short)0xDCBA){
		*(volatile unsigned short *)mempos = save_dword;
		break;
	    }
	    *(volatile unsigned short *)mempos = save_dword;
	}

       //SECOND PASS Test to be sure in RAM capacity
       before = ((unsigned long) &prom_init) & (127 << 20);
       offset = ((unsigned long) &prom_init) - before;
       for (mem = before + (1 << 20); mem < (reg_mem << 20); mem += (1 << 20))
         if (*(unsigned long *)(offset + mem) == *(unsigned long *)(prom_init))
	 {
    		mem -= before;
		break;
	 }

       //Calculate and set	
       memsize = memmeg << 20;
       detect_ram_sequence[0] = reg_mem;
       detect_ram_sequence[1] = memsize;

       //Select smallest size from pass
       if(mem < memsize){
	    memsize = mem;
	    memmeg  = mem >> 20;
	}

       detect_ram_sequence[2] = mem;
       detect_ram_sequence[3] = memsize;

       spin_unlock_irq(&rtlmem_lock);

#if defined(CONFIG_RT2880_ASIC) || defined(CONFIG_RT2880_FPGA)
	add_memory_region(0x08000000, memsize, BOOT_MEM_RAM);
#else
	add_memory_region(0x00000000, memsize, BOOT_MEM_RAM);
#endif
#else /* Fix mesize */
#if defined(CONFIG_RT2880_ASIC) || defined(CONFIG_RT2880_FPGA)
        add_memory_region(0x08000000, RAM_SIZE, BOOT_MEM_RAM);
#else
        add_memory_region(0x00000000, RAM_SIZE, BOOT_MEM_RAM);
#endif
#endif /* CONFIG_RAM_SIZE_AUTO */	

#ifdef DEBUG
	p = prom_getmdesc();
	prom_printf("MEMORY DESCRIPTOR dump:\n");
	psave = p;	/* Save p */
	while (p->size) {
		int i = 0;
		prom_printf("[%d,%p]: base<%08lx> size<%08lx> type<%s>\n",
			    i, p, p->base, p->size, mtypes[p->type]);
		p++;
		i++;
	}
	p = psave;	/* Restore p */

#endif
}

void __init prom_free_prom_memory(void)
{
        unsigned long addr;
        int i;

        for (i = 0; i < boot_mem_map.nr_map; i++) {
                if (boot_mem_map.map[i].type != BOOT_MEM_ROM_DATA)
                        continue;

                addr = boot_mem_map.map[i].addr;
                free_init_pages("prom memory",
                                addr, addr + boot_mem_map.map[i].size);
        }
}
