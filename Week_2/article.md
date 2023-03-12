# 2강 Introduction to OS: operations

## 1-5. 자원 관리 (Resource Management)

- 간략하게 어떠한 자원 관리가 일어나는지만 알아본다.

- 운영체제의 중요한 역할 중 하나가 `자원 관리`.

### Process Management (ch. 3)

- `프로세스` : storage 에 있는 컨텐츠, 파일 같은 프로그램이 **메모리에 로딩이 되어서 CPU 에 의해 실행이 되고 있는 active 한 상태** (ex. 워드 프로그램이 유저에 의해 실행되고 있는 중인 것)

- 프로세스를 위한 자원 : CPU time, memory, files, I/O devices... (컴퓨터에 있는 거의 모든 자원을 사용한다.)

  - 이러한 자원들을 프로세스에 할당하고 회수하는 동작을 OS 가 수행한다.

### Memory Management (ch. 9/10)

- 프로세스가 생성되면 프로세스에 있는 명령어나 데이터를 CPU 가 읽어서 여러 동작을 수행한다. (입출력 장치 포함)

- 프로세스가 사용하고 있는 **메모리 공간을 지속적으로 추적**하고, **어떤 프로세스가 얼만큼 사용하고 있는지 파악**한다.

- 프로세스에게 필요한 **메모리 공간을 할당하고 회수하는 동작**을 한다.

- **메모리 공간은 유한하기 때문에 공간을 회수하는 것이 중요**하다.

- 프로세스와 데이터 생성의 일부분인 `Loading` 과 `Unloading` 을 한다.

- 작업 관리자를 통해 어떤 프로세스가 실행되고 있는지 볼 수 있다.

### File - System Management (ch. 13/14)

- 2nd, 3rd storage 에 저장되어 있는 데이터들은 메인 메모리의 데이터와 달리 `파일 형태로 관리`를 한다.

- storage 를 물리적인 주소를 통해 접근하는 것이 아닌, `파일이라고 하는 logical 한 접근`을 한다.

- 파일 시스템 : 파일을 쉽게 관리할 수 있는 시스템. 파일들의 집합체. (ex. NTFS, ext)

### Mass - Storage Management (ch. 11)

- I/O 장치로써의 저장 장치를 관리.

- Secondary storage : HDD, SSD (NVM)

- **main memory 에 비해 성능이 낮아서 컴퓨터 시스템의 성능 bottleneck 이 일어날 수 있다.**

- bottleneck 을 최소화 해서 성능을 최대한 효율적으로 해야하는데 그런 일을 수행한다.

### I/O System Management (ch. 12)

- **컴퓨터 외부와의 동작, 데이터 교환, 종류 다양**

- I/O subsystem

  - 메모리 관리 component : buffering, caching, spooling...

  - CPU 에서 동일한 방식으로 접근하도록 하기 위해 CPU 쪽에 일반적인 **device-driver interface** 를 제공

  - 장치 쪽엔 **specific 한 device drivers**

## Cache Management

- `메모리 계층 구조`에서 아래로 내려갈 수록 용량은 커지지만 속도는 낮아진다. **레지스터 -> 캐시 -> 메인 메모리 -> secondary memory...**

- `Caching` : `자주 사용되는 데이터`를 **계층 구조에서 윗 단계로 올려서 성능을 개선시키는 것.**

<img width="497" alt="스크린샷 2023-03-12 오후 9 10 42" src="https://user-images.githubusercontent.com/87372606/224543707-2b47e578-83e1-4864-891d-38d6c392f0f3.png">

## 1-6. Security and Protection

- `Protection` : 프로세서나 유저가 **리소스에 접근 (access) 하는 것을 제어**하는 것 (controlling)

  - 동작을 하면서 의도치 않게 (악의적 x) 문제가 발생하는 경우를 막음.

  - authorized 인지 unauthorized 인지 구분을 통해..

  - 숨겨진 에러 (latent errors) 를 detecting 함으로써 시스템의 reliability (신뢰성) 을 향상시킨다.

- `Security` : **외부나 내부의 공격으로부터 시스템을 방어**한다. 악의적인 공격으로부터 보호하는 것.

- 유저 구분

  - 1. user ID, security ID (SID)

  - 2. group identifier

  - 3. temporary privilege escalation (해당 유저만 일시적 권한 상승)

## 1-7. 가상화 (Virtualizatioin)

<img width="309" alt="스크린샷 2023-03-12 오후 9 49 38" src="https://user-images.githubusercontent.com/87372606/224545744-bf14acb8-562e-4497-a70d-68c95be09782.png">

`가상화` : **하나의 컴퓨터에서 하드웨어를 `추상화`하여 `가상 컴퓨팅 환경`을 여러개 만드는 것.**

- 실제 하드웨어의 OS 가 있을 때, 그 위에 다른 OS 를 마치 어플리케이션 프로그램처럼 실행할 수 있게 된다.

- kernel 의 입장에서 VM 은 하나의 완벽한 컴퓨터 인 것.

- CPU 가 해당하는 OS 를 지원한다면 native codes 를 실행함으로써 높은 성능을 유지할 수 있다.

- Virtual machine manager (VMM)

  - (ex) VMWare, VirtualBox on host OS

  - VMWare ESXi, Citrix XenServer : host OSes --> 자체가 OS 역할을 수행하면서 VM 역할도 수행.

`Emulation` : **소프트웨어 적으로 컴퓨터 하드웨어를 simulation 하는 것.**

- (ex) Apple Rosetta : IBM Power on Intel x86

- M1, M2 는 Intel x86 에서 실행을 하면 Virtualization 과 함께 Emulation 도 같이 실행해야된다.

- 성능 저하 (performance degradation) 이 있다.

## 1-8. 분산 시스템 (Distributed Systems)

- **물리적으로 분리가 되어있는 컴퓨터 시스템**을 **모아서** `하나의 컴퓨터처럼 사용`하는 것.

- 컴퓨터 시스템의 집합은 **heterogeneous (여러 다른 종류들로 이뤄진) 가능**하다.

  - 동일한 성능, 운영체제 소프트웨어라면 homogeneous

- **네트워크로 연결**이 되어있다. (Networked)

  - 네트워크 간 데이터 교환은 파일 형태로 이뤄진다. (NFS)

  - 또는 특정 네트워크 function 을 통해 이뤄진다. (FTP)

- `Network operation system` : 총괄적으로 관리할 수 있는 운영체제

  - 네트워크에 걸쳐 파일 공유 (file sharing) 가 가능하게 한다.

  - 어떤 메세지를 교환하기 위해 다른 컴퓨터에 있는 프로세스와 통신이 가능한데 이것이 **자동적 (autonomously)으로 작동**한다. (different processes on different computers to exchange messages, it acts autonomously)

  - 그래서 사용자는 여러개의 컴퓨터를 사용하는 느낌이 아니라 하나의 컴퓨터인 것처럼 사용이 가능한 것이다.

## 1-9. Kernel Data Structures

1. `Lists` : 데이터 값의 모음을 sequence 로 취급

<img width="344" alt="스크린샷 2023-03-12 오후 10 21 12" src="https://user-images.githubusercontent.com/87372606/224547363-162d66aa-3bc2-48e0-8878-65a4e79d04c6.png">

    - linked list 구현 : singly, doubly, circularly

    - 다양한 사이즈, 데이터 삽입 / 삭제 에 용이하다.

    - 데이터 찾는 경우 최악의 경우 시간복잡도 O(n)

2. `Stacks` : last in, first out (**LIFO**)

3. `Queues` : first in, first out (**FIFO**)

4. `Trees` : linked through **parent - child**

<img width="253" alt="스크린샷 2023-03-12 오후 10 50 34" src="https://user-images.githubusercontent.com/87372606/224548987-bbc88693-786e-4417-b47a-79bf5ff71754.png">

- general tree (여러 갈래) / binary tree (두 갈래)

- binary search : left child <= parent <= right child \_ 최악의 경우 시간복잡도 O(n)

- balanced binary search (리눅스에선 red-black tree) : O(log2(n))

5. `Hash Functions and Maps`

<img width="242" alt="스크린샷 2023-03-12 오후 10 46 40" src="https://user-images.githubusercontent.com/87372606/224548789-591ee5f5-dc4b-4b7e-8b4b-2418bfe906be.png">

- 데이터를 넣고, numeric operation 을 거쳐 numeric value 가 나오면 어떤 table 의 index 로 사용한다.

- 해쉬 맵에서 해쉬 함수에서 거쳐 나온 numeric value (key) 를 가지고 인덱스를 보고 해당하는 셀의 값을 출력한다.

- Hash collision 발생 : 서로 다른 입력을 넣었는데 같은 출력이 나오는 경우 (다른 input 인데 해쉬 함수를 거쳐 나온 key 값이 같아질 수 있다)

- Hash collision 은 추가적인 linked list 를 만들어서 해결한다. (키 값이 같은 경우 data 에서 linked list 로..) -> 성능 저하가 발생한다.

6. `Bitmaps`

- 비트의 binary digits 를 하나의 string 으로 만들어서 각각의 비트가 n 개의 아이템의 status 를 나타내도록 함.

## 1-10. 컴퓨팅 환경 (Computing Environments)

1. `Traditional Computing`

- 느린 네트워크

- 서버, thin client (서버에 접속할 수 있게해주는 기능만 제공), mobile computer (모바일 기기) 를 위한 포탈 (portals) 존재.

- Batch 방식 -> interactive 방식 (서로 주고 받기)

- time sharing 방식 -> multiprogramming / multitasking 방식 (동시 작업 가능)

2. `Mobile Computing`

- 스마트폰 이나 태블릿 컴퓨터 (iOS, Android)

- 이메일 / 웹 브라우저

3. `Client - Server Computing`

- 서버에서 모든 일이 이뤄지고 클라이언트가 작업 요청하고 결과를 본다.

  - compute - server

  - file - server

4. `Peer - to - Peer Computing`

<img width="184" alt="스크린샷 2023-03-12 오후 11 04 58" src="https://user-images.githubusercontent.com/87372606/224549754-7e2013ec-0bf9-410d-81c1-7bd7846f0574.png">

- peer : 하나의 컴퓨팅 노드 (서버나 클라이언트)

- peer 끼리 이어줄 때 2가지 방법

  1. Centralized service lookup : 가운데에 서버가 있어서 각 피어가 어떤 작업을 할 수 있는지 파악해서 찾아주는 것.

  2. Client peer 가 서버 역할하는 peer 를 직접 찾는 방식 : discovery protocol 이 존재해야 한다.

5. `Cloud Computing` : 연산능력, 저장소, 어플리케이션을 서비스로 제공

- 가상화의 logical 확장 (ex. Amazon Elastic Compute Cloud (ec2))

- Public / Private / Hybrid cloud

- Saas (Software as a service) - ex. MS office

- Paas (Platform as a service) - DB server (ex. Redhat OpenShift)

- IaaS (Infrastructure as a service) - 통째로 컴퓨터를 빌려주고 사용하는 방식 (ex. Amazon EC2, Google GCE)

<img width="360" alt="스크린샷 2023-03-12 오후 11 12 44" src="https://user-images.githubusercontent.com/87372606/224550268-082c6030-e109-4741-b302-ef01f1f7dc83.png">

- **load balancer** 가 작업을 할당하는 역할을 하고, **cloud management services** 에서는 클라이언트가 요청하는 작업을 각각의 서버가 수행할 수 있도록 조절

6. `Real - Time Embedded Systems`

- `정해진 기능`만을 하고, OS 도 `제한된 features 만 제공`한다. (very specific tasks <-> general system)

- little or no 유저 인터페이스

- 대부분이 하드웨어 장치를 모니터링 하고 제어하는 용도

  1. 기능이 많고 복잡한 임베디드 시스템의 경우에는 General - purpose computers 를 사용한다. `특정 목적`의 어플을 실행시키기 위해 standard OS (Linux) 를 돌려 사용.

  2. 특정 목적의 임베디드 OS 를 사용하는 방식 (원하는 기능만 제공하도록 설계하는 것으로 일반 목적의 standard OS 를 사용하는 것보다 효율적)

  3. OS 없이 ASICs 라는 반도체 칩으로 작업을 수행하도록 함

- `Real - time OS` : 어떤 어플리케이션을 실행하는데 있어서 실행 결과가 나오는 시간을 반드시 지켜야하는 경우 (rigid time requirements)

  - (ex) fuel - injection systems, home - appliance controllers, weapon systems

## 1-11. Free and Open - Source Operation Systems

1. `Free software`

   - 소스 코드를 얻을 수 있고, 대가를 지불하지 않고 사용, 재배포, 수정이 가능한 소프트웨어

2. `Open - source software`

   - (ex) GNU / Linux with some distributions free

   - only source 만 open

3. `Closed - source`

   - MS Windows : MS 소유, 사용 제한, 소스코드 보호

4. `Hybrid approach`

   - 오픈 소스 커널 (Darwin) 을 사용하지만 회사 자체의 컴포넌트나 소스는 공개 안함

5. `Free Operation Systems`

   - GNU (GNU's Not Unix) : free (자유롭게) 사용할 수 있는

   - GNU tool : compilers, editors, utilities, libraries and games

6. `GNU / Linux`

   - GNU 라고 하면 OS 를 제외한 나머지 컴포넌트들이고 GNU/Linux 가 GUN + Linux 개념

   - 다양한 배포판 : Red Hat, Ubuntu,,,

   - Linux on windows : Virtualization 기술 (VirtualBox, Qemu, VMware)

7. `BSD UNIX`

   - system V 에서 파생된 UNIX 로 상당히 안정적이다.

   - 배포판 : FreeBSD, NetBSD, OpenBSD ,,,

8. `Solaris`

   - Sun 사 에서 만든 BSD UNIX 기반 SunOS

<img width="502" alt="스크린샷 2023-03-12 오후 11 44 12" src="https://user-images.githubusercontent.com/87372606/224552020-e82187a9-cad9-4080-bbc2-4a3303cebfaf.png">
