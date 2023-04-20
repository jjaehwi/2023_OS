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
