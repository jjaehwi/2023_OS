# 8강 CPU scheduling : Process scheduling algorithm

`chapter 5` 에서 배우는 내용

- 프로세스에 대해 공부할 때, ready state 의 프로세스를 CPU 에 할당하면 (`disaptch`) running state 가 됐다.

  - dispatch 를 하는 동작이 CPU scheduling 과 관련이 있다.

- 다양한 CPU scheduling algorithm 대해 알아본다.

- Scheduling criteria

- Multiprocessor and multicore scheduling

- Real - time OS 에서의 Scheduling algorithm

- Window, Linux, Solaris OS 에서 사용하고 있는 scheduling

- CPU scheduling algorithm 을 evaluate 하기 위한 모델링과 시뮬레이션

- 몇 가지 CPU scheduling algorithms 을 구현

---

- [5-1. Basic Concepts](#5-1-basic-concepts)

  - [CPU scheduler](#cpu-scheduler)

    - [Nonpreemptive or cooperative scheduling](#nonpreemptive-or-cooperative-scheduling-방식)

    - [Preemptive scheduling](#preemptive-scheduling-방식)

  - [Dispatcher](#dispatcher)

- [5-2. Scheduling Criteria](#5-2-scheduling-criteria)

- [5-3. Scheduling Algorithm](#5-3-scheduling-algorithm)

  - [FCFS](#first---come-first---served-fcfs-scheduling)

  - [SJF](#shortest---job---first-sjf-scheduling)

  - [Round - Robin](#round---robin-scheduling)

  - [Priority](#priority-scheduling)

  - [Multilevel Queue](#multilevel-queue-scheduling)

- [5-4. Thread Scheduling](#5-4-thread-scheduling)

- [5-5. Multi - Processor Scheduling](#5-5-multi---processor-scheduling)

  - [멀티프로세서 스케쥴링 접근법](#멀티프로세서-스케쥴링-접근법)

  - [Multicore Processors](#multicore-processors)

    - [Chip multithreading](#chip-multithreading-cmt)

  - [Multithreaded cores](#multithreaded-cores)

  - [Load Balancing](#load-balancing)

  - [Processor Affinity + NUMA](#processor-affinity-numa)

  - [Heterogeneous Multiprocessing (HMP)](#heterogeneous-multiprocessing-hmp)

- [Summary](#summary)

---

## 5-1. Basic Concepts

- **멀티 프로그래밍의 목적**은 `CPU 의 utilization 을 maximize 하는 것`이다.

- 프로세스가 running state 에 들어가서 실행되면 waiting state 로 갈 때까지 execution 된다.

  - I/O request 와 같은 동작이 필요할 경우 프로세스는 waiting state 로 간다.

  - 이 때, CPU 는 놀게 되는데 이 놀고있는 CPU 를 다른 일을 하게 하는 것이 멀티 프로그래밍

  - 이것을 하기 위해서는 메모리에 여러 개의 프로세스가 대기를 하고 있어야한다. (ready queue)

  - 그래서 CPU 가 idle 할 때, 다음 프로세스를 CPU 에서 바로 실행할 수 있도록 한다. (`scheduling before use`)

  - 이 때, `여러 개의 프로세스 중 어떤 프로세스를 선택할 것인가가 CPU scheduling 의 핵심`

<img width="303" alt="스크린샷 2023-04-21 오전 1 42 35" src="https://user-images.githubusercontent.com/87372606/233432772-f8691393-4167-4030-bb4c-f17ff8b61037.png">

- `CPU - I/O Burst Cycle`

  - `CPU burst` : CPU 가 실행을 하면서 연산을 진행하는 시간

  - `I/O burst` : I/O 컨트롤러가 진행하는 I/O operation 을 진행하는 시간

    - CPU 가 I/O 컨트롤러에게 명령을 내려서 응답을 기다리기 때문에 직접적으로 CPU 가 작업을 하진 않는 것.

  - (ex) 파일을 read 하거나 write 할 때, 네트워크을 통해 통신할 때, I/O burst 가 일어난다. CPU burst 와 I/O burst 가 번갈아 일어나는 것이 일반적인 프로세스의 execution 이다.

  - CPU burst 가 여러 개 있는데, 각 CPU burst 의 시간을 측정하여 모아 놓은 것이 Histogram of CPU - burst 그림이다. 그림을 보면 짧은 CPU - burst 가 많은 것을 알 수 있다.

<img width="517" alt="스크린샷 2023-04-21 오전 1 46 03" src="https://user-images.githubusercontent.com/87372606/233433522-97072de5-1191-4c39-bcbc-8159b249b82b.png">

- `I/O - bound 프로그램` : I/O burst 가 많은 프로그램, many short CPU bursts

- `CPU - bound 프로그램` : CPU burst 가 많은 프로그램, a few long CPU bursts

---

### CPU scheduler

- `CPU 스케줄러` : ready queue 에 있는 프로세스를 선택하여 CPU 에 allocation 한다. 그래서 **ready state 에서 running state 가 되도록 transition**.

- `Ready queue`

  - FIFO queue, 먼저 도착한 프로세스가 먼저 실행되는 방식

  - priority queue, 우선 순위를 두고

  - tree 구조

  - simply an unordered linked list, 링크에 따라

  - queue 의 형태에 따라 스케쥴링 하는 방식이 달라진다.

- PCBs 에는 queue 의 history 가 기록된다.

- `CPU - scheduling 이 일어나는 상황`

1. 프로세스가 시스템 콜을 실행한 결과로 running state 에서 waiting state 로 거거나 wait() 시스템 콜을 invocation

2. 프로세스가 running state 에서 ready state 로 가는 경우 (ex) interrupt 가 발생해서 강제로 ready 상태로 바뀜

3. 프로세스가 waiting state 에서 ready state 로 가는 경우 (ex) I/O 를 기다리다가 completion 된 경우 (I/O burst 가 끝나고 cpu burst 가 와야하므로)

   - waiting 상태에서는 바로 running 상태로 갈 수 없고 항상 ready 상태를 거쳐야함.

4. 프로세스가 termination 이 되는 경우

---

### Nonpreemptive or cooperative scheduling 방식

- `프로세스 자체가 CPU 를 스스로 released 할 때까지 계속해서 점유`하는 경우 (위의 상황에서 1, 4 의 상황)

  - (ex) I/O 동작에 의해 waiting 상태로 가거나, 스스로 wait 시스템 콜을 해서 wait 로 가는 경우

  - (ex) termination 되는 경우

  - 스케쥴링 입장에서 보면 그 프로세스가 언제 running 상태가 될 것인지 스케쥴러는 할 수 있는게 없고, 현재 실행되고 있는 프로세스가 스스로 빠져나올 때까지 기다리는 수 밖에 없다.

---

### Preemptive scheduling 방식

- `scheduling scheme 에 의해 state switching 이 가능한 경우` (위의 상황에서 2, 3 의 상황)

  - (ex) 스케쥴러에 의해 running state 에 있는 프로세스를 강제로 ready state 로 보낸다.

  - (ex) waiting state 에 있는 프로세스를 ready state 로 보낼 수 있다.

  - 이 때, ready state 로 갈 때 어느 위치에 배치할지, 프로세스가 언제 ready state 로 갈지 스케쥴러가 결정한다.

  - **여러 개의 프로세스 간에 data sharing 이 일어나는 경우 `race condition` 이 발생할 수 있어서 주의**해야한다.

---

### Dispatcher

- **ready state 에 있는 프로세스를 running state 로 바꿔주는 역할**을 한다.

  - **ready queue 에 있는 프로세스 중 선택해서 CPU 에서 실행**할 수 있도록 해주는 것

  - `CPU 코어에 대한 제어권을 프로세스에게 주는 것`

<img width="364" alt="스크린샷 2023-04-21 오전 2 20 21" src="https://user-images.githubusercontent.com/87372606/233441167-f6750504-f4ea-4ddd-92fc-70a835e9f222.png">

1. `switching context` 를 진행하고

- (ex) P0 프로세스를 PCB0 에 state 를 저장하고, 다음 프로세스를 PCB 에서 restore

2. `user mode 로 switching` 하고

- (ex) context switching 은 kernel mode 에서 이뤄지는데 restore 된 P1 이 실행되기 위해서는 user mode로 변환이 되어야한다.

3. user 프로그램에서 다시 시작할 수 있도록 `적절한 location 으로 Jumping` (pc 값을 바꾸는 것)

- (ex) P1 이 실행되기 위해서는 일반적으로 이전에 실행되다가 바뀐 상태가 있을테니까 그 멈췄던 상태에서부터 시작되어야하기 때문에 programe counter 를 이전 멈췄던 program counter 값으로 바꾼다.

- `dispatch latency`: 하나의 프로세스가 멈추고 다음 프로세스가 시작될 때까지의 시간

  - 실질적으로 유저 프로세스가 실행이 되지 않기 때문에 그 시간을 `dispatch latency` 라고 하는 것.

<img width="610" alt="스크린샷 2023-04-21 오전 2 24 05" src="https://user-images.githubusercontent.com/87372606/233441899-625ef3ce-dbfb-4cbc-b6a0-15072f348323.png">

- Linux 에서 `vmstate` 이라는 명령어를 통해 context switching 이 얼마나 일어나는지 알 수 있다.

  - (ex) vmstat 1 3 : 1초 동안 발생한 딜레이의 context 와 관련된 정보를 3 줄에 걸쳐 나타내라라는 명령어

<img width="553" alt="스크린샷 2023-04-21 오전 2 26 34" src="https://user-images.githubusercontent.com/87372606/233442449-8959d633-67b2-4044-8c71-c3ac6f7476eb.png">

---

## 5-2. Scheduling Criteria

- scheduling 과 관련된 기준은 scheduling algorithm 을 선택하는데 영향을 준다.

1. `CPU utilization`

- **CPU 가 idle 상태 없이 얼마나 바쁘게 돌아가는지** (busy 해야 좋은 알고리즘)

- 이론상 0 ~ 100 이지만 실제에서는 40 (작업량이 많지 않은 시스템) ~ 90 (작업량이 많은 시스템) 정도 이다.

- Linux, macOS, UNIX 는 CPU utilization 을 top command 를 통해 알 수 있다.

2. `Throughput`

- **주어진 단위 시간당 완료한 (completed) 프로세스의 수**

- start state 로 시작해서 termination 까지 된 프로세스 가 completed 프로세스

3. `Turnaround time`

- 프로세스가 실행하는데 필요한 시간 : 실제 CPU 가 처음 running state 에 들어가서 completed 될 때까지의 시간

- start 에서 **ready 상태로 transition 된 순간부터 termination 까지의 시간**

  - start 와 termination 을 제외한 중간의 ready, running, waiting state 에 머무르는 시간

4. `Waiting time`

- ready state (ready queue) 에 머무르는 시간

- `실제적으로 scheduling algorithm 에 영향을 미친다.`

  - running 과 waiting 에 머무르는 것은 알고리즘이 어떻게 할 수 없으므로

5. `Response time`

- **running 상태에서 CPU 가 실행이 되어서 실행 결과로 내부적으로 첫 번째 response 가 일어날 때까지의 시간** (`첫 번째 response 가 나타나는 시간`)

- 사용자가 보는 response 시간이 아님. (출력으로 전달하는 시간도 있기 때문)

- 목표

- **CPU utilization 과 throughput 을 `maximize`** 하기 위함.

  - CPU 가 최대한 바쁘게 돌아가고, 단위 시간당 실행되서 완료되는 프로세스가 많게 하는 것

- **turnaround time, waiting time, response time 은 `minimize`** 하기 위함

- Scheduling algorithm 을 최적화 하는 방법

  - 위의 5 가지 criteria 에 대해 평균 값이 높도록 하는 것

  - 경우에 따라 minimun or maximun value 가 원하는 값에 이르도록 하는 것이 중요할 수 있다.

    - (ex) guarantee service time

  - **interactive system 의 경우 response time 의 평균 값을 minimize 하는 것 보다 response time 의 variance (분포) 를 minimize 하는 것**이 중요

---

## 5-3. Scheduling Algorithm

- 알고리즘을 살펴보기 전 몇 가지 가정

  - 모든 프로세스는 하나의 CPU burst 만 존재한다.

  - average waiting time 을 비교한다.

  - 프로세싱 코어는 하나만 있다.

---

### First - Come, First - Served (FCFS) Scheduling

- Simplest

- FIFO queue 를 구현하면 자연스럽게 구현할 수 있다.

- `convoy effect` 가 발생할 수 있다.

  - CPU burst 가 큰 프로세스가 있을 경우 다른 프로세스들이 기다려야한다. (All the other processes wait for the one big process to get off the CPU)

  - (ex) I/O bound 프로세스가 많고 CPU bound 프로세스 (CPU burst 가 길다) 가 하나 있는 경우 (Lower CPU and device utilization)

  - 이런 경우 CPU burst 가 짧은 프로세스가 먼저 CPU 를 사용하도록 하는게 효과적이다

- Nonpreemptive 방식으로 동작하면 interactive system 에서 문제가 발생한다.

<img width="626" alt="스크린샷 2023-04-21 오후 7 49 26" src="https://user-images.githubusercontent.com/87372606/233617944-484e6698-d18b-47e5-8c0f-c093f55831a8.png">

- ready queue 에 들어갈 프로세스를 나열하고 각 프로세스의 CPU burst time 이 있다. gantt 차트를 통해 프로세스가 실행되는 과정을 나타낸다.

  - 먼저 온 것이 먼저 실행되기 때문에 1 의 경우 p2 와 p3 는 대기하게되는 비효율적이게 된다.

    - waiting time : p1 - 0, p2 - 24, p3 - 27 로 51 / 3 = 17 이 된다.

  - 2 의 경우는 p1 - 6, p2 - 0, p3 - 3 이므로 9 / 3 = 3 이 된다.

  - 이렇게 어떤 프로세스가 먼저 도착하냐에 따라 극단적으로 바뀔 수 있다.

---

### Shortest - Job - First (SJF) Scheduling

- CPU burst 시간이 짧은 job 부터 먼저 스케쥴링을 진행한다.

  - `shortest - next - CPU - burst` : 남아있는 CPU burst time 이 얼마인지 중요

  - `남아있는 CPU burst 시간이 가장 작은 프로세스를 먼저 dispatch 하는 방식`

    - 같다면 FCFS 방식을 따른다.

  - average waiting time 을 최소화하는 것을 목표로 한다.

- Nonpreemptive 방식

- Preemptive 방식 (`shortest - remaining - time first: SRTF`)

<img width="619" alt="스크린샷 2023-04-21 오후 7 57 55" src="https://user-images.githubusercontent.com/87372606/233619616-c79aed77-8ee0-46fa-a9fd-4248e012d9cb.png">

- 1 의 경우 도착하는 시간이 전부 같은 경우

- 2 의 경우 도착하는 시간이 다른 경우 (preemptive 방식을 가정할 때,,)

  - 도착한 것들 중 남아있는 CPU burst time 이 적은 것을 선택한다.

  - nonpreemptive 인 경우 p1 이 다 실행되고 p2 -> p4 -> p3 로 실행이 된다.

<img width="574" alt="스크린샷 2023-04-21 오후 8 09 44" src="https://user-images.githubusercontent.com/87372606/233621647-30bac176-d6f1-429b-a125-581c86d3f9da.png">

- SJF 는 구현할 때 문제가 발생한다 : remaining next - CPU - burst 정확히 알 수 없다.

  - 이런 경우 **approximate SJF scheduling** 을 사용한다.

  - **next CPU burst 가 과거의 CPU burst 와 비슷할 것이라고 예측**하는 것

---

### Round - Robin Scheduling

- `Preemptive 방식`을 따른다. : **프로세스간 context switching 이 가능한 경우 적용**가능하다.

- `Time quantum` : generally, 10 ~ 100 ms

- Ready queue: circular 동작을 할 수 있는 FIFO

- `하나의 프로세스가 CPU 를 1 time quantum 만큼 사용하고 switching 하는 방식`

  - **CPU burst < 1 TQ** : 프로세스는 쓸 만큼 다 쓰고 CPU 를 바로 release (termination or waiting 으로 감)

  - **CPU burst > 1 TQ** : interrupt 에 의해 context switch 돼서 쫓겨남 (ready 로 감)

    - timer 의 1 TQ 를 세팅해서 이 시간이 지나면 CPU 에 interrupt 를 보냄

<img width="606" alt="스크린샷 2023-04-21 오후 8 13 13" src="https://user-images.githubusercontent.com/87372606/233622284-a530d3ef-e36d-4b0b-8b2a-47fa06490771.png">

- TQ: 4 로 가정

  - p1 이 먼저 도착하여 실행이 되고 interrupt 에 의해 쫓겨나고 p2 가 실행된다.

  - p2 는 7 에서 끝이나고 그 다음 p3 가 실행이 되고 p3 도 10 에서 끝이난다.

  - 그리고 p1 만 남았기 때문에 4 씩 TQ 를 사용하면서 실행된다.

  - waiting time : p1 - 0 + 6, p2 - 4, p3 - 7 이기 때문에 17 / 3 = 5.66

- `average response time 을 줄이는 좋은 알고리즘`이다.

- **Time quantum 의 크기에 따라, 결과가 많이 달라진다.**

  - **Not too small, not too large**

  - rule of thumb : **80 % of CPU bursts < TQ**

- **Context switching 이 자주 일어나기 때문에 context switching time 도 중요**하다.

  - 일반적으로 10 microsec (< 0.1% of TQ)

<img width="502" alt="스크린샷 2023-04-21 오후 8 23 42" src="https://user-images.githubusercontent.com/87372606/233624211-a9f3180b-aa67-49e5-a129-fbfac3296f78.png">

- (ex) p1, p2, p3 가 있고 T = 10 일 때

  - TQ = 1 이면

    - turnaround time 은 p1 - 28, p2 - 29, p3 - 30 이 된다. 따라서 29

  - 만약 TQ = 10 이면

    - p1 - 10, p2 - 20, p3 - 30 이므로 20 (FCFS 방식이 됨)

---

### Priority Scheduling

- `Priority` : **각 프로세스에 주어지는 값으로 해당 프로세스가 얼마나 우선권을 가지고 실행되는지에 대한 것**

  - priority 가 높으면 먼저 실행된다. (값이 커야 우선권이 높은지, 작아야 높은지는 시스템마다 다르다.)

- 이 알고리즘은 `CPU 를 가장 높은 priority 를 갖는 프로세스에 대해 먼저 할당한다.`

  - FCFS 는 모든 프로세스가 같은 priority 를 갖는 경우였다.

  - SJF 는 next CPU burst 값의 작을 수록 높은 priority 를 갖는다. (inverse of next CPU burst)

<img width="601" alt="스크린샷 2023-04-21 오후 8 33 03" src="https://user-images.githubusercontent.com/87372606/233625705-140ae7f2-be48-4d41-9fa7-de8dfe54347e.png">

- priority 값이 작을 수록 높은 우선 순위로 가정.

  - p2 -> p5 -> p1 -> p4 -> p5 순으로 실행이 될 것.

- Preemptive 방식 or Nonpreemptive 방식

- `indefinite blocking 에 의한 Starvation 문제`가 발생할 수 있다.

  - low priority process 에 대해 starvation 문제가 발생

  - 프로세스가 들어오고나서 추가 프로세스가 들어오지 않을 경우 언젠가 모든 프로세스가 실행이 될텐데, 프로세스가 계속해서 ready queue 로 들어와서 채우게 되면 낮은 우선순위를 가진 프로세스가 나중에 들어온 프로세스 보다 우선순위가 낮아서 실행이 안되고 계속 기다리는 문제

  - `Aging` 을 통해 해결할 수 있다.

    - **ready queue 에 들어온 시간에 비례하여 priority 를 조정**하는 방식

- priority 가 같은 경우 FCFS 방식으로 사용될 수 있지만 그렇지 않은 경우도 있다.

  - 이 경우엔 round - robin 과 priority 방식을 합쳐서 사용될 수 있다.

<img width="574" alt="스크린샷 2023-04-21 오후 8 41 01" src="https://user-images.githubusercontent.com/87372606/233626950-1c4fd591-67db-49f9-9af3-5207ce8b705b.png">

- p2 와 p3 가 교대로 실행되는 것을 확인할 수 있다. (TQ = 2)

---

### Multilevel Queue Scheduling

- `여러 개의 ready queue 를 만들어서 다단계 scheduling 을 하는 방식`

- `queue 사이의 스케쥴링` 과 `queue 내부에서의 스케쥴`링 이 있다.

<img width="179" alt="스크린샷 2023-04-21 오후 8 43 36" src="https://user-images.githubusercontent.com/87372606/233627415-3a3b4f67-b2b3-4f0b-9275-9141222e2327.png">

- 각 priority 에 대해 별도의 queue 를 생성하고, 높은 우선순위를 가진 큐가 선택되면 큐 내부에서는 앞서 본 알고리즘들을 이용하여 프로세스들을 선택하여 CPU 로 보낸다.

  - 어떤 큐를 선택하는 것에 대해 적용 가능한 알고리즘이 있는 경우도 있다.

<img width="581" alt="스크린샷 2023-04-21 오후 8 46 40" src="https://user-images.githubusercontent.com/87372606/233627982-a63bedda-0e2f-4936-9b86-871cffaa25c4.png">

- 프로세스 형태에 따라 별도의 큐를 갖도록 하는 방식도 많이 사용된다.

  - 유저 에 비해 커널 이 우선순위를 가지게 한 것 (system 프로세스 > interactive 프로세스)

---

### Multilevel Feedback Queue Scheduling

- 앞서 본 Multilevel Queue Scheduling 은 한번 프로세스가 큐에 들어가면 그 queue 안에서만 동작을 하게 된다.

  - **queue 사이에서의 프로세스 이동이 허용되지 않는 것**.

- **Multilevel Feedback Queue Scheduling 은 `queue 사이 프로세스의 이동이 허용`되는 알고리즘** 이다.

- 지금까지 본 모든 CPU - scheduling 알고리즘을 포함할 수 있는 일반적인 형태의 알고리즘

  - 가장 복잡하다. 여러 개의 parameter 들이 사용되기 때문

  - 몇 개의 큐를 사용할지 (# of queues)

  - 큐를 위한 스케쥴링 알고리즘 (scheduling algorithm for each queue)

  - 프로세스를 언제 우선 순위가 높은 큐로 올려줄지 (when to upgrade to a higher priority queue)

  - 프로세스를 언제 우선 순위가 낮은 큐로 내릴지 (when to demote to a lower priority queue)

    - 우선순위 방식이 아닌 경우 큐를 이동하는 것에 대한..

  - starting queue 를 뭐로 할지

- (ex) long CPU - burst 프로세스를 우선순위 낮은 큐로 옮기거나, long - waiting process (오래 기다린 프로세스) 를 높은 우선순위 큐로 옮기는 것

<img width="261" alt="스크린샷 2023-04-21 오후 8 53 32" src="https://user-images.githubusercontent.com/87372606/233629087-819b1b77-0b16-4045-a2de-68433a9cd732.png">

- 처음 들어오는 프로세스는 q0 로 들어오는데 여기 queue 는 TQ = 8 인 round robin 알고리즘이 적용되는 곳 이다.

  - 만약 TQ = 8 동안 실행이 끝나지 않으면 ready queue 로 가서 기다려야하는데 multilevel 이기 때문에 해당 프로세스는 q1 으로 옮겨간다.

  - q1 은 TQ = 16 인 round robin 알고리즘이 적용되어 있다. 하지만 우선순위가 낮기 때문에 q0 에 대기하고 있는 프로세스가 없을 때 실행이 된다.

  - q1 에서도 끝내지 못한 경우 다음 우선순위가 낮은 q2 로 이동을 하고 이 곳은 FCFS 여서 nonpreemptive 방식으로 실행이 되고 여기서 차례가 와서 실행되면 일을 끝낼 수 있게 된다. 하지만 q0 와 q1 에 대기하고 있는 프로세스가 없어야만 한다.

  - q0 와 q1 이 empty 되지 않으면 q2 는 starving 상태가 될 수 있는데 이런 상황을 방지하기 위해 aging 을 적용하여 우선순위가 높은 queue 로 이동하도록 만들어 줄 수 있다.

---

## 5-4. Thread Scheduling

- 지금까지는 싱글 프로세스에 대해 scheduling 을 알아봤는데 thread scheduling 에 대해 알아본다.

- thread scheduling 의 경우에 user - level 과 kernel - level 스레드에 대해 scheduling issue 가 있다.

- `Contention Scope`

  - **Process - contention scope (PCS)** : thread 라이브러리에 의해 user level thread 를 LWP 에 스케쥴링

    - many - to - one 모델 or many - to - many 모델

    - 일반적으로 프로그래머가 정한 priority set 에 기반하여 스케쥴링을 한다. (어떤 스레드가 더 높은 우선순위를 갖는지..)

    - `PTHREAD_SCOPE_PROCESS`

  - **System - contention scope (SCS)** : OS 커널에 의해 LWP 의 kernel thread 를 실제 CPU 코어에 스케쥴링

    - LWP 가 있고 kernel thread 가 있는데 kernel thread 가 실제 CPU 에 assign 되어야 실행이 가능한 것이다.

    - 여러 kernel thread 중 어떤 것을 CPU 에 assign 해서 실행할 것인지에 대한 것.

    - `PTHREAD_SCOPE_SYSTEM`

- 리눅스와 macOS 에서는 PTHREAD_SCOPE_SYSTEM 만 허용한다.

---

## 5-5. Multi - Processor Scheduling

- `멀티프로세서 시스템`의 경우에는 스케쥴링이 더욱 복잡해진다.

  1. 하나의 프로세서 칩 안에 멀티 코어 CPU 가 있는 경우 (`Multicore CPUs` : 칩 하나에 프로세스를 실행할 수 있는 코어가 여러개인 것)

  2. 하나의 코어에서 여러 개의 스레드를 실행할 수 있는 경우 (`Multithreaded cores`)

  3. `NUMA systems`

  4. 동일한 종류의 프로세서가 아닌 서로 다른 종류의 프로세서 사이 멀티 프로세싱이 가능한 CPU (`Heterogeneous multiprocessing`)

---

### 멀티프로세서 스케쥴링 접근법

- `Asymmetric multiprocessing`

  - Master server (하나의 프로세싱 코어) : 모든 스케쥴링을 결정, I/O processing, 기타 다른 시스템 activities

  - 나머지 코어들은 유저 코드를 실행하는 방식

  - 스케쥴링은 마스터 서버에서만 이뤄지면 돼서 스케쥴링 자체는 간단하지만 마스터 서버에서 bottleneck 이 발생할 가능성이 있다.

    - 코어에서 부하가 걸리지 않더라도 master server 때문에 부하가 걸려서 CPU 코어가 idle 이더라도 유저 코드가 실행되지 않을 수 있는 것.

- `Symmetric multiprocessing (SMP)`

  - 모든 프로세서가 자체적으로 스케쥴링한다. (`self - scheduling`)

  - 앞서 본 싱글 프로세서를 위한 스케쥴링 알고리즘을 각 프로세서 별도 따로 적용될 수 있는 것

  - 이 때 사용될 수 있는 전략 (2 가지)

    1. 모든 스레드를 하나의 **공통된 (common) ready queue** 에 둬서 각 코어에 스레드를 할당하는 방식

    - race condition 이 발생할 가능성이 있다.

    2. 각 프로세서가 자기만의 **private queue** 를 가지고 있어서 private queue 에 대해서만 스케쥴링을 진행

    - 특정 큐에 많은 스레드가 존재하거나, 각 스레드가 CPU intensive 한 경우 해당 코어는 바쁘게 동작해야하지만 다른 코어는 놀고 있을 수 있는 코어 간 `load balancing` 문제가 발생할 수 있다.

<img width="533" alt="스크린샷 2023-04-26 오전 1 06 01" src="https://user-images.githubusercontent.com/87372606/234336880-604e75d8-7475-4861-8361-521bda01e5d9.png">

---

### Multicore Processors

- `멀티 코어는 하나의 프로세스 칩 안에 여러 개의 코어가 있는 경우`이다.

  - 각각의 코어들이 스레드를 별도로 실행시킬 수 있다.

- 멀티 코어를 갖는 SMP system : faster and consuming **less power**

  - 멀티 칩 프로세서에 비해 빠르고 더 작은 파워를 사용한다.

  - 프로세서가 메모리에 접근할 때, 직접 메인 메모리에 접근하면 `memory stall` 이 발생한다.

    - **`cache miss` 가 발생할 때 나타나는 현상**

    - 프로세서에 비해 메모리가 속도가 느리기 때문에 프로세서가 메인 메모리 D램 에 직접 접근하게 되면 오랫동안 기다려야한다.

    - 메모리에 접근할 경우, 메모리에 요청을 하고 데이터가 나오기 까지 긴 사이클을 기다려야 하는 것

    - 데이터를 읽어와서 내부에서 처리할 땐 compute cycle

    - **cache hit 해서 cache 에서 데이터를 읽어오는 경우 memory stall 은 발생하지 않는다.** (cache hit rate : 90% ~ 95%)

  - `multithreaded processing cores` : **기존 코어들은 하나의 스레드가 할당되면 하나만 처리가 가능**했는데 (하나의 스레드에 대해 레지스터를 할당하고 program counter 값을 그 스레드를 위해 사용하는 것), 하나의 코어 안에 연산기만 공유하고 레지스터나 pc 를 여러 set 를 두면 **스레드 여러 개가 각각의 하드웨어 유닛을 사용하여 하나의 코어에서 여러 스레드가 동시에 실행**되도록 할 수 있다.

    - memory stall 이 발생했을 때는 연산기를 사용하고 있지 않기 때문에, 다른 스레드에 compute cycle 을 진행하여 연산기를 사용할 수 있도록 한다.

    - 레지스터와 pc 값은 각 스레드에 별도로 할당되어있어서, 값이 사라질 필요 없고 스레드 간 context switching 이 필요하지 않다.

    - 이런 방식을 `chip multithreading (CMT)` 라고 한다.

<img width="533" alt="스크린샷 2023-04-26 오전 1 14 28" src="https://user-images.githubusercontent.com/87372606/234338861-a66a0703-7a28-44fc-82cd-212b16a3862a.png">

<img width="592" alt="스크린샷 2023-04-26 오전 1 20 50" src="https://user-images.githubusercontent.com/87372606/234340365-53ccd29b-4eab-4130-b8d4-89e9e2b5b273.png">

---

### Chip multithreading (CMT)

<img width="463" alt="스크린샷 2023-04-26 오전 1 26 54" src="https://user-images.githubusercontent.com/87372606/234341826-61c50609-b478-4e28-8497-a55234a723f2.png">

- 하드웨어 적으로 **구조가 여러 개의 스레드를 실행할 수 있도록 되어있는 것.** (**_프로세서는 하드웨어 threading 을 위한 것만 추가되어있다. 연산기 같은 것은 공유가 되고 register 와 pc 같은 최소한의 것들만 추가되는 것_**)

- `Intel` : hyper - threading (aka simultaneous multithreading or SMT) 이라고 부름

  - 코어 당 2 개의 스레드를 실행할 수 있도록 되어있다. (2 threads per core)

- Oracle Sparc M7 (8 개의 코어) : 8 threads per core

  - OS 입장에서는 64 개 (8 x 8) 의 logical cores 가 있다고 인식해여 스케쥴링을 진행할 때, 64 개의 threads 를 할당한다.

<img width="463" alt="스크린샷 2023-04-26 오전 1 28 40" src="https://user-images.githubusercontent.com/87372606/234342252-f27447e2-4b83-4eb0-80eb-da4d33d9cfb0.png">

- 예시

```
AMD Ryzen^TM 9 5950X

CPU 코어 수 : 16
스레드 수 : 32              // 코어 하나당 2 개의 스레드를 실행할 수 있도록 하드웨어 구조가 되어있는 것
최대 부스트 클럭 : 최대 4.9GHz // 코어를 특정 제한하여 그 내에서 사용할 경우 최대 4.9GHz 까지 동작 주파수를 끌어올려서 CPU 성능을 올릴 수 있다.
기본 클럭 : 3.4GHz          // 모든 코어를 사용할 때는 3.4GHz 로 동작
총 L2 캐시 : 8MB            // 모든 코어는 자체적인 L1 캐시를 갖고 있고 보통 1MB 이내 사이즈, L2 캐시는 코어 바깥에 있는 캐시로 모든 코어들이 공유하고 있는 캐시, 이 캐시를 통해 프로세서 내 각 코어들이 데이터를 공유할 수 있다.
총 L3 캐시 : 16MB           // L3 캐시는 하나의 반도체 실리콘 칩이 아니라 여러 개의 실리콘 칩이 있을 때, 하나의 큰 칩 안에 캐시를 둬서 이 칩들 사이의 데이터를 공유할 때 사용하는 것
                          // (라이젠 칩 안의 여러 개의 실리콘 칩과 L3 캐시 (multichip packaging 방식), 여러 개의 칩 안에 L2 캐시와 코어들, 코어 안에 L1 캐시와 연산기)
기본 TDP/TDP : 105W        // CPU 가 실행되면서 소모할 수 있는 파워 105W 인 것 --> 따라서 모든 코어를 사용할 때 3.4GHz 까지만 동작하는 것
Processor Technology for CPU cores : TSMC 7nm FinFET // 프로세서의 제조는 TSMC 7nm FinFET 방식을 사용했다는 것
```

- 멀티 칩 패키징 방식

<img width="318" alt="스크린샷 2023-04-26 오전 1 41 02" src="https://user-images.githubusercontent.com/87372606/234345279-341dc40e-340e-4655-8afd-d7e52c6dc92d.png">

---

### Multithreaded cores

- Multicore Processors 에서 Multithreading 할 수 있는 core 의 경우 두 가지 접근 방식

  1. `Coarse - grained` 방식 : 한 쪽의 스레드가 long - latency event 가 발생할 때까지 해당 스레드를 계속 실행시키는 것.

  - long - latency event 가 발생해서 실행 못하게 되면 (해당 스레드는 waiting 상태로 가는 것, memory stall 도 마찬가지) `thread switching` 을 진행한다.

  - 다른 스레드가 해당 코어에서 실행되는 것

  - long - latency 가 일어날 때까지 pipeline 형식인데, thread switching 이 일어나면 그 만큼 다시 채우는 `Pipeline flushing` 을 통해 그 지점까지 가야한다.

  - Pipeline flusing 이 일어나고 다시 채우는 사이클 만큼 공백이 발생한다. (high cost of thread switching)

    - context switching 은 PCB 에 필요한 정보를 저장하고 CPU 코어 안에서 해당 스레드를 몰아내고 다른 스레드가 들어오는 것 (cost 가 훨씬 높음)

    - Pipeline flusing 에 의한 thread switching 은 코어 안에 이미 스레드 0 과 1 이 존재하고 하드웨어 내부에서 스레드 0 의 instruction 을 실행하다가 스레드 1 의 instruction 으로 실행하도록 바꾸는 방식

  2. `Fine - grained` 방식 (interleaved) : 명령어 cycle 의 boundary 안에서 thread switching 이 일어남

  - 명령어가 실행되는 단계에 해당하는 하드웨어가 별도로 제공되어야한다.

  - Pipeline flushing 을 하지 않는다. switching 이 필요하면 멈춰있는 그 상태로 명령어가 실행되는 하드웨어로 가고, 그 부분에서부터 다른 스레드가 시작할 수 있게 한다.

  - cost 가 적다. 추가적인 하드웨어가 내부적으로 필요하기 때문에 (extra logic) 코어의 사이즈가 커질 수 있다.

- **`multiple thread 가 프로세스 코어 안에 있지만 concurrent 하게 실행되지만 parallel 하진 않다.`**

  - **연산기 하나 (실행을 위한 하드웨어) 를 공유하기 때문**에 두 스레드의 명령어를 동시에 실행하진 않는 것 (**sharing hardware for execution**)

- 하나의 프로세싱 코어 안에서 멀티 스레딩을 할 경우 `two level 의 스케쥴링`이 일어난다.

  - level 1

    - hardware threads : 프로세스 코어 안에 있는 멀티 스레드를 지원하는 기능 : `OS 입장에서 logical CPU 로 보인다.`

    - OS 는 커널 스레드를 logical CPU 에 할당한다. (many - to - many 같은 스케쥴링이 이뤄지는 것)

  - level 2

    - 하드웨어 내부에서는 하드웨어 스케쥴러가 내부에 할당된 스레드에 대해 실제 프로세싱 코어를 어느 스레드에 할당할지 스케쥴이 결정된다.

    - 동시에 실행되어야하는 스레드의 경우 서로 다른 코어에 할당을 해주고, 그럴 필요가 없으면 같은 코어에 할당하는 방식으로 스케쥴링을 할 수 있다. (sharing - aware algorithm)

<img width="514" alt="스크린샷 2023-04-26 오전 2 15 48" src="https://user-images.githubusercontent.com/87372606/234352874-0f2ab1ad-a64e-4408-bd5b-24df7faf1208.png">

---

### Load Balancing

- Load Balancing 은 모든 프로세스에 걸쳐 workload 를 균등하게 distribute 하도록 하는 것

- 두 가지 접근법

  1. `Push migration` : 정기적으로 각 프로세서의 load 를 checking 해서 만약 특정한 프로세스 코어에 workload 가 많다면 workload 가 적은 쪽으로 옮기는 방식

  - queue 에 있는 모든 task 들을 monitoring 해야하는 부담이 있지만, 가장 적절히 load balancing 가능

  2. `Pull migration` : 프로세스 자체가 자신이 idle 하게 되면 다른 프로세서 코어에 있는 queue 를 보고 대기하고 있는 task 를 가져와서 자기가 실행하는 것

  - idle 한 프로세서가 대기하고 있는 task 를 가져오기 때문에 load balancing 의 효과는 push 보다 안좋다.

  - 두 방식이 mutually exclusive 하지 않다. (ex) Linux CFS scheduler, FreeBSD ULE scheduler

- `Balanced load`

  1. 모든 queue 의 스레드 수 를 비슷하게 유지하는 것

  2. 모든 queue 에 걸쳐 thread priorities 를 동등하게 distribution

---

### Processor Affinity (NUMA)

- 코어에는 캐시가 붙어 있는데 캐시에는 스레드가 실행하는 코드에서 자주 사용하는 데이터들이 들어가 있다. 혼자 사용할 경우 큰 cache hit rate 를 가져갈 수 있지만,

- 여러 개의 스레드가 코어를 번갈아가며 사용할 경우, 각 스레드가 사용하는 데이터가 다를 수 있고 캐시는 용량 제한이 있기 때문에 각 스레드가 실행될 때 캐시는 지워지고 다시 써지는 비효율적으로 운용될 수 있다.

- `warm cache` : 어떤 프로세스나 스레드가 자신이 가져온 데이터를 캐시에 유지하면서 cache hit rete 를 충분히 유지하는 것

  - warm cache 의 장점을 활용하기 위해서는 스레드가 계속해서 같은 프로세서에서 실행되어야하는 것이 유리하다.

  - 만약 스레드가 다른 코어로 옮겨가게 되면 그 캐시를 채울 때까지 memory stall 을 경험해야한다.

- `Processor Affinity` : **하나의 스레드를 동일한 프로세서 에서 계속해서 실행하려고 하는 것**

  - cache miss 를 덜 발생시키기 위함

  - common queue 를 사용하는 것이 아니라, 코어 마다 별도의 ready queue 를 갖는 경우 구현이 쉽다.

- 두 가지 형태

  1. `Soft affinity` : 프로세스가 동일한 프로세서에서 실행되려고 최대한 노력하지만 **guaranteed 되진 않는다**. (load balancing 과 같은 이유)

  2. `Hard affinity` : system call 을 이용하여 **어떤 프로세스를 실행할 수 있는 프로세서의 집합을 정해놓고 그 안에서만 실행될 수 있도록** 하는 것.

  - Linux : soft affinity (default), hard affinity (using sched_setaffinity() system call)

- `non - uniform memory access (NUMA)` : 일반적인 프로세서 구조는 메모리가 가운데 있어서 다같이 공유하는 방식이지만, **NUMA 는 CPU 에 전용 메모리가 존재하고 CPU 와 메모리를 구성하는 시스템 사이를 interconnect 로 연결**했다.

  - 각 CPU 는 모든 메모리에 접근할 수 있다. (자기 구역의 메모리는 fast access, 다른 구역의 메모리는 slow access)

  - NUMA - aware 알고리즘 : NUMA 구조임을 미리 알고, 프로세스나 스레드가 실제 있는 메모리와 가까운 CPU 에 스케쥴링 해서 전체적인 성능을 향상시킨는 것.

<img width="640" alt="스크린샷 2023-04-26 오전 2 31 59" src="https://user-images.githubusercontent.com/87372606/234356477-0b0f2bd8-f87f-4ead-8f52-f26791db900d.png">

- **load balancing 과 processor affinity 는 서로 역작용한다. (Counteraction)**

  - 적절한 tradeoff

---

### Heterogeneous Multiprocessing (HMP)

- 앞서 다룬 멀티 프로세서에 대한 내용은 모든 프로세서는 동일한 성능을 갖는다는 가정하에 배웠지만, **요즘 프로세서들은 내부적 CPU 코어의 명령어는 동일하지만 성능은 동일하지 않은 코어를 섞어서 구성**하는 경우가 많다.

- (ex) ARM's bif.LITTLE 구조, Intel's hybrid technology (P/E core) 구조

  - P, BIG : 성능이 뛰어난 코어

    - interactive tasks (일정한 시간에 결과가 나와야하는 작업), high performance and short burst tasks (높은 성능이 요구되고 빠른 시간에 끝내야하는 작업)

    - 이런 작업이 필요없는 경우 **disabled 시켜서 power - saving mode**

    - 전력을 많이 소모하더라도 빠른 성능이 중요

  - E, LITTLE : 성능은 뛰어나지 않지만 전력소모가 적은 코어

    - low performance or long task (긴 시간에 끝내도 되고 낮은 성능으로도 끝낼 수 있는 작업), background jobs

    - 낮은 성능이더라도 더 작은 전력을 소모하도록 설계됨

```
Qualcomm SD8Gen2 : 1 x Cortex-X3 (성능이 젤 좋음) + 2 x Cortex-A715 + 2 x Cortex-A710 + 3 x Cortex-A510
Intel i9 13900k : 8P + 16E
```

---

## Summary

- CPU scheduling : ready queue 에서 대기중인 프로세스를 선택해서 dispatcher 를 이용하여 CPU 에 할당하는 작업

- Scheduling algorithms

  - preemptive 방식 과 non - preemptive 방식

  - Evaluating criteria

    (1) CPU utilization, (2) throughput, (3) turnaround time, (4) waiting time, (5) response time

  - FCFS : simplest, not efficient for waiting time

  - SJF : optimal for waiting time, 구현 어렵

  - RR : preemption 방식으로 동작, time quantum, 가장 짧은 response time

  - Priority : priority 를 주고 highest priority first

  - Multilevel queue : multiple queue 에 여러 프로세스를 넣어서 priority (다른 알고리즘 가능) 에 의해 지정하고, 각 큐에서의 스케쥴링 알고리즘을 지정 (queue 사이 이동 x)

  - Multilevel feedback queues : process migration between queues (queue 사이 이동 가능), 가장 일반적인 방식

    - 앞선 방식들은 Multilevel feedback queues scheduling 의 특별한 경우들임

- Multicore processors with multiple hardware threads (logical CPUs)

  - 멀티코어 프로세서의 경우 multiple hardware 스레드를 가질 수 있다.

  - 하나의 코어 안에 하드웨어 스레드가 여러 개 있으면 OS 커널은 각각을 logical CPU 로 보고 여기에 스레드를 할당하는 스케쥴을 진행

  - 멀티 프로세서에 대해 스케쥴링을 진행할 경우 load balancing 과 processor affinity 를 고려해야함
