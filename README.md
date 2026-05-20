# Dining Philosophers Problem

Classic synchronization problem implemented in C. Once with a **monitor** (mutex + condition variables), once with **semaphores**.

Both solutions share identical logic (`test`, `pickup`, `putdown`). Only the primitives differ.

---

## Files

| File | Approach |
|---|---|
| `dining_monitors.c` | Monitor — `pthread_mutex_t` + `pthread_cond_t` |
| `dining_semaphores.c` | Semaphores — `sem_t` |

---

## Build & Run

```bash
gcc -Wall -o dining_monitors   dining_monitors.c   -lpthread
gcc -Wall -o dining_semaphores dining_semaphores.c -lpthread

./dining_monitors
./dining_semaphores
```

---

## How it works

Five philosophers sit at a round table. Each alternates between thinking and eating. To eat, a philosopher needs both forks (shared with neighbours).

A philosopher only picks up forks if **neither neighbour is eating** — checked in `test()`. If the condition isn't met, the philosopher blocks until a neighbour finishes and tests them.

| Primitive | Monitor | Semaphore |
|---|---|---|
| Mutual exclusion | `pthread_mutex_lock/unlock` | `sem_wait/post(&mutex)` |
| Block philosopher | `pthread_cond_wait` | `sem_wait(&self[i])` |
| Wake philosopher | `pthread_cond_signal` | `sem_post(&self[i])` |

No deadlock — a philosopher never holds one fork while waiting for the other.
