# 11강 Memory management : Paging

- [9-3. Paging](#9-3-paging)

  - [기본 동작](#기본-동작)

  - [특징](#특징)

  - [Frame allocation example](#frame-allocation-example)

  - [Hardware support](#hardware-support)

  - [TLB 동작](#tlb-동작)

  - [Protection](#protection)

  - [Shared pages](#shared-pages)

---

## 9-3. Paging

- **variable partition 의 문제점을 해결하기 위해 등장**

- `paging 은 프로세스의 physical address space 가 contiguous 하지 않다.`

  - **프로세스가 나눠져서 메모리에 allocation** 됨.

---

### 기본 동작

<img width="470" alt="스크린샷 2023-05-22 오전 1 33 24" src="https://github.com/jjaehwi/2023_OS/assets/87372606/1dc4b9d9-46ae-428c-a19a-0dfbbb248f21">

- logical address 가 생성되면 `page number` 와 `page offset` 으로 구분된다.

<img width="620" alt="스크린샷 2023-05-22 오전 1 34 04" src="https://github.com/jjaehwi/2023_OS/assets/87372606/9edd32a6-5d19-439a-aefb-bc7b0ff66e26">

- Basic Method

  - 메모리를 일정한 사이즈의 block 으로 나눈다.

    - `page` : logical address 쪽에서 나눈 block (logical page)

    - `frame` : physical address 쪽에서 나눈 block (physical page)

  - `page number` : 각 logical page 에 붙혀지는 이름, page table 의 index

    - 모든 프로세스마다 page table 이 하나씩 존재한다.

  - CPU 에서 **logical address 를 생성하면 상위 비트가 page number 가 되고, 하위 비트가 page offset** 이 된다.

    - **page number 가 page table 의 index 로 가서 frame number 와 mapping 하여 page number 를 대체한다. 그것이 `frame address (physical address)` 가 된다.**

  - `page table` : page number 가 frame number 로 바꿀 수 있는 index 를 모아놓은 것

  - physical address 가 생성되면 해당 frame 을 찾아갈 수 있고 **d (offset) 을 이용해서 frame 의 시작 주소부터 offset 만큼이 실제 접근하고자하는 위치가 되는 것**이다. (그림에서는 low address 가 위쪽에 있는 모양)

---

<img width="620" alt="스크린샷 2023-05-22 오전 1 44 58" src="https://github.com/jjaehwi/2023_OS/assets/87372606/687747bf-beac-4ed3-b9fe-05b7d990a549">

- page size 는 power of 2 (2^n) 에 해당하는 크기만큼 사용한다. (typically 4 / 8 KB or lager)

  - logical address 가 m bits 일 때, page size 가 2^n 이면 offset 은 n bit 이고, page number 는 m - n bit 에 해당하는 size 를 갖게 된다.

<img width="451" alt="스크린샷 2023-05-22 오전 1 47 20" src="https://github.com/jjaehwi/2023_OS/assets/87372606/ee8643f5-49fe-4af7-b40a-66f95557dc86">

- logical memory 에서 4 개의 page 로 구성되는 프로세스가 있는 경우 page table 을 보면 0 1 2 3 이 index 되어 있고 각 index 에 해당하는 frame number 가 저장되어있다. physical memory 에서는 frame 0 ~ 7 까지 있고 각 페이지가 frame 에 allocation 된다.

  - 이런 동작은 하드웨어인 Memory management unit (MMU) 에서 이뤄진다.

  - 이러한 동작 덕분에 **하나의 프로세스를 연속적인 주소에 배정하지 않더라도 메모리에 접근하는데 문제가 없다.**

<img width="451" alt="스크린샷 2023-05-22 오전 1 49 59" src="https://github.com/jjaehwi/2023_OS/assets/87372606/8a59960f-cffb-4998-9fbf-a804ecb8f216">

- logical memory 에서 위에서부터 0000, 0001, 0010, 0011, 0100, 0101, 0110, 0111 ... 이런 식으로 앞에 2 비트는 page number 가 되고, page number 에 대해 page table 에서 frame number 를 mapping 할 수 있다. 뒤에 2 비트는 offset 이 된다.

  - (ex) d 가 위치한 0011 은 page table 을 지나면 10111 이 되고, offset 11 을 제외하면 10100 이 20 이므로 physical memory 의 20 으로 가서 offset 인 11 을 통해 d 를 찾을 수 있다.

---

### 특징

1. `No external fragmentation`

2. `Internal fragmentation 은 발생`할 수 있다.

   - 마지막 page 의 frame 에서만 발생한다.

   - (ex) page size : 2kB, and a process of 72,766 bytes --> 이 size 의 프로세스를 수용하기 위해 35 pages 와 1086 bytes 가 추가로 필요하다.

     - page size 는 2kB (2048 bytes) 이므로 2048 - 1086 만큼의 internal fragmentation 이 발생한다.

     - 36 개의 frame 이 필요하고 962 bytes 의 internal fragmentation 이 발생

   - fixed size partition 에 할당하는 것보다는 작은 internal fragmentation 이다.

   - page size 가 작을 수록 internal fragmentation 에서는 이점이있다.

     - 하지만 **page table 이 늘어나고 page table 을 관리하기위한 overhead 가 증가**한다.

3. `page sizes have grown as processes, data sets, and main memory have become larger`

   - overhead is involved in each page table entry (page table entry 를 관리할 때 발생하는 overhead)

   - Linux : a default page size (typically 4 KB), 또는 architecture dependent 한 larger page size called `huge pages`

     - architecture dependent : 리눅스 머신마다 다른 값을 가질 수 있다라는 의미

     - getpagesize() 시스템 콜 이나 getconf PAGESIZE 같은 쉘 명령어를 통해 확인 가능하다.

4. `Page - table entry`

   - 4 bytes long on a 32 - bit CPU : 2^32 의 physical page (4kB) 를 가질 수 있다. -> 2^44 bytes (16TB) of physical memory

     - logical address 가 32 bit 이기 때문에 관리할 수 있는 4 GB 여서 page table 에서 2^44 bytes 의 메모리를 mapping 할 수 있다고 해서 2^44 bytes 의 메모리를 사용할 수 있는 것은 아니다.

5. `운영체제 는 각 프로세스마다 page table 의 copy 를 하나씩 갖고 있다.`

   - copy 는 address 를 parameter 로 갖는 시스템 콜을 부를 때 이용된다.

     - ex. buffer address -> 프로세스 내의 특정 주소 영역을 buffer 로 사용하는 경우, 시스템 콜 하면서 OS 는 copy 로 부터 정보를 얻어서 사용할 수 있고 이 과정에서 address translation 이 사용된다.

   - CPU dispatcher : copy 는 CPU 스케줄링을 할 때 사용된다.

   - page table 을 copy 해서 사용하는 것은 context - switching 할 때, page table 관리에 필요한 시간만큼 추가로 더 소요되는 것이기 때문에 context - switch time 이 증가한다.

---

### Frame allocation example

<img width="630" alt="스크린샷 2023-05-22 오전 2 25 00" src="https://github.com/jjaehwi/2023_Network/assets/87372606/c203b7cd-ea00-46b5-ba5c-cc375b53b6e6">

- 새로운 프로세스가 있고 4 개의 page 를 사용한다. 14, 13, 18, 20, 15 의 사용하지 않는 frame list 가 있다. 이 5 개의 free - frame list 에 4 개의 page 를 할당한다.

  - new - process page table 이 생성된다. 그리고 남은 15 는 그대로 free - frame list 에 존재한다.

- **프로그래머는 logical page 들을 보고 있는 것이고, 실제로 각 page 에 해당하는 data 들은 physical memory 에 저장**되어있다.

  - 실제 데이터가 저장된 메모리와 프로그래머가 보는 메모리와의 차이가 있을 수 있다는 것

  - 하드웨어 적으로 MMU 를 통해 address - translation 되어 mapping 된다.

- `page table 은 프로세스마다 생성이 되지만` `free - frame list 나 frame table 은 시스템의 하나만 존재`한다.

---

### Hardware support

- page 마다 전용 hardware register 가 존재하면 효과적으로 address - translation 을 진행할 수 있지만 page table 이 큰 경우 적합하지않다.

  - page table 의 크기가 엄청 크기 때문에 (2^20) 여기에 전용 hardware register 를 사용하는 것은 현실적이지 않다.

- **page table 을 reloading 할 때마다 context - switching time 이 증가하기 때문에 추가적인 overhead 가 발생**한다.

  - page - table base register (PTBR) 이라는 large page table 을 메모리에 갖고 있어야한다.

  - hardware register 로 구현하지 않고 메모리에 구현하는 경우 레지스터에 비해 속도가 매우 느리다.(slow memory access time)

  - 메모리에 있는 page table 에 access 하고 나서 physical address 를 얻으면 또 데이터를 얻기 위해 메모리에 access 를 해야해서 **실질적으로 두 번의 memory access** 가 일어난다.

- `Translation Look - Aside Buffer (TLB)` : **위의 단점들을 해결하기 위해 TLB buffer 를 사용**한다.

  - `TLB` 은 일종의 CPU 에 있는 cache 와 비슷하게 **cache 역할을 하는 빠른 속도의 메모리로 구성**되어있다.(`special`, `small`, `fast - lookup hardward cache`)

  - large table 을 메모리에 두고 접근하는 것보다 cache 에서 읽어올 수 있으면 훨씬 빠르게 address - translation 을 진행할 수 있게 된다.

  - TLB 는 instruction pipeline 의 일부로 동작하기 때문에, performance penalty 가 발생하지 않는다.

    - TLB 접근을 위한 추가적인 cycle 이 필요하지 않기 때문에 명령어가 실행되면서 TLB 에서 address - translation 이 발생하면 메모리에 한 번만 접근해서 읽어오면 되는 것.

  - `TLB hit `: 패널티 없이 address - translation 이 일어나서 메모리에 있는 데이터에 접근 가능

  - `TLB miss` : TLB 안에 원하는 page table 에 대한 정보가 없는 것, 메모리에 접근해서 page number 를 읽어오고 TLB entry 에 추가해야한다. (메모리에 접근하는 시간 + TLB entry 에 추가하는 시간)

  - `Replacement policy` : TLB 가 full 인 경우 TLB miss 가 발생하면 새로운 entry 를 추가하기 위해 기존의 entry 와 대체해야한다.

    - 이 때 가장 오래전에 사용된 entry 를 제거하는 LRU, round - robin 방식, random 방식 등 여러 policy 가 있다.

    - OS 구현에서 결정 또는 CPU 설계시 결정

  - `Wired down entries` **in some TLBs** : key kernel code 와 같은 entry 는 중요하니까 replace 되지 않도록 고정한다. (never removed entries)

  - `Address - space identifier (ASIDs)` **in some TLBs** : 각 프로세스마다 unique 한 identifier 를 제공해서 구분할 수 있도록 한다.

    - 특정 프로세스를 address - space 에서 protection 하기 위한 방법

---

### TLB 동작

<img width="630" alt="스크린샷 2023-05-22 오전 2 58 26" src="https://github.com/jjaehwi/2023_Network/assets/87372606/5933b508-da4c-467b-9dd7-1ebcba1b0055">

- `TLB` 는 `page number 와 frame number 가 쌍으로 되어있는 table`

  - page table 에서는 p 가 index 이기 때문에 어디로 들어가야하는지 알 수 있지만 **TLB 는 cache 이기 때문에 p 가 들어오면 저장되어 있는 모든 page number 와 비교해서 매칭되는 frame number 가 있으면 찾아낸다.**

  - `있다면 TLB hit` 가 돼서 f 값을 바로 넣을 수 있다.

  - `없다면 TLB miss` 가 발생하면 메인 메모리에 존재하는 page table 에 접근하여 p 를 index 로 접근하여 f 를 찾아낸다. 찾아낸 entry 는 TLB 에 추가한다.

    - physical address 를 통해 physical memory 에 접근하는 것과 TLB entry 에 추가하는 동작이 동시에 진행하도록 만들 수 있어서 extra penalty 가 되지 않을 수도 있다.

---

- `Hit ratio and effective memory - access time` (한 번 access time = 10 으로 가정)

  - hit ratio = 80% 인 경우, 20% 는 miss (**miss 면 메모리에 두 번 access** 해야한다)

    - 0.80 x 10 + 0.20 x 20 = 12 ns (20% 의 성능 감소)

  - hit ratio = 99% 인 경우, 1% 는 miss

    - 0.99 x 10 + 0.01 x 20 = 10.1 ns (1% 의 성능 감소)

  - 하지만 TLB 를 사용하지 않는 경우 무조건 20 ns 이기 때문에 TLB 를 사용하는 것이 훨씬 effective 하다.

- `multiple levels of TLBs` (ex. Intel Core i7)

  - 128 - entry L1 instruction TLB and a 64 - entry L1 data TLB

    - L1 에서 miss -> 512 - entry 의 L2 TLB 사용 (L2 에서 entry 를 가져오는데 6 cycles 이 소모된다.)

    - L2 에서 miss -> 메모리에 접근해서 page - table entry 를 가져온다. (수 백 cycles 또는 OS 가 interrupt 를 걸고 작업하도록 처리)

- TLB 가 어떻게 구현돼있는지 따라 OS 의 paging 구현 방법이 달라진다. (`TLB design (hardware) affects paging implementation of the OS`)

---

### Protection

- `page table 에 protection bit 를 추가할 수 있다.` 앞에서 본 page table 에는 page index 와 frame number 만 있었는데 실제 page table 에는 다양한 정보가 있다.

  - **protection bit 는 read, write, execute 조합으로 구성**된다.

  - 동일한 프로세스에 할당된 frame 이더라도 frame 마다 protection bit 가 다르다면 특정 영역은 write 를 못하도록 protection 할 수도 있고, read 를 못하게 막는 등의 동작이 가능하다.

<img width="513" alt="스크린샷 2023-05-22 오전 3 26 02" src="https://github.com/jjaehwi/2023_Network/assets/87372606/faa70e11-cdb4-403f-be64-d4703644f65b">

- `Valid bit` : **logical address space 에서 해당 page 가 valid 한지 안한지 알려주는 비트**

  - (ex) 14 bit address 의 경우 주소는 0 ~ 16383 까지 사용하는데, 프로그램의 address 가 0 ~ 10468 까지만 사용한다면 14 bit 의 8 개의 page 중 0 ~ 5 까지의 6 개의 page 만 사용한다.

  - page table 에서 0 ~ 7 까지 8 개의 page 를 갖고 있지만 이 프로세스에 할당된 frame 은 6 개만 할당된다. 그리고 해당 page 에 대해 valid 로 표시되고 사용되지 않은 page 에 대해서는 invalid 를 표시한다.

  - invalid 에 접근하려고 하면 trap 발생해서 에러 처리

- `page - table length register (PTLR)` : page table 의 size 를 나타낸다.

  - valid address range 를 확인할 수 있다.

---

### Shared pages

- paging 시스템에서는 모든 메모리를 page 단위로 관리하기 때문에 두 개 이상의 프로세스가 shared 메모리 영역을 갖는 경우 그 shared 메모리 영역은 shared page 형태로 관리가 된다.

- 대부분의 프로세스는 standard C 라이브러리인 libc 라이브러리를 필요로 하기 때문에 메모리를 아끼기 위해 sharing 해야한다. (To save memory, sharing is desirable)

  - 공유하기 위해 특정 메모리 영역에 넣어두고 모든 프로세스가 해당하는 페이지에 접근할 수 있게한다.

  - `Reentrant code` 로 구성되어야함 : `non - self - modifying code` = **실행하는 동안 절대 변하지 않는 코드**

    - 실행하는 동안 값이 바뀌면 sharing 할 수 없다. 언제 사용하더라도 똑같은 값을 넣으면 항상 동일한 return 값이 나오는 경우에만 sharing 이 가능하다.

  - Each process has its own copy of registers and data storage

    - **각 프로세스는 libc 라는 라이브러리를 공유 (예시) 하지만 대신 register 와 local data 는 자신의 copy 를 가진다.**

- 컴파일러, window 시스템, db 시스템 처럼 서로 공유할 수 있는 라이브러리들은 shared page 를 통해 공유해서 memory 를 효과적으로 사용할 수 있다.

<img width="566" alt="스크린샷 2023-05-22 오전 3 31 45" src="https://github.com/jjaehwi/2023_Network/assets/87372606/3da2aaf9-3864-4cc1-9e71-ece359e6cb1e">

- 세 개의 프로세스가 있을 때, 모두 4 개의 page 를 차지하는 libc 를 사용하고 있다.

  - 각 프로세스마다 별도의 page table 을 갖고 있을테지만, libc 에 대한 frame 은 전부 3 4 6 1 로 되어있다. (`서로 다른 page table 이지만 동일한 frame number 를 갖고 있는 것`)

  - 각각의 프로세스에서 libc 에 해당하는 부분은 physical memory 에서 볼 때 전부 같은 곳에 접근하게된다.
