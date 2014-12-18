/*
 * Copyright (c) 2006 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 * 
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */

#include <string.h>
#include <kern/assert.h> 
#include <mach/i386/vm_param.h>
#include <mach/i386/kern_return.h>
#include <vm/vm_kern.h>

#include <i386/proc_reg.h>

#include <IOKit/IOLib.h>

//#include <i386/pmap.h>
//#include "vmx_shims.h"

/* dealing with physical memory */
#include <vm/pmap.h>
typedef struct pmap *pmap_t;
extern vm_map_t kernel_map;
extern pmap_t kernel_pmap;
extern ppnum_t pmap_find_phys(pmap_t pmap, addr64_t va);

void *vmx_pcalloc(void) {
	char *pptr;
	pptr = IOMallocAligned(PAGE_SIZE, PAGE_SIZE);
	if (pptr == NULL) return (NULL);
	bzero(pptr, PAGE_SIZE);
	return (pptr);
}

addr64_t vmx_paddr(void *va) {
	addr64_t ret = ptoa_64(pmap_find_phys(kernel_pmap, (addr64_t)(uintptr_t)va));
  ret += ((u64)va) & PAGE_MASK;
  return ret;
}

void vmx_pfree(void *va) {
	IOFreeAligned(va, PAGE_SIZE);
}

vmcs *allocate_vmcs() {
  u64 vmx_msr = rdmsr64(MSR_IA32_VMX_BASIC);
  printf("msr %llx\n", vmx_msr);

  vmcs *ret;
  ret = vmx_pcalloc();
  ret->revision_id = vmx_msr & 0xFFFFFFFF;

  return ret;
}

