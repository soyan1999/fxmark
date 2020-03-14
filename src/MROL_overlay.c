#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include "fxmark.h"
#include "util.h"

static void set_test_root(struct worker *worker, char *test_root)
{
    struct fx_opt *fx_opt = fx_opt_worker(worker);
    sprintf(test_root, "%s/%d", fx_opt->root, worker->id);
}

static int pre_work(struct worker *worker)
{
    struct bench *bench = worker->bench;
    char test_root[PATH_MAX];
    char test_dir[PATH_MAX];
    char merged[PATH_MAX];
    char upper[PATH_MAX];
    char lower[PATH_MAX];
    char work[PATH_MAX];
    char cmd[PATH_MAX];
    char file[PATH_MAX];
	int fd, rc = 0;
    int ncpu = bench->ncpu;
    int max = FILE_MAX / ncpu / 2;

    /* create test root */
    set_test_root(worker, test_root);
    sprintf(merged, "%s/merged", test_root);
    sprintf(test_dir, "%s/lower/dir", test_root);
    sprintf(upper, "%s/upper", test_root);
    sprintf(lower, "%s/lower", test_root);
    sprintf(work, "%s/work", test_root);

	rc = mkdir_p(merged);
    if (rc) goto err_out;
    rc = mkdir_p(test_dir);
    if (rc) goto err_out;
    rc = mkdir_p(upper);
    if (rc) goto err_out;
    rc = mkdir_p(work);
    if (rc) goto err_out;

	/* create test file */
    for(;worker->private[0] < max;++worker->private[0]){
	    sprintf(file, "%s/n-%" PRIu64 ".dat", test_dir, worker->private[0]);
	    if ((fd = open(file, O_CREAT | O_RDWR, S_IRWXU)) == -1)
        {
            if(errno == ENOSPC){
                --worker->private[0];
                rc = 0;
                goto out;
            }
            rc = errno;
            goto err_out;
        }
        close(fd);
    }
out:
    return rc;
err_out:
	bench->stop = 1;
	rc = errno;
	goto out;
}

static int main_work(struct worker *worker)
{
	struct bench *bench = worker->bench;
	int fd, rc = 0;
	uint64_t iter = 0;
    char test_root[PATH_MAX];
    char test_dir[PATH_MAX];
    char file[PATH_MAX];

    set_test_root(worker, test_root);
    sprintf(test_dir, "%s/merged/dir", test_root);
	for (iter = 0; iter < worker->private[0] && !bench->stop; ++iter) {
	    sprintf(file, "%s/n-%" PRIu64 ".dat", test_dir,iter);
	    if ((fd = open(file, O_RDWR, S_IRWXU)) == -1){
            rc = errno;
            goto err_out;
        }
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

struct bench_operations o_opn_lw_ops = {
	.pre_work  = pre_work, 
	.main_work = main_work,
    .post_work = post_work,
};
