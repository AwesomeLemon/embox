/**
 * @file
 * @brief
 *
 * @date 05.07.2012
 * @author Andrey Gazukin
 * @author Anton Kozlov
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <fs/vfs.h>
#include <fs/fs_driver.h>
#include <fs/fsop.h>
#include <sys/file.h>
#include <embox/block_dev.h>
#include <drivers/ramdisk.h>
#include <kernel/thread.h>

#include <cmd/shell.h>

#include <mem/page.h>
#include <embox/test.h>

#include <err.h>

__EMBOX_TEST_SUITE_AUTORUN("fs/file test", false);
static int exec_shell_cmd(char *cmdline) __attribute__((used));

#if 0
TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME			"vfat"
#define FS_DEV			"/dev/ramdisk"
#define FS_TYPE			12
#define FS_BLOCKS		124
#define MKDIR_PERM		0700
#define FS_DIR			"/tmp"
#define FS_DIR			"/tmp"
#define FS_FILE1		"/tmp/1/2/3/1.txt"
#define FS_FILE2		"/tmp/1/2/3/2.txt"
#define FS_DIR3			"/tmp/1/2/3"
#define FS_DIR2			"/tmp/1/2"
#define FS_DIR1			"/tmp/1"
#define FS_TESTDATA		"qwerty\n"
#define FS_DTR			"/tmp/dtr"
#define FS_MV_SUB		"/tmp/dtr/sub"
#define FS_MV_SUB_F1	"/tmp/dtr/sub/file1"
#define FS_MV_F1		"/tmp/dtr/file1"
#define FS_MV_F2		"/tmp/dtr/sub/file2"
#define FS_MV_F2_NAME	"file2"
#define FS_MV_F3		"/tmp/dtr/sub/file3"
#define FS_MV_F3_NAME	"file3"
#define FS_MV_RENAMED	"/tmp/renamed"
#define FS_TESTDATA		"qwerty\n"
#define FS_MV_LONGNAME	"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnamtoolongnamtoolongnamtoolongnamtoolongnam" \
						"toolongnam"
#define FS_FLOCK		"/tmp/flock"
#endif

#define FS_TEST_MOUNTPOINT "/mnt/fs_test"

static char fs_test_temp_buf[512];

static const char fs_test_rd_file[] = FS_TEST_MOUNTPOINT "/rd_file";
static const char fs_test_rd_file_content[] = "This is read-only file with original content string";
static void fs_test_read(void) {
	int fd;
	int nread;

	test_assert(0 <= (fd = open(fs_test_rd_file, O_RDONLY)));

	nread = read(fd, fs_test_temp_buf, sizeof(fs_test_temp_buf));

	test_assert_equal(nread, sizeof(fs_test_rd_file_content));
	test_assert_zero(strcmp(fs_test_temp_buf, fs_test_rd_file_content));

	close(fd);
}

static const struct stat fs_test_rd_file_stat = {
	.st_mode = 0,
	.st_uid = 0,
	.st_gid = 0,
	.st_size = 0,
};
static void fs_test_stat(void) {
	struct stat st, fst;
	int fd;

	stat(fs_test_rd_file, &st);

	test_assert(0 <= (fd = open(fs_test_rd_file, O_RDONLY)));
	test_assert_zero(fstat(fd, &fst));
	close(fd);

	test_assert(0 == memcmp(&st, &fst, sizeof(struct stat)));

	test_assert_equal(st.st_mode, fs_test_rd_file_stat.st_mode);
	test_assert_equal(st.st_uid, fs_test_rd_file_stat.st_uid);
	test_assert_equal(st.st_gid, fs_test_rd_file_stat.st_gid);
	test_assert_equal(st.st_size, fs_test_rd_file_stat.st_size);
}

TEST_CASE("Test read-only operations on fs") {

	fs_test_read();

	fs_test_stat();
}

#if 0
static void fs_test_write(void) {
	int file;

	test_assert(0 <= (file = open(FS_FILE1, O_WRONLY)));
	test_assert_zero(lseek(file, 0, SEEK_END));
	test_assert(0 < write(file, FS_TESTDATA, strlen(FS_TESTDATA)));
	test_assert_zero(close(file));
}


static void fs_test_copy(void) {
	int src_file, dst_file;
	char buf[PAGE_SIZE()];
	int bytesread;

	test_assert(0 <=  (src_file = open(FS_FILE1, O_RDONLY)));
	test_assert(0 <=  (dst_file = open(FS_FILE2, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)));
	test_assert_zero(lseek(dst_file, 0, SEEK_SET));

	bytesread = 0;
	while (1) {
		test_assert(0 <=  (bytesread = read(src_file, buf, PAGE_SIZE())));
		if(0 >= bytesread) {
			break;
		}
		test_assert(0 < write (dst_file, buf, bytesread));
	}

	test_assert_zero(close(src_file));
	test_assert_zero(close(dst_file));
}

static void fs_test_rename(void) {
	/* Prepare directories and files for tests */
	test_assert_zero(mkdir(FS_DTR, MKDIR_PERM));
	test_assert_zero(mkdir(FS_MV_SUB, MKDIR_PERM));
	test_assert(-1 != creat(FS_MV_F1, S_IRUSR | S_IWUSR));
	test_assert(-1 != creat(FS_MV_F2, S_IRUSR | S_IWUSR));

	/* Check error code for non-existent file */
	test_assert(-1 == rename("no_such_file", FS_MV_F3));
	test_assert(ENOENT == errno);

	/* Check error code if destination file exists */
	test_assert(-1 == rename(FS_MV_F1, FS_MV_F2));
	test_assert(EINVAL == errno);

	/* Check error code with too long source file name */
	test_assert(-1 == rename(FS_MV_LONGNAME, "no_matter"));
	test_assert(ENAMETOOLONG == errno);

	/* Check error code with too long destination file name */
	test_assert(-1 == rename("no_matter", FS_MV_LONGNAME));
	test_assert(ENAMETOOLONG == errno);

	/* Test with relative paths */
#ifdef ENABLE_RELATIVE_PATH
	test_assert_zero(rename(FS_MV_F2, FS_MV_F3_NAME));
	test_assert_zero(rename(FS_MV_F3, FS_MV_F2_NAME));
#endif

	/* Renaming one file and renaming it back */
	test_assert_zero(rename(FS_MV_F2, FS_MV_F3));
	test_assert_zero(rename(FS_MV_F3, FS_MV_F2));

	/* Add test with recursive renaming */
	test_assert_zero(rename(FS_DTR, FS_MV_RENAMED));
	test_assert_zero(rename(FS_MV_RENAMED, FS_DTR));

	/* Add tests with directory as destination */
	test_assert_zero(rename(FS_MV_F1, FS_MV_SUB));
	test_assert_zero(rename(FS_MV_SUB_F1, FS_DTR));

	/* Test cleanup */
	test_assert_zero(remove(FS_DTR));
}

static void fs_test_move(void) {
	/* This should be improved to not use hard-coded paths */
	char *cmd_recursive_err = "mv /tmp/dtr /tmp/tmpdtr";
	char *cmd_force_err = "mv /tmp/dtr/file1 /tmp/dtr/sub/file2";
	char *cmd_multi_err =
				"mv /tmp/dtr/file1 /tmp/dtr/sub/file2 /tmp/file";

	char *cmd_simple = "mv /tmp/dtr/file1 /tmp/dtr/sub/tmpfile";
	char *cmd_simple_back = "mv /tmp/dtr/sub/tmpfile /tmp/dtr/file1";
	char *cmd_recursive = "mv -r /tmp/dtr /tmp/tmpdtr";
	char *cmd_recursive_back =  "mv -r /tmp/tmpdtr /tmp/dtr";
	char *cmd_multi =
			"mv /tmp/dtr/file1 /tmp/dtr/sub/file2 /tmp";
	char *cmd_force = "mv -f /tmp/file1 /tmp/file2";

	/* Prepare directories and files for tests */
	test_assert_zero(mkdir(FS_DTR, MKDIR_PERM));
	test_assert_zero(mkdir(FS_MV_SUB, MKDIR_PERM));
	test_assert(-1 != creat(FS_MV_F1, S_IRUSR | S_IWUSR));
	test_assert(-1 != creat(FS_MV_F2, S_IRUSR | S_IWUSR));

	/**
	 * Error codes tests
	 */

	/* Move directory without recursive key */
	test_assert(-EINVAL == exec_shell_cmd(cmd_recursive_err));

	/* Overwrite without force key */
	test_assert(-EINVAL == exec_shell_cmd(cmd_force_err));

	/* Multi-source with not directory as destination */
	test_assert(-EINVAL == exec_shell_cmd(cmd_multi_err));

	/**
	 * Valid tests
	 */

	/* Simple test */
	test_assert_zero(exec_shell_cmd(cmd_simple));
	test_assert_zero(exec_shell_cmd(cmd_simple_back));

	/* Recursive test */
	test_assert_zero(exec_shell_cmd(cmd_recursive));
	test_assert_zero(exec_shell_cmd(cmd_recursive_back));

	/* Multi-source test */
	test_assert_zero(exec_shell_cmd(cmd_multi));

	/* Force test */
	test_assert_zero(exec_shell_cmd(cmd_force));

	/* Test cleanup */
	test_assert_zero(remove(FS_DTR));
}


TEST_CASE("generic file test") {

	fs_test_write();

	fs_test_copy();

	fs_test_rename();

	fs_test_move();

	/*fs_test_flock();*/
}
#endif

/* Exec shell command and return it's exit code */
static int exec_shell_cmd(char *cmdline) {
	static const struct shell *sh;

	if (!sh) {
		sh = shell_any();
		if (!sh) {
			return -ENOSYS;
		}
	}

	return shell_exec(sh, cmdline);
}

#if 0
static struct thread *fftt, *sftt;
static void *first_flock_test_thread(void *arg);
static void *second_flock_test_thread(void *arg);

static void fs_test_flock(void) {
	int fd;
	sched_priority_t l = 200, h = 210;

	/* Prepare file and threads for test */
	test_assert(-1 != (fd = open(FS_FLOCK, O_CREAT, S_IRUSR | S_IWUSR)));

	fftt = thread_create(THREAD_FLAG_SUSPENDED, first_flock_test_thread,
			(void *) &fd);
	test_assert_zero(err(fftt));


	sftt = thread_create(THREAD_FLAG_SUSPENDED, second_flock_test_thread,
			(void *) &fd);
	test_assert_zero(err(sftt));

	test_assert_zero(thread_set_priority(fftt, l));
	test_assert_zero(thread_set_priority(sftt, h));

	test_assert_zero(thread_launch(fftt));
	test_assert_zero(thread_join(fftt, NULL));
	test_assert_zero(thread_join(sftt, NULL));

	test_assert_emitted("abcdefg");

	/* Test cleanup */
	test_assert_zero(remove(FS_FLOCK));
}

static void *first_flock_test_thread(void *arg) {
	int fd = *((int *) arg);

	test_emit('a');
	test_assert_zero(flock(fd, LOCK_EX));
	test_emit('b');
	test_assert_zero(thread_launch(sftt));
	test_emit('d');
	test_assert_zero(flock(fd, LOCK_UN));
	test_emit('g');

	return NULL;
}

static void *second_flock_test_thread(void *arg) {
	int fd = *((int *) arg);

	test_emit('c');

	/* Try to non-blocking acquire busy lock */
	test_assert(-1 == flock(fd, LOCK_EX | LOCK_NB));
	test_assert(EWOULDBLOCK == errno);

	/* Acquire just unlocked by fftt exclusive lock */
	test_assert_zero(flock(fd, LOCK_EX));
	test_emit('e');

	/* Convert lock to shared */
	test_assert_zero(flock(fd, LOCK_SH));

	/* Acquire one more shared lock */
	test_assert_zero(flock(fd, LOCK_SH | LOCK_NB));

	/* Release first shared lock */
	test_assert_zero(flock(fd, LOCK_UN));

	/* Convert share to exclusive */
	test_assert_zero(flock(fd, LOCK_EX));

	/* Release second lock */
	test_assert_zero(flock(fd, LOCK_UN | LOCK_NB));

	test_emit('f');

	return NULL;
}
#endif

#if 0
static int setup_suite(void) {
	int fd, res;

	if (0 != (res = err(ramdisk_create(FS_DEV, FS_BLOCKS * PAGE_SIZE())))) {
		return res;
	}

	/* format filesystem */
	if (0 != (res = format(FS_DEV, FS_NAME))) {
		return res;
	}

	/* mount filesystem */
	if (0 != (res = mount(FS_DEV, FS_DIR, FS_NAME))) {
		return res;
	}

	if (0 != (res = mkdir(FS_DIR1, MKDIR_PERM))) {
		return res;
	}

	if (0 != (res = mkdir(FS_DIR2, MKDIR_PERM))) {
		return res;
	}

	if (0 != (res = mkdir(FS_DIR3, 0777))) {
		return res;
	}

	if (-1 == (fd = creat(FS_FILE1, S_IRUSR | S_IWUSR))) {
		return -errno;
	}

	close(fd);

	return 0;
}

static int teardown_suite(void) {

	if (remove(FS_FILE1) ||	remove(FS_FILE2) ||
		remove(FS_DIR3)  ||	remove(FS_DIR2)  ||
		remove(FS_DIR1)) {
		return -1;
	}
	if (umount(FS_DIR) || ramdisk_delete(FS_DEV)) {
		return -1;
	}
	return 0;
}
#endif
