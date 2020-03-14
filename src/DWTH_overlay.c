/**
 * Nanobenchmark: Read operation
 *   RF. PROCESS = {read private file}
 */
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
    char *page=NULL;
    struct bench *bench = worker->bench;
    char test_root[PATH_MAX];
    char file[PATH_MAX];
    int fd=-1, rc = 0;

    /* allocate data buffer aligned with pagesize*/
    if(posix_memalign((void **)&(worker->page), PAGE_SIZE, PAGE_SIZE))
        goto err_out;
    page = worker->page;
    if (!page)
        goto err_out;

    /* create test root */
    set_test_root(worker, test_root);
    rc = mkdir_p(test_root);
    if (rc) return rc;

    /* create a test file */
    snprintf(file, PATH_MAX, "%s/n_file_rd.dat", test_root);
    if ((fd = open(file, O_CREAT | O_RDWR | O_LARGEFILE, S_IRWXU)) == -1)
        goto err_out;

    /* set flag with O_DIRECT if necessary*/
    if(bench->directio && (fcntl(fd, F_SETFL, O_DIRECT) == -1))
        goto err_out;

    for(worker->private[0] = 0; worker->private[0] < 100000; worker->private[0]++) {
        rc = write(fd, page, PAGE_SIZE);
        if (rc != PAGE_SIZE) {
            if (errno == ENOSPC) {
                worker->private[0]--;
                break;
            }
            goto err_out;
        }
    }
    rc = 0;

out:
    /* put fd to worker's private */
    worker->private[1] = (uint64_t)fd;
    free(page);
    worker->page = NULL;
	return rc;
err_out:
	bench->stop = 1;
	rc = errno;
	goto out;
}

static int main_work(struct worker *worker)
{
    struct bench *bench = worker->bench;
    int fd=-1, rc = 0;
    uint64_t iter = 0;

    fd = (int)worker->private[1];
    for (iter = worker->private[0] - 1; iter > 0 && !bench->stop; --iter) {
        if (ftruncate(fd, iter * PAGE_SIZE) == -1) {
            rc = errno;
            goto err_out;
        }
    }
out:
	close(fd);
	worker->works = (double)(worker->private[0] - iter);
	return rc;
err_out:
	bench->stop = 1;
	rc = errno;
	goto out;
}

struct bench_operations o_trnc_hst_ops = {
    .pre_work  = pre_work,
    .main_work = main_work,
};
