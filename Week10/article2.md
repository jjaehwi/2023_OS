# Deadlock

- Deadlock with mutex locks

- 데드락이 발생할 수 있는 4 가지 necessary conditions

  - Deadlock situation in a resource allocation graph

- Solution to deadlock

  - preventing deadlocks

  - deadlock avoidance

  - deadlock detection

  - recovering from deadlock

---

- [8-1. System Model](#8-1-system-model)

- [8-2. Deadlock in Multithreaded Applications](#8-2-deadlock-in-multithreaded-applications)

  - [Livelock](#livelock)

- [8-3. Deadlock Characterization](#8-3-deadlock-characterization)

  - [System resource - allocation graph](#system-resource---allocation-graph)

- [8-4. Methods for Handling Deadlocks](#8-4-methods-for-handling-deadlocks)

- [Summary](#summary)

---

## 8-1. System Model

- `Resources`

  - CPU cycles, files, I/O devices (ethernet, WIFI, SSD ...)

  - 한 type 에 대해 multiple instances 가 존재하는 경우 : 각 instance 에 대해 request 를 만족할 수 있도록 allocation 되어야한다.

  - **mutex lock 과 semaphore 또한 시스템 리소스이다.**

    - the most common sources of deadlock

- `Utilization sequence of a thread for a resource`

  1. **Request** (리소스가 available 하지 않으면 wait)

  2. **Use**

  3. **Release**

  - request 와 release 는 시스템 콜을 사용한다.

    - request() and release() of a device

    - open() and close() of a file

    - allocate() and free() memory

    - wait() and signal operations on semaphores

    - acquire() and release() of a mutex lock

- `Resource table` : 리소스의 상태를 나타내는 정보를 모아놓은 테이블

  - 특정 리소스, 리소스에 대한 각 instance 가 free 인지 allocate 되어 있는지, 되어있다면 tid 나 waiting queue 에 얼마나 많은 스레드가 있는지 등에 대한 정보를 갖고 있다.

---

## 8-2. Deadlock in Multithreaded Applications

- 데드락은 멀티스레드 상황에서 발생하기 쉽다.

- set 안에 모든 스레드들이 waiting 하고 있을 때, set 안의 하나의 스레드가 이벤트를 발생시켜서 진행해야하는 상황에서 진전 (progress) 이 있을 수 없다.

  - Every thread in a set is blocked waiting for an event caused only by another thread in the set

<img width="625" alt="스크린샷 2023-05-13 오전 1 37 37" src="https://github.com/jjaehwi/2023_OS/assets/87372606/fbfb5ae2-cf34-4277-9a3c-1e785940df32">

- 멀티스레드 Pthread 프로그램에서 데드락 예시

  - 데드락은 특정 상황에서 발생할 수 있다. `항상 발생하는 것이 아니라 특정 조건을 만족해야 데드락이 발생`한다.

    - **스레드 1 과 스레드 2 가 동시에 실행이 되는 상황**

      - 스레드 1 은 첫 번째 뮤텍스를 얻은 후 두 번째 뮤텍스를 얻고, 스레드 2 는 두 번째 뮤텍스를 얻은 후 첫 번째 뮤텍스를 얻는다.

      - 이 때, 어느 한 쪽이 뮤텍스를 unlock 하지 않으면 두 스레드는 뮤텍스를 얻을 때까지 기다린다. unlock 으로 가기 위해서는 뮤텍스를 2 개 얻어야하는데 그런 상황이 발생하지 못한다.

    - **스레드 1 과 스레드 2 가 sequential 하게 실행되는 경우**

      - 스레드 1 이 뮤텍스를 얻고 unlock 한 후 스레드 2 가 뮤텍스를 얻고 unlock 한다.

      - 이 경우에는 데드락이 발생하지 않는다.

---

### Livelock

- `Livelock` : **스레드가 끊임없이 어떠한 action 을 시도하는데 실패하는 것**

  - `not blocked`, **but no progress** (동작을 진행하고 넘어가야하는데 같은 동작이 반복되어서 진전이 없는 것)

  - running state 에 있어서 **running 하고 있긴하다.**

  - less common than deadlock

<img width="643" alt="스크린샷 2023-05-13 오전 1 49 19" src="https://github.com/jjaehwi/2023_OS/assets/87372606/d77bc924-d269-4a38-81bd-c12bb1278015">

- trylock 은 mutex 를 요청하되, 그것이 available 하지 않을 때 계속 waiting 하는 것이 아니라 포기하고 다시 요청한다.

  - 함수를 빠져 나왔다가 다시 진입하는 것.

  - 얻지 못하면 else 가 실행되므로 얻은 mutex 를 unlock 하고 다시 while 문을 돈다.

- 스레드 1 과 스레드 2 가 동시에 같은 속도로 실행되고 있다면, 첫 mutex 를 얻은 후 **다음 mutex 에 대해 서로 얻지 못하고 계속해서 처음 mutex 만 unlock 하고 acquire 하고 만 반복하는 livelock 상황이 발생**한다.

  - 두 스레드가 `약간의 시간 차이만 두고 실행되더라도 문제는 발생하지 않는다.`

  - 실패하게되면 다음 실행부터 random 하게 시간차이를 두고 실행한다. (ex) Ethernet

---

## 8-3. Deadlock Characterization

- `데드락이 발생하는 필요조건 4 가지` : **4 가지가 동시에 만족**되어야지 데드락이 발생한다.

  1. `Mutual exclusion` : **resource in a nonsharable mode**

  - 하나의 리소스 instance 를 두 개 이상의 스레드가 공유하지 못할 경우

  - 하나의 스레드가 리소스를 hold 하고 있으면 다른 스레드는 리소스에 접근할 수 없다.

  2. `Hold and wait` : A thread holding a resource and waiting additional resources held by other.

  - 하나의 스레드가 리소스를 assign 받아서 holding 하고 있는 상황에서 다른 스레드가 갖고 있는 리소스를 waiting 하고 있는 것

  3. `No preemption` : a resource can be released only voluntarily

  - 리소스는 한 번 할당되면 스레드가 release 할 때 까지 강제로 뺏기지 않는다.

  4. `Circular wait` : implies hold - and - wait, **not completely independent**

  - hold and wait 상황을 만족하는 상황에서 시작과 끝이 request 로 서로 물려있는 경우

- `Resource - Allocation Graph` 를 통해 조건이 만족하는지 쉽게 파악 가능하다.

<img width="418" alt="스크린샷 2023-05-13 오전 2 01 58" src="https://github.com/jjaehwi/2023_OS/assets/87372606/d9dd2c0a-0918-4197-b21a-eb027dfc3780">

- 스레드 1 이 first_mutex 를 holding 하고 있는 상황에서 스레드 2 에 할당되어 있는 second_mutex 를 waiting 하는 **hold and wait 상황**

- 하나의 리소스 (mutex) 는 하나의 스레드에만 할당되고 공유되지 못하는 **mutual exclusion 상황**

- first_mutex 가 스레드 1 에 할당되어 있으면 스레드 1 이 release 하기 전까지 스레드 2 나 커널이 first_mutex 를 뺏어갈 수 없는 **no preemption 상황**

- 스레드 1 이 first_mutex 를 holding 하고 있는 상황에서 스레드 2 에 할당되어 있는 second_mutex 를 waiting 하는 hold and wait 상황 에서 스레드 2 가 first_mutex 를 waiting 하고 있는 **circular wait 상황**

---

### System resource - allocation graph

<img width="230" alt="스크린샷 2023-05-13 오전 2 27 39" src="https://github.com/jjaehwi/2023_OS/assets/87372606/7e9c3661-b097-434f-a1af-5a8cecbf1a8a">

- `a set of vertices V` and `a set of edges E`

  - `V` : T = {T1, T2, ... Tn} : active threads and R = {R1, R2, ... Rm} : resource types

  - `Request edge` : **Ti -> Rj (스레드에서 리소스로 가는 edge)**: Ti 스레드가 Rj 리소스의 instance 를 request 하고 waiting

  - `Assignment edge` : **Rj -> Ti (리소스에서 스레드로 가는 edge)** : Rj 리소스의 instance 가 Ti 스레드에 allocate 되어있다는 의미

- (ex) T = {T1, T2, T3}, R = {R1, R2, R3, R4}, E = {T1 -> R1, T2 -> R3, R1 -> T2, R2 -> T2, R2 -> T1, R3 -> T3}

  - `Resource instances` : 리소스 내의 **dot 는 instance 의 개수** (dot 개수 만큼 스레드에 allocation 가능하다)

  - `Thread states` : edge 화살표를 통해 request 하고 allocation 되었는지 waiting 하고있는지 알 수 있다.

  - `사이클이 존재하지 않는다 = 데드락이 없다.`

<img width="530" alt="스크린샷 2023-05-13 오전 2 35 09" src="https://github.com/jjaehwi/2023_OS/assets/87372606/9322edb6-8bc5-4615-adde-b0ca65ac7d1c">

- `사이클 존재` : **데드락이 발생할 수도 있다. (deadlock may exist)**

  - **각 리소스마다 한 개의 instance 만 존재한다면, 사이클이 존재할 경우 데드락이 발생**한다. (필요충분 : necessary and sufficient)

    - 그림 1 에서 사이클이 있고 R2 의 instance 가 2 개지만 하나는 T2 에 allocation 되어 있기 때문에 deadlock 이 발생한다. (T1 과 T2 모두 waiting 하고 있는 상황)

  - **리소스에 여러 instances 가 존재한다면 사이클은 데드락을 발생시킬 수도 있다.** (necessary but not sufficient)

    - 그림 2 에서 T4 가 release 하게 되면 instance 가 T3 에 할당이 될 수 있고 사이클이 존재하지 않을 수 있기 때문에 waiting 상태가 해소되고 deadlock 이 발생하지 않게 된다. (T4 는 waiting 하고 있지 않은 상황)

---

## 8-4. Methods for Handling Deadlocks

1. `Ignore the problem` : **데드락 무시**

   - Common Linux and Windows

   - 데드락이 발생했을 때 운영체제에서 하는 일은 없고 커널이나 응용프로그래머가 2 번 방법을 통해 조치를 취한다.

   - cheapest

2. `To prevent or avoid deadlocks` : **데드락 발생을 미연에 방지하거나, 데드락이 발생하는 상황을 피한다.**

   - deadlocked state 로 가지않도록 막는 방법 (request 와 allocation 할 때마다 알고리즘이 동작)

   - **Prevention** : 데드락이 발생하는 4 가지 조건 중 적어도 하나를 만족하지 못하도록 한다.

     - resource 에 대한 request 에 대해 제한을 둔다.

   - **Avoidance** : OS 에 request 와 resource 를 어떻게 사용할지에 대한 추가적인 정보를 미리 알려준다.

     - 어떤 스레드가 리소스를 언제 요청하고 어떻게 사용할지 미리 알고 allocation 을 결정한다.

   - 미리 예측하고 피해가도록 하는 것이기 때문에 most expensive

3. `Allow deadlocked state` : **데드락 상태를 허용하고, 발생할 시 detect 해서 recover**

   - (ex) database

   - deadlock 이 발생하는지 주기적으로 detect 하는 알고리즘, detect 시 recover 하는 알고리즘 필요 (**방법 1 보단 overhead 가 크지만 방법 2 보단 overhead 가 적다**)

   - deadlock 을 발견하면 manual 하게 recovery 하는 것이 가장 단순한 방법 (`원인이 되는 프로세스를 kill`)

- `Performance and expense` : 방법 1 처럼 완전 무시하지 않는 이상 추가적인 overhead 때문에 성능 저하가 있고 deadlock 을 피하거나 막는 알고리즘이나 recovery 하는 알고리즘에 대해 높은 비용이 발생할 수 있다.

  - performance 와 expense 사이 `tradeoff`

---

## Summary

- `Deadlock`

  - 리소스를 request 하고 hold 하는 과정에서 데드락이 발생

  - 주로 멀티 스레딩 환경에서 여러 스레드가 리소스를 사용하려고 할 때 발생

    - 데드락을 해결하기 위해 데드락에 관련된 다른 스레드나 프로세스가 해결 해줘야하는데, 데드락에선 모든 스레드나 프로세스가 waiting 상태이기 때문에 해결할 수가 없다.

- `Livelock`

  - 아무 것도 못하고 waiting 상태에 있는 것이 아닌, 각 프로세스나 스레드들이 running 하고 있는데 progress 가 없는 것

- `4 necessary conditions`

  - 4 가지 필요 조건 모두 만족할 때 데드락이 발생할 수 있다.

- `Resource - Allocation Graph`

  - 데드락이 발생할지 안할지 쉽게 알 수 있다.

- `Handling deadlocks`

  - Ignoring

  - Preventing or Avoiding

  - Detect and Recovery
