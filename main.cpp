#include "mbed.h"

//Prototypes
void generate_usage_fault_zero_div(void);
void generate_usage_fault_inv_thumb_bit(void);
void generate_bus_fault_inv_access(void);
void generate_bus_fault_unaligned_access(void);
void generate_memmanage_fault_execute_xn(void);
void test_func(int i);
typedef void (*test_func_ptr_t)(int i);
test_func_ptr_t test_func_ptr;

// main() runs in its own thread in the OS
int main() {
  
    printf("\nMbed-OS exception handler test\n");
    while (true) {
        printf("\nForcing exception\n");
        //generate_usage_fault_zero_div();
        //generate_usage_fault_inv_thumb_bit();
        //generate_bus_fault_inv_access();
        //generate_memmanage_fault_execute_xn();
        generate_bus_fault_unaligned_access();
    }
}

void generate_usage_fault_zero_div()
{
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
    int x = 1;
    int y = 0;
    int s = x / y;
    printf("s=%d",s);
    return;
}

void generate_usage_fault_inv_thumb_bit(void)
{
    test_func_ptr = test_func;
    printf("\nOriginal test_func_ptr=0x%08X", test_func_ptr);
    test_func_ptr = (test_func_ptr_t)((uint32_t)test_func_ptr & 0xFFFFFFFE);
    printf("\nModified(Thumb bit cleared) test_func_ptr=0x%08X\n", test_func_ptr);
    test_func_ptr(1);
}

void test_func(int i)
{
  printf("\ni= 0x%08X\n", i);
}

void generate_bus_fault_inv_access()
{
    uint32_t inv_addr = 0xFFFFFFF0;
    uint32_t val = *(uint32_t *)inv_addr;
    
    printf("\nval= %X", val);
    
    return;
}

void generate_bus_fault_unaligned_access()
{
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    uint32_t inv_addr = 0xAAA3;
    uint32_t val = *(uint32_t *)inv_addr;
    
    printf("\nval= %X", val);
    
    return;
}

void generate_memmanage_fault_execute_xn()
{
    test_func_ptr = (test_func_ptr_t)(0xC0000000);
    test_func_ptr(1);
}



