# 9강 Synchronization : Synchronization, mutex, semaphore

`chapter 6` 에서 배우는 내용

- Multi - thread 프로그래밍을 할 때, data sharing 을 할 때 발생할 수 있는 문제를 해결할 수 있는 synchronization Tool 에 대해 공부한다.

- Critical - section 문제와 race condition 란?

- H/W 솔루션 : memory barriers, compare - and - swap operations, atomic variables

- S/W 솔루션 : Mutex locks, semaphores, monitors, condition variables

- critical - section 문제를 해결하는 tools 에 대한 Evalutation

---

- [6-1. Background](#6-1-background)

- [6-2. The Critical - Section Problem](#6-2-the-critical---section-problem)

  - [예시1](#ex-1-a-kernel-data-structure-maintaining-a-list-of-all-open-files)

  - [예시2](#ex-2-simultaneous-creating-child-process)

  - [예시3](#ex-3-kernel-data-structures-for-maintaining-memory-allocation-for-maintaining-process-lists-and-for-interrupt-handling)

  - [싱글코어 환경에서 해결방안](#solution-in-a-single---core)

  - [Handling in OS](#critical-section-handling-in-os)

- [6-3. Peterson's Solution](#6-3-petersons-solution)

  - [Reordering 상황에서 peterson's solution](#reordering-상황에서-petersons-solution)

- [6-4. Hardware Support for Synchronization](#6-4-hardware-support-for-synchronization)

  - [Memory Barriers](#memory-barriers)

  - [Hardware Instructions](#hardware-instructions)

  - [Atomic Variables](#atomic-variables)

- [6-5. Mutex Locks](#6-5-mutex-locks)

- [6-6. Semaphores](#6-6-semaphores)

  - [Semaphore Usage](#semaphore-usage)

  - [Semaphore Implementation](#semaphore-implementation)

- [6-7. Monitors](#6-7-monitors)

  - [Monitor Usage](#monitor-usage)

- [Summary](#summary)

---

## 6-1. Background

- producer - consumer problem 을 통해 synchronization 에 대한 문제가 발생하는 경우를 알아보자.

<img width="324" alt="스크린샷 2023-04-28 오전 1 27 44" src="https://user-images.githubusercontent.com/87372606/234928606-653ea3c8-875d-4545-b804-a436e51429ee.png">

- producer 는 buffer 를 next produced 를 이용하여 계속 채우는 프로세스이다.

  - 새로운 버퍼를 채울 때마다 count 를 ++ 한다.

- consumer 는 buffer 에서 값을 읽어가서 값을 소모하는 프로세스이다.

  - 버퍼를 읽어갈 때마다 count 를 -- 한다.

- 이 때, count++ 와 count-- 때 문제가 발생할 수 있다.

```
// intruction level 에서 count++ 의 동작
register1 = count // count 값을 CPU register 에 저장
register1 = register1 + 1 // 값을 increase
count = register1 // increase 한 값을 다시 메모리에 저장

// intruction level 에서 count-- 의 동작
register2 = count
register2 = register2 - 1
count = register2
```

<img width="474" alt="스크린샷 2023-04-28 오전 1 31 39" src="https://user-images.githubusercontent.com/87372606/234929476-527b5e09-d3dc-4a6c-86b3-603d2f44e6e6.png">

- producer 와 consumer 가 **_멀티 스레드로 구현되어서 동시에 실행될 때, interleaving 되어서 concurrent 하게 실행될 경우_** 위 그림처럼 문제 (incorrect state) 가 발생할 수 있다.

  - producer 와 consumer 를 한번 씩 실행해서 최종적인 count 는 5 가 되어야하는데 concurrent 하고 interleaving 되어서 count 값이 잘못나올 수 있다.

    - 마지막에 producer 가 실행되었으면 6 이고, consumer 가 실행되면 4 가 되는데 결론적으로 둘 다 잘못됨.

  - count increase 와 decrease 하는 `명령어 들이 서로 interleaving` 될 수 있다.

- 이런 식으로 **동일한 데이터에 대해 concurrent 하게 접근하여 처리할 때, thread 가 어떤 순서로 접근하느냐에 따라 결과값이 다르게 나오는 경우를 `race condition` 이라고 한다.**

  - 이런 문제를 해결하기 위해 `Process synchronization and coordination` 이 필요하다.

---

## 6-2. The Critical - Section Problem

- `Critical section` : **하나의 프로세스 내에서 `shared data 를 업데이트 시키는 부분`**에 대한 코드를 뜻한다.

  - 반드시 `하나의 프로세스만 critical section 을 실행`하고 있어야한다. (**Only one process in the critical section**)

  - 앞서 본 producer - consumer 문제는 producer 와 consumer 가 동시에 critical section 을 실행하고 있었기 때문에 발생한 것.

- data sharing 을 cooperative 하게 진행하기 위해 각 프로세스나 스레드가 `동기화`되도록 설계되어야한다. (**to design to synchronize activity**)

- `일반적인 프로세스의 구조`

  - entry section 을 거쳐 critical section 에 접근한다.

  - exit section 을 통해 critical section 을 벗어나고

  - shared data 를 다루지 않는 나머지 (remainder) section 에 접근한다.

```
// 일반적인 프로세스의 구조
while(true){
    entry section
        critical section
    exit section
        remainder section
}
```

- Critical - section problem 해결을 위한 3 가지 조건

  1. `Mutual exclusion` : critical section 에는 하나의 프로세스만 접근할 수 있다.

  2. `Progress` : entry section 에 들어간 프로세스들은 다음 critical section 에 누가 들어갈지 결정하는데 참여하고, 누가 들어갈지에 대한 결정은 무한히 미뤄지면 안된다. (일정시간 내에 결정되어서 누군가는 critical section 에 들어갈 수 있게 되는 것)

  3. `Bounded waiting` : 어떤 프로세스가 critical section 에 들어가겠다고 request 를 하고 granted 를 받을 때까지 기다리는 시간이 무한하지 않고 일정시간 내에 결정되어야한다.

---

### EX 1. a kernel data structure maintaining a list of all open files

- A 와 B 프로세스가 있고 C 라는 파일을 open 하려고 할 때,

  - A 가 먼저 파일을 open 하고 B 가 접근하는 경우 A 가 C 라는 파일을 open 해서 open file list 에 올려놓으면 B 는 파일을 open 할 필요없이 open file list 를 보고 C 라는 파일에 바로 접근 가능하다.

  - 동시에 접근하려고 할 때, open file list 를 check 할 것이고 C 가 open 되어있지 않으면 A, B 둘 다 kernel data structure 를 update 하려고 할 것이다.

    - 그 과정에서 open file list 에 C 가 두 번 올라가는 등의 `race condition 이 발생`할 수 있다.

---

### EX 2. Simultaneous creating child process

<img width="474" alt="스크린샷 2023-04-28 오전 1 54 04" src="https://user-images.githubusercontent.com/87372606/234934635-76f92067-2955-4cb1-9f4c-551da2ea8248.png">

- 두 개의 프로세스가 child 프로세스를 동시에 생성하는 경우

  - P0 와 P1 프로세스가 child 프로세스를 생성하면서 pid 를 요청하는데, next_available_pid 가 2615 번으로 동시에 읽어가게 되면 둘의 child 프로세스 pid 가 동일해지게 된다.

  - id 는 unique 해야하기 때문에 문제가 된다.

---

### EX 3. kernel data structures for maintaining memory allocation, for maintaining process lists, and for interrupt handling

- 메모리를 allocation, 프로세서의 리스트를 다루는 경우, interrupt 를 핸들링하는 경우 kernel data structure 를 통해 관리한다.

- 앞서 본 open files list 와 마찬가지로 다수의 프로세스가 동시에 access 할 경우 race condition 이 발생할 수 있다.

---

### Solution in a Single - core

- `preemption 이 없이 순서대로 실행` : shared variable 이 바뀌는 동안 preemption 이 없으면 interrupt 가 발생하지 않기 때문에 race condition 이 발생할 수 없다. (싱글 코어이기 때문)

  - 멀티 코어인 경우 preemption 에 상관없이 둘 이상의 스레드가 동시에 실행되면서 shared variable 값이 바뀌는 상황이 발생할 수도 있다.

---

### Critical section handling in OS

- `Preemptive kernels` : kernel mode 에서 실행되는 동안 프로세스가 preempted 되는 것을 허용한다.

  - SMP 구조에선 설계가 어렵다 : 하나의 프로세스가 전체적인 프로세스 할당을 제어하는 것이 아닌 독립적으로 동작하기 때문에 critical section 문제를 해결하도록 구현하는 것이 어렵다.

  - `more responsive` 하기 때문에 preemptive 방식을 선호한다.

  - `real - time programming 에 적합`하기 때문에 preemptive 방식을 선호한다.

  - 현대 대부분 OS 는 대부분 Preemptive 방식을 채택

- `Nonpreemptive kernel` : kernel mode 프로세스가 계속해서 동작한다.

  - 커널 모드가 더이상 필요없는 경우, 블락킹이 발생한 경우, CPU 제어권을 자발적으로 양보하는 경우에만 kernel mode 에서 exit 한다.

  - race condition 이 발생하지 않는다.

---

## 6-3. Peterson's Solution

- `Peterson's Solution` : critical section 을 해결하는 방법 중 하나로 **classic** 한 sw based solution 이다.

  - 현대적인 프로세서를 사용하는 경우 제대로 작동하지 않을 수 있다. (좋은 알고리즘 아이디어를 제공)

  - critical section 과 remainder section 사이에서 교대로 실행하는 두 프로세스에 대해서만 제한적으로 적용된다.

    - 두 개의 프로세스에 대한 race condition 을 해결하는 것

```
int turn;           // critical section 에 누가 들어갈지 정하는 변수
boolean flag[2];    // 해당 프로세스가 critical section 에 들어가겠다고 의사를 표현하는 bool 변수

while(true){
    flag[i] = true; // Pi 프로세스가 critical section 에 enter 하겠다고 표시
    turn = j;       // turn 값은 j 로 설정, 내가 아닌 다른 프로세스를 설정해서 Pj 가 critical section 에 들어가서 실행하는 것을 허용하는 것
    while (flag[j] && turn == j)
        ;
        /* critical section */
    flag[i] = false;
        /* remainder section */
}
```

```
int turn;
boolean flag[2];

while(true){
    flag[j] = true;
    turn = i;
    while (flag[i] && turn == i)
        ;
        /* critical section */
    flag[j] = false;
        /* remainder section */
}
```

- 프로세스 i 가 critical section 에 들어가겠다고 표시를 하고,

  - 프로세스 j 를 확인하여 프로세스 j 도 critical section 에 접근한다고 조건이 만족되면 프로세스 j 를 기다렸다가 끝난 후 프로세스 i 가 critical section 에 접근한다.

  - critical section 을 빠져 나와서 프로세스 i 를 false 로 만들고 remainder section 에 접근한다.

  - 다른 상대방 프로세스도 flag 가 true 여서 critical section 에 접근하겠다고 한 경우에만 기다리는 것이고, 아예 flag 값이 false 여서 entry section 에 진입하지 않은 경우 while 을 거치지 않고 바로 critical section 에 접근할 수 있다.

    - 두 프로세스 사이 경쟁이 없으면 자연스럽게 critical section 에 접근하는 것이고,

    - 경쟁이 있는 경우 먼저 entry section 에 진입한 프로세스가 critical section 에도 먼저 접근한다.

- `3 가지 조건`에 대해

  1. `Mutual exclusion` : flag 는 동시에 true 가 될 수 있다. 하지만 turn 값은 서로 다르게 0 또는 1 일 수 밖에 없다. (서로 다른 turn 값으로 지정해주기 때문)

  - 그러므로 critical section 에는 하나의 프로세스만 접근 가능하다.

  2. `Progress`

  - single entry : 하나만 entry 에 접근한 경우

  - Both : 두 개의 프로세스가 동시에 진입한 경우 - Pi 가 먼저 도착해서 while 에 머물러 있는 경우 늦게 도착한 Pj 는 turn 값을 세팅하여 먼저 도착한 Pi 가 critical section 에 접근할 수 있다.

  3. `Bounded - waiting`

  - single entry : 하나만 접근하는 경우

  - Both : Pj 가 먼저 접근해서 critical section 에 있는 경우 Pi 는 waiting, Pj 가 무한루프에 있지 않으면 언젠가 빠져나오고 빠져나오면서 flag[j] 를 false 로 만들고 Pi 는 while 루프에서 빠져나올 수 있고, 즉시 critical section 에 접근할 수 있다.

    - **waiting for at most one entry**

    - 하나의 프로세스만 critical section 에 접근하는 경우 계속 critical section 에 진입할 수 있지만 동시에 접근하려는 경우 무조건 번갈아가면서 critical section 에 접근하게 된다.

---

### Reordering 상황에서 peterson's solution

- 현대의 프로세서나 컴파일러들은 성능 향상을 위해 명령어의 순서를 변경할 수 있어서 Peterson's Solution 이 제대로 작동하지 않을 수 있다.

- Single - threaded : working correctly

  - 최종적인 결과값은 동일하기 때문에 문제 없다.

- `shared data 를 갖고 있는 multithreaded 프로그램 은 문제가 된다.`

```
// Data
boolean flag = false;
int x = 0;

// Thread 1
while (!flag)
;
print x;

// Thread 2
x = 100;
flag = true;

// 결과
Correct answer : 100
Reordering in thread 2 : 0 or 100
Reordering in thread 1 : 0 or 100
```

- 스레드 1 과 스레드 2 는 data dependency 가 존재하지 않기 때문에 프로세서나 컴파일러가 동작 순서를 필요에 따라 바꿀 수 있다.

  - thread 1 에서도 while 문과 print 사이 dependency 가 없기 때문에 명령어의 순서도 바뀔 수 있다.

- 스레드 1, 2 중 뭐가 먼저 실행되던 correct answer 는 100 이 되어야할 것 같다.

  - 스레드 2 의 명령어 순서가 바뀌는 경우

    - 스레드 1 은 while 을 기다리다가 스레드 2 가 실행되어 flag 가 true 가 되면 x 가 100 이 되기전에 print 하여 0 이 나올 수 있다.

    - 또는 x 가 먼저 바뀌어서 100 이 될 수도 있다.

  - 스레드 1 의 명령어 순서가 바뀌는 경우

    - 스레드 1 이 실행되면 0 이 출력된다.

    - 스레드 2 가 실행되어 x 가 100 으로 바뀐 뒤 출력된다.

  - `race condition` 이 발생한 것

<img width="874" alt="스크린샷 2023-04-28 오전 2 42 48" src="https://user-images.githubusercontent.com/87372606/234947301-0dcdd4ed-394b-4b9a-aa39-1c2fda954f5f.png">

- reordering 상황이 발생하면 동시에 critical section 에 접근하는 상황이 생기고 Peterson's solution 을 통해 synchronization 을 해결할 수 없게 된다.

---

## 6-4. Hardware Support for Synchronization

- SW 기반의 솔루션은 현대의 컴퓨터 구조에서 동작을 제대로 하지 않는 경우가 있어서 동기화를 위해 HW 적인 support 가 필요하다.

  - `Memory Barriers`

  - `Hardware Instructions`

  - `Atomic Variables`

---

### Memory Barriers

- 메모리 모델의 종류

  - `Strongly ordered` : 하나의 프로세서가 메모리의 내용을 변경하면 다른 모든 프로세서가 그 내용을 즉시 볼 수 있다.

  - `Weakly ordered` : 하나의 프로세서가 메모리의 내용을 변경하면 다른 모든 프로세서가 그 내용을 볼 수 있는 것이 보장되지 않는다.

- `Memory barriers` (memory fences) : 메모리에서 어떤 변화가 발생했을 때, **이것을 propagated 되도록 하는 instruction** (strongly ordered 모델이 되도록 만들어 주는 것)

  - instructions that can force any changes in memory to be propagated

  - **모든 load 와 store 동작이 이후 load 와 store 동작 전에 완벽하게 끝나도록 하는 것** (ensures that all loads and stores are completed before any subsequent load or store operations)

```
(ex)
// Data
boolean flag = false;
int x = 0;

// Thread 1
while (!flag)
  memory barrier();
print x;

// Thread 2
x = 100;
memory barrier();
flag = true;
```

- thread 1 에서 memory barrier() 를 실행하면 flag 값을 완전히 loading 해야 print x 를 할 수 있다.

  - flag 값을 완벽하게 읽어와서 true, false 를 판단하는 것 (flag 를 읽어오기 전에 print x 를 하는 동작이 일어나지 않음)

- thread 2 에서도 memory barrier() 를 통해 x = 100 으로 바뀌어야지 flag 가 true 로 바뀌는 동작을 한다. (x = 100 이 되기 전 flag 가 true 로 되는 동작이 일어나지 않음)

---

### Hardware Instructions

- **to test and modify the content of a word** or to **swap the contents of two words** `atomically` that is, as one `uninterruptible` unit

  - word 의 내용을 **test 하고 modify** 하거나 두 개의 words 의 내용을 **바꾸는 동작**을 수행할 때, `atomically` 수행하도록 만드는 instruction (몇 단계에 걸쳐 수행되야하는 작업)

  - `atomic 하게 동작`하는 것 : 명령어가 `중간에 방해받지 않고 `시작부터 끝까지 실행이 완벽히 끝나도록 하는 것

    - 사이사이 다른 명령어가 실행되지 않도록 하고, 연속해서 동작하여 끝내도록 하는 방식

```
(ex) Conceptual instruction : test and set()
// 서로 다른 코어에서 실행되면 parallel 하게 수행되면 interleaving 으로 문제가 발생했었다.
// 서로 다른 코어에서 동작하더라도 명령어가 interrupt 되지 않고 연속적으로 수행되어야한다.
// 공유하는 데이터에 대해 코어가 달라도 sequential 하게 동작해야한다.

boolean test_and_set(boolean *target){
  boolean rv = *target;
  *target = true;
  return rv;
}

do{
  while(test_and_set(&lock))
    ; / *do nothing */
      /* critical section */
    lock = false;
      /* remainder section */
} while (true);
```

- target 을 입력으로 받아서 rv 에 저장을 해두고 target 을 true 로 만들고 입력으로 받은 target (rv) 을 return

- true 인 동안 while 에 계속 머무르고, loop 를 빠져나오면 critical section 에 들어가고 exit 해서 critical section 에서 빠져나오면 lock 을 false 로 세팅하고 remainder section 을 진행한다.

  - 처음 lock 이 false 이면 while 문을 빠져나와 바로 critical section 에 접근할 수 있다. (입력받은 target 을 return 하므로)

  - 그 다음 target 은 true 가 됐기 때문에 다른 프로세스 또는 스레드가 test_and_set 을 실행하더라도 while 을 빠져나오지 못하고 entry 에서 대기를 한다.

  - 그리고 처음 critical section 에 접근한 프로세스 또는 스레드가 critical section 을 빠져나오면서 lock 을 false 로 만들어주니까 다른 프로세스나 스레드가 test_and_set 명령어에 의해 false 를 받아 while 문을 나오고 critical section 으로 접근 가능하다.

---

```
(ex) Conceptual instruction : compare and swap()

int compare_and_swap(int *value, int expected, int new_value){
  int temp = *value;
  if(*value == expected)
    *value = new_value;
  return temp;
}

while(true){
  while(compare_and_swap(&lock, 0, 1) != 0)
    ; /* do nothing */
    /* critical section */
  lock = 0;
    /* remainder section */
}

// satisfying mutual - exclusion
// not satisfying bounded - waiting


```

- 입력으로 3 개의 파라마터를 받는다.

  - value 를 temp 에 저장하고 value 와 expected 가 같으면 value 에 new_value 를 assign 하고 temp 를 return 한다.

    - 첫 번째 value 값을 return 하고, 1, 2 번 값이 같으면 new_value 를 value 에 주는 것

- lock 이 처음에 false 였으면 while 을 빠져나와 critical section 으로 접근 가능하고, lock 은 0 (false) 이므로 new_value 1 (true) 가 된다.

  - 첫 번째 프로세스나 스레드가 critical section 에 진입한 후 그 이후 프로세스나 스레드가 critical section 에 접근하지 못한다. (lock 이 true 이기 때문에)

  - 첫 번째 프로세스가 스레드가 critical section 에 exit 한 후 lock 이 0 (false) 가 되면 기다리던 프로세스나 스레드 들이 false 가 된 lock 을 받아 while 문을 나오고 critical section 에 접근한다.

- **하나의 프로세스나 스레드가 critical section 에 접근하고 나머지는 밖에서 대기**하기 때문에 `mutual - exclusion` 을 만족한다.

  - 하지만 `bounded - waiting` 은 만족하지 못한다.

```
// satisfying mutual - exclusion and bounded - waiting
// n 개의 프로세스 또는 스레드가 있을 때 waiting 이라고 하는 boolean array 선언 후 false 로 초기화
boolean waiting[n]; // initialized to false
int lock;           // initialized to 0

// 프로세스 i 에 대한
while(true){
  waiting[i] = true;
  key = 1;

  while(waiting[i] && key == 1)
    // key = lock
    key = compare_and_swap(&lock, 0, 1); // lock = 1

  waiting[i] = false;
    /* critical section */

  j = (i + 1) % n; // next to i

  while((j != i) && !waiting[j])
    j = (j + 1) % n;

  if(j == i)
    lock = 0;
  else
    waiting[j] = false;

  /* remainder section */
}
```

- false 로 초기화된 waiting 배열을 프로세스 i 에 대해 true 로 바꿔주고 key 값을 1 로 설정한다.

- waiting[i] && key == 1 이 true 가 돼서 while 루프 안으로 들어오고 compare_and_swap 을 실행해서 key 에는 lock 이 assign 된다.

  - 처음 lock 은 0 이기 때문에 key 도 0 이 되어 while 문을 빠져나올 수 있다. 이후에 진입한 프로세스의 경우에는 compare_and_swap 을 실행한 뒤 lock 이 1 로 바뀐 상태이기 때문에 key 가 1 이 되어서 while 에서 빠져나오지 못하고 루프 안에 갇혀있게 된다.

- 처음에 빠져나온 프로세스나 스레드는 waiting[i] 를 false 로 설정하고 critical section 으로 들어간다.

  - 프로세스 i 의 waiting 이 false 가 되었으니 waiting 하고 있지 않다고 한 것

- critical section 을 빠져나온 후 j 값을 찾는다. (i + 1 은 프로세스 i 의 다음 프로세스)

  - 다음 프로세스에 대해 n 의 모듈러 값을 찾고 그 값을 j 로 할당

- j 가 i 와 같지 않고 waiting[j] 가 false 이면 (기다리고 있지 않으면) 조건을 만족한다.

  - while 에 들어오면 j 를 1 씩 증가시키는 모듈러 연산을 하는데

    - j 와 i 가 같아지거나, waiting 이 true 가 되는 경우 루프를 빠져나올 수 있다.

- `j 가 i 와 같아져서 빠져나온 경우` : 다음에 있는 모든 프로세스와 비교를 했는데 결국 자기 자신 차례에 올 동안 while 을 빠져나가지 못했다는 뜻.

  - 한 바퀴 돌아서 결국 자기 자신 까지 온 것

  - lock 을 0 으로 세팅한다.

    - **critical section 에 접근할 특정 프로세스를 못 찾은 것이기 때문에 lock 을 0 으로 세팅해서 먼저 compare_and_swap 을 실행한 프로세스가 loop 를 빠져나올 수 있게 하는 것** (아무나 기회)

- `waiting 이 true 가 되어서 빠져나오는 경우` : i 다음 프로세스들을 조사하다가 프로세스가 while 문에서 오래 기다려서 waiting 이 true 가 된 경우이다.

  - **특정 프로세스를 찾아 false 로 만들어 줌 으로써 특정 프로세스를 critical section 에 접근할 수 있도록 해준 것.**

  - waiting 을 false 로 세팅한다. 프로세스 i 가 프로세스 j 의 waiting 을 false 로 만들어 주면서 위의 while 문에 묶여있던 프로세스 j 를 탈출 시키고 critical section 에 접근하도록 함.

  - `bounded - waiting 조건이 만족`되는 것

---

### Atomic Variables

- `Atomic Variables` : int 나 boolean 같은 `기본적인 data type `에 대해 `atomic operation 을 제공하는 variable`

  - **provides atomic operations on basic data types such as integers and booleans**

```
(ex) sequence 를 atomic integer 라 할 때,

increment(&sequence);

void increment(atomic_int *v){
  int temp;
  do{
    temp = *v;
  }
  while(temp != compare_and_swap(v, temp, temp+1));
}
```

- increment 함수에서 파라미터를 atomic_int \*v 로 정의하고

- do 에서 atomic_int \*v 를 temp 에 assign 하고 조건이 만족하면 계속해서 수행한다.

  - compare_and_swap 명령어를 통해 v, temp, temp + 1 을 넣는다.

  - v 와 temp 가 서로 같으면 v 에 temp + 1 을 assign 하고 원래의 v 값을 return 한다.

  - 원래의 v 와 temp 가 같지 않으면 while 루프를 돌게 된다.

- 첫 번째 섹션에서만 while 문을 통과하고 두 번째 이후 섹션부터는 v 는 temp + 1 로 temp 와 달라져서 while 에 계속 머물러 있게 된다.

- `한계` : atomic variable 의 경우 `모든 상황에서 race condition 을 해결할 수 있는 해결책은 아니다`.

  - (ex) bounded - buffer problem (producer - consumer)

  - bounded - buffer 인 경우 크기가 유한해서 consumer 가 데이터를 더 빨리 사용해서 empty 될 경우

  - comsumer 2 개가 동시에 실행돼어 버퍼가 차기를 기다릴 때,

  - producer 가 하나의 item 만 생성해서 버퍼에 넣으면 두 consumer 는 buffer 를 계속 보고 있다가 buffer 가 생기는 순간 consumer 는 데이터를 사용한다.

  - 이 때, consumer 가 각 코어에서 실행되고 있고, 위의 코드를 사용하면 both consumers could exit their while loops

  - 실제 데이터는 하나 밖에 없어서 both consumer 가 빠져나오는 것은 옳지 않다.

---

## 6-5. Mutex Locks

- 일반적으로 hardware 기반의 synchronization 솔루션들은 복잡할 뿐만아니라 응용프로그래머가 접근할 수 없는 경우가 많다.

- `Mutex (mutual exclusion) lock` : higher - level software tools 중 가장 간단하다.

  - **critical section 을 보호하면서 race condition 이 발생하는 것을 막는다.**

  - lock 을 획득하여 critical section 에 접근하고 critical section 실행이 끝나면 lock 을 release 하고 remainder section 을 실행한다.

```
while (true){
  acquire lock
    critical section
  release lock
    remainder section
}

// acquire() 는 lock 이 available 하지 않으면 while 에서 기다리면서 CPU 를 사용하면서 기다린다.
// available 이 true 가 되면 while 을 빠져나오면서 다시 available 을 false 로 만들어놓는다.
// available 이 true 가 되는 것은 lock 을 획득한 것이고, 다시 available 을 false 로 만들어서 다른 프로세스가 lock 을 획득하지 못하도록 하는 것
acquire(){
  while(!available)
    ; /* busy wait */
  available = false;
}

// release() 는 available 을 true 로 만들고 나오면 대기하고 있던 다른 프로세스가 while 을 탈출하면서 lock 을 획득할 수 있게 된다.
release(){
  available = true;
}
```

- lock 을 획득하기 위해 acquire() 를 호출하거나 빠져나올 떄 release() 함수를 호출하는데 이 함수들은 `atomically 하게 실행`되어야한다.

  - **다른 함수가 available 에 접근해서 값을 바꿀 수 없어야한다.**

- acquire 함수를 실행할 때 `busy waiting` 을 하게 된다.

  - while 루프 안에서 계속적으로 루프 내부를 돌고 있기 때문에 `프로세스가 아무것도 안하고 있지만 CPU 를 점유하고 있게 되는 문제`가 있다.

  - `Spinlock` : the processes "spins" while waiting

  - wait 하는 동안 **context switch 가 발생하지 않기 때문에 멀티코어 시스템에선 많이 사용**된다.

---

## 6-6. Semaphores

- `Semaphores` : 정교한 방법으로 동기화를 하는 강력한 tool (more robust tool for synchronization in more sophisticated ways)

  - mutex lock 과 유사해보이지만 훨씬 강력하고 Dijkstra 가 개발했다.

- **_Semaphore S_**

  - `integer variable` : `wait()` 와 `signal()` 이라는 atomic operations 에 의해 access 가 가능한 변수

  - `wait()` : called P (from proberen, "to test")

  - `signal()` : called V (from verhogen, "to increment")

```
wait(S){
  while (S<=0)
    ; // busy wait
  S--;
}

signal(S){
  S++;
}
```

- `wait 함수`

  - 세마포어를 가지고 세마포어가 0 보다 작거나 같은 경우 busy wait 를 진행하고 0 보다 커지면 빠져나와서 S 를 감소시키고 wait 를 빠져나온다.

  - 빠져나오면서 S 를 감소시키면서 나와서, 다른 프로세스가 접근하면 다시 while 루프를 돌게된다.

  - `처음 접근한 프로세스만 while 루프를 빠져나와서 S--`를 할 수 있다.

- `signal 함수는 단순히 S 를 증가`시키는 것

  - 어떤 프로세스가 critical section 을 빠져나가면서 S 를 증가시키면 S 에 가장 먼저 접근한 프로세스는 S 가 0 이었을 때 증가됐으므로 S = 1 로 while 을 탈출하고 다시 S-- 를 한다.

- `세마포어와 관련된 모든 modifications 동작은 atomic 하게 동작 해야한다.`

  - Atomic execution on all modifications of the semaphore

  - wait() : S<=0 (S 값 비교할 때) and S-- (S 값 감소시킬 때) atomic 하게 진행되어야함.

---

### Semaphore Usage

1. Counting semaphore : 제한이 없는 도메인 (over an unrestricted domain)

- 세마포어가 integer 값을 가진다. (-2, -1, 0, 1, 2 ...)

- (ex) 유한한 숫자의 리소스에 대해 control 할 때 사용 (to control access to a given resource consisting of a finite number of instances)

  - wait() : 0 이 될 때 까지 decrementing 하고 0 이 되면 resource blocked 해서 다른 프로세스가 리소스에 접근할 수 없다.

  - signal() : 프로세스가 리소스를 release 할 때마다 incrementing the count

- (ex) 동시에 concurrent 하게 실행되는 S1 state 를 가진 프로세스 P1 과 S2 state 를 가진 P2 에 대해

  - S1 이 실행되고 S2 가 실행되어야하는 경우 (S1 의 결과를 S2 가 사용하는 경우)

  - synch 가 0 보다 커야 빠져나와서 S2 가 실행되므로 P1 프로세스 signal 을 하고 P2 프로세스에 wait 를 한다.

```
// P1
S1;
signal(synch); // S1 을 실행하고 signal 을 한다.

// P2
wait(synch); // 실행안되고 기다리고 있다.
// S1 다음 signal 이 되기 때문에 S1 다음 값이 증가해서 wait 를 빠져나오면 S2 가 실행될 수 있다.
S2;
```

2. Binary semaphore

- 0 과 1 만 사용하는 방식

- mutex lock 과 비슷하게 동작한다.

---

### Semaphore Implementation

- `busy waiting 을 막기 위해 wait() 와 signal() 을 modified`

- waiting process 는 세마포어와 연관된 waiting queue 로 suspended --> **waiting state**

  - 세마포어와 연관되어 `critical section 으로 진입하기 위해 대기`하고 있는 것

  - 프로세스가 세마포어의 list 에 추가가 되고 빠져나올 때는 세마포어의 list 를 업데이트해서 제거한다.

  - **_waiting list 에 프로세스가 들어갔다가 나오는 방식으로 동작하면서 busy wait 를 해결한다._**

- signal operation 에 의해 세마포어 값이 증가되면 restarted

  - waiting 상태에서 ready 상태가 될 수 있다.

  - **waiting 상태의 프로세스는 무조건 ready 상태가 되었다가 running 상태로 간다.**

- 세마포어의 변수인 s->value 는 음수가 될 수 있다.

  - 음수인 것은 해당 세마포어에 대해 대기하고 있는 프로세스의 숫자가 1개 이상보다 더 많다.

  - 대기하고 있는 프로세스의 수가 증가할 수록 세마포어의 값은 계속 감소한다.

  - 몇 개의 프로세스가 대기하고 있는지 알 수 있다.

- `wait() 와 signal() 은 atomic execution`

  - **모든 코어에 대해 interrupt 를 disable**

  - **compare_and_swap()** 이나 **spinlocks 방식을 제공**해야한다.

```
// 세마포어 구조체 정의
typedef struct {
  int value;
  struct process *list; // waiting state 에 있는 프로세스를 담는 리스트
} semaphore;

// wait 함수
wait(semaphore *S){
  S -> value--; // value decrement
  if (S->value < 0){
    add this process to S -> list;
    sleep(); // 세마포어를 waiting 상태로 보내는 것, Semaphore waiting queue
    // waiting state 에 있는 모든 프로세스는 list 에 추가된다.
  }
}

// signal 함수
signal (semaphore *S){
  S -> value++;
  // S 가 0 보다 작거나 같다는 것은 대기하고 있는 프로세스가 있다는 것
  if(S -> value <= 0){
    // P 를 정할 수 있는데 여기서 우선 순위를 주는 policy 를 정할 수 있다.
    remove a process P from S -> list; // list 에서 프로세스 하나 제거
    wakeup(P);  // list 에서 제거된 프로세스를 wait 에서 ready 로 보내는 것
  }
}
```

- **list 에 추가하고 sleep() 으로 보냄으로써 (waiting 상태로) busy wait 발생을 해결한 것이다.**

---

## 6-7. Monitors

- synchronization problem due to programming errors (룰을 지키지 않아서 정상 동작하지 않은 것)

  - critical section 에 동시접근하는 문제가 발생

    - `race condition` 발생

  - `permanent blocking` 발생 : 접근이 아예 막혀버리는 경우

```
// 1. 동시에 접근이 되는 경우 : signal 을 먼저 하고 wait 를 하면 critical section 에 무조건 접근이 가능하다.
// wait 와 signal 을 통해 entry 와 exit 하는 프로세스가 또 있는 경우 동시에 접근이 되면서 충돌이 발생한다.
signal(mutex);
...
critical section
...
wait(mutex);

// 2. 동시에 접근이 되는 경우 : 무조건 critical section 에 접근하고 signal 을 하면 다른 프로세스도 critical section 에 접근이 가능하다.
// 다른 프로세스가 critical section 에 접근 중일 때 본인이 들어가서 충돌하고 signal 을 해서 다른 프로세스가 접근을 못하게 될 수 있다.
// 다른 프로세스가 접근할 수 없을 때, signal 이 되면 접근할 수 있게되어 동시 접근이 될 수 있다.
...
critical section
...
signal(mutex);

// 3. 접근을 못하게 되는 경우 : wait 로 critical section 에 진입했는데 나오면서 또 wait 를 하면 mutex 를 풀어주지 않고 또 - 가 된다.
wait(mutex);
...
critical section
...
wait(mutex);

// 4. 접근을 못하게 되는 경우 : 나올 떄 signal 함수를 실행하지 않는 경우 : 세마포어 값이 증가하지 않고, 뮤텍스 lock 이 풀어지지 않는다.
wait(mutex);
...
critical section
...
```

- 이러한 프로그래밍 에러에 의해 발생하는 문제를 해결하는 방법은 `synchronization tools 를 high - level language 의 construct 로 만들어서 사용`하는 것

  - `Monitor` 가 이런 방법 중 하나이다.

---

### Monitor Usage

- `monitor type` : **abstract data type (ADT)**

  - **프로그래머가 정의한 operations set 을 포함**한다.

  - **mutual exclusion 을 할 수 있는 방법이 제공**된다.

  - declares the variables : 타입의 instance 의 state 를 정의

    - variables 에 대해 동작하는 body 가 존재

  - 다양한 프로세스가 모니터 안의 변수나 함수에 대해 직접적인 접근을 할 수 있다.

    - monitor 내부에서만 local 하게 동작

<img width="280" alt="스크린샷 2023-04-29 오후 7 40 23" src="https://user-images.githubusercontent.com/87372606/235298457-5d76f332-4e06-400b-ba0a-9762313db654.png">

<img width="307" alt="스크린샷 2023-04-29 오후 7 38 36" src="https://user-images.githubusercontent.com/87372606/235298414-00069735-d421-4410-be51-63a6f0879589.png">

- monitor 가 있으면 하나의 프로세스가 접근해서 shared data 나 initialization code 를 통해 프로세스가 해야되는 다양한 function 들을 실행하고 이 동작을 다른 프로세스들은 밖에서 기다리고 있다가 자기 차례가 됐을 때, monitor 안으로 들어와서 실행한다.

  - `한번에 하나의 프로세스만 monitor 안에서 활성화` 된다. (Only one process at a time is active within a monitor)

  - **critical section 에 대해 race condition 이 발생하는 것을 막는다.**

  - synchronization schemes 에 따라 모델링을 할 때, 원하는 모든 기능을 포함시키기 어렵다.

    - `synchronization 에만 중점`을 둬서 만든 type 이기 때문에 실질적으로 각 프로세스의 성능을 최대로 끌어올리기에는 부족하다.

---

- 맞춤형 synchronization 을 위해 `condition construct` 를 만들었다.

  - condition x 나 y 에 대해 synchronization 을 할 수 있다.

  - wait 와 signal 함수를 invoke

  - x 에 대해 어떤 조건이 만족했을 때, x.wait 를 해서 프로세스를 suspend

  - 이후 signal 을 통해 suspend 된 프로세스를 release

```
condition x, y;
x.wait();    // process is suspended
x.signal(); // release a suspended process. No effect without any waiting process
```

- **세마포어에서는 signal 을 하면 어떤 식으로든 세마포어에 영향을 미치는데**, condition 을 이용한 monitor 에서의 signal 함수는 **대기하고 있는 프로세스 (waiting process) 가 없으면 signal 함수를 실행하지 않은 것처럼 영향을 주지 않는다.**

- (ex) Q: suspended, P: x.signal() --> Q is resumed 될 때 2 가지 가능성

  1. signal and wait : P 가 x.signal 을 통해 release 됐지만 Q 가 resumed 되고 monitor 를 떠날 때까지 기다리거나, another condition 을 기다린다.

  - P either waits until Q leaves the monitor or waits for another condition

  2. signal and continue : P 가 signal 을 통해 release 됐지만 P 가 계속 어떤 행동을 취해서 monitor 에 머물러 있는 경우 Q 는 resume 했지만 P 가 monitor 를 떠날 때까지 기다리거나, another condition 을 기다린다.

  - Q either waits until P leaves the monitor or waits for another condition

  - P 가 signal 하고 나서 어떤 행동을 하냐에 따라 1, 2 상황이 발생할 수 있다.

---

## Summary

- `Race condition` : 프로세스의 명령어들이 수행되는 과정에서 실행 순서에 따라 서로 다른 결과를 나타내는 경우

  - shared data 의 값이 의도치 않게 바뀔 수 있기 때문

- `Critical section` : shared data 를 갖고 있는 영역, possible race condition

- `A solution to the critical section problem`

  1. mutual exclusion

  2. progress

  3. bounded waiting

- 세 가지 조건을 만족하는 software solutions : `Peterson's solution`

- `Hardware support` for the critical section problem

  1. memory barriers

  2. hardware instructions (CAS and atomic variables)

- `Mutex lock` : mutual exclusion 에 충실한 방법, acquire a lock

- `Semaphores` : mutual exclusion, integer value

- `Monitor` : high - level form of process synchronization. condition variables

---
