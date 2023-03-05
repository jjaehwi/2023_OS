# 1강 Introduction to OS: computer

- **Organization, Architecture, Structure, Operation 측면**에서 `컴퓨터 구조에 대해 복습`한다.

- 이후 운영체제의 중요 기능들인 `Process`, `Memory`, `Storage Management`, `Protection and Security`, `Kernel Data Structures`, `Computing Environments` 에 대해 배운다.

## 1-1. 컴퓨터 시스템의 구조 (What OS Do..)

- `하드웨어` : provides basic computing resources (CPU, memory, I/O devices)

- `응용 프로그램` : solve the computing problems of the users

- `운영 체제` : Controls and coordinates use of hardware for various applications and users

  - 하드웨어와 응용 프로그램 사이를 연결시킨다.

- `사용자` : 사람, 다른 컴퓨터..

<img width="266" alt="스크린샷 2023-03-05 오후 8 33 41" src="https://user-images.githubusercontent.com/87372606/222958009-3902902d-9e56-4758-875f-d460f96db762.png">

**User View 에서의 OS 역할**

- 응용 프로그램의 interface

- Personal computing : 사용의 편의, 동작 성능, 보안

- 임베디드 : 특정 목적으로 사용되기 때문에 사용자가 간섭하는 경우가 적어서 존재하지 않거나 거의 없음.

**System View 에서의 OS 역할**

- Interface with OS

- CPU 타임 (한 컴퓨터 프로그램이 CPU를 차지하여 일을 한 시간의 양), 메모리 공간, 저장 공간, 입출력 장치 등의 자원들을 효율적이고 공정하게 할당.. (`Resource allocator`)

- Control program (유저의 프로그램을 실행시키고, 입출력 장치를 컨트롤)

### Defining OS

**컴퓨터 시스템의 목적** : 프로그램을 실행시키고, 유저의 문제를 쉽게 해결하도록 함

- 응용 프로그램 실행에 다양한 operation(작동, 동작, 운영) 들이 요구되는데 `입출력 장치를 제어`, `자원들을 할당하거나 추가`, `공통적인 operation` 에 대해 **OS 가 역할을 한다.**

**Componets of OS**

1. `Kernel` : 컴퓨터에서 항상 동작하는 하나의 프로그램 (running at all times)

2. System programs : OS 와 밀접한 관계이지만 커널은 아님, 커널이 필요할 때마다 실행

3. Application programs : OS 와 상관없지만 사용자의 필요에 의해 실행되는 프로그램

- 거의 대부분의 코드들이 OS 위에서 동작 (runs on top of an OS) 한다.

- OS 가 응용 프로그램을 실행시키는 과정을 알아야 효과적인 코딩을 할 수 있다. (proper, efficient, secure)

## 1-2. 컴퓨터 시스템 구성 (Organization)

<img width="610" alt="스크린샷 2023-03-05 오후 8 50 44" src="https://user-images.githubusercontent.com/87372606/222958726-6596d5c0-5657-49a6-914b-11da575a21c2.png">

- 입출력 장치와 CPU 가 동시에 동작을 하고, 컨트롤러 들이 메모리에 데이터를 read / write 할 때 CPU 도 메모리를 사용한다. (competing for memory cycles : 메모리 사이클 경쟁)

- CPU 는 메인 메모리에서 데이터를 읽고 주변 장치 (local buffers) 에 데이터를 준다. (CPU 는 연산을 해야하는데 데이터를 주고받고 하는데 쓰이면 효율성이 떨어진다. 그래서 DMA 라는 기능이 생겨서 데이터 교환에 관여하지 않아도 되어서 성능이 올라갔다.)

- `장치 드라이버 (Device driver)` : OS 와 장치 사이 translation

  - CPU 가 각각 장치들에 대해 이해할 필요 없이 장치 드라이버가 각 장치의 device controller 를 이해하고 있다.

  - OS 에게 장치로부터 `표준 interface` 를 제공한다. (어떤 장치건 동일한 interface 로 접근 가능)

Q. 프로그램이 data 를 요청해서 API -> device driver -> device controller -> device 로 데이터를 response 받는데,, 이 때 요청을 보내 놓고 어떻게 response 를 받을까?

A. CPU 에는 device 로 부터 들어오는 `interrupt 신호`를 받을 수 있는 `입출력 핀`이 있다. 그래서 device 는 그 핀으로 interrupt 신호를 보내고, 신호를 받으면 기존 동작을 멈추고 interrupt 를 처리하는 코드를 실행한다. 후에 발전해서 interrupt controller 가 생겨 따로 신호를 받아 처리하고 CPU 로 보내는 방식이 되었다.

**Interrupts**

- OS 와 하드웨어 사이 interact 의 핵심

- 주변장치 하드웨어로부터 CPU 로 보내는 일종의 triggering signal

  1. CPU는 신호를 받으면 하던 동작을 멈춘다. (stop)

  2. 즉시 정해진 메모리 주소에 가서 신호를 받으면 어떻게 해야하는지에 대한 `service routine` 프로그램 을 실행한다.

  3. service routine 이 끝나면 전에 멈춰놨던 동작을 다시 실행한다. (resume)

<img width="346" alt="스크린샷 2023-03-05 오후 9 21 39" src="https://user-images.githubusercontent.com/87372606/222960184-f821f360-fb53-41c1-b55f-68b4fdd35d7f.png">

1. CPU 에서 I/O 요청이 발생할 때, 장치 드라이버가 I/O 장치를 초기화한다.

2. I/O 컨트롤러에서 I/O 가 초기화된다.

3. 동작을 하고 나서 I/O 는 입력 준비상태가 되거나, 출력이 완료되거나, 에러가 발생하고 interrupt 신호가 발생한다.

4. CPU 가 interrupt 신호를 받으면 `interrupt handler (service routine)` 를 통해 `transfer control` 이 일어난다.

5. interrupt handler 가 데이터를 읽어오거나 쓰는 동작을 한다.

6. CPU 는 이전 동작을 resumes

- `Interrupt - request line (IRQ)` : 주변 장치 컨트롤러가 CPU 에 동작을 요청할 때 interrupt 를 요청하는 것

  - maskable (IRQ) : IRQ 신호가 추가로 들어와도 현재 처리 중인 interrupt 신호 처리하고 나서 처리

  - nonmaskable (FIQ) : FIQ 신호가 추가로 들어오면 현재 거를 멈추고 새로 들어온 신호 먼저 처리

- interrupt - controller 에 기능들이 추가되었다.

  - interrupt 요청을 잠시 막는 기능 (maskable 기능을 interrupt 컨트롤러에서 하는 것)

  - 적절한 원인을 파악해서 interrupt handler

  - 여러개의 interrupt 신호가 동시에 들어왔을 때 우선순위를 둬서 판단하는 기능

## Storage Structure

<img width="611" alt="스크린샷 2023-03-05 오후 11 26 13" src="https://user-images.githubusercontent.com/87372606/222966440-eb657699-3f4a-4b20-a3b3-1ec496677062.png">

<img width="611" alt="스크린샷 2023-03-05 오후 11 31 35" src="https://user-images.githubusercontent.com/87372606/222966758-fd1233d3-bf92-402c-9d9a-eb43befeae20.png">

**폰 노이만 구조 (von Neumann architecture)**

- `메모리에 프로그램을 저장하고 메모리에 저장된 프로그램과 데이터를 읽어와서 사용`하는 방식 (CPU - register - memory 구조), `주소는 byte 단위`

**Bootstrap program** : 컴퓨터가 power - on 되고 처음 실행되는 프로그램

- 컴퓨터가 시작될 때 가리키는 주소 0 번지는 BIOS (Basic Input / Output System) 를 가리킨다. (BIOS 에서 기능이 확장된 UEFI (Unified Extentiable Firmware Interface) 로 바뀌었다.)

- 이 프로그램이 실행되면서 하드웨어의 이상을 체크하고, secondary storage 의 OS 를 로딩한다.

### I/O Structure

- 사용자와의 인터페이스를 위해서 I/O 동작이 필수적이어서, 사실상 모든 프로그램은 I/O 동작을 포함하고 있다.

- OS 가 커지는 이유는 `I/O Managing` 하는 부분이 많아져서 그렇다.

  - I/O 를 어떻게 관리하느냐가 OS 의 reliability 와 performance 를 결정한다.

  - device 의 종류가 다양하다.

<img width="313" alt="스크린샷 2023-03-05 오후 11 46 59" src="https://user-images.githubusercontent.com/87372606/222967574-b4229c3a-4516-41a7-a66d-8455d5738af4.png">

- CPU 가 I/O 에 직접 접근해서 처리를 하면 성능이 낮아지기 때문에 이 문제를 해결하기 위해 `Direct memory access (DMA)` 가 등장했다.

  - CPU 는 device 에 data request 만 하고 나면 device controller 가 data block 을 직접적으로 메인 메모리로 보내고 CPU 는 읽어오는 동작을 한다. (device <-> main memory)

  - `with no intervention(간섭) by the CPU`

  - interrupt 를 하나 보내서 동작(데이터를 보냈음)이 완료되었음을 알린다.

## 1-3. 컴퓨터 시스템 구조 (Architecture)

**Single - Processor Systems**

- one core CPU : 프로그램을 실행할 수 있는 CPU 가 하나만 존재하는 것. 한 번에 하나의 프로그램만 실행할 수 있다.

- 제한된 명령어와 동작을 가지는 special - purpose processors 가 있을 수 있다. (그래픽, co - processor)

**Multiprocessor Systems**

- `Multiple processor cores and / or CPU chips`

- 여러개의 프로세서가 시스템 버스 (system bus) 를 공유하고, **시스템 버스를 통해 메모리 혹은 주변 장치와 데이터를 교환**한다.

- N 개의 프로세서가 있을 때 N 배의 성능향상을 기대하지만 실질적으로는 < N 이다.

<img width="399" alt="스크린샷 2023-03-06 오전 12 03 54" src="https://user-images.githubusercontent.com/87372606/222968493-e3576563-3a36-4599-bacf-d47e28275710.png">

```
CPU : 명령어를 수행하는 하드웨어 (logical 한 개념 - 명령을 수행하는 logical 한 하나의 block)

Processor : 눈에 보이는 physical 한 chip

Core : CPU 내부에 있는 가장 기본적인 연산 유닛 (하나의 코어는 하나의 프로그램을 실행시킬 수 있는 기능)

Multicore : CPU 하나에 multiple 한 computing core 유닛을 가지는 것

Multiprocessor : Processor 가 여러개 인 것
```

멀티 프로세스가 있는 구조를 어떻게 만드느냐에 따라 달라질 수 있다.

**SMP (Symmetric Multiprocessing)**

- 서로간의 프로세서가 독립적으로 동작을 하고, 그렇기 때문에 동시에 여러개의 프로세스를 실행시킬 수 있다.

- 각각 서로 다른 프로그램들을 실행하다보니 `Imbalanced workloads` 가 발생할 수 있다.

- `메모리는 서로 공유`하는 형태이다.

<img width="235" alt="스크린샷 2023-03-06 오전 12 12 30" src="https://user-images.githubusercontent.com/87372606/222968980-7594e9a2-cc6d-4942-94b3-7aa61e05882c.png">

**Multicore processor**

- `멀티코어 프로세서는 프로세서 안에 여러개의 코어`를 가지고 있다.

- 각 코어가 독립적인 레지스터와 캐시를 가지고 있고 두 코어 사이 데이터를 교환하기 위한 공유된 캐시를 가지고 있다.

<img width="215" alt="스크린샷 2023-03-06 오전 12 14 58" src="https://user-images.githubusercontent.com/87372606/222969116-613f57d8-9bb1-416e-a84a-42f13aedb8dc.png">

**NUMA (Non - uniform memory access)**

- 각각의 CPU 가 메모리를 공유하지 않고 각각 자신의 로컬 메모리를 가지고 있다. 하지만 physical address space 는 하나로 사용하고 있다. (서로 다른 메모리에도 접근을 할 수 있다.)

- 하지만 다른 CPU 에 붙어있는 메모리에 접근하기 위해서는 latency 가 커진다. (가급적 다른 CPU 에 붙어있는 메모리에 대한 접근을 최소화하는게 중요)

<img width="233" alt="스크린샷 2023-03-06 오전 12 17 34" src="https://user-images.githubusercontent.com/87372606/222969272-ab2985a3-d299-441c-b235-4d9ccb8dc49e.png">

**Blade servers**

- 하나의 컴퓨터 본체 안에 여러 개의 보드를 넣어서 사용하는 것. (하나의 보드가 하나의 컴퓨터)

- 여러 OS 를 포함하는 것처럼 사용할 수 있다.

**Clustered Systems**

- `여러 개의 컴퓨터`를 LAN 이나 InfiniBand 같은 통신 시스템을 통해 `느슨하게 연결한 컴퓨터 시스템`

- storage - area network 로 메모리를 공유한다. (밖에서 보면 하나의 컴퓨터, 클러스터 시스템인 것)

- `High availability service (고가용성)` (다른 컴퓨터로 옮겨 수행하면 graceful degradation 이지만 작동은 하는)

- Asymmetric 방식 : hot - standby (한 컴퓨터는 항상 대기 상태여서 문제가 생기면 바로 가져와서 수행하는..)

  - 한 컴퓨터는 항상 대기 상태여서 성능이 symmetric 에 비해 느리다.

- Symmetric 방식 : 그룹을 나눠서 그룹 내에서 모니터링 한다. 문제가 발생하는 순간 그룹 내의 다른 컴퓨터가 일을 가져와서 수행

  - 문제가 발생하지 않으면 모든 컴퓨터가 사용되므로 성능이 asymmetric 보다 좋다.

<img width="273" alt="스크린샷 2023-03-06 오전 12 21 00" src="https://user-images.githubusercontent.com/87372606/222969482-313f962e-e926-432f-a36a-8d65442aa407.png">

## 1-4. 운영 체제 동작 (Operating System Operations)

- 운영 체제가 컴퓨터에서 작동하기 위해 컴퓨터를 power on 했을 때, `bootstrap program 을 이용해 운영 체제가 loading` 되어야 한다.

  - 이 때, OS 는 kernel 에 해당하는 부분을 로딩한다.

- 이후 `kernel` 이 필요한 서비스들을 로딩한다.

**`Kernel`** : 컴퓨터 시스템과 유저에게 어떤 서비스를 제공하는 소프트웨어, OS 의 가장 핵심적인 유닛

- `System daemons` : 멀티태스킹 운영 체제에서 데몬은 사용자가 직접적으로 제어하지 않고, 백그라운드에서 돌면서 여러 작업을 하는 프로그램을 말한다.

- 부팅할 때 메모리로 로딩된다. (loaded into memory at boot time)

- 커널이 실행되는 동안 항상 같이 실행된다. (run the entire time the kernel is running)

- Ex. `Linux: systemd`

**Trap (exception)**

- software - generated interrupt (소프트웨어적 흐름의 변화), `어떤 프로세스가 특정 시스템 기능을 사용하려고 할 때 그 기능을 운영체제에게 요청하는 방법`

- 프로그램 내부에서 일어나는 에러에 의해 발생 (invalid memory access, division by zero...)

- 유저 프로그램에 의해 발생 (ex. 유저 프로그램에서 I/O 장치에 접근하려 할 때 -> IO operation) : OS 에 서비스 요청을 해서 OS 가 접근 동작을 수행한다. (`system call`)

### Multiprogramming and Multitasking

- CPU 가 idle 상태가 되었을 때, 다른 프로그램을 실행시키는 방식 으로 한 번에 하나 이상의 프로그램을 실행할 수 있다.

**Multiprogramming** : to run more than one program at a time

- CPU 사용량 증가 + 유저 만족도 증가

- `Process : 실행중인 프로그램` (일반적인 프로그램은 secondary storage 에 저장되어 있고 실행시키기 위해 메모리로 불러와야한다. 메모리로 불러오면 CPU 에 의해 실행이 될 수 있다. 이 때 CPU 에 의해 active 하게 실행되고 있는 프로그램)

- `Context switching` : 어떤 요청을 보내서 결과를 기다릴 때 응답이 와야 다음 동작을 하는데 응답을 기다릴 때 CPU 는 idle 상태가 된다. 이 때, context switching 이 일어나는데,, `서로 다른 프로세스를 실행하는 것`을 말한다. `현재 실행되고 있는 프로세스가 waiting 상태에 들어갔을 때 발생`

- 하나의 OS 에 여러 개의 프로세스를 메모리에 가지고 있을 수 있고, 프로세스 1 이 실행되고 있는 동안 다른 프로세스들은 기다리다가 프로세스 1 이 `waiting 상태`가 되면 프로세스 2 가 실행되고,, 이런식으로 동작한다. CPU 가 idle 상태 일 때, 기다리던 다른 프로세스들이 CPU 를 사용하면서 CPU 의 사용률을 높인다.

<img width="123" alt="스크린샷 2023-03-06 오전 12 45 34" src="https://user-images.githubusercontent.com/87372606/222970703-ba2523aa-c59e-4f77-9564-dc0ebae72540.png">

**Multitasking** : logical extension of multiprogramming

- 멀티프로그래밍에서는 context switching 이 현재 실행중인 프로세스가 waiting 인 상태일 때(CPU 를 사용할 수 없는 상태 일 때) 만 일어나는데, 이것을 `인위적으로 빈번하게 일어나도록 한 것.`

  - Frequent context switching -> a fast response time

  - 프로세스 1 이 CPU 를 사용하고 있을 때, 이것을 강제로 끊고 다음 프로세스가 이용하도록 넘겨주는 방식

  - 사용자가 불편함을 느끼지 않을 정도로,, 조금만 기다리면 금방 또 프로세스가 실행될 수 있게,,

### Dual - Mode and Multimode Operation

- `Kernel mode` : 운영 체제가 시스템 프로세스를 실행시키는 모드

- `user mode` : 유저 프로그램이 실행되는 모드

- 이런 모드를 사용하는 이유는 유저의 잘못된, 악의적인 프로그램으로부터 OS 를 보호한다. (Protection OS)

- Hardware starts in kernel mode

<img width="413" alt="스크린샷 2023-03-06 오전 1 07 02" src="https://user-images.githubusercontent.com/87372606/222971840-fdec4e3d-abd7-4dcc-8fe1-b13248770316.png">

- 유저 시스템에 있다가 시스템 콜을 요청한다. 시스템 콜을 요청하면 trap 이 발생해서 커널 모드로 모드 change 가 일어나고 mode bit 가 바뀐다. 시스템 프로세스가 실행되고 요청한 시스템 콜을 커널 모드에서 실행되고 그 결과를 return 하면서 mode bit 를 바꾸고 유저 모드로 바뀐다.

**Timer**

- 유저 프로그램이나 시스템 프로그램이 실행될 때, `무한 루프나 대기상태에서 escape` 할 수 있게 해준다.

- 주기적으로 interrupt 신호를 만들어서 CPU 로 보내주는 역할을 한다.

  - 정상적으로 프로그램이 실행되면 timer 는 지속적으로 reset 되면서 CPU 에 interrupt 신호를 보내지 않는다.

  - 만약 timer 에 대한 interrupt 가 들어오면 interrupt handler 는 그에 따른 정해진 조치를 취한다.
