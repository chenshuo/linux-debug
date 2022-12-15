/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */
/*
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * Copyright(c) 2022 Intel Corporation. All rights reserved.
 */

#ifndef __SOUND_SOC_SOF_IPC4_PRIV_H
#define __SOUND_SOC_SOF_IPC4_PRIV_H

#include <linux/idr.h>
#include <sound/sof/ext_manifest4.h>
#include "sof-priv.h"

/* The DSP window indices are fixed */
#define SOF_IPC4_OUTBOX_WINDOW_IDX	1
#define SOF_IPC4_DEBUG_WINDOW_IDX	2

enum sof_ipc4_mtrace_type {
	SOF_IPC4_MTRACE_NOT_AVAILABLE = 0,
	SOF_IPC4_MTRACE_INTEL_CAVS_1_5,
	SOF_IPC4_MTRACE_INTEL_CAVS_1_8,
	SOF_IPC4_MTRACE_INTEL_CAVS_2,
};

/**
 * struct sof_ipc4_fw_data - IPC4-specific data
 * @manifest_fw_hdr_offset: FW header offset in the manifest
 * @num_fw_modules : Number of modules in base FW
 * @fw_modules: Array of base FW modules
 * @nhlt: NHLT table either from the BIOS or the topology manifest
 * @mtrace_type: mtrace type supported on the booted platform
 * @mtrace_log_bytes: log bytes as reported by the firmware via fw_config reply
 */
struct sof_ipc4_fw_data {
	u32 manifest_fw_hdr_offset;
	int num_fw_modules;
	void *fw_modules;
	void *nhlt;
	enum sof_ipc4_mtrace_type mtrace_type;
	u32 mtrace_log_bytes;
};

/**
 * struct sof_ipc4_fw_module - IPC4 module info
 * @sof_man4_module : Module info
 * @m_ida: Module instance identifier
 * @bss_size: Module object size
 * @private: Module private data
 */
struct sof_ipc4_fw_module {
	struct sof_man4_module man4_module_entry;
	struct ida m_ida;
	u32 bss_size;
	void *private;
};

extern const struct sof_ipc_fw_loader_ops ipc4_loader_ops;
extern const struct sof_ipc_tplg_ops ipc4_tplg_ops;
extern const struct sof_ipc_tplg_control_ops tplg_ipc4_control_ops;
extern const struct sof_ipc_pcm_ops ipc4_pcm_ops;
extern const struct sof_ipc_fw_tracing_ops ipc4_mtrace_ops;

int sof_ipc4_set_pipeline_state(struct snd_sof_dev *sdev, u32 id, u32 state);
int sof_ipc4_mtrace_update_pos(struct snd_sof_dev *sdev, int core);
#endif
