#ifndef __FX_H__
#define __FX_H__
#include <linux/limits.h>
#include "bench.h"

#define FX_OPT_MAX_PRIVATE 4
#define FILE_MAX 10000000
#define BLOCK_MAX 20000000

struct fx_opt {
	char root[PATH_MAX];
	uint64_t private[FX_OPT_MAX_PRIVATE];
};

#define fx_opt_bench(__b) ((struct fx_opt *)((__b)->args))
#define fx_opt_worker(__w)  fx_opt_bench((__w)->bench)

struct cmd_opt {
	struct bench_operations *ops;
	int ncore;
	int nbg;
	int duration;
	int directio;
	char *root;
	char *profile_start_cmd;
	char *profile_stop_cmd;
	char *profile_stat_file;
};

/* benchmarks */ 
extern struct bench_operations n_inode_alloc_ops;
extern struct bench_operations n_blk_alloc_ops;
extern struct bench_operations n_blk_wrt_ops;
extern struct bench_operations n_dir_ins_ops;
extern struct bench_operations n_jnl_cmt_ops;
extern struct bench_operations n_mtime_upt_ops;
extern struct bench_operations n_file_rename_ops;
extern struct bench_operations n_file_rd_ops;
extern struct bench_operations n_file_rd_bg_ops;
extern struct bench_operations n_shfile_rd_ops;
extern struct bench_operations n_shfile_rd_bg_ops;
extern struct bench_operations n_shblk_rd_ops;
extern struct bench_operations n_shblk_rd_bg_ops;
extern struct bench_operations n_dir_rd_ops;
extern struct bench_operations n_dir_rd_bg_ops;
extern struct bench_operations n_shdir_rd_ops;
extern struct bench_operations n_shdir_rd_bg_ops;
extern struct bench_operations n_priv_path_rsl_ops;
extern struct bench_operations n_path_rsl_ops;
extern struct bench_operations n_path_rsl_bg_ops;
extern struct bench_operations n_spath_rsl_ops;
extern struct bench_operations u_file_cr_ops;
extern struct bench_operations u_file_rm_ops;
extern struct bench_operations u_sh_file_rm_ops;
extern struct bench_operations u_file_tr_ops;

extern struct bench_operations o_opn_hst_ops;
extern struct bench_operations o_opn_up_ops;
extern struct bench_operations o_opn_lw_ops;
extern struct bench_operations o_crt_hst_ops;
extern struct bench_operations o_crt_up_ops;
extern struct bench_operations o_crt_lw_ops;
extern struct bench_operations o_ulnk_hst_ops;
extern struct bench_operations o_ulnk_up_ops;
extern struct bench_operations o_ulnk_lw_ops;
extern struct bench_operations o_rnm_up_ops;
extern struct bench_operations o_rdir_hst_ops;
extern struct bench_operations o_rdir_up_ops;
extern struct bench_operations o_rdir_lw_ops;
extern struct bench_operations o_rd_hst_ops;
extern struct bench_operations o_rd_up_ops;
extern struct bench_operations o_rd_lw_ops;
extern struct bench_operations o_wrt_hst_ops;
extern struct bench_operations o_wrt_up_ops;
extern struct bench_operations o_wrt_lw_ops;
extern struct bench_operations o_apd_hst_ops;
extern struct bench_operations o_apd_up_ops;
extern struct bench_operations o_apd_lw_ops;
extern struct bench_operations o_trnc_hst_ops;
extern struct bench_operations o_trnc_up_ops;
extern struct bench_operations o_trnc_lw_ops;

#endif /* __FX_H__ */
