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
