# 4강 Process : Concept, state transition

`Chapter 3 에서 배우는 내용 : Process`

- 프로세스의 구성요소 : 표현 방법과 스케쥴링

  - [3-1. Process Concept](#3-1-process-concept)

    - [Memory layout of a C program](#memory-layout-of-a-c-program)

    - [Process State](#process-state)

    - [Process Block](#process-block)

  - [3-2. Process Scheduling](#3-2-process-scheduling)

    - [Queueing diagram](#queueing-diagram)

    - [CPU Scheduling / Context Switch](#cpu-scheduling--context-switch)

- 시스템 콜을 이용한 프로세스의 생성과 소멸

  - [3-3. Operations on Processes](#3-3-operations-on-processes)

    - [fork() system call](#creating-a-separate-process-using-the-unix-fork-system-call)

    - [Process Termination](#process-termination)

- Shared memory 와 message passing 을 이용한 IPC (Interprocess communication)

  - pipes 방식, POSIX shared memory 방식

  - 소켓과 RPC (remote procedure calls) 를 사용한 클라이언트 - 서버 방식

- 리눅스에서 커널 모듈 디자인

## 3-1. Process Concept

- Program (실행 파일 : passive) `in execution` : active

  - 실행 파일 (executable file) 들은 disk (second storage) 같은 곳에 file 형태로 저장되어 있다.

  - 실행 파일이 디스크에서 메모리로 loading 이 되어서 CPU 에 의해 실행이 될 때, 프로그램이 active 한 상태가 되고 이것을 `프로세스`라고 한다.

- Jobs (batch system) 에서 `time-shared 방식`이 되면서 `tasks` 나 `user programs` 로 변경되었다.

- OS 의 internal activities

- 프로세스의 현재 움직임에 대한 status 를 표현할 때 사용하는 자료

  - `program counter` + `processor's registers`

  - program counter : CPU 안에 있는 레지스터의 일종으로, 다음에 실행할 명령어의 주소값이 들어있다.

  - processor's registers : 실행하는 명령어가 동일하다고 하더라도 사용하는 데이터가 다르다면 레지스터의 값이 다를 것이고 그러면 state 가 다른 것이다.

- 프로세스의 `memory layout` (프로그램이 프로세스가 되기 위해서는 메모리로 로딩이 되어야하는데, 로딩이 되었을 때의 메모리의 구성)

  - `Text` : 실행가능한 코드 (프로세서가 인식할 수 있는 명령어)

  - `Data` : global variables (코드에서 global 하게 선언한 변수)

  - `Heap` : 런타임에서 동적으로 할당되는 variable 들

  - `Stack` : 임시적인 데이터 저장공간 (temporary data storage)

    - (ex) activation record 들 : function parameters, return addresses, local variables ...

- 실질적으로 프로그램은 text 와 data 에 해당하는 부분들이 포함하여 description, 프로세스는 heap 과 stack 영역까지 추가로 포함하는 것이다.

- 프로세스 자체가 다른 코드를 실행하기 위한 실행 환경이 될 수 있다.

  - (ex) JAVA : OS 위에 JVM 이 실행되고 코드가 실행된다.

---

### Memory layout of a C program

<img width="668" alt="스크린샷 2023-03-25 오전 12 21 52" src="https://user-images.githubusercontent.com/87372606/227567848-87b2e755-2c7f-46fd-88ff-e7a0e4bca36e.png">

```
#include <stdio.h>
#include <stdlib.h>
int x;
int y = 15;
int main(int argc, char *argv[]){
    int *values;
    int i;

    values = (int *)malloc(sizeof (int)*5);

    for(int i=0; i<5; i++)
        values[i] = i;

    return 0;
}
```

- 소스코드에 해당하는 부분이 컴파일돼서 프로세스에서 실행가능한 코드들을 만들면 그 코드들은 text 영역으로 들어간다.

- 여러 데이터들이 프로세스 메모리에 분포하는데, 메인함수의 파라미터들은 스택 위에 저장이 된다. (실행해서 입력이 될 경우에)

- global 변수 중 초기화가 안된 변수는 uninitialized data 영역에 초기화 된 변수는 initialized data 영역에 들어간다.

- local 변수 중 i 와 values 는 stack 영역에 들어가고, malloc 을 통해 동적 할당된 메모리 영역은 heap 영역에 들어간다.

<img width="457" alt="스크린샷 2023-03-25 오전 12 25 13" src="https://user-images.githubusercontent.com/87372606/227568785-067ff7f2-d6a2-4a12-9e90-2e16c17fb861.png">

---

### Process State

<img width="536" alt="스크린샷 2023-03-25 오전 12 30 38" src="https://user-images.githubusercontent.com/87372606/227570270-1e6e818e-43dd-483a-823c-2d9378887238.png">

- 프로세스가 실행이 되는 과정에서 다양한 state 를 갖는다. 프로세스의 현재 activity 에 따라 정의된다.

- new : 프로세스가 처음 생성된 상태

- ready : 프로세스가 실행되기 위해 대기하고 있는 상태 (CPU 를 할당받지 못해서 현재 실행되지 못하는 경우)

  - ready state 에 `queue` 가 있는데, queue 에 빈자리가 있을 때 생성된 프로세스가 넘어올 수 있다. (admitted)

  - running state 에 자리가 나면 scheduler 가 dispatch

- running : 프로세스가 실행되고 있는 상태 (CPU 를 점유하고 있는 process)

  - running state 에 자리가 있어야 갈 수 있다. (CPU 코어 하나당 한 자리)

  - 할 일을 다 끝낸 후 terminated 상태로 exit

  - time - shared 방식에서는 할당된 시간만큼 일을 다 하지못하면 running 상태에서 쫓겨나서 ready 상태로 간다 :

  - interrupt 가 들어와서 현재 실행하고 있는 것을 멈추고 다른 걸 해야하는 경우 ready 로 쫓겨난다.

  - I/O 동작 혹은 네트워크 통신과 같은 이벤트 동작 들은 CPU 동작에 비해 느리기 때문에 응답이 올 때까지 오랜 시간을 기다린다. 그 경우 프로세스는 waiting 상태로 가서 I/O 가 종료되거나 이벤트가 발생할 때까지 기다린다.
    <queue 구조인데.. 그 동작이 필요해서 running 상태에 올렸는데 waiting 상태로 가면 다른 동작들이 running 이 끝날때까지 기다렸다가 나와야하는건가..>

- terminated : 모든 실행을 끝내고 이 state 를 거쳐 종료된다.

- waiting : 현재 당장 running 상태로 갈 수 없는 프로세스들이 이 state 에 있다.

---

### Process Block

<img width="140" alt="스크린샷 2023-03-25 오전 1 14 48" src="https://user-images.githubusercontent.com/87372606/227581793-04dc93cf-9ad3-49a3-8dc6-979bd0795088.png">

- 프로세스를 생성하면 text, data, heap, stack 영역 외에 `PCB (Process Control Block)` 이라고 하는 영역이 생성된다.

  - `TCB (task control block)`

- 특정한 프로세스와 관련된 다양한 정보들을 포함하고 있다.

- 프로세스를 시작하거나, 재시작할 때 필요한 모든 데이터와 accounting data (여러 사용자가 있는 경우 중요한 데이터) 를 모아놓은 저장소

- process number (PID) : 프로세스 관련 정보를 얻을 때 PID 를 통해 구분

- program counter : CPU 에 있고, 다음 실행할 명령어의 주소값을 가지고 있다.

- registers : CPU 에 포함된 다양한 레지스터에 저장된 값들

- memory limits : 프로세스가 사용하고 있는 메모리의 주소 범위

- list of open files : 프로세스가 사용하고 있는 오픈된 파일의 리스트

- `CPU - scheduling information` : running 상태에 있는 프로세스가 CPU 를 사용하고 있는데, CPU 를 어떤 프로세스가 언제 어떻게 사용하게 할 것인지에 관한 정보

  - process priority, pointers to scheduling queues, other scheduling parameters

- `Memory - management information` : 메모리 관리 관련 정보, 메모리의 시작 주소와 끝나는 주소, 프로세스가 사용하고 있는 메모리 공간과 관련된 정보

  - value of the base and limit registers and the page tables, or the segment tables..

- `Accounting information` : CPU 의 사용량, 실제 사용 시간, time limits, accounting numbers, job or process numbers

- `I/O status information` : 프로세스에 할당된 I/O 장치들의 리스트

- PCB 를 보면 프로세스가 현재 어떤 상태에 있고, 메모리를 어떻게 사용하고 있고, I/O 장치도 어떻게 사용하고 있는지 모든 정보들을 알 수 있다.

---

### Threads (ch.4)

- `CPU 에선 실질적으로 Threads 단위로 실행`된다.

- 하나의 프로세스 안에는 `여러개의 threads` 가 있을 수 있다. (**multicore system**)

- PCB 는 multiple threads 에 대한 thread 정보도 포함한다.

---

## 3-2. Process Scheduling

- 컴퓨터 시스템에 여러개의 프로세스가 존재하는 경우에는 어떤 프로세스를 실행시킬 것인지 결정해야한다. `프로세스의 실행과 트랜지션을 프로세스 스케쥴러`가 한다.

  - 사용가능한 프로세스 중에서 특정한 프로세스를 선택해서 그 프로세스를 실행하거나 적절한 state 로 트랜지션 하도록 한다.

    - **멀티프로그래밍의 목적**은 `CPU utilization 을 최대화` 하는 것이다.

    - **Time sharing 의 목적**은 여러 프로세스가 CPU 를 고루 사용할 수 있도록 하는 것 : `frequent switching` (`multitasking`)

- Multiprogramming : # of process > # of cores

  - 각각의 CPU 코어는 한번에 한 프로세스만 run 할 수 있다. : 실행되지 못하고 기다리고 있는 ready state 의 프로세스가 존재한다.

  - `Degree of multiprogramming` : 현재 메모리에 존재하는 프로세스의 수 (한번에 몇 개의 프로세스를 동시에 메모리에 올려놓을 수 있는지)

    - degree of multiprogramming 이 클 수록 메모리에 많은 프로그램이 올라와 있다는 것

- CPU - bound / I/O bound process 로 나눌 수 있다.

  - CPU - bound 프로세스는 CPU 를 많이 사용하기 때문에 running 상태에 많이 머물러 있는다.

    - 이런 경우 다른 프로세스가 CPU 를 사용할 기회가 적어지기 때문에 CPU 사용이 다 끝나지 않았어도 강제적으로 running 상태에서 ready 상태로 보내게 되는데 이 때 `frequent switching` 이 사용된다.

  - I/O bound 프로세스는 running 상태에서 비교적 빠르게 waiting 상태로 벗어난다.

- Ready queue : ready 상태에 있는 프로세스들은 runnning state 로 갈 준비가 되어있다.

- Wait queue : wait 상태에 있는 프로세스 들은 이벤트를 기다리고 있다.

- 큐는 linkied list 형태로 구성이 되는데 `PCB 에 해당하는 각 정보`들을 가지고 있다.

<img width="347" alt="스크린샷 2023-03-25 오후 1 17 13" src="https://user-images.githubusercontent.com/87372606/227695559-a21929d9-c5ab-4200-9e98-ee7676e76808.png">

---

### Queueing diagram

<img width="428" alt="스크린샷 2023-03-25 오후 1 18 24" src="https://user-images.githubusercontent.com/87372606/227695602-3a3ae448-a1a1-43bf-a87f-634c11ebe2ef.png">

- ready 큐에 있는 프로세스들은 스케쥴러에 의해 `dispatch` 되고 CPU 를 사용하므로 running 상태로 간다.

- running 상태가 끝나면 동작을 완료해서 `termination` 이 되던지 wait 큐로 가던지, ready 큐로 간다.

- frequent switching 은 일정한 시간이 지나면 동작을 멈추고 강제적으로 ready 상태로 보낸다 : `time slice expired`

- I/O 요청을 하거나, child 프로세스를 생성하거나, 외부로부터 interrupt 가 들어오는 경우 wait 큐로 간다.

- **wait 큐에서 ready 상태로 갈 때 높은 우선순위를 갖는 경우가 많다.**

---

### CPU Scheduling / Context Switch

- `CPU Scheduler (Short - term scheduler)` : running 상태로 가는 CPU 와 관련된 스케쥴링을 하는 스케쥴러

  - ready 큐에 있는 프로세스를 선택해서 CPU 코어를 할당해주는데 이 동작은 매우 빈번하게 일어난다.

- `Swapping` : 현재 메모리에 상주하고 있는 프로세스를 메모리에서 일시적으로 제거하는 동작 (ch.9)

  - 메인 메모리에는 한계가 있기 때문

- `Context Switch` : 하나의 프로세스가 running 상태를 오래 점유하지 못하도록 빈번하게 스위칭을 발생하게 함.

  - 컴퓨터에서의 Context : 프로세스를 실행하는데 필요한 정보, 프로세스를 나타내는 모든 정보

    - includes the value of the CPU registers, the process state, and memory - management information in PCB

  - context 와 관련된 정보를 save 하고 restore 한다.

<img width="388" alt="스크린샷 2023-03-25 오후 1 34 29" src="https://user-images.githubusercontent.com/87372606/227696240-9d0fa8a7-fd4d-4688-b489-ff9bc162e3f2.png">

- 프로세스 P0 와 프로세스 P1 이 context switching 이 일어나는 과정

  - 현재 P0 가 running 상태에 있다. P0 의 running 상태가 끝나고 state 가 ready 로 가는 경우 PCB0 영역에 필요한 context 정보를 저장한다. (현재의 register 값, 다시 시작하는데 필요한 정보)

  - 중단되었던 P1 이 다시 실행되어야하는데 P1 이 중단될 때 PCB1 에 저장했던 context 정보를 reload 해야한다.

  - P1 이 running 상태로 넘어오고 P0 는 ready 상태로 간다.

  - P1 동작이 끝났을 때 (interrupt 나 시스템 콜에 의해 중단될 경우) 그 상태의 context 정보를 PCB1 에 저장하고 다시 P0 를 실행시키기 위해 PCB0 에 있던 context 정보를 reload 해서 P0를 running 상태로 실행시킨다.

  - context 를 PCB 에 save 하고 restore (reload) 하는 시간 (`context - switch time`)은 CPU 가 idle 한 `overhead` 이다.

    - **프로세스가 실행되는 시간이 context switching 시간보다는 커야 CPU utilization 을 높일 수 있다.**

---

## 3-3. Operations on Processes

<img width="505" alt="스크린샷 2023-03-25 오후 1 52 55" src="https://user-images.githubusercontent.com/87372606/227696927-e0d6afcc-356f-430a-89d8-016fdf492c06.png">

- 프로세스의 생성 : **하나의 프로세스가 생성되고 그 프로세스가 다른 프로세스를 생성시킬 수 있다.** 생성된 프로세스는 고유한 프로세스 아이디를 가진다. (`PID`) 이는 프로세스의 다양한 attribute 에 access 할 수 있는 index 이다.

- 프로세스가 새로운 프로세스를 생성했을 경우에 `parent - child` 관계가 된다.

- chile 프로세스는 parent 로 부터 PCB 를 copy 한다. (`PID 값은 copy 되지 않는다.`)

- 리눅스의 경우 `systemd` 로 모든 프로세스를 생성한다.

- child 프로세스를 생성할 때 리소스를 받아오는 방법

  - `From OS` : parent 프로세스와 별개로 메모리에 별도 영역에 별도 프로세스처럼 동작을 한다.

    - parent 와 비슷한 동작과 성능을 낼 수 있다.

    - text 와 data 부분 copy 하기 때문에 메모리가 중복되서 사용된다.

  - `From parent` : parent 내부에 child 가 존재하면서 parent 와 리소스를 share 한다.

    - subset 만 사용하도록 메모리가 제한이 된다.

    - text 와 data 부분을 공유받아 메모리를 효과적으로 사용할 수 있다.

- parent 프로세스의 실행 상태

  - child 를 생성하고 나서 child 와 동시에 자기 할 일을 한다. (`Concurrent with child`)

  - child 가 실행한 결과를 받아서 실행하거나 child 가 실행된 뒤 실행한다. (`Waiting until the termination of child`)

- child 의 address - space

  - parent 의 것을 그대로 복사 (duplicate of the parent), parent 가 해야할 일을 child 가 나눠서 하는 경우

  - 새로운 프로그램을 로드 (new program loaded)

---

### Creating a separate process using the UNIX fork() system call

<img width="476" alt="스크린샷 2023-03-25 오후 2 18 39" src="https://user-images.githubusercontent.com/87372606/227697910-3f47e2f6-ee48-457d-930d-ae99cab9c197.png">

```
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
int main(){
    pid_t pid;
    /* fork a child process */
    pid = fork(); // pid(p)>0 : pid of child process; pid(c) : 0
    if (pid < 0) { // error occured. Child starts here
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0) { // child process
        execlp("/bin/ls","ls",NULL); // child != parent now
    }
    else { // parent process
        // parent will wait for the child to complete
        wait(NULL); // transit to wait state
        printf("Child Complete");
    }
    return 0;
}
```

- fork() 를 통해 새로운 child 프로세스를 생성하면 이 때까지의 모든 값이 copy 되기 때문에 parent, child PID 값이 존재한다.

  - parent 프로세스의 PID 값은 > 0 으로 , child 프로세스의 PID 값은 자기자신이기 때문에 0

- 에러가 발생할 경우 `PID 값이 음수`가 return 되므로 에러 처리

- `PID == 0 인 경우 child 프로세스`이고 execlp 라는 시스템 콜을 이용해 ls 라는 리스트 리눅스 명령어를 실행

  - `exec` 는 child 가 생성된 영영에 다른 프로그램을 가져와서 사용하는 방식

  - bin 디렉토리 밑에 어떤 파일이 있는지를 보여주는 동작

- `PID != 0 (PID > 0) 인 경우 parent 프로세스`이고 wait 를 통해 child 프로세스가 끝날 때까지 기다렸다가 끝나면 wait 시스템 콜에서 빠져나와서 출력하고 끝난다.

  - child 프로세스가 exit() 가 실행되면 wait() 시스템 콜에 값을 넘겨 주기 때문에 child 프로세스가 terminate 됐다는 것을 알 수 있다.

---

### Process Termination

- `exit()` 시스템 콜을 통해 child 프로세스의 termination 이 이뤄진다.

  - `리소스는 deallocated` 된다. (including physical and virtual memory, open files, and I/O buffers are deallocated)

  - parent 프로세스가 wait 하고 있는 경우 `return a status value to waiting parent process`

```
(EX.1) exit status = 1

// exit with status 1
exit(1); // explicitly or implicitly

(EX.2)
pid t pid;
int status;
pid = wait(&status) // parent gets exit status and child's pid
// pid 와 status 를 정의해놓고 wait 시스템 콜을 부를 때 status 와 리턴 값을 pid 에 저장하도록 하다.
// pid 에는 종료가 되는 child 의 pid 가 저장된다. 어떤 child 가 어떤 status 를 가지고 종료되었는지 알 수 있다.
```

- `Zombie` 프로세스 : wait 가 call 되기 전에 child 가 termination 되는 경우

  - wait 가 call 되기 전까지만 이 상태로 있는 것. (usually temporary)

- `Orphan` 프로세스 : parent 가 어떤 원인에 의해 wait 를 call 하기 전에 termination 되는 경우

  - 정상적인 상황이 아니다. 리눅스는 child 를 init(systemd)의 새로운 child 로 할당해서 exit() 를 통해 termination 되면 systemd 가 받아서 처리한다.

- 어떤 프로세스가 다른 프로세스에 의해 termination 될 수 있다. (parent 는 child 를 termination 시킬 수 있고, OS 도 termination 시킬 수 있다.)

  - child 가 과도하게 리소스를 많이 사용해서 parent 도 정상적이지 못할 때

  - child 에게 준 task 가 있는데 더 이상 그 일을 할 필요 없지만 계속 하고 있는 경우

  - parent 가 비정상적으로 exit 하거나 termination 된 경우 (`cascading termination`)
