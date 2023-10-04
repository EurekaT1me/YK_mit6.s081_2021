#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void) {
    int n;
    if (argint(0, &n) < 0)
        return -1;
    exit(n);
    return 0;  // not reached
}

uint64
sys_getpid(void) {
    return myproc()->pid;
}

uint64
sys_fork(void) {
    return fork();
}

uint64
sys_wait(void) {
    uint64 p;
    if (argaddr(0, &p) < 0)
        return -1;
    return wait(p);
}

uint64
sys_sbrk(void) {
    int addr;
    int n;

    if (argint(0, &n) < 0)
        return -1;
    addr = myproc()->sz;
    if (growproc(n) < 0)
        return -1;
    return addr;
}

uint64
sys_sleep(void) {
    int n;
    uint ticks0;

    if (argint(0, &n) < 0)
        return -1;
    acquire(&tickslock);
    ticks0 = ticks;
    while (ticks - ticks0 < n) {
        if (myproc()->killed) {
            release(&tickslock);
            return -1;
        }
        sleep(&ticks, &tickslock);
    }
    release(&tickslock);
    backtrace();

    return 0;
}

uint64
sys_kill(void) {
    int pid;

    if (argint(0, &pid) < 0)
        return -1;
    return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void) {
    uint xticks;

    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);
    return xticks;
}

uint64
sys_sigalarm(void) {
    int interval;
    uint64 handler;
    if (argint(0, &interval) < 0) {
        return -1;
    }
    if (argaddr(1, &handler) < 0) {
        return -1;
    }
    struct proc *p = myproc();
    p->interval = interval;
    p->handler = handler;
    p->last_interval = 0;
    p->intimer = 0;
//    printf("%d\n",p->interval);
//    printf("%p\n",p->handler);

    return 0;
}

uint64
sys_sigreturn(void) {
//    TODO: code here
    struct proc *p = myproc();
    p->trapframe->kernel_satp = p->timer_trapframe.kernel_satp;   // kernel page table
    p->trapframe->kernel_sp = p->timer_trapframe.kernel_sp;     // top of process's kernel stack
    p->trapframe->kernel_trap = p->timer_trapframe.kernel_trap;   // usertrap()
    p->trapframe->epc = p->timer_trapframe.epc;           // saved user program counter
    p->trapframe->kernel_hartid = p->timer_trapframe.kernel_hartid; // saved kernel tp
    p->trapframe->ra = p->timer_trapframe.ra;
    p->trapframe->sp = p->timer_trapframe.sp;
    p->trapframe->gp = p->timer_trapframe.gp;
    p->trapframe->tp = p->timer_trapframe.tp;
    p->trapframe->t0 = p->timer_trapframe.t0;
    p->trapframe->t1 = p->timer_trapframe.t1;
    p->trapframe->t2 = p->timer_trapframe.t2;
    p->trapframe->s0 = p->timer_trapframe.s0;
    p->trapframe->s1 = p->timer_trapframe.s1;
    p->trapframe->a0 = p->timer_trapframe.a0;
    p->trapframe->a1 = p->timer_trapframe.a1;
    p->trapframe->a2 = p->timer_trapframe.a2;
    p->trapframe->a3 = p->timer_trapframe.a3;
    p->trapframe->a4 = p->timer_trapframe.a4;
    p->trapframe->a5 = p->timer_trapframe.a5;
    p->trapframe->a6 = p->timer_trapframe.a6;
    p->trapframe->a7 = p->timer_trapframe.a7;
    p->trapframe->s2 = p->timer_trapframe.s2;
    p->trapframe->s3 = p->timer_trapframe.s3;
    p->trapframe->s4 = p->timer_trapframe.s4;
    p->trapframe->s5 = p->timer_trapframe.s5;
    p->trapframe->s6 = p->timer_trapframe.s6;
    p->trapframe->s7 = p->timer_trapframe.s7;
    p->trapframe->s8 = p->timer_trapframe.s8;
    p->trapframe->s9 = p->timer_trapframe.s9;
    p->trapframe->s10 = p->timer_trapframe.s10;
    p->trapframe->s11 = p->timer_trapframe.s11;
    p->trapframe->t3 = p->timer_trapframe.t3;
    p->trapframe->t4 = p->timer_trapframe.t4;
    p->trapframe->t5 = p->timer_trapframe.t5;
    p->trapframe->t6 = p->timer_trapframe.t6;

    p->intimer = 0;
    return 0;
}