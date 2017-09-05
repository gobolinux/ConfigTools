/*
 * Print partition name and flags *only*, not including any other
 * meta-data whatsoever.
 *
 * Written by Lucas C. Villa Real <lucasvr@gobolinux.org>
 * Released under the GNU GPL version 2 or above.
 */
#include <stdio.h>
#include <ctype.h>
#include <parted/parted.h>

typedef enum {
	OP_NONE,
	OP_FLAGS,
	OP_FSTYPE,
	OP_NAME
} op_t;

void usage(char *programname, int exit_code)
{
	printf("Syntax: %s <option> [disk]\n\n", programname);
	printf("Available options are:\n"
			"  --flags         print partition flags\n"
			"  --fstype        print partition file system type\n"
			"  --name          print partition name\n"
			"\n");
	exit(exit_code);
}

size_t print_flags(PedPartition *part, char *buf, size_t n, size_t left)
{
	PedPartitionFlag flag = 0;
	size_t count = 0;

	while ((flag = ped_partition_flag_next(flag))) {
		if (ped_partition_get_flag(part, flag) && left > 0) {
			size_t bytecount = snprintf(&buf[n], left, "%s,", ped_partition_flag_get_name(flag));
			count += bytecount;
			left -= bytecount;
			n += bytecount;
		}
	}
	if (buf[n-1] == ',')
		buf[n-1] = '\0';
	return count;
}

size_t print_fstype(PedPartition *part, char *buf, size_t n, size_t left)
{
	const PedFileSystemType *fstype = part->fs_type;
	size_t count = 0;

	if (fstype && fstype->name)
		count = snprintf(&buf[n], left, "%s", fstype->name);
	return count;
}

size_t print_name(PedPartition *part, char *buf, size_t n, size_t left)
{
	const PedDiskType *disktype = part->disk->type;
	const char *name;
	size_t count = 0;

	if (disktype && disktype->name && !strcasecmp(disktype->name, "msdos")) {
		/* msdos partition tables don't support labeling partition */
		return 0;
	}
	name = ped_partition_get_name(part);
	if (name)
		count += snprintf(&buf[n], left, "%s", name);
	return count;
}

int main(int argc, char **argv)
{
	PedPartition *part = NULL;
	PedDevice *dev = NULL;
	PedDisk *disk = NULL;
	char *user_dev;
	op_t op = OP_NONE;

	if (argc == 1)
		usage(argv[0], 1);
	else if (! strcasecmp(argv[1], "--flags"))
		op = OP_FLAGS;
	else if (! strcasecmp(argv[1], "--fstype"))
		op = OP_FSTYPE;
	else if (! strcasecmp(argv[1], "--name"))
		op = OP_NAME;
	else {
		printf("Invalid option '%s'\n\n", argv[1]);
		usage(argv[0], 1);
	}
	user_dev = argc > 2 ? argv[2] : NULL;

	ped_device_probe_all();
	while ((dev = ped_device_get_next(dev))) {
		if (user_dev && strcmp(user_dev, dev->path))
			continue;
		disk = ped_disk_new(dev);
		while ((part = ped_disk_next_partition(disk, part))) {
			if (part->num < 0)
				continue;

			char buf[512];
			memset(buf, 0, sizeof(buf));
			size_t n = 0, left = sizeof(buf) - 1;

			int pathlen = strlen(dev->path);
			if (pathlen > 0 && isdigit(dev->path[pathlen-1])) {
				/* format is something like /dev/nvme0n1p1 */
				n += snprintf(buf, left, "%sp%d:", dev->path, part->num);
			} else {
				/* format is something like /dev/sda1 */
				n += snprintf(buf, left, "%s%d:", dev->path, part->num);
			}
			left -= n;

			if (op == OP_FLAGS)
				print_flags(part, buf, n, left);
			else if (op == OP_FSTYPE)
				print_fstype(part, buf, n, left);
			else if (op == OP_NAME)
				print_name(part, buf, n, left);

			printf("%s\n", buf);
		}
		part = NULL;
		ped_disk_destroy(disk);
	}
	ped_device_free_all();
	return 0;
}
