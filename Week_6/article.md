# 6강 Threads : Concept and operation

`Chapter 4 에서 배우는 내용 : Threads & Concurrency`

- 병렬 처리와 다중 처리에 대해 공부한다.

- 스레드를 구성하는 기본 컴포넌트

  - 스레드와 프로세스의 차이점을 중심으로

- 멀티스레드 프로세스를 설계하는데 어떤 이점이 있고 어려운 점이 있는지

- Implicit threading

  - thread pools, fork - join, Grand Central Dispatch 방식

- 리눅스에서 스레드 예제

  - Design multithreaded applications using the Pthreads threading APIs

---

- [4-1. OverView](#4-1-overview)

  - [Motivation](#motivation)

  - [Benefits](#benefits-of-multithreaded-programming)

- [4-2. Multicore Programming](#4-2-multicore-programming)

  - [Challenges](#multicore-programming-challenges)

  - [성능 향상 포인트 : Amdahl's Law](#amdahls-law)

  - [Types of Parallelism](#parallelism-의-종류)

- [4-3. Multithreading Models](#4-3-multithreading-models)

- [4-4. Thread 라이브러리](#4-4-thread-libraries)

  - [Pthread 예제](#pthread)

---

## 4-1. Overview

<img width="580" alt="스크린샷 2023-04-05 오후 7 40 54" src="https://user-images.githubusercontent.com/87372606/230057670-8f9d937f-fcd3-451e-93f4-cae3ee99c493.png">

- `스레드 (Thread)` : CPU 를 활용하는데 있어서 가장 기본적인 유닛 (A basic unit of CPU utilization)

  - **스레드 ID, program counter, register set, stack 으로 구성**

  - 스레드들은 code, data, open files 나 signals 같은 OS resources 를 `공유`한다.

- 하나의 프로세스 안에 `다중 스레드 (Multiple threads)` 가 존재하는 경우 **한번에 하나 이상의 작업을 수행**할 수 있다. (more than one task at a time)

  - **_코드, 데이터, 파일들은 공유하고 각 스레드 마다 register, stack, program counter 를 가지고 있다._**

---

### Motivation

<img width="580" alt="스크린샷 2023-04-05 오후 7 46 59" src="https://user-images.githubusercontent.com/87372606/230058929-d3a6a160-ad1d-4663-8ccf-0767e13d7758.png">

- 멀티스레드 응용프로그램의 예시

  - Creating photo thumbnails : 분리된 이미지에 대해 분리된 스레드를 생성하여 동시에 여러 이미지에 대해 스레드 수 만큼의 일을 처리할 수 있다.

  - Web browser : 이미지, 텍스트를 display 해주는 스레드 / retrieve data 하는 스레드

  - 워드 프로세서 : display 하는 스레드 / 입력에 반응하는 스레드 / 맞춤법 검사를 하는 스레드

- **다중 컴퓨터 코어 (multiple computing cores) 를 통해 CPU 를 많이 사용하는 일 (CPU - intensive tasks) 을 병렬적으로 처리**할 수 있다.

- 웹 서버

  - single 프로세스가 request 들을 accept 할 때

  - request 를 받을 때마다 요청에 대한 서비스를 제공하기 위해 별도의 프로세스를 생성한다.

  - 이런 경우에 별도의 프로세스를 생성하는 것보다 별도의 스레드를 생성하는 것이 더 효율적이다.

---

### Benefits of multithreaded programming

- `Responsiveness` (응답성)

  - 프로세스 중 일부가 blocking 되거나 오랫동안 동작하더라도, 해당 스레드를 제외하고 나머지 스레드는 동작을 한다.

  - UI 를 디자인할 때 유용하다.

- `Resource sharing`

  - 스레드들은 메모리와 리소스들을 공유한다.

- `Economy` (경제성)

  - 스레드를 생성하는 것은 생성시간이 프로세스보다 적고 메모리도 더 적게 소모한다.

  - context switching 도 더 빠르다.

- `Scalability` (확장성)

  - 서로 다른 프로세스 코어가 존재할 때, 각각 스레드를 할당하여 병렬로 실행시킬 수 있다.

  - 코어 숫자가 늘어나면 스레드 숫자를 늘릴 수 있다. (스레드 생성을 조절 가능하다)

---

## 4-2. Multicore Programming

<img width="944" alt="스크린샷 2023-04-05 오후 7 58 55" src="https://user-images.githubusercontent.com/87372606/230061335-6d7347ba-c255-456e-9ef4-7260dcef3d6f.png">

- **멀티코어 (`Multicore`) : 하나의 프로세싱 칩에 컴퓨팅 코어가 다수 있는 것 (`multiple computing cores on a single processing chip`)**

- 멀티코어가 장착되어 있는 시스템에서 프로그래밍

  - 멀티 스레드 프로그래밍은 concurrency 를 증가시킴으로써 멀티 코어를 효과적으로 사용할 수 있는 mechanism 을 제공한다.

- `Concurrency (다중 실행)` : **모든 task 가 실행이 되고 있는 것**

  - `single core` : 스레드 실행이 시간에 따라 time sharing 하면서 실행 (**시간 사용을 짧게짧게 해서 사용자는 모든 task 가 실행되고 있다고 느낀다.**)

  - `multicore` : 코어 개수 만큼 병렬로 실행 가능 (`parallel = parallelism = simultaneous`)

  - concurrency 는 parallelism 개념을 포함하고 있으며 parallel 하지 않아도 싱글 코어로도 time sharing 을 통해 구현할 수 있다.

---

### Multicore programming Challenges

- `Identifying tasks`

  - 서로 분리되어있고 concurrent 하게 수행될 수 있는 task 들을 식별해서 분리해놔야한다.

  - 서로 dependency 가 없어야지만 parallel 하게 수행될 수 있다.

- `Balance`

  - 스레드가 수행할 task 들이 서로 비슷한 정도의 작업을 하도록 해야한다.

  - 작업을 잘 분산해야한다.

- `Data splitting`

  - 별도의 코어에서 실행할 데이터들을 적절히 분리되어야한다.

  - 각 스레드들이 영향을 받지 않고 독립적으로 실행될 수 있다.

- `Data dependency`

  - 여러 task 가 있을 때 각 task 들이 다른 task 의 출력을 받아 작업을 실행하는 상황이 발생하면 여러 스레드에서 독립적으로 실행하기 어렵다.

  - dependency 가 있을 때 task 의 실행 순서를 적절하게 맞춰야한다.

  - 이런 데이터 dependency 의 동기화에 대해서 ch.6 에서 다룬다.

- `Testing and debugging`

  - 멀티 코어에서 병렬적으로 실행되는 프로그램에 대해 test 하고 디버깅 하는것은 더 어렵다.

---

### Amdahl's Law

<img width="546" alt="스크린샷 2023-04-05 오후 8 21 03" src="https://user-images.githubusercontent.com/87372606/230065969-2a0f9fed-5470-42d8-abbc-bf466bccda27.png">

- N 개의 프로세서 코어, S 는 serial execution (dependency 가 있는 경우) 이 필요한 task 의 일부분 (0 <= S <= 1)

- N 개의 프로세서가 있을 때 전체 시스템의 `speed up 은 maximum 1/(S+((1-S)/N))` 이 된다.

  - speed up 효과는 1/S 에 의해 결정된다.

- 50 % 의 serial execution portion 이 존재할 경우 성능향상은 최대 2 에 수렴한다.

- `serial execution portion 을 얼마나 잘 줄이냐는 것이 멀티코어 프로그래밍을 할 때 핵심이다.`

---

### Parallelism 의 종류

<img width="462" alt="스크린샷 2023-04-05 오후 8 26 05" src="https://user-images.githubusercontent.com/87372606/230067047-258fd185-b649-4c4c-a67f-31ba110d8ed1.png">

1. `Data parallelism`

   - multiple 코어에 데이터를 어떻게 잘 분산시킬 수 있을 까...

   - 데이터를 잘 분산시켜서 각 코어에서 비슷한 operation 을 수행할 수 있도록 한다.

   - (ex) N 개의 수를 summation 할 때, 0 ~ N/2 - 1 까지 core 0 의 스레드 A 에 할당하고, N/2 ~ N - 1 까지 를 core 1 의 스레드 B 에 할당

<img width="462" alt="스크린샷 2023-04-05 오후 8 26 17" src="https://user-images.githubusercontent.com/87372606/230067090-0080629b-3f5c-47b8-8c05-12cc1bcb1798.png">

2. `Task parallelism`

   - multiple 코어에 대해 tasks 자체 (스레드) 를 어떻게 잘 분산시킬지 = 어떤 작업을 하도록 분산할지에 대해 (distributing tasks)

   - 각각의 스레드들은 독자적인 작업을 수행한다. (unique operation)

   - 동일한 데이터 혹은 다른 데이터에 대해서도 작업을 할 수 있다.

   - (ex) core 0 의 스레드 A 는 평균 구하기, core 1 의 스레드 B 는 분산 구하기

- 두 방식은 Mutually exclusive 한 것이 아니라, 조합이 되어서 동작될 수 있다. (hybrid approach)

---

## 4-3. Multithreading Models

- `유저 스레드` : 커널의 도움 없이 운영체제 유저 스페이스에서 실행되는 스레드

- `커널 스레드` : 운영체제가 직접적으로 지원하면서 관리하는 스레드

  - (ex) Windows, Linux, MacOS

- 유저 스레드와 커널 스레드를 연결하는 방식에 따라..

<img width="377" alt="스크린샷 2023-04-05 오후 9 07 21" src="https://user-images.githubusercontent.com/87372606/230075642-5c05236b-ca30-49d5-b50a-56f1aedbdefb.png">

1. `Many - to - One 모델`

   - 유저 스페이스에서 여러 개의 유저 스레드가 실행되고 있을 때, 커널 스페이스에서 커널 스레드와 연결이 되어서 여러 개의 유저 스레드가 번갈아가면서 커널 스레드를 통해 실행되는 방식

   - `Maps many user - level threads to one kernel thread`

   - 유저 스페이스 에서는 **thread libraray 에 의해 스레드를 관리하기 때문에 효과적**이다.

   - 커널 스레드가 blocking system call 에 의해 막히면 전체 프로세스가 block 된다.

   - **하나의 커널 스레드와 연결되기 때문에 하나의 스레드만 실행이 되고 multicore system 에서 실행되어도 `parallel 한 동작이 불가능`하다.**

   - 유저 스페이스에서 생성하는 스레드의 개수에는 제한이 없다. (but no parallelism)

<img width="298" alt="스크린샷 2023-04-05 오후 9 11 12" src="https://user-images.githubusercontent.com/87372606/230076423-070c1d21-9dca-4e1b-ae9b-6cc57ab73466.png">

2. `One - to - One 모델`

   - **하나의 유저 스레드와 하나의 커널 스레드가 각각 연결**된다.

   - 하나의 스레드가 blocking system call 하더라도 다른 스레드는 실행될 수 있다.

   - **multiprocessor 가 있는 시스템에서는 `parallel 하게 multiple 스레드가 동작`**할 수 있다.

   - 1 대 1 mapping 이기 때문에 유저 스레드와 커널 스레드의 숫자가 동일하고, 너무 많아지면 시스템 성능에 부담이 될 수 있기 때문에 일반적으로 제한을 둔다.

   - cores in processors 가 증가하면서 제한이 의미가 없어졌다. (**코어의 숫자가 증가하면서 수용할 수 있는 스레드가 많아지기 때문**)

   - (ex) Linux and Windows

<img width="294" alt="스크린샷 2023-04-05 오후 9 11 23" src="https://user-images.githubusercontent.com/87372606/230076439-0bb9e0ea-d458-4df3-8d99-9da3b9b18e07.png">

3. `Many - to - Many 모델`

   - 유저 스레드와 유저 스레드보다 더 작거나 같은 숫자의 커널 스레드를 multiplexing 하게 연결한 방식

   - 커널 스레드의 개수는 코어 수에 따라 결정

   - 유저 레벨에서는 스레드의 제한이 없고 여러 스레드가 실행되기 때문에 parallelism 하다.

   - 구현이 복잡하다. (multiplexing 때문)

<img width="294" alt="스크린샷 2023-04-05 오후 9 18 07" src="https://user-images.githubusercontent.com/87372606/230077988-510c6ec9-52af-42ae-8dc5-eb1ddb5f16cb.png">

4. `Two - level 모델`

   - many to many 모델과 one to one 모델을 조합한 것

   - one to one 모델에서는 반드시 실행되어야하는 유저 스레드를 커널 스레드가 항상 supported 하고 있기 때문에 안정적으로 실행이 가능하다.

   - 나머지는 many to many 모델에서 우선순위에 따라 적절하게 수행된다.

---

## 4-4. Thread Libraries

- 스레드 라이브러리 : 스레드를 생성하고 관리하기 위한 API 들의 집합

  - user - level 라이브러리 : local function call 형태로 구현

  - kernel - level 라이브러리 : system call 형태로 구현

  - (대표적인 예) `POSIX Pthreads`

    - POSIX 는 유닉스, 리눅스 표준이므로 유닉스나 리눅스에서 에서 사용하는 thread 라이브러리

    - user - level, kernel - level 둘 다 가능하다.

  - Windows thread 라이브러리는 커널 레벨 라이브러리로만 구현해서 유저 레벨에서 사용할 수 없다.

  - 자바 스레드 API : 자바 프로그램은 host system 에서 JVM 형태로 실행되기 때문에 host system 의 운영체제에 따라 형태가 달라진다.

    - Windows API on Windows, Pthreads on UNIX, LINUX, and MacOS

- `스레드 라이브러리를 사용하는 경우 전역변수들은 POSIX 나 Windows 에서 모든 스레드가 공유한다.`

- `Asynchronous threading`

  - parent 와 child 가 concurrent 하게 실행되고, 서로 독립적으로 (dependency 가 거의 없이) 실행이 된다.

  - parent 와 child 는 data sharing 이 거의 없다.

- `Synchronous threading`

  - parent 는 child 가 완전 termination 이 된 후 다시 재시작 할 때까지 기다려야한다.

  - 스레드 사이 data sharing 이 많은 경우 이 방식으로 구현한다.

---

### Pthread

- IEEE 1003.1c 라는 표준으로 만들어졌고, 스레드의 behavior 를 규정하는 specification 이다.

  - `a specification for thread behavior, not an implementation`

<img width="988" alt="스크린샷 2023-04-05 오후 9 33 44" src="https://user-images.githubusercontent.com/87372606/230081653-9d8eb28a-8d56-4e09-b6f3-176e28c86a64.png">

- `예제`

  - pthread.h 를 include

  - thread 간 data sharing 을 위해 전역변수 sum 을 선언

  - runner 함수는 thread 가 생성될 때 call 하는 함수

  - pthread_t 는 thread id 를 정의하기 위한 data type

  - pthread_attr_t 는 thread 의 스택 size, 스케쥴링 정보와 같은 attribute

  - pthread_attr_init 로 thread 의 attribute 를 초기화

  - pthread_create 함수를 통해 thread 를 생성

    - integer parameter N(arg[1])을 통해 스레드를 생성

    - i 를 1 ~ N 까지 더하는 summation 을 하는데 마지막 최종 더하는 값이 N. 이 값을 main 을 실행시킬 때 argv 로 입력하면 받아서 스레드를 생성한다.

  - pthread_join 은 thread 가 실행 완료되었을 때 함수를 빠져나간다.

  - parent 프로세스와 thread 는 sum 을 공유하고 있고 thread 에서 runner 함수를 실행시킨다.

    - param 을 main 실행시킬 때 argv 로 받은 N 값을 받았다. 그리고 이 값을 integer 로 변환하여 저장한다.

    - 그리고 더하는 연산을 수행한 후 pthread_exit 함수로 빠져나간다.

- `멀티 스레드방식`

  - multithread 방식에서는 define 으로 thread 수를 정해놓는다.

  - pthread_t 의 아이디를 workers 라고 하는 이름으로 10 개의 생성한다. 그리고 create 를 통해 생성을 한다.

  - for 루프를 통해 pthread_join 을 하면 10 개의 thread 가 끝난 이후 다른 작업을 하도록 만들 수 있다.

---
