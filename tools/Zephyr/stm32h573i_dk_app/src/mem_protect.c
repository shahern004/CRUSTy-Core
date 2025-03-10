/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include "mem_protect.h"

/* Log module declaration */
LOG_MODULE_REGISTER(mem_protect, CONFIG_LOG_DEFAULT_LEVEL);

#ifndef CONFIG_BOARD_QEMU_CORTEX_M3
/* Hardware-specific includes for STM32H573I-DK */
#include <zephyr/arch/arm/aarch32/mpu/arm_mpu.h>
#include <zephyr/sys/barrier.h>

/* MPU region configuration */
static const struct arm_mpu_region mpu_regions_hw[MEM_REGION_COUNT];
#else
/* For QEMU we'll use a static buffer to simulate secure memory */
#define SECURE_MEM_SIZE (16 * 1024)
static uint8_t secure_mem_buffer[SECURE_MEM_SIZE];
#define SECURE_MEM_ADDR ((uintptr_t)secure_mem_buffer)

/* Simple memory region simulation structure */
static struct {
    uintptr_t base_addr;
    size_t size;
    bool configured;
    uint8_t access_perm;
} mpu_regions[MEM_REGION_COUNT];
#endif

/* Simple allocation tracking for secure memory */
#define MAX_ALLOCATIONS 16
static struct {
    void *ptr;
    size_t size;
    bool used;
} secure_allocations[MAX_ALLOCATIONS];

/* State variables */
static bool mem_protection_active = false;
static size_t secure_mem_used = 0;
static size_t secure_mem_next_offset = 0;

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
/**
 * @brief Configure a simulated MPU region (simplified for QEMU)
 *
 * @param region_type Type of region to configure
 * @param base_addr Base address of the region
 * @param size Size of the region in bytes
 * @param access_perm Access permissions for the region
 * @return MEM_PROTECT_SUCCESS on success, error code on failure
 */
static int configure_memory_region(mem_region_type_t region_type, 
                                  uintptr_t base_addr, 
                                  size_t size,
                                  mem_access_perm_t access_perm)
{
    if (region_type >= MEM_REGION_COUNT) {
        LOG_ERR("Invalid region type: %d", region_type);
        return MEM_PROTECT_ERROR_REGION;
    }

    /* Store region information */
    mpu_regions[region_type].base_addr = base_addr;
    mpu_regions[region_type].size = size;
    mpu_regions[region_type].access_perm = access_perm;
    mpu_regions[region_type].configured = true;

    LOG_DBG("Memory region %d configured: addr=0x%x, size=%u, perm=%d", 
            region_type, (unsigned int)base_addr, (unsigned int)size, access_perm);
            
    return MEM_PROTECT_SUCCESS;
}
#else
/**
 * @brief Configure a hardware MPU region on STM32H573I-DK
 *
 * @param region_type Type of region to configure
 * @param base_addr Base address of the region
 * @param size Size of the region in bytes
 * @param access_perm Access permissions for the region
 * @return MEM_PROTECT_SUCCESS on success, error code on failure
 */
static int configure_hw_memory_region(mem_region_type_t region_type,
                                    uintptr_t base_addr,
                                    size_t size,
                                    mem_access_perm_t access_perm)
{
    uint32_t rasr_attr = 0;
    
    if (region_type >= MEM_REGION_COUNT) {
        LOG_ERR("Invalid region type: %d", region_type);
        return MEM_PROTECT_ERROR_REGION;
    }
    
    /* Convert internal access permissions to ARM MPU attributes */
    switch (access_perm) {
        case MEM_ACCESS_READ_ONLY:
            rasr_attr = ARM_MPU_AP_RO;
            break;
        case MEM_ACCESS_READ_WRITE:
            rasr_attr = ARM_MPU_AP_RW;
            break;
        case MEM_ACCESS_READ_EXECUTE:
            rasr_attr = ARM_MPU_AP_RO;
            /* Note: XN (Execute Never) bit is 0 for executable regions */
            break;
        case MEM_ACCESS_NO_ACCESS:
            rasr_attr = ARM_MPU_AP_PRIV_NA_USER_NA;
            break;
        default:
            LOG_ERR("Invalid access permission: %d", access_perm);
            return MEM_PROTECT_ERROR_ACCESS;
    }
    
    /* Configure normal memory type with optimal caching */
    rasr_attr |= ARM_MPU_CACHEABLE | ARM_MPU_BUFFERABLE;
    
    /* Configure the MPU region */
    struct arm_mpu_region region = {
        .name = "secure_region",
        .base = base_addr,
        .attr = {
            .rasr = ARM_MPU_RASR(
                (access_perm == MEM_ACCESS_READ_EXECUTE) ? 0 : 1, /* XN bit */
                rasr_attr,
                0, /* Memory type: TEX=0b000 */
                1, /* Shareable */
                1, /* Cacheable */
                1, /* Bufferable */
                0, /* Subregions disabled */
                ARM_MPU_SIZE_TO_REG(size) /* Region size */
            )
        }
    };
    
    /* Apply the region configuration to the MPU */
    int ret = arm_core_mpu_region_add(&region);
    if (ret != 0) {
        LOG_ERR("Failed to configure MPU region: %d", ret);
        return MEM_PROTECT_ERROR_CONFIG;
    }
    
    LOG_DBG("Hardware MPU region %d configured: addr=0x%x, size=%u, perm=%d", 
            region_type, (unsigned int)base_addr, (unsigned int)size, access_perm);
            
    return MEM_PROTECT_SUCCESS;
}
#endif /* CONFIG_BOARD_QEMU_CORTEX_M3 */

int mem_protect_init(void)
{
    int ret;
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    LOG_INF("Initializing memory protection simulation (QEMU)...");

    /* Initialize allocation tracking */
    for (int i = 0; i < MAX_ALLOCATIONS; i++) {
        secure_allocations[i].ptr = NULL;
        secure_allocations[i].size = 0;
        secure_allocations[i].used = false;
    }

    /* Initialize region configuration */
    for (int i = 0; i < MEM_REGION_COUNT; i++) {
        mpu_regions[i].configured = false;
    }

    /* Clear secure memory buffer */
    memset(secure_mem_buffer, 0, SECURE_MEM_SIZE);

    /* Define secure data region */
    ret = configure_memory_region(MEM_REGION_SECURE_DATA, 
                                 SECURE_MEM_ADDR, 
                                 SECURE_MEM_SIZE / 2, 
                                 MEM_ACCESS_READ_WRITE);
    if (ret != MEM_PROTECT_SUCCESS) {
        LOG_ERR("Failed to configure secure data region: %d", ret);
        return ret;
    }

    /* Define crypto buffer region (use part of secure memory) */
    ret = configure_memory_region(MEM_REGION_CRYPTO_BUFFER, 
                                SECURE_MEM_ADDR + (SECURE_MEM_SIZE / 2), 
                                SECURE_MEM_SIZE / 2, 
                                MEM_ACCESS_READ_WRITE);
    if (ret != MEM_PROTECT_SUCCESS) {
        LOG_ERR("Failed to configure crypto buffer region: %d", ret);
        return ret;
    }

    /* Memory protection is now active */
    mem_protection_active = true;
    LOG_INF("Memory protection simulation initialized successfully");
#else
    LOG_INF("Initializing hardware memory protection (STM32H573I-DK)...");
    
    /* Initialize allocation tracking */
    for (int i = 0; i < MAX_ALLOCATIONS; i++) {
        secure_allocations[i].ptr = NULL;
        secure_allocations[i].size = 0;
        secure_allocations[i].used = false;
    }
    
    /* Check if MPU is available */
    #if defined(CONFIG_ARM_MPU)
        LOG_INF("Hardware MPU is available");
        
        /* Disable MPU during configuration */
        arm_core_mpu_disable();
        
        /* Define secure data region - use SRAM section for secure data */
        /* Note: actual addresses would be derived from linker script symbols */
        /* Using placeholder addresses for now */
        uintptr_t secure_data_base = 0x20000000;
        size_t secure_data_size = 0x4000; /* 16KB */
        
        ret = configure_hw_memory_region(MEM_REGION_SECURE_DATA,
                                       secure_data_base,
                                       secure_data_size,
                                       MEM_ACCESS_READ_WRITE);
        if (ret != MEM_PROTECT_SUCCESS) {
            LOG_ERR("Failed to configure secure data region: %d", ret);
            return ret;
        }
        
        /* Define crypto buffer region */
        uintptr_t crypto_buf_base = secure_data_base + secure_data_size;
        size_t crypto_buf_size = 0x4000; /* 16KB */
        
        ret = configure_hw_memory_region(MEM_REGION_CRYPTO_BUFFER,
                                      crypto_buf_base,
                                      crypto_buf_size,
                                      MEM_ACCESS_READ_WRITE);
        if (ret != MEM_PROTECT_SUCCESS) {
            LOG_ERR("Failed to configure crypto buffer region: %d", ret);
            return ret;
        }
        
        /* Define code protection region */
        /* Note: Using fixed values for example, actual values from linker */
        uintptr_t code_base = 0x08000000;
        size_t code_size = 0x10000; /* 64KB */
        
        ret = configure_hw_memory_region(MEM_REGION_CODE,
                                      code_base,
                                      code_size,
                                      MEM_ACCESS_READ_EXECUTE);
        if (ret != MEM_PROTECT_SUCCESS) {
            LOG_ERR("Failed to configure code region: %d", ret);
            return ret;
        }
        
        /* Define peripheral protection region for security-sensitive peripherals */
        /* Note: Using fixed values for example */
        uintptr_t periph_base = 0x40000000;
        size_t periph_size = 0x1000; /* 4KB */
        
        ret = configure_hw_memory_region(MEM_REGION_PERIPHERAL,
                                      periph_base,
                                      periph_size,
                                      MEM_ACCESS_READ_WRITE);
        if (ret != MEM_PROTECT_SUCCESS) {
            LOG_ERR("Failed to configure peripheral region: %d", ret);
            return ret;
        }
        
        /* Enable MPU */
        arm_core_mpu_enable();
        
        /* Memory barrier to ensure all memory operations complete */
        barrier_dsync_fence_full();
        barrier_isync_fence_full();
        
        mem_protection_active = true;
        LOG_INF("Hardware MPU initialized successfully");
    #else
        LOG_WRN("Hardware MPU not available, using software-only protection");
        /* Initialize software protection similar to QEMU */
        
        /* Calculate SRAM secure memory region */
        uintptr_t secure_mem_base = 0x20000000;  /* Start of SRAM */
        size_t secure_mem_size = 0x8000;         /* 32KB for secure memory */
        
        secure_mem_used = 0;
        secure_mem_next_offset = 0;
        
        /* Memory protection is limited but active */
        mem_protection_active = true;
        LOG_INF("Software-only memory protection initialized");
    #endif
#endif /* CONFIG_BOARD_QEMU_CORTEX_M3 */
    
    return MEM_PROTECT_SUCCESS;
}

int mem_protect_configure_region(mem_region_type_t region_type, mem_access_perm_t access_perm)
{
    if (!mem_protection_active) {
        LOG_ERR("Memory protection not initialized");
        return MEM_PROTECT_ERROR_INIT;
    }
    
    if (region_type >= MEM_REGION_COUNT) {
        LOG_ERR("Invalid region type: %d", region_type);
        return MEM_PROTECT_ERROR_REGION;
    }
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    if (!mpu_regions[region_type].configured) {
        LOG_ERR("Region %d not configured", region_type);
        return MEM_PROTECT_ERROR_CONFIG;
    }
    
    /* Configure region with new permissions but same base and size */
    mpu_regions[region_type].access_perm = access_perm;
    LOG_INF("Region %d permissions updated to %d", region_type, access_perm);
#else
    #if defined(CONFIG_ARM_MPU)
        /* Reconfigure hardware MPU region with new permissions */
        /* Note: In real implementation, we would need to track the regions */
        /* For now, we will reconfigure with placeholders */
        
        /* Disable MPU during reconfiguration */
        arm_core_mpu_disable();
        
        /* Reconfigure the region based on type */
        uintptr_t base;
        size_t size;
        
        /* Simplified region info - in actual implementation, track these */
        switch (region_type) {
            case MEM_REGION_SECURE_DATA:
                base = 0x20000000;
                size = 0x4000;
                break;
            case MEM_REGION_CRYPTO_BUFFER:
                base = 0x20004000;
                size = 0x4000;
                break;
            case MEM_REGION_CODE:
                base = 0x08000000;
                size = 0x10000;
                break;
            case MEM_REGION_PERIPHERAL:
                base = 0x40000000;
                size = 0x1000;
                break;
            default:
                LOG_ERR("Invalid region type: %d", region_type);
                return MEM_PROTECT_ERROR_REGION;
        }
        
        int ret = configure_hw_memory_region(region_type, base, size, access_perm);
        if (ret != MEM_PROTECT_SUCCESS) {
            /* Re-enable MPU even on failure */
            arm_core_mpu_enable();
            return ret;
        }
        
        /* Re-enable MPU */
        arm_core_mpu_enable();
        
        /* Memory barrier to ensure all memory operations complete */
        barrier_dsync_fence_full();
        barrier_isync_fence_full();
        
        LOG_INF("Hardware MPU region %d permissions updated to %d", 
                region_type, access_perm);
    #else
        LOG_WRN("Hardware MPU not available, permission change simulated");
    #endif
#endif
    
    return MEM_PROTECT_SUCCESS;
}

void *mem_protect_alloc_secure(size_t size, size_t align)
{
    void *ptr = NULL;
    int idx = -1;
    
    if (!mem_protection_active) {
        LOG_ERR("Memory protection not initialized");
        return NULL;
    }
    
    if (size == 0) {
        LOG_ERR("Invalid allocation size: 0");
        return NULL;
    }
    
    /* Ensure align is a power of 2 */
    if (align & (align - 1)) {
        LOG_ERR("Alignment must be a power of 2");
        return NULL;
    }
    
    /* Find a free allocation slot */
    for (int i = 0; i < MAX_ALLOCATIONS; i++) {
        if (!secure_allocations[i].used) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        LOG_ERR("No free allocation slots");
        return NULL;
    }

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Check if there's enough memory in simulated buffer */
    if (secure_mem_used + size > SECURE_MEM_SIZE) {
        LOG_ERR("Not enough secure memory");
        return NULL;
    }
    
    /* Align the offset */
    size_t aligned_offset = (secure_mem_next_offset + align - 1) & ~(align - 1);
    
    /* Allocate memory from simulation buffer */
    ptr = &secure_mem_buffer[aligned_offset];
    secure_allocations[idx].ptr = ptr;
    secure_allocations[idx].size = size;
    secure_allocations[idx].used = true;
    
    /* Update next offset and used memory */
    secure_mem_next_offset = aligned_offset + size;
    secure_mem_used += size;
#else
    /* On real hardware, use k_malloc_align for secure memory allocations */
    /* In a real secure application, we would use a dedicated secure heap */
    
    #if defined(CONFIG_ARM_MPU)
        /* For demonstration, allocate with k_malloc_align */
        ptr = k_malloc_align(size, align);
        if (ptr == NULL) {
            LOG_ERR("Failed to allocate secure memory");
            return NULL;
        }
        
        secure_allocations[idx].ptr = ptr;
        secure_allocations[idx].size = size;
        secure_allocations[idx].used = true;
        
        secure_mem_used += size;
    #else
        /* Simplified allocation for no-MPU case */
        /* Similar to QEMU simulation but from system heap */
        ptr = k_malloc_align(size, align);
        if (ptr == NULL) {
            LOG_ERR("Failed to allocate secure memory");
            return NULL;
        }
        
        secure_allocations[idx].ptr = ptr;
        secure_allocations[idx].size = size;
        secure_allocations[idx].used = true;
        
        secure_mem_used += size;
    #endif
#endif
    
    /* Clear the allocated memory for security */
    memset(ptr, 0, size);
    
    LOG_DBG("Allocated %u bytes of secure memory at %p (total used: %u)",
            (unsigned int)size, ptr, (unsigned int)secure_mem_used);
    
    return ptr;
}

void mem_protect_free_secure(void *ptr)
{
    int idx = -1;
    
    if (!mem_protection_active) {
        LOG_ERR("Memory protection not initialized");
        return;
    }
    
    if (ptr == NULL) {
        LOG_WRN("Attempt to free NULL pointer");
        return;
    }
    
    /* Find the allocation slot */
    for (int i = 0; i < MAX_ALLOCATIONS; i++) {
        if (secure_allocations[i].used && secure_allocations[i].ptr == ptr) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        LOG_ERR("Attempt to free untracked memory: %p", ptr);
        return;
    }
    
    /* Clear the memory before freeing */
    memset(ptr, 0, secure_allocations[idx].size);
    
    /* Update memory usage */
    secure_mem_used -= secure_allocations[idx].size;
    
#ifndef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Only free memory on real hardware, not needed for simulation buffer */
    k_free(ptr);
#endif
    
    /* Mark slot as unused */
    secure_allocations[idx].used = false;
    secure_allocations[idx].ptr = NULL;
    secure_allocations[idx].size = 0;
    
    LOG_DBG("Freed secure memory at %p (total used: %u)",
            ptr, (unsigned int)secure_mem_used);
}

void mem_protect_sanitize(void *ptr, size_t size)
{
    if (!mem_protection_active) {
        LOG_WRN("Memory protection not initialized");
        /* Still sanitize even if memory protection is not active */
    }
    
    if (ptr == NULL) {
        LOG_WRN("Attempt to sanitize NULL pointer");
        return;
    }
    
    /* Secure erase - use volatile to prevent compiler optimization */
    volatile uint8_t *p = (volatile uint8_t *)ptr;
    for (size_t i = 0; i < size; i++) {
        p[i] = 0;
    }
    
#ifndef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Memory barrier to ensure writes are completed */
    barrier_dsync_fence_full();
#endif
    
    LOG_DBG("Sanitized %u bytes of memory at %p", (unsigned int)size, ptr);
}

bool mem_protect_is_active(void)
{
    return mem_protection_active;
}

bool mem_protect_is_secure_region(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return false;
    }
    
    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t end = addr + size;
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Check if address range is within secure memory simulation buffer */
    return (addr >= SECURE_MEM_ADDR && end <= (SECURE_MEM_ADDR + SECURE_MEM_SIZE));
#else
    /* Check if memory is in a tracked secure allocation */
    for (int i = 0; i < MAX_ALLOCATIONS; i++) {
        if (secure_allocations[i].used) {
            uintptr_t alloc_start = (uintptr_t)secure_allocations[i].ptr;
            uintptr_t alloc_end = alloc_start + secure_allocations[i].size;
            
            if (addr >= alloc_start && end <= alloc_end) {
                return true;
            }
        }
    }
    
    #if defined(CONFIG_ARM_MPU)
        /* For hardware MPU, we would check if the memory is in a secure region */
        /* This would require additional tracking of MPU regions */
        /* Simplified implementation - check if in SRAM secure region */
        uintptr_t secure_start = 0x20000000;  /* Start of secure SRAM (example) */
        uintptr_t secure_end = 0x20008000;    /* End of secure SRAM (example) */
        
        return (addr >= secure_start && end <= secure_end);
    #else
        /* For software-only protection, limited verification */
        return false;
    #endif
#endif
}
