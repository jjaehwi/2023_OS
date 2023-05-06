# 10강 Synchronization examples

`chapter 7` 에서 배우는 내용

- Synchronization problems

  - bounded - buffer

  - readers - writers

  - dining - philosophers

- Specific synchronization used by Linux and Windows

  - POSIX

- Design and develop solutions to process synchronization problems using POSIX APIs.

---

- [7-1. Classic Problems of Synchronization](#7-1-classic-problems-of-synchronization)

  - [Bounded - Buffer Problem](#bounded---buffer-problem)

  - [Readers - Writers Problem](#readers---writers-problem)

  - [Dining - Philosophers Problem](#dining---philosophers-problem)

    - [Semaphore 솔루션](#semaphore-솔루션)

    - [Monitor 솔루션](#monitor-솔루션)

- [7-2. Synchronization within the Kernel](#7-2-synchronization-within-the-kernel)

  - [Synchronization in Windows](#synchronization-in-windows)

  - [Synchronization in Linux](#synchronization-in-linux)

- [7-3. POSIX Synchronization](#7-3-posix-synchronization)

  - [POSIX Mutex Locks](#posix-mutex-locks)

  - [POSIX Semaphores](#posix-semaphores)

  - [POSIX Condition Variables](#posix-condition-variables)

- [Summary](#summary)

---

## 7-1. Classic Problems of Synchronization

### Bounded - Buffer Problem

<img width="614" alt="스크린샷 2023-05-06 오후 11 51 55" src="https://user-images.githubusercontent.com/87372606/236631512-406426ab-a074-476e-b355-89f205a41259.png">

- n 개의 buffer 를 갖는 pool 이 있고, 각 버퍼는 아이템 하나씩을 갖는다.

- 3 가지의 세마포어를 선언했다.

  - mutex lock 을 위한 mutex

  - empty 버퍼의 개수를 위한 empty

  - full 버퍼의 개수를 위한 full

- `producer`

  - while 루프

    - next_produced 에서 item 을 생성하고 empty 와 mutex 를 기다린다.

      - 한 개의 아이템을 갖고 있기 때문에 (모든 버퍼는 full 아니면 empty)

      - **empty 버퍼가 있어야 produced 된 item 을 저장**할 수 있다.

      - empty 가 있어야 wait 를 통과

      - `mutex lock 을 통해 하나의 프로세스만 empty 버퍼 에 접근`할 수 있도록 한다.

    - mutex 를 얻은 후 empty 버퍼에 접근해서 item 을 추가해서 full 로 만든다.

    - 작업이 끝나면 signal 을 통해 mutex 를 release 하고, signal(full) 을 통해 full 버퍼를 하나 증가시킨다.

- `consumer`

  - while 루프

    - 하나 이상의 full 버퍼가 있어야한다. (item 을 consume)

    - full 버퍼가 있으면 mutex lock 을 기다린다.

    - `mutex lock 을 얻으면` `critical section 에 진입`해서 `next_consumed 에 item 을 가져다가 저장`한다. (full 버퍼에서 item 을 읽어오는 것)

    - 작업이 끝나면 mutex 를 release 하고 empty 버퍼를 하나 증가시킨다.

    - next_consumed 에 저장된 item 을 이용할 수 있게됐다.

- 버퍼에는 item 하나밖에 안들어가기 때문에 두 개 이상의 프로세스가 버퍼에 item 을 채우려고 하면 문제가 발생하고 이를 막기위해 mutex lock 을 이용하여 하나의 프로세스만 접근하도록 한 것이다.

---

### Readers - Writers Problem

- 여러 개의 concurrent processes 사이에서 data 를 share 할 때 발생하는 문제

- `Writer 와 다른 프로세스가 동시에 db 에 접근하는 경우` (**하나의 writer 가 db 에서 쓰는 동작을 하고 있어야함**)

  - shared object 를 사용하기 위해 writer 가 permission 을 얻은 경우가 아니라면 reader 는 기다릴 필요가 없다.

    - `writer starvation` : reader 가 계속 접근해서 read 동작을 하면 writer 가 동작을 못한다.

  - writer 가 shared data 영역에 permission 을 얻어 작업을 하면 다른 프로세스들은 shared 영역을 읽지도 쓰지도 못한다.

    - `reader starvation` : writer 가 계속 permission 을 얻어 접근하면 reader 가 동작을 못한다.

<img width="624" alt="스크린샷 2023-05-07 오전 12 00 02" src="https://user-images.githubusercontent.com/87372606/236631921-1a30a1ee-53bf-4205-854b-24716da40d2e.png">

- rw_mutex : db 를 업데이트할 때 사용 - 첫 번째로 진입하거나 마지막에 빠져나오는 프로세스가 사용한다.

- mutex : mutual exclusive 조건을 만족시키기 위해 read_count 값에 한번에 하나의 프로세스만 접근하도록 하기 위한 mutex

- read_count : reading process 의 개수

- `writer`

- while 루프

  - rw_mutex 를 얻기 위해 기다린다.

  - rw_mutex 를 얻으면 wrting 동작을 수행하고

  - 빠져나오면서 singal(rw_mutex) 를 통해 release 한다.

- `reader`

- while 루프

  - mutex 를 얻기 위해 기다린다.

  - mutex 를 얻으면 read_count 를 증가시키고

  - read_count 가 1 이면 rw_mutex 를 기다린다. (**첫 번째 reader 인 경우**)

    - writer 가 있다면 끝나기를 기다린다. writer 가 동작하고 있지 않으면 rw_mutex 를 얻는다.

  - rw_mutex 를 얻으면 signal(mutex) 를 통해 mutex 를 release 한다.

    - mutex 는 read_count 를 업데이트하기 위한 것이기 때문에 더이상 필요가 없는 것

  - rw_mutex 를 얻었으므로 reading 동작을 수행하고

  - mutex 를 기다렸다가 read_count 를 1 감소시키고

  - read_count 가 0 이되면

    - **read 를 위해 기다리는 프로세스가 없는 것**이므로 rw_mutex 를 release 한다.

  - mutex 를 release 한다.

- writer 는 rw_mutex lock 을 얻어야 write 동작으로 들어갈 수 있다. 끝나면 release 해줘야한다.

- reader 는 mutex 를 통해 read_count 를 count 하는데, 첫 번째 reader 에 대해서 rw_mutex (이제부터 read 동작 한다~ 고 알려주는 것) 를 기다리고 다음 reader 부터는 read_count 가 2 이상이 되므로 rw_mutex 를 기다리지 않고 바로 mutex 를 release 하고 read 동작을 한다.

  - reader 는 read_count 를 차례차례 증가시키고 read 동작을 한다.

  - read 동작이 끝난 reader 는 mutex 를 얻어서 read_count 를 감소시키고 마지막 reader 라면 read_count 는 0 이 될 것이기 때문에 rw_mutex 를 release 하고 mutex 도 release 하면서 빠져나온다.

  - **일단 read 를 시작하면 rw_mutex 를 얻고, 다수의 reader 가 critical section 에서 read 할 수 있도록 한다. 이후 더 이상 reader 의 read 동작이 없으면 rw_mutex 를 release 하여 다음 read 나 write 동작을 할 수 있도록 한다.**

---

### Dining - Philosophers Problem

<img width="172" alt="스크린샷 2023-05-07 오전 12 24 55" src="https://user-images.githubusercontent.com/87372606/236633098-bc42a42a-d475-4031-bd2e-48dbbc20e744.png">

- 5 명의 철학자가 의자에 앉아서 밥을 먹을 때, 젓가락이 한 개씩 다섯 개 밖에 없다.

  - 동시에 2 명의 철학자만 밥을 먹을 수 있다.

- 의자에 앉으면 thinking 과 eating 동작을 할 수 있다.

  - `Thinking` : no interaction (젓가락을 내려놓고 밥을 먹지 않음)

  - `Eating` : 두 개의 젓가락을 이용하여 밥을 먹는다. (한 번에 하나씩 젓가락을 획득한다.)

- **_프로세스들이 concurrent 하게 동작할 때, concurrency 를 어떻게 제어할 것인지에 대한 방법_**

<img width="223" alt="스크린샷 2023-05-07 오전 12 25 09" src="https://user-images.githubusercontent.com/87372606/236633108-5c864540-f5cf-43fe-af42-4b14dbcecc8f.png">

---

### Semaphore 솔루션

- semaphore chopstick[5]; (5 개의 세마포어 선언, 각 값은 1 로 초기화)

- 연속된 위치에 있는 젓가락을 얻어야 그 가운데 앉은 철학자가 eating 을 할 수 있다.

- while 문

  - chopstick[i] 를 기다리고 획득을 하면 연속으로 옆에 있는 chopstick[(i+1)%5] 을 기다린다.

  - wait 를 통해 두 개의 젓가락을 얻으면 eat 동작을 할 수 있다.

  - eat 동작이 완료되면 chopstick 을 내려놓는 (release) 동작을 한다.

  - 이후에는 think 동작을 한다.

- `deadlock` 이 발생할 수 있다.

  - `deadlock` : **프로세스들이 실행하는데 있어서 진전 없이 막혀있는 상태, 기다린다고 해소되지 않는 상황**

  - 모든 철학자가 자신의 오른쪽에 있는 젓가락을 집는다고 할 때, 모든 철학자가 젓가락을 하나씩 들고 있기 때문에 다음 wait 를 할 수 없다.

  - **해소하는 방법**

    - 5 자리 중 한번에 최대 4 명까지만 자리에 앉을 수 있게 한다.

    - 젓가락을 선택할 때, 한 젓가락만 보지않고 두 젓가락이 모두 available 할 때 젓가락을 선택한다. (한 번에 한 명의 철학자만 critical section 에 진입해서 젓가락을 선택한다.)

    - asymmetric solution : 홀수 자리에 앉은 철학자는 왼쪽의 젓가락을, 짝수 자리에 앉은 철학자는 오른쪽에 있는 젓가락을 선택하도록 한다.

- deadlock 외에 starvation 도 해소를 해야한다. (`deadlock - free != starvation - free`)

---

### Monitor 솔루션

- `Deadlock - free`, `Possible starvation`

<img width="581" alt="스크린샷 2023-05-07 오전 12 44 05" src="https://user-images.githubusercontent.com/87372606/236633913-b3134552-a3bf-4c7d-a3b1-d49d2f43fd1f.png">

- THINKING, HUNGRY, EATING 상태를 가지는 배열 state[5];

- 각각의 철학자들을 나타내는 condition self[5];

- `pickup` : eating 전에 실행되는 젓가락을 집는 함수

  - state 를 HUNGRY 로 만들고 test 함수를 실행한다. (이웃이 EATING 인지 확인)

  - test 함수에서 조건을 만족하면 (이웃이 EATING 상태가 않으면) 본인은 EATING 으로 갈 수 있다.

  - state[i] 가 EATING 이 아니라면 (test 를 했는데 조건을 만족하지 못해서 HUNGRY 인 상태)

    - wait 상태에서 젓가락이 available 할 때까지 기다린다.

- `putdown` : eating 끝난 뒤에 젓가락을 내려두는 함수

  - EATING 이 끝난뒤 상태를 THINKING 으로 만들고

  - 왼쪽에 있는 철학자에 대해 test 를 하고

  - 오른쪽에 있는 철학자에 대해 test 를 한다.

  - test 조건을 만족할 경우 해당 철학자가 EATING 상태가 된다.

    - wait 상태에 있는 프로세스를 wakeup 하는 역할을 하는 것

    - singal 역할을 하는 것

- `test` : 좌우에 있는 철학자가 eating 하고 있는지 아닌지 판단하는 함수

  - 본인을 중심으로 왼쪽의 철학자가 EATING 이 아니고

  - 본인을 중심으로 오른쪽의 철학자가 EATING 이 아니고

  - 본인이 HUNGRY 라면

  - state[i] 를 EATING 으로 두고 signal 함수를 실행한다.

- `initialization_code` : 전체 state 를 THINKING 으로 초기화하는 함수

- `Monitor` 는 **한 번에 하나의 프로세스만 실행을 할 수 있기 때문에 함수를 실행하는데 있어서 synchronization 문제가 발생하지 않는다.**

---

## 7-2. Synchronization within the Kernel

- `커널이 제공하는 synchronization`

---

### Synchronization in Windows

- 커널이 global resource 를 access 하는 경우

  - `single - processor system` : 커널이 global resource 에 access 할 때, interrupt handler 가 실행되지 않도록 막는다.

  - `multiprocessor system` : spinlock 을 이용해서 global resource 에 접근하는 것을 보호한다.

    - 효율을 위해, 어떤 thread 가 spinlock 을 가지고 있을 때, preempted 되지 않도록 한다.

- **Outside the kernel** : `dispatcher object` 사용

  - mutex locks, semaphores, events, timer 를 이용하여 스레드에 대한 synchronization 을 한다.

    - shared data 영역을 보호할 때는 mutex lock

    - Semaphore : ch 6.6 참고

    - Events : 기대하는 condition 이 발생하면 기다리던 thread 에 알려주는 방식

    - Timer : expired time 을 알려준다. (time quantum 을 사용하는 경우)

  - dispatcher object 는 signaled state 에 있을 때만 available 하고 object 를 acquire 할 수 있다. (waiting 상태에서 ready 상태가 된다.)

  - nonsignaled state 에 있으면 available 하지 않고, blocked 된다. (ready 상태에서 waiting 상태가 된다.)

<img width="488" alt="스크린샷 2023-05-07 오전 1 31 14" src="https://user-images.githubusercontent.com/87372606/236636001-3dcc6a96-997f-475f-95b3-39e91a73d584.png">

- **User - mode mutex** 에서는 `critical section object` 를 사용한다.

  - 커널이 관여하지 않고도 acquire 하고 release 하는 동작을 할 수 있다.

  - spinlock 을 사용하고 waiting 시간이 길어지면 커널 mutex 로 변경한다.

---

### Synchronization in Linux

- `Atomic integer` : `atomic_t` 를 통해 atomic operation 을 지원한다.

  - atomic math operation 을 수행할 수 있다.

  - locking mechanisms 을 사용하지 않기 때문에 locking mechanisms overhead 를 피할 수 있다.

  - 간단한 synchronization 에 유용

- atomic operation 은 하나의 operation 을 수행할 때 끊이지 않고 연속적으로 수행함으로써 중간값이 변경되는 것을 막는다.

  - 해당 operation 을 수행하는 동안 mutual exclusion 방식을 보장하는 것

<img width="540" alt="스크린샷 2023-05-07 오전 1 34 52" src="https://user-images.githubusercontent.com/87372606/236636184-0f126949-de53-45e0-a814-3537e105ae29.png">

- A(x), a(o) --> `atomic_t counter`

- `Mutex lock`

  - mutex_lock() 과 mutex_unlock 을 통해 구현

  - Lock 이 unavailable 한 경우 sleep state 로 보내서 spin lock 처럼 running state 에서 기다리지 않도록 한다.

- `Spinlock`

  - single processor : kernel preemption 을 disable/enable

    - spinlock 을 사용하지 않는 것

    - kernel preemption 을 disable 함으로써 spinlock 처럼 해당 스레드가 preemption 되지 않고 기다릴 수 있도록 하는 것

    - `preempt_disable()`, `preempt_enable()`

  - multiple processor : spin lock acquire/release

<img width="350" alt="스크린샷 2023-05-07 오전 1 38 11" src="https://user-images.githubusercontent.com/87372606/236636358-7e7f40dc-e6fb-4530-849c-2c8c252b40b4.png">

- `spinlock 과 mutex lock 은 nonrecursive`

  - acquire 를 한 후 두 번째 lock 을 받을 수 없는 것

  - **첫 번째 lock 을 release 하지 않는 한 새로운 lock 을 얻을 수 없다.**

---

## 7-3. POSIX Synchronization

- POSIX APIs 를 제공함으로써 user level 에서 synchronization 을 할 수 있도록 한다.

  - OS kernel 의 일부분이 아니기 때문에 POSIX API 를 사용하는 모든 OS 에서 사용 가능

<img width="415" alt="스크린샷 2023-05-07 오전 1 49 42" src="https://user-images.githubusercontent.com/87372606/236636854-02c7521d-c91f-4516-9d0a-1098507cd997.png">

---

### POSIX Mutex Locks

- `pthread_mutex_t` : mutex lock 을 위한 data type

- `pthread_mutex_init()` 함수 : mutex 생성

  - Parameter : pointer, NULL (default attribute)

- `pthread_mutex_lock()` / `pthread_mutex_unlock()` 함수 : mutex acquired / released

  - mutex lock 을 acquire 하려고 했는데 available 하지 않다면 해당 thread 는 blocking 된다.

- correct operation 시 0 을 return

  - nonzero error code

---

### POSIX Semaphores

- POSIX SEM extension 에 속해있다.

- type : named 와 unnamed

  - 두 개의 세마포어를 생성하고 프로세스 사이에서 공유하는 방법이 조금 다르다.

- `POSIX Named Semaphores (Linux, macOS)`

  - 세마포어의 name 을 이용해서 서로 관련없는 프로세스들이 공통 세마포어를 사용할 수 있도록 해준다.

  - sem_open 을 통해 "SEM" 이라는 이름의 세마포어 생성

    - O_CREAT 는 세마포어가 이미 생성되어서 open 되어있으면 그 세마포어를 사용한다는 뜻. 처음이라면 생성.

  - sem_wait 를 통해 wait 동작

  - sem_post 를 통해 signal 동작

<img width="636" alt="스크린샷 2023-05-07 오전 1 52 21" src="https://user-images.githubusercontent.com/87372606/236636983-62c7d3cc-2198-443e-b439-1db474569c51.png">

- `POSIX Unnamed Semaphores`

  - 세마포어에 대한 포인터

  - parameter

    - 세마포어의 포인터

    - 공유 수준을 나타내는 flag

    - 0 : 동일한 프로세스가 생성한 스레드들만 세마포어를 공유한다. (shared only by threads belonging to the creater)

    - non - zero : `shraed memory region 에 있는 각각 프로세스들 사이 sharing 이 가능`하다. (shared between separate processes in a region of shared memory)

    - 세마포어의 초기값

  - sem_init 을 통해 세마포어 생성

  - sem_wait 를 통해 wait 동작

  - sem_post 를 통해 signal 동작

<img width="560" alt="스크린샷 2023-05-07 오전 1 53 01" src="https://user-images.githubusercontent.com/87372606/236637011-80a343de-59f2-4143-8ea2-81d37cf54bd5.png">

---

### POSIX Condition Variables

<img width="598" alt="스크린샷 2023-05-07 오전 2 04 25" src="https://user-images.githubusercontent.com/87372606/236637509-ce77213e-978b-448e-b61a-da6fca4feb59.png">

- Pthread in C : mutex lock 과 함께 condition variable 이 사용된다.

- pthread_cond_t 를 통해 condition variable 을 선언

- pthread_cond_init 을 통해 초기화

- a != b 인 경우 pthread_cond_wait 를 계속 실행해서 조건이 만족되기를 기다린다. (wait for condition a==b)

  - a 와 b 가 같아지면 while 에서 빠져나와서 mutex lock 을 release

---

## Summary

- Classic problems of process synchronization

  - bounded - buffer

  - readers - writers

  - dining - philosophers problems

- Windows 에서의 synchronization tools

  - dispatcher objects

  - events

- Linux 에서의 synchronization tools

  - atomic variables

  - spinlocks

  - mutex locks

- POSIX API 에서의 synchronization tools

  - mutex locks

  - semaphores

    - named

    - unnamed

  - condition variables
