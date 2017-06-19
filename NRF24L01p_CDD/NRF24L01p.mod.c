#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xbca7617b, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xe1775d83, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0x451d3e2b, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x9ea10bb1, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x162455f7, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x8a8930a9, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x1d277ebb, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x6a338294, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0xe2d5255a, __VMLINUX_SYMBOL_STR(strcmp) },
	{ 0x97255bdf, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
	{ 0x349cba85, __VMLINUX_SYMBOL_STR(strchr) },
	{ 0x69163cf5, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0x93b44950, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x84d93545, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x3e8348da, __VMLINUX_SYMBOL_STR(gpiod_unexport) },
	{ 0x92e79ea, __VMLINUX_SYMBOL_STR(gpiod_export) },
	{ 0x834567b5, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0xb06d6add, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0xc73af6d, __VMLINUX_SYMBOL_STR(mutex_trylock) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xe4a48145, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "F4EC2AE7EF86BF3AD214CFD");
