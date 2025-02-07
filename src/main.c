#include <stdio.h>
#include <stdlib.h>
#include <cpu.h>
#include <memory.h>
#include <graphic.h>
#include <helper.h>

int main(int argc, char **argv) {
    
    load_font_sprits_to_mem();
    if (load_rom_to_mem(argv[1]))
        return 1;
    
    clear_display();
    init_cpu();

    while(1){
        loop_cpu();
    }
    
    return 0;
}