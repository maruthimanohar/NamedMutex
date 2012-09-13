#include "NamedMutex.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

NamedMutex::NamedMutex(const char *name)
{
	_key = atoi(name);
	_sem_id = semget(_key, 1, IPC_CREAT | 0666);
	printf("Process %d, key:%d semid:%d\n", getpid(), _key, _sem_id);
	_locked = 0;
}

void
NamedMutex::lock(void)
{
	if (_locked)
		return;
	int result = 0;
	int nsops = 2;
	struct sembuf sops[2];
	sops[0].sem_num = 0;
	sops[0].sem_op = 0;
	sops[0].sem_flg =  SEM_UNDO;

	sops[1].sem_num = 0;
	sops[1].sem_op = 1;
	sops[1].sem_flg = SEM_UNDO;

	if ((result = semop(_sem_id, sops, nsops)) == -1 )
	{
		perror("lock: semop failed:");
		exit(1);
	}
	_locked = 1;
	printf("Process %d , acquired the lock\n", getpid());
}

bool
NamedMutex::lock(int time_secs)
{
	if (_locked)
		return true;
	int result = 0;
	int nsops = 2;
	struct sembuf sops[2];
	sops[0].sem_num = 0;
	sops[0].sem_op = 0;
	sops[0].sem_flg = SEM_UNDO | IPC_NOWAIT;

	sops[1].sem_num = 0;
	sops[1].sem_op = 1;
	sops[1].sem_flg = SEM_UNDO | IPC_NOWAIT;

	if ((result = semop(_sem_id, sops, nsops)) == 0 )
	{
		_locked = 1;
		printf("Process %d, acquired the lock\n", getpid());
		return true;
	}
	if (errno != EAGAIN)
		return false;
	sleep(time_secs); // sleep for the specified time and try again.

	if ((result = semop(_sem_id, sops, nsops)) == 0 )
	{
		_locked = 1;
		printf("Process %d, acquired the lock in second attempt\n", getpid());
		return true;
	}
	printf("Process %d , unable to acquire the lock\n", getpid());
	return false;
}

void
NamedMutex::unlock(void)
{
	if (!_locked)
		return;
	int result = 0;
	int nsops = 1;
	struct sembuf sops[1];
	sops[0].sem_num = 0;
	sops[0].sem_op = -1;
	sops[0].sem_flg = SEM_UNDO;

	if ((result = semop(_sem_id, sops, nsops)) == -1 )
	{
		perror("unlock: semop failed:");
		exit(1);
	}
	_locked = 0;
	printf("Process %d , released the lock\n", getpid());
}
