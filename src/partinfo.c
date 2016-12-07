/*
 * Print partition name and flags *only*, not including any other
 * meta-data whatsoever.
 *
 * Written by Lucas C. Villa Real <lucasvr@gobolinux.org>
 * Released under the GNU GPL version 2 or above.
 */
#include <stdio.h>
#include <parted/parted.h>

int main(int argc, char **argv)
{
	PedPartitionFlag flag = 0;
	PedPartition *part = NULL;
	PedDevice *dev = NULL;
	PedDisk *disk = NULL;
	char *user_dev;
	char buf[128];
	size_t n = 0, left = sizeof(buf) - 1;

	user_dev = argc > 1 ? argv[1] : NULL;

	ped_device_probe_all();
	while ((dev = ped_device_get_next(dev))) {
		if (user_dev && strcmp(user_dev, dev->path))
			continue;
		disk = ped_disk_new(dev);
		while ((part = ped_disk_next_partition(disk, part))) {
			if (part->num < 0)
				continue;
			memset(buf, 0, sizeof(buf));
			n += snprintf(buf, left, "%s%d:", dev->path, part->num);
			left -= n;
			while ((flag = ped_partition_flag_next(flag))) {
				if (ped_partition_get_flag(part, flag) && left > 0) {
					n += snprintf(&buf[n], left, "%s,", ped_partition_flag_get_name(flag));
					left -= n;
				}
			}
			if (buf[n-1] == ',')
				buf[n-1] = '\0';
			printf("%s\n", buf);
			left = sizeof(buf) - 1;
			flag = 0;
			n = 0;
		}
		part = NULL;
		ped_disk_destroy(disk);
	}
	ped_device_free_all();
	return 0;
}
