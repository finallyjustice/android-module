#include <sys/ptrace.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <android/log.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

// TAG for logcat
#define TAG "ptrace_log"

// usage for command
static void help(int argc, char **argv)
{
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "Please use: %s [pid]\n", argv[0]);
}

// obtain the current system call number
static long get_syscall_no(int pid, struct pt_regs *regs)
{
	long sys_no = 0;
	ptrace(PTRACE_GETREGS, pid, NULL, regs);
	sys_no = ptrace(PTRACE_PEEKTEXT, pid, (void *)(regs->ARM_pc-4), NULL);

	if(sys_no == 0)
		return -1;

	if(sys_no == 0xef000000)
	{
		sys_no = regs->ARM_r7;
	}
	else
	{
		if((sys_no&0x0ff00000) != 0x0f900000)
			return -1;
		sys_no &= 0x000fffff;
	}

	return sys_no;
}

// process the information of system call
static void monitor_syscall(pid_t pid)
{
	long sys_no = 0;
	struct pt_regs regs;
	sys_no = get_syscall_no(pid, &regs);

	if(sys_no <= 0)
	{
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "get syscall no error");
	}
	else if(sys_no == __NR_ioctl)
	{
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "monitor ioctl syscall");
	}
	else
	{
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "monitor other syscall");	
	}
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		help(argc, argv);
		return 1;
	}

	// pid of monitored process
	pid_t target_pid;
	// status of monitored process
	int status;
	// obtain pid from input parameters
	target_pid = atoi(argv[1]);

	if(target_pid < 0)
	{
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "invalid process pid");
		return 1;
	}

	// attach to target process
	if(ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) != 0)
	{
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "ptrace attach error: %s", strerror(errno));
		return 1;
	}

	__android_log_print(ANDROID_LOG_DEBUG, TAG, "start monitor process: %d", target_pid);

	while(1)
	{
		wait(&status);
		if(WIFEXITED(status))
			break;
		monitor_syscall(target_pid);
		ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);
	}

	// detach from target process
	ptrace(PTRACE_DETACH, target_pid, NULL, NULL);
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "monitor finished");   

	return 0;
}
