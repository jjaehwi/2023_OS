# 11강 Memory management : Contiguous Allocation

- [9-2. Contiguous Memory Allocation](#9-2-contiguous-memory-allocation)

  - [Memory Allocation](#memory-allocation)

  - [Fragmentation](#fragmentation)

---

## 9-2. Contiguous Memory Allocation

- 프로세스를 생성하기 위해서는 메모리에 프로세스 생성을 위한 메모리 공간을 할당해야한다.

- 일반적인 유저 프로세스는 생성과 종료를 반복하지만 OS 는 시스템이 부팅되어 shutdown 될 때까지 메모리에 항상 상주하는 프로세스이다.

  - OS 는 메모리의 가장 low address 나 가장 high address 에 할당한다.

  - Linux 나 Windows 는 **OS 를 high memory address 에 위치**시키고 **프로세스는 low memory address 부터 할당**하여 **OS size 가 가변적이더라도 어느정도 마진이 있어서 관리가 편리**하다.

<img width="595" alt="스크린샷 2023-05-21 오후 11 08 27" src="https://github.com/jjaehwi/2023_OS/assets/87372606/3eda8e63-2a20-4e47-b0a1-a27299cf481f">

- `Memory Protection` : OS 뿐만아니라 프로세스끼리도 서로의 메모리 공간을 침범하지 않도록 보호해야한다.

  - `relocation and limit register 방법`

  - CPU 에서 logical address 를 생성하면 logical address 가 limit register 를 넘어가지는 확인한다. 제한에 걸리지 않으면 통과시키고 그렇지 않으면 trap 을 발생시켜서 addressing error 를 발생시킨다. (위쪽 boundary 를 넘어가는지 체크하는 것)

  - 자기에게 할당된 프로세스 메모리 영역내에 포함되어 있으면 넘어가고 (limit register 부분을 통과하는 것) relocation register 에 들어있는 base address 를 통해서 프로세스에게 할당된 메모리 영역에서 logical address 가 offset 의 역할을 하여 base address 부터 할당된 영역 내의 physical address 를 생성해서 할당된 영역을 사용할 수 있다.

---

### Memory Allocation

<img width="595" alt="스크린샷 2023-05-21 오후 11 26 29" src="https://github.com/jjaehwi/2023_OS/assets/87372606/7d8ff083-a425-4e2b-a652-459fcfead309">

- `OS 는 high memory 에 할당되어있고 low memory 부터 생성된 프로세스가 할당`되는 것을 확인할 수 있다.

- Memory allocation scheme 중 `Contiguous memory allocation 은 하나의 프로세스를 연속된 메모리 주소에 할당하는 방법`이다.

  - **프로세스마다 서로 다른 size 를 갖는 variable partition scheme 을 사용**한다.

  - 각 **partition 은 정확히 하나의 프로세스를 포함**한다.

  - partition table 에는 `holes` 과 `occupied partition` 에 대한 정보가 있다.

    - `hole` : **partition 사이에 비어있는 공간**

      - hole 에 대한 정보가 있어야 다음 새로운 partition 을 생성할 때, 비어있는 공간을 찾아서 생성할 수 있다.

      - (ex) process 8 이 termination 되어 hole 이 된고 그 hole 에 새로운 프로세스 9 가 생성되어 할당된다. 이후 프로세스 5 가 termination 되면 그 만큼의 공간이 또 hole 이 된다.

      - **hole 은 항상 연속된 공간을 가지는 것이 아니고 불연속적인 공간에 여러 개의 hole 이 존재**할 수 있다.

    - 모든 hole 에 대해 새로운 프로세스가 할당되는 것이 불가능한 상황이 되면 프로세스 생성을 reject 하고 error 메시지를 생성하던지, wait queue 에 넣어서 waiting 상태에 있다가 생성 가능한 hole 이 생기면 그 때 생성시키는 방법을 취한다.

      - `hole 의 효율적인 관리가 필요`하다.

- `Dynamic storage allocation problem` : 동적으로 storage 를 allocation 하는 경우 프로세스를 hole 에 allocation 할 때, 어떤 방식을 취할지에 대한 문제가 발생한다.

  1. `First fit` : allocate the first hole that is big enough. faster

     - 프로세스를 수용할 만큼의 크기를 갖는 hole 을 처음 발견하는대로 바로 할당한다.

  2. `Best fit` : allocate the smallest hole that is big enough

     - 모든 hole 을 조사해서 생성 가능한 hole 중 가장 작은 hole 을 선택한다.

  3. `Worst fit` : allocate the largest hole. Worst in terms of storage utilization

     - 모든 hole 을 조사해서 가장 큰 hole 을 선택한다. 가장 큰 hole 에 생성하면 그만큼 또 남게 되고 또 다른 프로세스를 할당할 가능성이 생기는 이점이 있다. 하지만 실제로는 결과가 가장 안좋다.

---

### Fragmentation

<img width="273" alt="스크린샷 2023-05-21 오후 11 45 24" src="https://github.com/jjaehwi/2023_OS/assets/87372606/ee4bd5a7-3881-42c8-b1fc-b2f632b24be2">

- `Fragmentation` : 프로세스를 생성해서 메모리를 할당하면 hole 이 여러 곳에 발생하고 프로세스의 생성과 종료가 많이 반복되면 어느 순간 hole 들이 프로세스를 수용할 수 없을 정도의 작은 사이즈로 흩어져서 남아있게 된다.

  - **_External fragmentation_** : 프로세스에 할당된 공간 바깥에서 발생하는 경우

    - worst case : 모든 두 프로세스 사이에 홀이 존재하는 것

- **50 - percent rule** : N 개의 프로세스 공간 을 allocation 할 경우, 통계적으로 0.5N 에 해당하는 fragmented block 이 발생한다.

  - 전체 공간의 2/3 는 프로세스 공간이고 1/3 은 fragmentation 에 의해 사용할 수 없는 공간이 되는 것

- `hole 이 많아지면 작은 hole 들이 어디에 위치하는지 계속 tracking 을 해야하는데 이 때에도 많은 overhead 가 발생`한다. (The overhead to keep track of small holes)

  - 특히 variable partition 을 갖는 경우 심하다.

  - 해결방안 : **partition size 를 고정** (fixed partition size)시킨다.

    - 전체 메모리 공간에 대해 일정한 size 의 partition 을 만들어놓고 프로세스를 partition 안에 배정하는 것. 만약 partition 보다 크다면 여러 partition 을 합쳐서 할당한다.

    - 이 경우 partition 안에서 또 사용하지 못하는 hole 이 발생하고 이 경우를 **_internal fragmentation_** 이라고 한다.

- `Internal fragmentation` : partition 내부에 사용하지 못하는 공간이 발생하는 것

  - 결국 fragmentation 이 발생하는 것을 막을 수 없다.

- External fragmentation 의 경우 `Compaction` 을 통해 공간을 확보하는 방법이 있다.

  - `Compaction` : **기존의 hole 들을 전부 모아서 하나의 큰 block 로 만드는 것** (**place all free memory together in one large block**)

    - 그림에서는 프로세스 8 위 아래에 두 개의 hole 이 있는데 process 8 을 아래로 내리면 더 큰 사이즈의 하나의 hole 로 만들 수 있다. 그러면 다른 프로세스를 할당할 수 있는 가능성이 커진다.

    - **그 만큼의 메모리를 다 읽어서 컨텐츠들을 base 를 옮겨서 다시 write 하는 과정이 필요**하다. 이 과정동안 어떤 프로세스의 실행을 위해 메모리에 접근할 수 없다.

      - 메모리는 한 번에 하나의 프로세스만 접근해서 read / write 동작을 할 수 있기 때문이다.

    - execution time 동안 dynamic 하게 relocation 하는 경우에만 가능하다.

    - expensive
