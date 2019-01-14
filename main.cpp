#include "mbed.h"

typedef enum _ExceptType {
    USAGE_FAULT_ZERO_DIV,
    USAGE_FAULT_INVALID_THUMB_BIT,
    BUS_FAULT_INVALID_ACCESS,
    BUS_FAULT_UNALIGNED_ACCESS,
    MEMMANAGE_FAULT_EXECUTE_XN
} ExceptType;

//Prototypes
void exception_generator(ExceptType except_type);
void generate_usage_fault_zero_div(void);
void generate_usage_fault_inv_thumb_bit(void);
void generate_bus_fault_inv_access(void);
void generate_bus_fault_unaligned_access(void);
void generate_memmanage_fault_execute_xn(void);
void test_func(int i);
typedef void (*test_func_ptr_t)(int i);
test_func_ptr_t test_func_ptr;
static int reboot_error_happened = 0;

#if MBED_CONF_PLATFORM_CRASH_CAPTURE_ENABLED
void mbed_error_reboot_callback(mbed_error_ctx *error_context) {
    reboot_error_happened = 1;
    mbed_reset_reboot_error_info();
}
#endif

// main() runs in its own thread in the OS
int main() {
  
    if(reboot_error_happened) {
        printf("\nRebooted and Halted...test completed\n");
        while(1) __WFI(); 
    }
    printf("\nMbed-OS exception handler test\n");
    exception_generator(BUS_FAULT_UNALIGNED_ACCESS);
    printf("Forcing exception failed\n");
}

MBED_NOINLINE void exception_generator(ExceptType except_type)
{
    printf("\nForcing exception: %d\n", except_type);
    
    switch(except_type)
    {
        case USAGE_FAULT_ZERO_DIV:
            generate_usage_fault_zero_div();
            break;//We shouldnt reach here
        case USAGE_FAULT_INVALID_THUMB_BIT:
            generate_usage_fault_inv_thumb_bit();
            break;//We shouldnt reach here
        case BUS_FAULT_INVALID_ACCESS:
            generate_bus_fault_inv_access();
            break;//We shouldnt reach here
        case BUS_FAULT_UNALIGNED_ACCESS:
            generate_bus_fault_unaligned_access();
            break;//We shouldnt reach here
        case MEMMANAGE_FAULT_EXECUTE_XN:
            generate_memmanage_fault_execute_xn();
            break;//We shouldnt reach here
        default:
            printf("\nInvalid exception generator\n");
    }
}

MBED_NOINLINE void generate_bus_fault_unaligned_access()
{
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    uint32_t inv_addr = 0xAAA3;
    uint32_t val = *(uint32_t *)inv_addr;
    
    printf("\nval= %X", val);
    
    return;
}

MBED_NOINLINE void generate_usage_fault_zero_div()
{
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
    int x = 1;
    int y = 0;
    int s = x / y;
    printf("s=%d",s);
    return;
}

MBED_NOINLINE void generate_usage_fault_inv_thumb_bit(void)
{
    test_func_ptr = test_func;
    printf("\nOriginal test_func_ptr=0x%08X", test_func_ptr);
    test_func_ptr = (test_func_ptr_t)((uint32_t)test_func_ptr & 0xFFFFFFFE);
    printf("\nModified(Thumb bit cleared) test_func_ptr=0x%08X\n", test_func_ptr);
    test_func_ptr(1);
}

MBED_NOINLINE void test_func(int i)
{
  printf("\ni= 0x%08X\n", i);
}

MBED_NOINLINE void generate_bus_fault_inv_access()
{
    uint32_t inv_addr = 0xFFFFFFF0;
    uint32_t val = *(uint32_t *)inv_addr;
    
    printf("\nval= %X", val);
    
    return;
}

MBED_NOINLINE void generate_memmanage_fault_execute_xn()
{
    test_func_ptr = (test_func_ptr_t)(0xC0000000);
    test_func_ptr(1);
}



