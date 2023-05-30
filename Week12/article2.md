# 12강 Virtual Memory : Copy-On-Write and Page Replacement

- [10-3. Copy - on - Write](#10-3-copy---on---write)

- [10-4. Page Replacement](#10-4-page-replacement)

  - [Basic page replacement](#basic-page-replacement)

  - [Page Replacement Algorithm : FIFO Page Replacement](#page-replacement-algorithm--fifo-page-replacement)

  - [Page Replacement Algorithm : Optimal Page Replacement](#page-replacement-algorithm--optimal-page-replacement)

  - [Page Replacement Algorithm : LRU Page Replacement](#page-replacement-algorithm--lru-page-replacement)

---

## 10-3. Copy - on - Write

- **_fork() system call 을 통해 parent 프로세스를 복사해서 child 프로세스를 생성할 수 있다._**

  - parent 프로세스의 내용을 전부 복사해서 child 프로세스를 만드는 것은 시간이 오래걸릴 것이다.

    - **child 에서 값을 read 만 하는 경우 굳이 copy 를 하지않고 parent 프로세스의 값을 사용**할 수 있다.

    - page sharing 을 통해 이미 있는 parent 프로세스의 page 를 재활용하는 것.

  - `Bypass demand paging`

  - demand paging 시스템을 사용하는 경우 parent 프로세스를 일부 혹은 전혀 copy 하지 않은 상태에서 child 프로세스를 실행하면 page fault 가 발생한다.

    - 그에 따라 이후 동작에서 parent 를 copy 해오는데, **값을 읽는 경우 굳이 copy 하지 않고 parent 프로세스에 있는 page 나 frame 에서 읽어오기만 하면 된다.**

  - 이런 방법을 통해 **child 프로세스 생성의 속도가 빨라지고 child 에게 할당하는 page 도 줄어든다.**

  - `fork() - exec()` : **fork 하고 exec 하는 경우 copy 했던 모든 데이터는 날아가기 때문에 parent 의 address space 에 있는 내용을 copy 하는 작업은 불필요한 작업**이다.

    - child 의 page 와 parent 의 page 는 **서로 다른 virtual address space 에 존재하지만 물리적으로는 같은 address frame 에 있는 데이터를 공유해도 상관없다.**

- parent 하고 child 가 초기에는 동일한 frame 을 공유하는데 frame 에 해당하는 page 의 값이 바뀌기 전까지는 공유하는데 문제가 없다. (**parent and child processes initially to share the same pages.**)

  - **shared 된 page 를 `copy - on - write` 라고 marking** 해놓은 상태에서 parent 나 child 가 동작하다가 해당 page 에 write 를 하는 경우, parent 나 child 는 서로 다른 값을 작성할 수 있다. (**어느 한 쪽이라도 write 하여 값이 modify 되는 것**)

  - 그러면 parent 와 child 가 사용하는 page 는 서로 다른 값을 갖는 page 가 될 것이고 더이상 sharing 을 할 수 없는 것이다. 이 경우 **_copy 를 생성해서 서로 다른 frame 을 사용하도록 해야한다._**

<img width="542" alt="스크린샷 2023-05-30 오후 3 08 05" src="https://github.com/jjaehwi/2023_OS/assets/87372606/67007d96-6135-46e0-b7eb-14eb9591ccc6">

- 프로세스 1 과 프로세스 2 가 (parent - child 관계 또는 서로 다른 프로세스) 실행되면서 `각자 page 가 physical memory 에서 동일한 frame 의 page 를 공유`하고 있는 상황

<img width="542" alt="스크린샷 2023-05-30 오후 3 22 48" src="https://github.com/jjaehwi/2023_OS/assets/87372606/50d7403e-b9e1-48bd-83ec-b59ba0fafb4c">

- 프로세스 2 가 **copy - on - write 라고 마킹되어있는 page C 에 write 하는 동작**을 하면, 값이 바뀔 것이고 **프로세스 1 에서는 page C 의 copy 를 만들어서 접근하고, 프로세스 2 는 write 하여 값을 modify** 한다.

- 또는 **프로세스 1 이 write 하는 경우 page C 를 copy 하고 나서 copy 에 값을 쓰는 경우도 있을 수 있다.**

- `Only the pages that are modified by either process are copied` (**_프로세스에 의해 변형이 일어나는 페이지만 copy_** 한다.)

  - physical memory 의 사용을 최대한 억제

  - page copy 에 따른 성능 저하를 예방

---

## 10-4. Page Replacement

- demand paging 을 사용하여 **degree of multiprogramming 이 증가하게 되면**, **_실제 physical memory 가 감당할 수 있는 사이즈보다 더 많은 메모리 용량을 할당하는 상황이 발생할 수 있다._** (일부만 할당됐던 프로세스들이 더 많은 공간을 요구해서,,)

  - demand paging -> increase degree of multiprogramming -> over - allocating memory

- **system memory : program pages (프로그램 실행에 필요한 page) + I/O 버퍼 (I/O 동작을 할 때 필요)**

  - physical memory 를 프로세스에만 할당할 수 없고 시스템이 I/O 버퍼로 일부 사용한다.

  - free frame 이 존재하지 않는 상황이 발생할 수 있다.

  - 이 때, 해당 프로세스의 로딩을 막고 waiting / termination 하는 방법도 있지만 기**존에 실행되고 있는 프로그램 프로세스들은 성능이 저하되더라도 계속해서 실행될 수 있게 하는 것이 낫기 때문에 `page replacement` 를 통해 `page 를 swapping` 한다.**

<img width="608" alt="스크린샷 2023-05-30 오후 3 48 08" src="https://github.com/jjaehwi/2023_OS/assets/87372606/66d89294-898e-41e9-a23d-0ef4c15b2eca">

- 프로세스 1 의 logical memory 중 page 1 의 컨텐츠 B 를 실행하기 위해 프로세스 1 의 page table 가보니 해당 page 가 invalid 상태이다.

  - **logical address space 에서는 valid 하지만 현재 해당 page 가 메모리에 올라와있지 않고 backing store 에 있는 것**

- backing store 에 있는 해당 page 를 physical memory 에 로딩하려고 하는데 physical memory 의 0, 1 은 커널이 사용하고 있고 나머지는 다른 프로세스의 page 로 꽉 차있어서 **free frame 이 존재하지 않는 상황**이다.

  - 이 중 **사용하지 않는 page 를 하나 backing store 로 보내고 빈 자리에 B 를 채워넣는 동작을 해야한다.**

- 프로세스 2 의 page table 에도 G 는 invalid 되어있고 backing store 에서 physical memory 에 로딩해야한다.

- 이런 경우 **page replacement 가 진행돼서 free frame 을 확보해서 그 자리에 필요한 page 를 로딩**할 수 있다.

---

### Basic page replacement

<img width="489" alt="스크린샷 2023-05-30 오후 4 06 56" src="https://github.com/jjaehwi/2023_OS/assets/87372606/0db8b070-fbf3-4c9b-8f27-f141a12242be">

- `Replacement` : 두 page 에 대한 transfer 가 필요하다.

  - 밖으로 빼낼 page = `victim page` 에 대해 `page out` 동작을 해야한다.

    - victim page 를 backing store 로 보내서 저장하고 해당 frame 은 free frame 이 된다.

    - 해당 frame 을 invalid 로 설정해둔다.

  - backing store 에 있는 필요로하는 page 를 physical memory 로 로딩하여 free frame 에 채워넣는다. (`page in`)

  - `page table 을 reset`

- **page table 에 `modify bit (dirty bit)` 라는 flag 를 추가**할 수 있다.

  - page in 이 된 이후 값이 바뀌면 1 로 set 하고

  - 값이 바뀌지 않고 read 만 했다면 0 을 유지한다.

  - `Not set` : no page out (backing store 에 있는 값과 차이가 없는 것 = 바뀐게 없기 때문)

    - replacement 의 page transfer 를 한 번만 하면 되기 때문에 성능 향상

<img width="351" alt="스크린샷 2023-05-30 오후 4 06 13" src="https://github.com/jjaehwi/2023_OS/assets/87372606/9f93dc7b-6787-4919-8c3e-c4863c593ff6">

- `page replacement 를 진행할 경우 생각해야할 문제 2 가지`

  1. **_frame - allocation 알고리즘_**

     - **최초에 프로세스에게 frame 을 몇 개 할당**해야할까..

     - demand paging 과 같은 방식을 사용하면 기본적으로 프로세스가 생성될 때 몇 개의 frame 은 할당해야한다. 이 때, 몇 개를 할당하는것이 optimal 할까.. 라는 것

     - **할당하는 frame 이 많아지면** replacement 는 많이 발생하지 않지만 degree of multiprogramming 이 감소

     - **할당하는 frame 이 적게하면** 그만큼 replacement 가 많이 발생

     - 어느 점으로 수렴해가기 때문에 무작정 많이 할당한다고 좋은 것이 아님

  2. **_page - replacement 알고리즘_**

     - page out 할 frame 을 어떤 방식으로 선택할 것인가?

---

### Page Replacement Algorithm : FIFO Page Replacement

- `FIFO Page Replacement` : **queue 에 page 를 넣고 가장 먼저 사용된 page 부터 replace 하는 방식** (Replace the page at the head of the queue)

<img width="737" alt="스크린샷 2023-05-30 오후 4 24 35" src="https://github.com/jjaehwi/2023_OS/assets/87372606/2a473de1-35ed-46ba-8888-c769a6f4c658">

- page frame 3 칸이 할당된 예시

  - 비어있을 때는 7 에 대해 page fault 가 발생한 후 해당 명령어나 값을 읽어올 수 있다.

  - 0 을 참조하려고 할 때도 page fault 가 발생한 후 해당 명령어나 값을 읽어올 수 있다.

  - 1 을 참조하려고 할 때도 page fault 가 발생한 후 해당 명령어나 값을 읽어올 수 있다.

  - 2 를 사용하려고 할 때, 꽉 차 있기 때문에 page replacement 를 해야하는데 가장 먼저 들어갔던 7 을 내보내고 그 자리를 2 로 채운다.

  - 0 에 접근할 때 page fault 가 발생하지 않는다.

  - 3 을 참조하려고 할 때, 꽉 차 있기 때문에 가장 오래된 0 을 3 으로 replacement 한다.

  - 이런 방식으로 계속 진행된다..

<img width="391" alt="스크린샷 2023-05-30 오후 4 30 30" src="https://github.com/jjaehwi/2023_OS/assets/87372606/2178688a-b6e3-4d9a-92b9-a94ecd9c9072">

- 할당되는 page frame 수에 따라 page fault 가 얼마나 발생하는지 나타낸 그래프

  - 3 -> 4 때 오히려 증가하는 현상을 `Belady's anomaly` 라고 한다. (frame number 가 증가했는데 page fault 가 오히려 증가한 것)

---

### Page Replacement Algorithm : Optimal Page Replacement

- Belady's anomaly 가 발생하지 않도록 하기 위함

- `Optimal Page Replacement` : **현재 frame 에 있는 page 중 앞으로 가장 오랜 시간동안 사용되지 않을 page 를 replace**

  - **정해진 frame 숫자에 대해 가장 낮은 page fault** 가 발생한다. (The lowest possible page fault rate for a fixed number of frames.)

<img width="691" alt="스크린샷 2023-05-30 오후 4 36 20" src="https://github.com/jjaehwi/2023_OS/assets/87372606/b9132c83-e85d-4676-85f9-125591294d41">

- 구현이 어렵다 : 미래에 어떤 것을 참조할지 알아야한다.

  - 앞으로 사용할 page 의 리스트가 주어지지 않은 경우 현실적으로 불가능

---

### Page Replacement Algorithm : LRU Page Replacement

- `Least recently used (LRU) Page Replacement` : **optimal 알고리즘을 근사해서 만든 방법**

  - 과거를 미래에 대한 근사치로 생각하는 것 (use the recent past as an approximation of the near future)

  - **_과거 가장 오랫동안 사용하지 않았던 page 를 선택하는 방식_** (The page that has not been used for the longest time.)

    - 현재를 중심으로 가장 먼 미래에 사용할 page 를 과거 가장 오랫동안 사용하지 않았던 page 와 같다고 생각하는 것.

<img width="505" alt="스크린샷 2023-05-30 오후 4 42 45" src="https://github.com/jjaehwi/2023_OS/assets/87372606/41b8a6c4-78c8-492b-a0f9-0d72391b1653">

- optimal 보다는 크지만 FIFO 보다는 작은 page fault

  - 현실적

- 과거이지만 해당 page 가 언제 access 됐는지 알아야한다. (**hardware assistance** 가 있어야 구현 가능)

  1. `Counters` : 각 page 가 참조될 때마다 참조된 시간을 저장해두는 것

     - 가장 최근에 aceess 한 page 의 시간이 가장 큰 값을 가질 것이기 때문에 가장 작은 시간 값을 가지는 page 를 replace (시간이 0 으로 wrapping 되는 새로운 문제를 해결해야함)

  2. `Stack` : stack 을 만들어두고 stack 의 가장 맨 위에 가장 최근 access 한 page 를 넣는 방식

     - stack 의 가장 아래에 있는 page 가 가장 오랫동안 access 되지 않은 page 이기 때문에 이 page 를 replace

<img width="291" alt="스크린샷 2023-05-30 오후 4 47 17" src="https://github.com/jjaehwi/2023_OS/assets/87372606/715f122f-74c5-4a9e-a253-b0ea65c203b4">

---

## Summary

- Virtual memory 가 무엇인지..

  - 정의와 장점

- Demand paging 방식이란 무엇인지

  - page fault 가 발생

- Copy - on - write

  - page modify 가 발생할 때 copy 를 함으로써 시스템 효율을 증가시키고 메모리 사용률을 개선하는 방식

- page - replacement algorithm

  - free frame 이 없는 경우 replacement 를 해야한다.

  - FIFO, optimal, LRU 알고리즘

  - Global page - replacement algorithm vs local page - replacement algorithm

- Thrashing

  - 프로세스가 실행할 때 page fault 가 많이 발생하면 결국 명령어를 실행하는 시간보다 paging 시간이 오래 소모가 되면서 프로세스가 거의 실행되지 못하고 page in, page out 의 swapping 동작만 발생하는 상황

- Memory compression

  - free memory 를 확보할 때, backing store 에 저장하는 대신 기존 몇 개의 frame 을 묶어서 압축한 후 다른 frame 에 저장함으로써 추가로 free frame 을 확보하는 방식

- Other Considerations

  - virtual memory 에서 추가로 고려해야될 것

  - prepaging

  - page size

  - TLB reach

  - program structure

- OS 예시 : Linux, Windows
