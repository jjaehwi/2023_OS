# 11강 Memory management : Background

`chapter 9 에서 배우는 내용`

- Memory management unit (MMU)

  - MMU 를 통한 logical address 와 physical address 사이 address 변환에 대해 배운다.

- Contiguous memory allocation

  - 프로세스가 생성되기 위해 운영체제는 메모리에 프로세스가 올라갈 주소를 지정한다.

  - 이 때, 프로세스에 있는 모든 코드와 데이터를 연속된 주소에 저장하는 방식이 Contiguous memory allocation

  - 연속된 메모리에 주소를 할당하는 방법으로 First - fit, Best - fit, Worst - fit 정책에 대해 배운다.

- Internal and external fragmentation

  - fragmentation : 메모리에 프로세스 영역을 할당하면 프로세스와 프로세스 사이 혹은 프로세스 내부에 사용하지 못하는 영역이 발생하는 부분

- Paging system

  - 운영체제에서 가장 많이 사용되는 paging system

  - 메모리를 page 라는 작은 단위로 나눠서 관리

  - page 에 있는 주소를 찾아가기 위해 Translation look - aside buffer (TLB) 에 대해 공부한다.

  - page 를 구현하는 방식으로 hierarchical paging, hashed paging, inverted page table 에 대해 살펴본다.

  - IA - 32, x86 - 64, ARMv8 architecture 의 주소 변환에 대해 살펴본다.

---

- [9-1. Background](#9-1-background)

  - [Address Binding](#address-binding)

  - [Logical address vs Physical address](#logical-address-vs-physical-address)

  - [Dynamic Loading](#dynamic-loading)

  - [Dynamic Linking and Shared Libraries](#dynamic-linking-and-shared-libraries)

---

## 9-1. Background

- Memory management 를 이해하기 위한 배경

- `CPU 는 main memory 와 register 만을 직접적으로 access` 할 수 있다. (**CPU can access directly main memory and the registers only** : data holder)

  - CPU 가 접근할 수 있는 데이터를 가지고 있는 부분이 main memory 와 register 인 것

  - 컴파일러에 의해 사용할 register 가 결정된다.

  - main memory 는 변수 형태로 사용되고 주소값을 설정하는 것은 컴파일러나 linker 에 의해 지정된다.

- **main memory 에 접근하는데에는 많은 cycle 을 소모**한다.

  - 메모리 접근하는 명령을 수행하면 response 가 올 때까지 프로세서는 동작을 멈추고 기다린다. (processor stall)

  - `cache 를 이용해서 processor stall 을 최소화`한다.

  - (ex) DDR5 DRAM latency : 13.75 ~ 18.18ns 이고 5.8GHz 에서 동작하는 프로세서에서는 80 ~ 106 cycles 정도를 기다려야한다. 한 번 접근했던 메모리나 인접한 메모리에 접근할 경우 3.2GHZ 일 때 22 cycles ~ 6.4GHz 일 때 46 cycles 정도를 기다린다. (완전 새로운 장소에 access 할 때 보단 짧지만 그래도 많은 cycle 을 기다리는 것은 불가피하다.) 메모리에 접근할 경우 성능 저하가 발생하는 것이 예측가능하다.

<img width="427" alt="스크린샷 2023-05-21 오후 7 54 18" src="https://github.com/jjaehwi/2023_OS/assets/87372606/992a1506-8a99-47fc-8fe4-6e4fd15ef216">

- `Main memory 에 접근할 때 OS 영역이 user process 로부터 protection 되어야하고, user process 도 또 다른 user process 로 부터 보호되어야한다.` (Protection OS from a user process and user processes from another user during memory access)

  - 공유하도록 허용되지 않았는데 본인이 아닌 다른 프로세스가 접근해서 데이터를 바꿀 수 있으면 안된다.

  - `서로 다른 분리된 공간을 할당`한다. (**separate per - process memory space**)

  - 하드웨어적인 구현을 통해 프로세스가 다른 프로세스에 의해 침범받지 않도록 한다.

    - **OS 에 의해 제어되는 base register 와 limit register 방법 사용**

    - (ex) CPU 가 address 를 내보내면 base address 와 비교해서 클 경우 통과시키고, base + limit 보다 작으면 통과시켜서 해당 address 가 메모리에 접근할 수 있는 방식

    - 이 동작은 memory management unit 이라는 곳에서 동작하는데 통과하지 못하고 아래로 내려가게 되면 신호를 발생시켜서 OS 에 trap 을 발생시키고 trap 에 의해 OS 에서 일종의 시스템 콜 (addressing error 와 관련된) 에 해당하는 handler 코드가 실행된다.

<img width="623" alt="스크린샷 2023-05-21 오후 7 56 49" src="https://github.com/jjaehwi/2023_OS/assets/87372606/0f2b1c0f-f0fd-47f0-b62b-5332963b46a4">

---

### Address Binding

- Address Binding : 실행 코드에서 사용되는 주소와 실제 물리적인 주소와 연결시키는 것

<img width="466" alt="스크린샷 2023-05-21 오후 8 13 01" src="https://github.com/jjaehwi/2023_OS/assets/87372606/979fbea4-cdd5-4f56-8360-33fb0b0ac852">

- 일반적인 유저의 소스 프로그램은 실행파일이 되어서 메모리에 로딩되어 프로세스가 된다. (**source program -> executable -> process**)

  - 소스 프로그램을 컴파일하면 `object 파일`이 되고 object 파일은 high - level 언어를 프로세스가 이해할 수 있는 프로세스의 명령어 조합으로 만든다.

  - 이 때, **address 관련 부분은 상대적인 주소만 기록하고 최종적인 주소가 결정되진 않는다.**

  - object 파일은 linker 를 통해 다른 object 파일이나 라이브러리와 함께 조합되어 `실행파일`을 만든다.

  - `실행파일`에는 **실행에 필요한 명령어와 코드가 포함되어있고, 이 때 전체 코드에 대한 주소를 결정**할 수 있다.

    - **object 파일에서는 다 떨어져있기 때문에 각각 다른 부분에서 선언되고 정의되어있을 수 있기 때문에 주소를 결정하지 못하지만 실행파일에서는 다 모여있기 때문에 위치가 고정되어 비로소 최종 주소를 결정할 수 있는 것**이다.

  - `loader` 가 **disk 에서 메모리로 코드를 옮기고 이 과정을 통해 프로그램이 메모리에서 실행되어 프로세스가 된다.**

    - OS 에 따라 dynamically linked libraries 가 사용될 수 있는데 실행파일에 포함되어있지 않고 별도로 존재하고 있는 것을 사용하는 것

---

- `Compile time` : 컴파일러가 compile 하는 시점

  - binds symbolic addresses (variable) to **relocatable ones** (고정되지 않고 바뀔 수 있는 실제적인 주소값)

  - compile time 에서 할당되는 주소는 나중에 linker 를 통해서 실행 파일이 만들어질 때 다른 값으로 바뀔 수 있다.

  - 그러나 memory 의 주소값을 확실히 알고 있는 경우 주소값을 확정할 수 있다. (절대 주소값)

    - memory location known -> absolute code (with linker)

  - 주소값을 확정한 후 memory 의 주소값이 바뀌는 경우 recompile 하고 link 도 다시 진행해야 실행시킬 수 있다.

- `Load time` : 실행 파일이 loading 되는 시점, disk 에 있는 프로그램을 memory 에 올리는 것

  - **relocatable addresses to absolute ones**

  - memory location `known` -> **absolute code after linker**

    - 정해진 동작만 하거나 동작이 비교적 간단해서 OS 도 없고 하나의 프로그램만 실행하는 시스템

  - memory location `unknown` -> **final binding at load time**

    - 메모리 위치를 미리 알 수 없는 경우 load 할 때 알 수 있어서 그 때 최종적인 메모리 주소를 결정할 수 있다.

  - address `change` -> **reload** the code

    - known 할 때, 주소가 바뀌면 code 를 recompile 하는 것이 아니라 reload 만 해도 된다.

    - 주소는 base address + 상대적인 address 로 구성되기 때문에 loading 하면서 base address 를 바꾸는 방식으로 전체 address 를 rebinding 하는 것

- `Execution time` : 실제 메모리에 올라가서 실행되는 시점, 실제 명령어가 실행되는 시간

  - 실행되는 동안 프로세스의 relocation 이 일어날 수 있는 경우 -> loading 도중 binding 을 할 수 없기 때문에 (loading 하고나서 프로세스의 위치가 바뀌는 경우) run time 도중 최종 binding 이 일어나도록 한다.

    - Special hardware 가 필요하다.

  - 대부분의 OS 는 execution time 에 binding 을 진행한다.

---

### Logical address vs Physical address

<img width="595" alt="스크린샷 2023-05-21 오후 8 34 54" src="https://github.com/jjaehwi/2023_OS/assets/87372606/3f56a873-8c92-4f2a-b03d-390313be3664">

- `Logical address` : CPU 가 생성하는 주소

  - CPU 가 명령어를 실행하면서 주소값이 생성되면 그 주소값을 내보내서 logical address 를 생성

- `Physical address` : 메모리 시스템 에서 메모리가 가지고 있는 주소

  - 메모리가 설치되면 할당되어 있는 주소

- 프로세서가 메모리에 access 하기 위해서 physical address 값이 있어야 정확한 장소에 가서 메모리를 가져올 수 있다.

  - logical address 와 physical address 는 일치하거나, 일치하지 않다면 중간에 일치시켜주는 장치가 필요하다.

  - logical address 와 physical address 가 같지 않은 경우 logical address 를 `virtual address` 라고 한다.

    - logical address 의 공간과 physical address 의 공간이 다를 수 있고 logical address 에서 다루는 공간이 physical address 에 존재하지 않는 공간일 수 있기 때문에 가상의 주소라고 하는 것.

- `Memory - management unit (MMU)` : run - time mapping from virtual to physical addresses

  - **logical address 와 physical address 사이에서 실시간으로 mapping 하는 `하드웨어`**

  - **relocation register 를 사용해서 간단하게 구현**가능하다.

---

### Dynamic Loading

- **Dynamic Loading 의 목적** : **메모리 공간을 효과적으로 사용하기 위함**

  - 메모리에 프로세스를 생성할 때, 모든 프로그램 코드를 loading 하면 data 와 stack, heap 까지 포함해서 올라가는데 실행 파일보다 더 큰 메모리 공간을 차지한다.

  - 메모리에 프로세스를 loading 할 때, 제한이 있고 프로세스가 차지하는 메모리 영역을 최소화시키면서 실행에는 지장없도록 해야한다.

- `우선 메인 프로그램은 load 되어 실행`되어야한다.

  - 메인에서 실행되는 function 들이 있을텐데, **function 이 필요할 때만 routine 이 load** 되는 것 (a routine is loaded when it is called)

  - error routine 이나 잘 실행되지 않는 function 이 있는 경우 useful 하다.

  - loaded program portion may be much smaller although the total size may be large

- dynamic loading 을 위한 라이브러리를 제공한다.

---

### Dynamic Linking and Shared Libraries

- `Dynamically linked libraries (DLLs)` : system libraries `linked when the user programs are run`

  - 해당 라이브러리 코드가 실행 파일에 포함되어있지 않다가 (메모리의 다른 곳에 존재) 실행 파일이 실행되는 도중 해당 routine 이 필요한 경우 그 부분이 존재하는 곳으로 가서 실행시키는 것.

- 특정 OS 는 DLLs 를 지원하지 않고 `static linking` 을 지원한다.

  - system libraries 가 loading 할 때 binary program image 에 포함되어 프로세스에 들어가야한다.

- DLLs 를 하게 되면 메모리의 한 군데에 존재하고 `여러 프로세스가 공유해서 사용`한다. (`shared libraries`)

- DLLs 를 사용하는 방식은 dynamic library 에 있는 rountine 을 프로세스에서 사용하려고 할 때, loader 가 DLL 의 위치를 찾는다.

  - 아무도 사용하지 않았다면 메모리에 올라와있지 않을 가능성이 크고 (disk 에 있는 경우) 이 경우에는 해당 DLL 을 찾아서 메모리에 loading 해야한다.

- **Version information** included for library updates : libraries with corresponding version are used

  - DLL 을 사용할 때 version information 을 확인해서 테스트한 후 양쪽에서 버전이 맞도록 코딩해야함.

- 프로세스가 자기 영역 바깥의 DLL 이 있는 메모리 공간에 접근하여 사용해야하기 때문에 OS 에 의해 관리된다. (managed by OS)
