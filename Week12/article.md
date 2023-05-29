# 12강 Virtual Memory : Background and Demanding page

`chapter 10 에서 배우는 내용`

- Virtual memory 란 무엇이고, 사용할 때 어떤 이점이 있는가?

- 9 장에서 살펴본 paging 시스템에서 page 가 필요할 때, physical memory 에 page 를 load 하는 방법

- page 를 loading 할 때, physical memory 에 빈 공간이 없는 경우 기존에 있던 page 를 내보내고 빈 자리에 새로운 page 를 로딩하는 `page - replcaement` 알고리즘

- 프로세스의 working set 과 프로그램의 locality

  - paging 시스템이 유용하게 동작할 수 있다라는 걸 알아봄

- Linux, Windows 10 에서 virtual memory 가 어떻게 쓰이는지

---

- [10-1. Background](#10-1-background)

  - [Virtual memory](#virtual-memory)

- [10-2. Demand Paging](#10-2-demand-paging)

  - [Handling page fault](#handling-page-fault)

  - [Demand page 특징](#demand-page-특징)

  - [Performance of demand paging](#performance-of-demand-paging)

---

## 10-1. Background

- **모든 프로그램이 실행되기 위해서는 `instruction 이 physical memory 에 올라와있어야하고`, 메모리에서 명령어를 실행**한다.

  - **메모리에 있는 instruction (코드) 을 프로세서가 fetch 를 해서 프로세스 내에서 실행**하는 것.

  - physical memory 사이즈에 따라 로딩할 수 있는 프로그램의 크기가 한계가 생긴다.

  - 큰 사이즈의 메모리를 사용하는 경우 or 메모리에 여러 개의 멀티프로그래밍을 하는 경우, 프로세스 마다 physical memory 의 일부를 할당하는데 프로세스에 할당하는 **메모리의 크기가 프로그램 크기보다 작은 경우** 어떻게 해야할까?

    - 모든 프로그램의 코드를 전부 로딩하지 않더라도 프로그램 실행에 문제가 없는 경우가 있다.

      - code to handle unusual `error conditions` : **rare error occurrence -> almost never executed**

      - **arrays, lists, tables** : worst case 에 맞게 충분한 공간을 잡아두는데, 대부분 평소에는 일부만 사용해도 문제가 없음

      - **특정한 options 이나 특정 features** 이 잘 사용하지 않고 드물게 사용되는 경우

      - **자주 사용되는 함수여도 동시에 사용되진 않는다.** (Frequently used functions are not used at the same time)

  - 위의 상황들을 볼 때, 모든 코드들이 메모리에 올라와 있어야 프로그램이 정상 작동하는 것은 아니라는 것을 확인할 수 있다.

  - **_프로그램 일부분만 로딩해서 실행시키더라도 실행하는데 큰 문제가 없다._**

---

- `Partial loading of a program to memory`

  - **Programming an `extremely large virtual address space`** : 프로그래밍 하는 작업을 간단하게 만들 수 있다.

    - (ex) address 가 0 ~ 10000 번지 일 때, 이 범위를 벗어나지 않도록 address boundary 를 신경쓰면서 코드를 짤텐데, 0 ~ 무제한 이라면 메모리 범위 문제를 신경쓸 필요 없이 구현만 잘 하면 될 것.

  - **프로그램이 physical memory 를 차지하는 크기를 줄이는 것** : 멀티프로그래밍의 degree 를 증가시킨다.

    - 하나의 시스템에 있는 physical memory 에 여러 개의 프로세스를 로딩해서 프로세스들이 concurrent 하게 실행될 수 있도록 하는 것.

    - (ex) P1, P2 를 실행할 수 있는 physical memory 에서 P1 과 P2 를 절반씩만 로딩하면 (실행에 지장없도록 일부분만 로딩하는 것), 빈 공간에 다른 프로세스들을 로딩하여 멀티프로그래밍의 degree 가 증가할 수 있다. (2 -> 2 + 추가 로딩된 프로세스 수)

  - **Less I/O for loading or swapping** : I/O 작업이나 swapping 작업이 줄어들면 프로그램이 더 빠르게 동작할 수 있다.

---

### Virtual memory

<img width="460" alt="스크린샷 2023-05-30 오전 2 37 01" src="https://github.com/jjaehwi/2023_OS/assets/87372606/bfb61118-d6dd-461a-b052-512692b340c6">

- **_logical address 와 physical address 가 서로 다를 때, logical address 를 virtual address 라고 부른다._** 이 때, **virtual address 에 해당하는 메모리 공간이 `virtual memory`** 이다.

  - **logical memory 와 physical memory 를 분리시켜서 사용할 수 있다는 장점**

  - `virtual address space` : **연속된 메모리에서 프로세스의 logical view**

    - 메모리에 하나의 프로세스가 contiguous 하게 로딩되어 있을 때, virtual address space 의 관점에서 보면 프로세스에서 차지하고 있는 주소를 0 ~ 필요한 만큼 주소를 할당하는 것

    - logical memory 입장에서 physical memory 의 주소들을 생각할 필요없이 해당 프로세스 하나만 있다고 생각하고 프로그래밍을 할 수 있는 것

  - **실제 physical page frame 에 있는 프로세스는 contiguous 하지 않을 수 있다.**

    - logical address space (virtual address space) 에서는 연속적으로 프로세스가 분포되어있다고 생각하지만, 실제 physical address space 에는 page 단위로 매칭이 되면서 연속적이지 않게 분포되어 있을 수 있는 것

<img width="340" alt="스크린샷 2023-05-30 오전 2 39 58" src="https://github.com/jjaehwi/2023_OS/assets/87372606/1da43f10-dde3-4edc-9da1-205fbac8e931">

---

### Sparse address space

- `Sparse address space` : **virtual address space 중 holes 를 포함하고 있는 것.**

  - 연속적으로 분포하고 있는데, 이 중 hole 이 포함되어 있는 것.

  - `유용한 이유`

    1. hole 이 존재할 때, stack 이나 heap 이 증가할 때, 추가적인 page 의 할당 없이 해당 영역을 사용할 수 있다. (The holes can be filled as the stack or heap segments grow)

    2. 프로그램이 실행되는 도중 dynamically link libraries or possibly other shared objects 를 사용하는 경우 hole 을 이용해서 배치시킬 수 있다.

<img width="496" alt="스크린샷 2023-05-30 오전 2 46 00" src="https://github.com/jjaehwi/2023_OS/assets/87372606/e2f122f2-98ee-439d-873f-114b33d46570">

- `Page sharing` : 여러 개의 프로세스가 **파일이나 메모리를 서로 공유**할 수 있다.

  - system libraries (dynamically link libraries)

    - 각각의 프로세스 안에 포함시키지 않고 밖에 두고 필요할 때만 불러서 사용

  - 프로세스들 사이에서 IPC 를 위해 메모리 공간을 공유하는 경우

  - **서로 공유하는 page 의 경우 virtual address space 의 일부**이다. 하지만 **shared pyhsical memory 라는 점에서 차이**가 있다. (Part of virtual address space, but in the shared physical memory)

---

## 10-2. Demand Paging

- 하나의 프로세스가 메모리로 로딩될 때, 모든 페이지가 메모리에 로딩될 필요가 없다는 것을 알았다.

  - **프로세스의 각각의 페이지는 우리가 필요할 때, 메모리에 로딩**되어도 된다. (`demand paging 시스템`)

- `Basic Concepts`

  - 한 프로세스의 page 를 **일부는 메모리에 두고** **일부는 secondary storage 에 둔** 상태에서 시작하는 방법

    - 이 동작을 하기 위해 어느정도 hardware support 가 필요하다. (메모리에 있는 page 와 secondary storage 에 있는 page 를 구분할 수 있는 기능)

      - page table 에 `valid - invalid bit` 를 둬서

        - `valid` 는 logical address space 에 존재하는 page 이고 실제로 메모리에도 올라와 있다라는 의미

        - `invalid` 는 해당 page 가 logical address space 에 포함되어 있지 않은 page 이거나, 현재 메모리에 없고 secondary storage 에 있는 경우.

        - invalid 하더라도 해당 page 를 access 하기 전에는 문제가 되지 않는다. 해당 page 를 access 하는 경우 invalid 하다면 page fault 가 발생.

<img width="457" alt="스크린샷 2023-05-30 오전 2 55 52" src="https://github.com/jjaehwi/2023_OS/assets/87372606/26f50fa6-6d20-43bb-9a5a-58c7f065bc67">

- logical address space 에서 page 는 0 ~ 7 까지 page 가 있고 이에 대응되는 컨텐츠는 A ~ H 까지 존재한다.

- page table 에서는 0 page -> 4 frame, 2 page -> 6 frame, 5 page -> 9 frame 이다.

- physical memory 에 가면 A, C, F 가 올라와있고 그에 해당하는 page 가 frame number 와 함께 valie 로 표시되어 있다.

- logical address space 에서 보이는 A ~ H 의 모든 page block 은 **backing store (secondary storage)** 에 존재하고 있다.

- **physical memory 에 올라와있는 해당 page 는 프로세스를 실행하면서 값이 modify 될 수 있고, 그러면 physical memory 에 있는 것과 secondary storage 에 있는 것이 서로 달라질 수 있다.**

---

### Handling page fault

<img width="541" alt="스크린샷 2023-05-30 오전 3 12 48" src="https://github.com/jjaehwi/2023_OS/assets/87372606/aecd0af4-d368-4ab9-9494-5697e4adadbe">

- memory 에 올라와있지 않은 page 를 요구하는 경우 발생하는 page fault

1. logical address space 에서 특정한 address 를 `reference` 한다.

2. page table 에 갔는데 invalid 하다면 `trap` 이 발생하고 OS 를 통해 handler 가 실행된다.

   - page fault 를 처리하는 handler 가 실행되면 메모리에 없는건지, 아예 잘못된 page 를 access 하려는 건지 체크한다.

     - `Invalid` 한 경우 : termination

       - (ex) **0 ~ 7 까지인데 8 에 접근하는 등..logical address space 에 포함되어 있지 않은 경우**

     - `valid` 한 경우 : **_backing store (secondary storage) 에 있는 해당 page 를 physical memory 에 올리는 page in_** 과정을 한다.

       - (ex) **page 자체는 valid 한데 physical memory 에 올라와있지 않은 경우**

3. page is on backing store : backing store 에서 `page in`

4. 원하는 page 를 `physical memory 의 free frame 을 assign 받아 로딩`한다.

5. 새로운 frame 의 page 가 올라왔기 때문에 `page table 을 업데이트`해야한다.

   - 해당 frame 을 넣고 valid 로 바꾼다.

6. 문제가 발생했던 `instruction 을 재시작`한다.

---

### Demand page 특징

1. `Pure demand paging`

   - 일반적인 demand paging 에서 하나의 프로세스가 실행이 되면 처음 실행되는 page 연관된 page 들을 몇 개 로딩해놓고 시작을 할 수 있다. 이러면 첫 번째 명령을 실행할 때 page fault 가 발생하는 것을 방지할 수 있다.

   - **_pure demand paging 은 프로세스를 시작할 때 아무 page 도 로딩하지않고, 프로세스 메모리 영역만 할당한 상태에서 바로 시작한다._** (Process start with no pages in memory)

   - **첫 번재 명령을 실행하자마자 instruction 이나 data 에 대해 page fault 가 발생**한다. (page table 의 모든 영역이 invalid 이기 때문)

   - **locality of reference**

     - instruction 이나 data 에 대해 처음 page fault 가 발생한 이후 locality 때문에 당분간 page fault 가 발생하지 않는다.

   - 프로세스가 시작하고 나서 page fault 때문에 대기하는 시간이 길어질 수 있지만 전체적으로 프로세스를 시작하고 page 를 미리 로딩하는 경우에 비해 `불필요한 page 를 처음부터 로딩할 가능성이 적어져서` performance 에서 큰 문제가 발생하진 않는다.

2. `Hardware to support demand paging`

   - demand paging 을 위해서는 hardware support 가 필요하다.

   - **page table 과 secondary storage 가 있어야한다.**

     - page table 이 pure demand paging 을 할 수 있는 방식으로 구현되어 있어야한다.

   - page fault 발생 이후 해당 명령어를 다시 실행할 수 있는 기능이 있어야한다.

<img width="423" alt="스크린샷 2023-05-30 오전 3 29 22" src="https://github.com/jjaehwi/2023_OS/assets/87372606/2f2c830e-42be-4727-b8a4-b55892a0861f">

3. `Free - Frame List`

   - page fault 가 발생하거나, 새로운 프로세스가 생성돼서 해당 page 를 physical memory 에 로딩하는 경우 어떤 frame 이 비어있는지 알아야하고 해당 frame 을 할당하고 page table 을 업데이트할 수 있다.

   - 시스템 (커널) 은 현재 차지하고 있지 않은 frame 을 linked list 형태로 가지고 있다.

   - **_zero - fill - on - demand_** : 할당하기 전에 해당 frame 에 대해 "zeroed - out" 하는 것

     - **특정 메모리 frame 을 전부 0 으로 채우는 동작** -> 많은 시간 소요

     - 사용하고자하는 frame 이 읽어와서 채우는 방식이 아니라 할당만 하거나 (공간만 배정하는 것) 배열이나 데이터가 0 으로 초기화되는 경우 의미가 있다.

---

### Performance of demand paging

- `Effective access time = (1 - p) x ma + p x page fault time`

  - p : page fault 가 발생할 확률 (0 <= p <= 1)

  - ma : memory access time (200ns 로 가정)

  - page fault time 을 알아야 effective access time 을 계산할 수 있다.

<img width="755" alt="스크린샷 2023-05-30 오전 3 38 06" src="https://github.com/jjaehwi/2023_OS/assets/87372606/c16da4a1-1fa2-46ed-ac54-21b907b3f5a3">

- Page fault time 을 측정하기 위해 page fault 가 발생했을 때 일어나는 동작을 알고, 각각에 대해 걸리는 시간을 알아야한다.

- page fault 를 해결하는 sequence = `handler 에서의 핵심적인 동작 3 가지` (자세한 내용은 교재 참고)

  1. page fault interrupt 를 서비스할 때까지 기다리고 서비스되는 시간 (핸들러 준비동작)

  2. secondary storage 에 있는 page 를 읽어오는 시간 (HDD 가 아니고 SSD 를 쓰는 경우 많이 빨라져서 (~1ms))

  3. 프로세스를 재시작하는 시간

- **_effective access time 은 page fault rate 에 비례한다._**

- `Swap space` 의 경우

  - secondary storage (HDD 나 SSD) 에서 **일반적으로 file system 을 통해 file 형태로 access** 하는 것이 효과적이어서 그런 방식으로 하는데 file system 을 거치는 단계 때문에 overhead 가 발생해서 성능이 떨어진다.

  - `swap space 는 file system 을 적용하지 않은 secondary storage 에 별도의 영역을 사용하는 것`이다. **이 영역은 file system 을 사용하지 않고 직접 access 를 하도록 해서 file system 을 이용하는 것보다 훨씬 빠르다.**

  - **page demanding 하는 경우 page 들은 swap space 에 저장**한다.

  - `처음에는 file system 을 사용하지만 page in 이후 page out 할 때 swap space 에 저장하는 방식` (demand page from the file system initially but write the pages to swap space as they are replaced)

    - Windows, Linux 에서 사용, 아래 방식보다 성능이 더 좋다.

  - `프로세스 시작할 때 swap space 에 전제 파일 이미지를 copy 해놓고 시작하는 방식` (copying an entire file image into the swap space at process startup)

    - 생성 과정을 오래걸릴 수 있지만 page in 이나 page out 동작을 file systme 으로 할 때보다 빠를 것이다.
