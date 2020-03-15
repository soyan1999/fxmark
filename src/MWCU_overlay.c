/**
 * Nanobenchmark: ADD
 *   IA. PROCESS = {create empty files at /test/$PROCESS}
 *      - TEST: inode alloc
 */	      
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include "fxmark.h"
#include "util.h"

static void set_test_root(struct worker *worker, char *test_root)
{
	struct fx_opt *fx_opt = fx_opt_worker(worker);
	sprintf(test_root, "%s/%d", fx_opt->root, worker->id);
}

static int pre_work(struct worker *worker)
{
	char test_root[PATH_MAX];
    char test_dir[PATH_MAX];
    char merged[PATH_MAX];
    char upper[PATH_MAX];
    char lower[PATH_MAX];
    char work[PATH_MAX];
    char cmd[PATH_MAX];
    int rc = 0;

	set_test_root(worker, test_root);
    sprintf(merged, "%s/merged", test_root);
    sprintf(test_dir, "%s/upper/dir", test_root);
    sprintf(upper, "%s/upper", test_root);
    sprintf(lower, "%s/lower", test_root);
    sprintf(work, "%s/work", test_root);

	rc = mkdir_p(merged);
    if (rc) goto err_out;
    rc = mkdir_p(test_dir);
    if (rc) goto err_out;
    rc = mkdir_p(lower);
    if (rc) goto err_out;
    rc = mkdir_p(work);
    if (rc) goto err_out;

    sprintf(cmd, "sudo mount -t overlay overlay -o lowerdir=%s,upperdir=%s,workdir=%s %s", lower, upper, work, merged);
    if (system(cmd)) goto err_out;

out:
	return rc; 
err_out:
	rc = errno;
	goto out;
}

static int main_work(struct worker *worker)
{
	char test_root[PATH_MAX];
    char test_dir[PATH_MAX];
	struct bench *bench = worker->bench;
	uint64_t iter;
	int rc = 0;

	set_test_root(worker, test_root);
    sprintf(test_dir, "%s/merged/dir", test_root);
	for (iter = 0; !bench->stop; ++iter) {
		char file[PATH_MAX];
		int fd;
		/* create and close */
		snprintf(file, PATH_MAX, "%s/crt-%" PRIu64 ".dat", 
			 test_dir, iter);
		if ((fd = open(file, O_CREAT | O_RDWR, S_IRWXU)) == -1)
			goto err_out;
		close(fd);
	}
out:
	worker->works = (double)iter;
	return rc;
err_out:
	bench->stop = 1;
	rc = errno;
	goto out;
}

static int post_work(struct worker *worker) {
    char test_root[PATH_MAX];
    char cmd[PATH_MAX];
    int rc = 0;

    set_test_root(worker, test_root);
    sprintf(cmd, "sudo umount %s/merged", test_root);
    if(system(cmd)) goto err_out;

out:
    return rc;
err_out:
    rc = errno;
    goto out;
}

struct bench_operations o_crt_up_ops = {
	.pre_work  = pre_work, 
	.main_work = main_work,
    .post_work = post_work,
};
