/*
 * Copyright (C) 2016 Red Hat, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Todd Gill <tgill@redhat.com>
 */

#ifndef LIB_LIBSTRATIS_H_
#define LIB_LIBSTRATIS_H_

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>
/*
 * stratis_ctx
 *
 * library user context - reads the config and system
 * environment, user variables, allows custom logging
 */
struct stratis_ctx;
struct stratis_ctx *stratis_ref(struct stratis_ctx *ctx);
struct stratis_ctx *stratis_unref(struct stratis_ctx *ctx);

int stratis_context_new(struct stratis_ctx **ctx);

void stratis_set_log_fn(struct stratis_ctx *ctx,
        void (*log_fn)(struct stratis_ctx *ctx, int priority, const char *file,
                int line, const char *fn, const char *format, va_list args));
int stratis_get_log_priority(struct stratis_ctx *ctx);
void stratis_set_log_priority(struct stratis_ctx *ctx, int priority);
void *stratis_get_userdata(struct stratis_ctx *ctx);
void stratis_set_userdata(struct stratis_ctx *ctx, void *userdata);

char * stratis_get_user_message(int stratis_code);
char * stratis_get_code_token(int stratis_code);
char * stratis_get_dev_type_message(int stratis_code);
char * stratis_raid_user_message(int stratis_code);
char * stratis_get_raid_token(int stratis_code);
char * stratis_get_dev_type_token(int stratis_code);


#define MAX_STRATIS_NAME_LEN 256


	/** Unknown */
#define		STRATIS_DEV_TYPE_UNKNOWN	-1
#define		STRATIS_DEV_TYPE_REGULAR	0
#define 	STRATIS_DEV_TYPE_CACHE		1
#define		STRATIS_DEV_TYPE_SPARE		2
#define 	STRATIS_DEV_TYPE_MAX		3

typedef struct scache_table {
	GHashTable *table;
} scache_table_t;

typedef struct sdev_table {
	GHashTable *table;
} sdev_table_t;

typedef struct svolume_table {
	GHashTable *table;
} svolume_table_t;

typedef struct spool_table {
	GHashTable *table;
} spool_table_t;

typedef struct spool {
	int id;
	int size;
	char name[MAX_STRATIS_NAME_LEN];
	char dbus_name[MAX_STRATIS_NAME_LEN];
	sd_bus_slot *slot;
	sdev_table_t *sdev_table;
	svolume_table_t *svolume_table;
	scache_table_t *scache_table;
} spool_t;

typedef struct svolume {
	int id;
	int size;
	spool_t *parent_spool;
	struct svolume *parent_volume;
	char name[MAX_STRATIS_NAME_LEN];
	char mount_point[MAX_STRATIS_NAME_LEN];
	char quota[MAX_STRATIS_NAME_LEN];
	char dbus_name[MAX_STRATIS_NAME_LEN];
	sd_bus_slot *slot;
} svolume_t;

typedef struct sdev {
	int id;
	int size;
	spool_t *parent_spool;
	char name[MAX_STRATIS_NAME_LEN];
	char dbus_name[MAX_STRATIS_NAME_LEN];
	sd_bus_slot *slot;
	int type;
} sdev_t;

typedef struct scache {
	int id;
	int size;
	spool_t *parent_spool;
	char name[MAX_STRATIS_NAME_LEN];
	char dbus_name[MAX_STRATIS_NAME_LEN];
	sd_bus_slot *slot;
	int type;
} scache_t;




/* Return codes */
#define STRATIS_OK					0		/* Ok */
#define STRATIS_ERROR				1
#define STRATIS_NULL				2
#define STRATIS_MALLOC				3
#define STRATIS_NOTFOUND			4
#define STRATIS_POOL_NOTFOUND		5
#define STRATIS_VOLUME_NOTFOUND		6
#define STRATIS_DEV_NOTFOUND		7
#define STRATIS_CACHE_NOTFOUND		8
#define STRATIS_BAD_PARAM			9
#define STRATIS_ALREADY_EXISTS		10
#define STRATIS_NULL_NAME			11
#define STRATIS_NO_POOLS			12
#define STRATIS_LIST_FAILURE		13
#define STRATIS_ERROR_MAX			14


#define	STRATIS_RAID_TYPE_UNKNOWN	-1
	/** Single */
#define	STRATIS_RAID_TYPE_SINGLE	0
	/** Mirror between two disks. For 4 disks or more, they are RAID10.*/
#define	STRATIS_RAID_TYPE_RAID1		1
	/** Block-level striping with distributed parity */
#define	STRATIS_RAID_TYPE_RAID5		2
	/** Block-level striping with two distributed parities, aka, RAID-DP */
#define	STRATIS_RAID_TYPE_RAID6		3
#define STRATIS_RAID_TYPE_MAX		4


/*
 * Stratis
 *
 */

int stratis_sdev_get(struct stratis_ctx *ctx, sdev_t **sdev, char *name);
int stratis_cache_get(struct stratis_ctx *ctx, scache_t **sdev, char *name);

/*
 * Pools
 */

int stratis_spool_create(struct stratis_ctx *ctx, spool_t **spool, const char *name,
        sdev_table_t *disk_table, int raid_level);
int stratis_spool_destroy(struct stratis_ctx *ctx, spool_t *spool);
int stratis_spool_get(struct stratis_ctx *ctx, spool_t **spool, char *name);
char *stratis_spool_get_name(spool_t *spool);
int stratis_spool_get_id(spool_t *spool);
int stratis_spool_get_list(struct stratis_ctx *ctx, spool_table_t **spool_list);
int stratis_spool_add_dev(spool_t *spool,  sdev_t *sdev);
int stratis_spool_remove_devs(spool_t *spool, sdev_table_t *sdev_table);
int stratis_spool_remove_dev(spool_t *spool, char *name);
int stratis_spool_get_dev_table(spool_t *spool, sdev_table_t **sdev_table);

int stratis_spool_add_cache(spool_t *spool, scache_t *scache);
int stratis_spool_remove_cache_devs(spool_t *spool, sdev_table_t *scache_table);
int stratis_spool_get_cache_dev_table(spool_t *spool, scache_table_t **scache_table);

int stratis_spool_get_volume_list(spool_t *spool,
        svolume_table_t **svolume_table);
int stratis_spool_list_size(spool_table_t *spool_list, int *list_size);
int stratis_spool_table_find(spool_table_t *spool_list, spool_t **spool,
        char *name);


/*
 * Volumes
 */
int stratis_svolume_create(svolume_t **svolume, spool_t *spool, char *name,
        char *mount_point, char *qutoa);
int stratis_svolume_destroy(svolume_t *svolume);
int stratis_svolume_get(struct stratis_ctx *ctx, svolume_t **svolume, char *poolname, char *volumename);
char * stratis_svolume_get_name(svolume_t *svolume);
int stratis_svolume_rename(svolume_t *svolume, char *name);
int stratis_svolume_set_quota(svolume_t *svolume, char *quota);
int stratis_svolume_set_mount_point(svolume_t *svolume, char *mount_point);
int stratis_svolume_get_id(svolume_t *svolume);
char *stratis_svolume_get_mount_point(svolume_t *svolume);

int stratis_svolume_table_create(svolume_table_t **svolume_table);
int stratis_svolume_table_destroy(svolume_table_t *svolume_table);
int stratis_svolume_table_eligible_disks(sdev_table_t **disk_table);
int stratis_svolume_table_devs(spool_t *spool, sdev_table_t **disk_table);
int stratis_svolume_table_size(svolume_table_t *svolume_table, int *list_size);
int stratis_svolume_table_find(svolume_table_t *svolume_table, svolume_t **svolume,
        char *name);
int stratis_svolume_create_snapshot(svolume_t *svolume, spool_t *spool,
			svolume_t **snapshot, char *name);


/*
 * Devs
 */
int stratis_sdev_create(sdev_t **sdev, spool_t *spool,char *name,
		int type);

char *stratis_sdev_get_name(sdev_t *sdev);
int stratis_sdev_get_id(sdev_t *sdev);

/*
 * Cache
 */

int stratis_scache_create(scache_t **scache, spool_t *spool,char *name,
		int type);
char *stratis_scache_get_name(scache_t *scache);
int stratis_scache_get_id(scache_t *scache);
int stratis_scache_create(scache_t **scache, spool_t *spool,char *name, int type);


/*
 * Cache Lists
 */

int stratis_scache_table_create(scache_table_t **sdev_table);
int stratis_scache_table_destroy(scache_table_t *scache_table);

/*
 * Device Lists
 */

int stratis_sdev_table_create(sdev_table_t **sdev_table);
int stratis_sdev_table_destroy(sdev_table_t *sdev_table);
int stratis_sdev_table_add(sdev_table_t *sdev_table, sdev_t *sdev);
int stratis_sdev_table_remove(sdev_table_t **sdev_table, char *sdev);
int stratis_sdev_table_size(sdev_table_t *sdev_table, int *list_size);



#endif /* LIB_LIBSTRATIS_H_ */