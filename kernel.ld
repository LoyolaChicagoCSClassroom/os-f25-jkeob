/* kernel.ld — 32-bit i386 kernel, Multiboot2 */

ENTRY(main)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
  /* Load the kernel at 1 MiB */
  . = 0x100000;

  /* Multiboot2 header must be in first 8 KiB and 8-byte aligned */
  .multiboot ALIGN(8) : {
    KEEP(*(.multiboot))
  }

  /* Code */
  .text ALIGN(4K) : {
    *(.text*)
  }

  /* Read-only data */
  .rodata ALIGN(4K) : {
    *(.rodata*)
  }

  /* Initialized data */
  .data ALIGN(4K) : {
    *(.data*)
  }

  /* Uninitialized data (zero-initialized at load) */
  .bss ALIGN(4K) : {
    *(.bss*)
    *(COMMON)
  }

  /* End marker (optional) */
  _end_kernel = .;
}
