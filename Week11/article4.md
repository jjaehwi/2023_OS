# 11강 Memory management : Page table and swapping

- [9-4. Structure of the Page Table](#9-4-structure-of-the-page-table)

  - [Hierarchical Paging](#hierarchical-paging)

  - [Hashed Page Tables](#hashed-page-tables)

    - [Clustered Page Tables](#clustered-page-tables)

  - [Inverted Page Tables](#inverted-page-tables)

- [9-5. Swapping](#9-5-swapping)

  - [Paging 시스템에서 Swapping](#paging-시스템에서-swapping)

- [Summary](#summary)

---

## 9-4. Structure of the Page Table

- Hierarchical Paging

- Hashed Page Tables

  - Clustered Page Tables

- Inverted Page Tables

---

### Hierarchical Paging

- page table size 계산

  - 32 - bit logical address space + page size 4 KB (2^12) 인 경우

    - page table 은 1M (2^32 / 2^12 = 2^20)

    - page table 에서 frame 과 information 을 지정하는 entry 가 4 bytes 인 경우 physical address space 는 4 MB (1 M x 4 B)

    - page table 에만 전체 메모리 중 4 MB 를 사용해야한다.

<img width="629" alt="스크린샷 2023-05-23 오전 3 28 30" src="https://github.com/jjaehwi/2023_OS/assets/87372606/6b121e5c-d613-4561-a96c-bd19ec992f55">

- 이런 문제를 해결하는 방법 : page table 을 여러 개의 작은 부분으로 나누어서 사용하는 방법

  - **page number 를 p1 과 p2 라는 index 로 나눈다.**

  - p1 : index into the `outer page table`

    - p1 을 이용해서 outer page table 에서 entry 를 선택한다.

    - 이 index 를 통해 여러 개의 page table 의 page 들 중 하나를 선택할 수 있게 된다.

  - p2 : displacement (차이) within the page of the `inner page table`

    - p2 를 이용해서 page table 내에 있는 entry 를 선택할 수 있다. 그 entry 를 통해 실제 frame 에 접근한다.

    - **frame 내에서는 displacement 값을 통해 physical address 로 접근**한다.

<img width="329" alt="스크린샷 2023-05-23 오전 3 31 25" src="https://github.com/jjaehwi/2023_OS/assets/87372606/b576d024-89a8-46bc-941b-33b60c2735ee">

- 32 bit 인 경우 다시 한번 정리....

  - logical address 에서 p1 을 이용해서 outer page table 에서 page of page table 의 원하는 page 에 접근

  - p2 를 이용해서 필요한 frame 을 선택

  - displacement 를 통해 실제 필요한 메모리 주소에 접근

<img width="291" alt="스크린샷 2023-05-23 오전 3 33 37" src="https://github.com/jjaehwi/2023_OS/assets/87372606/0ef1139a-9e41-4289-a263-5ac5ab24641f">

- 64 bit 인 경우 hierarchical page table 방식은 적합하지 않다.

  - 2nd outer page 의 크기가 2^34 까지 올라가기 때문..

---

### Hashed Page Tables

<img width="580" alt="스크린샷 2023-05-23 오전 3 36 25" src="https://github.com/jjaehwi/2023_OS/assets/87372606/29284f87-778a-4be4-8972-0bf2ee324229">

- `Hashed Page Tables` 구성

  1. `virtual page number`

  2. `mapped page frame`

  3. `a pointer to the next element`

- logical address 가 p d 형태로 생성되면 page number 를 hash function 을 통해 hash table 을 search 한다.

  - **field 1 (virtual page number) 에 해당하는 page number 를 linked list 의 첫 번째 element 와 비교**한다. (p 하고 q 하고 비교 = match 하지 않음)

    - **match 한다면 그 table 의 field 2 (mapped page frame) 가 frame 이 된다.**

    - **match 하지 않으면** 그 다음 entry 를 찾아가야하는데, **field 3 (a pointer to the next element) 의 포인터를 이용해서 다음 entry 를 찾아가서 동작을 반복**한다.

- linked list 가 길지 않고 한 두개 내에서 만족할 때 효과적이다.

---

### Clustered Page Tables

- Hashed page tables 의 변형

  - linked list 의 field 2 가 frame 을 바로 가리키는 것이 아닌 몇 개의 pages 를 가리키도록 한다. (such as 16)

- `field 1 끼리 match 하면 field 2 에서 page table 로 가는 것.`

  - page table 에 frame number 가 있어서 여기서 변환이 이뤄지는 것

- page table 의 개수가 얼마 안될 때 유용하다.

  - sparse address spaces 인 경우 유용한 것

  - noncontiguous 한 메모리 접근

  - address space 가 여기 저기 scattered 되어 있을 때 유용하다. (page 들이 연속해서 있는 것이 아닌 것)

---

### Inverted Page Tables

- 일반적인 page table 의 단점은 address space 가 커질 수록 entry 가 엄청 많다는 것과 page table 이 프로세스 마다 하나씩 생성되어야 한다는 점.

- `Inverted Page Tables` : **메모리에서 실제 real page (or frame) 을 entry 로 갖는 table** 을 만들자라는 개념

  - 각각의 entry 는 virtual address + process 와 관련된 정보 (**Each entry consists of the virtual address + information about the process**)

    - `기존의 table 의 index 는 page 인데, inverted page table 에서는 index 가 frame 인 것.`

    - **프로세스마다 table 이 존재하는 것이 아니라 이제 system 당 하나의 table 이 존재** 하게 되는 것

  - **only one page table and only one entry for each frame** (하나의 page table 만 존재하고, 각 frame 에 대해 하나의 entry 만 존재하게 된다.)

  - `ASID (address - space identifier)` 를 요구하는 경우가 있다.

    - (ex) 64 - bit UltraSPARC and PowerPC

<img width="524" alt="스크린샷 2023-05-23 오전 3 50 50" src="https://github.com/jjaehwi/2023_OS/assets/87372606/f6ba34ec-32be-4256-872d-8877a7f5e05c">

- CPU 에서 logical address 를 생성하면, entry 에 virtual address 와 process 에 관련된 정보가 있다. (pid 와 page number(p) 가 포함된 address 가 생성되는 것)

  - 이 address 를 이용해서 inverted page table 을 search 한다.

  - pid 와 p 가 matching 되는 entry 를 찾으면 그 index 가 바로 frame 이므로 physical address 를 생성할 수 있다.

  - 이 때, pid 가 ASID 역할을 하게 된다.

- `Search time problem` 발생 : `기존 page table` 은 **p 가 바로 index 가 되어서 search 가 필요없이 바로 entry 를 찾을 수 있었는데** **`inverted page table` 에서는 위에서부터 search 를 해서 찾아야한다.**

  - search 시간이 증가한다는 단점 (Increased time to search the table)

  - 시스템 당 하나의 table 만 가지기 때문에 기존 page table 을 사용할 때에 비해 메모리 공간을 절약하는 장점이 있지만 search time 이 증가해버리는 것.

  - search 시간을 줄이는 방법을 생각해야한다.

    - `hash table` 사용 : 하나 많아야 두 세개 이내의 linked list 에서 search 를 끝내는 방식으로 동작할 수 있을 때 유용하다.

      - 최대 두 번의 메모리 read 를 통해 메모리에 접근할 수 있다. (hash table 한 번, inverted page table 한 번)

    - TLB 같은 방법을 통해 성능을 증가시킬 수도 있다.

- `Shared memory problem` 발생 : 각 프로세스마다 갖고 있는 page table 의 frame number 를 entry 로 가지면 shared memory 를 사용할 수 있지만 `inverted page table 의 경우`에는 **시스템 마다 하나의 table 만 가지기 때문에** `shared memory 에 해당하는 frame 에 mapping 되는 page 가 하나밖에 들어가지 못한다.`

  - 여러 개의 page 가 **shared memory 에 해당하는 frame 에 mapping 되는 page 를 sharing 하는 경우** 그 page number 를 한꺼번에 저장할 수 있는 방법이 없다.

  - context - switching 을 할 때마다 page fault 가 발생했을 때 해당하는 page number 를 교체하는 작업이 필요해서 overhead 가 추가로 발생한다. (Replacement for each context - switching after page fault)

---

## 9-5. Swapping

<img width="608" alt="스크린샷 2023-05-23 오전 4 06 34" src="https://github.com/jjaehwi/2023_OS/assets/87372606/5dda37e8-951a-418d-9f57-6bbfa6a578be">

- `Swapping` : **memory size 가 충돌하지 않을 때**, **memory 에 있는 프로세스를 용량이 더 큰 backing store 라 부르는 secondary storage 에 임시로 저장을 했다가 다시 필요할 때 해당하는 프로세스를 memory 로 불러오는 방법**

  - 실행하고자하는 모든 프로세스가 차지하는 physical address space 가 real physical memory 를 초과하는 경우 swapping 을 사용해야한다. (**total physical address space of all processes to exceed the real physical memory**)

  - backing store 를 사용하지 않고 waiting 상태에 대기시키고 있다가 기존 프로세스가 빠져나가면 실행시키는 방법을 사용할 수 있지만 **멀티 프로그래밍을 하면서 여러 개의 프로세스가 실행될 필요가 있다고 하면 성능의 저하를 감소하더라도 swapping 을 사용**한다. (`목적 : increasing the degree of multiprogramming`)

- `Standard swapping 동작` : **프로세스를 통째로 backing store 로 보냈다가 (swap out) 다시 필요할 때 불러오는 (swap in) 동작을 반복하면서 swapping 을 하는 것**

  - 메모리에 있는 프로세스를 backing store 로 옮기는 작업은 많은 시간이 소요되고 불러오는 작업 역시 많은 시간이 소요된다.

  - **swapping 을 하면 시스템 성능 저하는 감수해야한다.**

---

### Paging 시스템에서 Swapping

<img width="602" alt="스크린샷 2023-05-23 오전 4 12 39" src="https://github.com/jjaehwi/2023_OS/assets/87372606/7c9709af-b382-4ae2-9a38-462b563d1e28">

- `standard swapping 에 비해 penalty 가 적다.`

  - paging 시스템은 모든 메모리를 page 단위로 나눴기 때문에 어떤 프로세스를 loading 할 때, 모든 프로세스를 loading 하지 않고 `필요한 page 만 loading 할 수 있기 때문`이다.

  - **실제 프로세스가 차지하는 메모리 공간보다 더 적은 메모리 공간으로도 실행이 가능**하다.

  - swapping 을 진행할 때도 전체 프로세스를 swapping 하는 것이 아닌 사용하지 않는 일부분만 backing store 에 저장하는 page out 과 해당 page 부분만 불러오는 page in 동작을 통해 소요되는 시간을 줄일 수 있다.

    - `performance 의 성능 감소 폭이 standard swapping 에 비해 적다.`

- 그래도 성능 감소가 있어서 swapping 은 잘 사용하지 않고, increasing the degree of multiprogramming 이 아니라면 사용하지 않는다. 하지만 이마저도 요즘엔 충분한 크기의 메모리가 확보되기 때문에 swapping 없이 메인 메모리에서 가능해졌다.

  - 하지만 역시 성능저하를 느끼지 않을 만큼 page 단위로 백그라운드에서 일어나고 요즘 컴퓨터 시스템과 운영체제에서는 성능 저하를 체감하기 어렵다.

---

## Summary

- base address 와 limit address 를 이용해서 프로세스를 위한 메모리 allocation method

- Binding symbolic address to physical addresses 동작은 다음과 같은 경우에 일어난다.

  1. compile time

  2. load time

  3. execution time

- CPU 가 생성하는 logical address 가 있고, 실제 물리적인 메모리에 존재하는 physical address 가 있다.

  - CPU 가 메모리에 접근할 때, logical address 를 변환시켜주는 address translation 과정이 필요한데 이것은 컴퓨터 시스템에 있는 하드웨어에서 이뤄진다.

- 프로세스를 메모리에 allocation 할 때, contiguous memory allocation 의 경우 프로세스의 다양한 사이즈에 맞게 partition 을 생성한다.

  - 프로세스를 위한 새로운 partition 을 생성하는 방법

    1. first fit

    2. best fit

    3. worst fit

- 현대의 대부분 OS 는 paging 방식을 사용한다.

  - page 와 frame 을 이어주는 page table 을 이용하여 address translation

  - page table size 가 커지는데,, 성능 향상을 위해 TLB 라는 일종의 cache 를 사용한다.

  - page table 을 구현하는 방식

    - Hierarchical paging

    - Hashed page tables

    - Inverted page tables

- Increasing the degree of multiprogramming 을 위한 Swapping
