extern "C" {
#include "qemu/qemu-plugin.h"
}
#include <stdio.h>

QEMU_PLUGIN_EXPORT int qemu_plugin_version = QEMU_PLUGIN_VERSION;

static void vcpu_tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb* tb)
{
    size_t n_insns = qemu_plugin_tb_n_insns(tb);
    printf("> New TB of size %llx\n", n_insns);
        
    for (int i = 0; i < n_insns; i++) {
        struct qemu_plugin_insn* insn = qemu_plugin_tb_get_insn(tb, i);
        const uint8_t* opcodes = (uint8_t*)qemu_plugin_insn_data(insn);
        size_t ins_size = qemu_plugin_insn_size(insn);
        printf(" > Instruciton: ");
        for (int i = 0; i < ins_size; ++i) { printf("%X", opcodes[i] & 0xFF); }
        printf("\n");
    }

    return;
}

static void plugin_exit(qemu_plugin_id_t id, void* p)
{
    printf("> Goodbye from plugin. %lld\n", id);
}

QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id,
    const qemu_info_t* info,
    int argc, char** argv)
{
    printf("> Hello From Plugin! ID: %llx | vcpu_tb_trans: %p | Address of qemu_plugin_register_vcpu_tb_trans_cb: %p \n", id, vcpu_tb_trans, qemu_plugin_register_vcpu_tb_trans_cb);
    {
        printf(" > Target name: %s\n", info->target_name);
        printf(" > Is this a full system emulation?: %d\n", info->system_emulation);
        printf(" > Initial number of vCPUs: %d\n", info->system.smp_vcpus);
        printf(" > Maximum possible number of vCPUs: %d\n", info->system.max_vcpus);
    }

    qemu_plugin_register_vcpu_tb_trans_cb(id, vcpu_tb_trans);
    printf("> after qemu_plugin_register_vcpu_tb_trans_cb\n");
    qemu_plugin_register_atexit_cb(id, plugin_exit, NULL);
    printf("> Everything was registered\n");

    return 0;
}