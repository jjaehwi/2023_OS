# 3강 Computer system architecture and Operation

`Chapter 2 에서 배우는 내용`

- 운영체제가 제공하는 다양한 서비스

- 시스템 콜

- OS 를 구성하는 다양한 방법

- OS 설치, 커스터마이징, 부팅

- OS 성능 모니터링

## 2-1. OS 의 다양한 서비스

<img width="590" alt="스크린샷 2023-03-18 오후 4 57 13" src="https://user-images.githubusercontent.com/87372606/226093170-c087c4d2-3e95-434d-8200-914f227344cb.png">

- 유저 또는 다른 시스템 프로그램이 제일 위에 있고, 유저 인터페이스를 통해 운영체제가 서비스를 제공한다. 그리고 유저의 어떤 요청은 `시스템 콜`을 통해 운영체제가 제공하는 다양한 서비스를 유저에게 제공한다.

- 서비스 중 많은 부분은 **하드웨어를 제어**하는 부분이다.

- 서비스의 종류

  - 프로그램 실행

  - 입출력 장치 운영 (키보드 / 마우스 / 터치, 네트워크, 디스플레이)

  - 파일시스템 조작

  - Communications (IPC)

  - 에러 detection : HW failure, I/O 에러, 유저 프로그램 에러 -> 에러 핸들링

  - Resource 할당 : 컴퓨터의 리소스를 프로세스에 할당하여 효과적으로 사용

  - Logging : tracking records for accounting or statistics

  - Protection and security : authorization (적합한 권한에 맞게 자원을 사용하는지) and authentication (인증을 통해 정당한 사용이냐 아니냐를 판단, 악의적인 접근을 막음)

## 2-2. 유저와 운영체제의 인터페이스

- `Command Interpreters` 를 통해 이뤄진다.

  - 유저가 지정한 명령어를 읽어드리고 실행하는 방식 : 명령어는 실행을 포함한 파일 조작이 대부분

  - 명령어 자체에 필요한 정보를 포함 (self - containing) (윈도우)

  - 명령어 창 / 파워 셸 (윈도우)

  - 명령어는 해당 시스템 프로그램을 실행시킴 (유닉스 / 리눅스)

  - C shell (csh), Bourne - Again shell (bash), Korn shell (Ksh)... (유닉스 / 리눅스)

- `GUI (Graphical User Interface)` 등장

- Touch - Screen Interface

  - 모바일 등에서 터치와 제스처 사용

- 인터페이스 선택 : 개인의 선호, 어플리케이션에 따라 달라질 수 있다.

## 2-3. 시스템 콜

- 운영체제가 제공하는 `서비스를 제공하기 위한 일종의 인터페이스`

  - 특정한 작업을 위해서 C, C++, 어셈블리어 로 작성된 **함수의 집합**

**Example : file copy**

```
1. Obtaining file names
- command 방식 : cp in.txt out.txt
- Program asks 방식 : typed by the user

2. 파일 이름을 얻으면 input file 을 opening 하고 reading

3. output file 에 writing, 파일이 없다면 creating (2,3 과정에 대한 에러 핸들링을 위한 또 다른 시스템 콜 필요)

4. input file 에서 데이터를 읽고 쓰는 것을 반복, 에러 체킹도 반복됨 (모두 시스템 콜 필요로 하는 작업)

5. 모든 작업이 끝나면 file 을 closing

6. 완료가 되었다는 display messages 발생

* 위의 이 모든 작업들은 시스템 콜을 요구하는 작업이다. *
```

<img width="344" alt="스크린샷 2023-03-18 오후 5 25 36" src="https://user-images.githubusercontent.com/87372606/226094440-34715584-cc03-43fd-aa98-f8db2c1e594f.png">

### API (Application Programming Interface)

- 이런 시스템 콜은 **`API`** 를 통해 이뤄진다.

  - API 는 **어떤 파라미터를 보내고 어떤 리턴 값을 주는지 에 대한 available 한 함수의 집합**을 를 정의한다.

  - 함수를 사용하는데 필요한 최소한의 정보만으로 그 함수를 불러서 사용하는 것.

**Example : read()**

```
#include <unistd.h> (이 헤더에 정의되어 있는 함수이다.)
ssize_t read (int fd, void *buf, size_t count)

ssize_t : 리턴 값
read : 함수 이름
(int fd, void *buf, size_t count) : 파라미터들
int fd : file descriptor
void *buf : Data buffer
size_t count : 버퍼 안에 들어가는 maximum bytes
```

- API 를 사용하기 때문에 `portability` (다른 시스템, 운영체제에 동일한 코드를 가져가서 사용하는 것) 를 증가시킴

### RTE (Run-time environment)

- API 는 말그대로 인터페이스 만을 제공한다. 시스템 콜과 관련된 API 에서도 API 뿐만 아니라 API 를 실행할 수 있는 코드가 존재해야 한다. 그러한 것을 모아놓은 것이 RTE 이다.

- `RTE` : `어플리케이션을 실행하는데 필요한 소프트웨어의 전체 집합`

  - 시스템 콜 인터페이스를 통해서 시스템 콜을 할 경우, 실질적으로 시스템 콜과 연결되어 그 **시스템 콜이 동작**을 할 수 있도록 해준다.

  - caller 는 API 의 조건을 만족하기만 하면 된다. (파라미터를 맞추고 리턴 값에 맞춰 값을 받음) 또한 OS 시스템이 어떻게 동작하는지 이해해야한다.

  - OS 에서 제공하는 서비스의 시스템 콜이 어떻게 일을 수행하는지에 대해서는 RTE 가 관리를 하기 때문에 알 필요 없다.

<img width="338" alt="스크린샷 2023-03-18 오후 5 41 13" src="https://user-images.githubusercontent.com/87372606/226095087-b686d6ad-85c1-4fb9-a8ec-c674d42ed7a0.png">

- printf() 는 사용자가 사용하는 함수, write() 는 시스템 콜에서 API 를 통해 사용하는 함수

- 파라미터를 넘겨주는 방법

  - 레지스터를 이용하여 넘기는 방법

  - 데이터가 많은 경우 메모리에 블록을 잡고 그 주소를 를 넘기는 방법 (리눅스에서는 >5 이면 이 방법 사용)

  - 블록 메모리 (스택 구조) 를 사용할 경우 푸시, 팝을 통해 파라미터를 저장하고 다시 읽어가는 방식으로 동작 가능.

### 시스템 콜의 종류

1. `Process control`

- create process / terminate process

- load / execute

- process attributes 를 읽어옴 (get), process attributes 를 사용, 값 변경등.. (set)

- wait event, signal event 관련

- memory allocate and free

2. `File management`

- 파일 생성 / 삭제

- open / close

- read / write, reposition

- get / set

3. `Device management`

- request device / release device

- read / write / reposition

  - 파일 시스템과 마찬가지고 레지스터에 주소가 할당되어 있으면 그 주소에 값을 읽어오고 데이터를 쓰는 것.

  - 프로세서 입장에선 2nd storage 나 메모리, device 에 접근하는 동작이 동일한 방식으로 진행됨.

- get / set

- logically attach / detach devices

  - 물리적으로 붙어있다고 사용가능한 것이 아니라 device 를 리스트에 저장해서 attach 하고 활성화된 device 가 뭔지 알아야한다.

4. `Information maintenance`

- get / set time or data

- get / set system data

- get / set process, file or device attributes

5. `Communications`

- communication connection 생성 / 삭제

- send / receive messages

- transfer status information

- attach or detach 원격 장치

6. `Protection`

- get / set file permissions (권한)

## 2-5. 링커와 로더 (Linkers and Loaders)

<img width="338" alt="스크린샷 2023-03-18 오후 6 10 07" src="https://user-images.githubusercontent.com/87372606/226096461-ed6ae6e4-a8b4-4b56-989b-431c6fc82bb6.png">

- `컴파일러 (Compiler)` : High level 언어로 작성한 소스 파일을 컴파일하고, `Relocatable object file 이 생성`한다.

  - Relocatable : 메모리에 올라가는 주소를 재배치할 수 있음.

  - object file : 프로세서가 인식할 수 있는 명령으로 구성된 파일

- `링커 (Linker)` : object file 을 모아서 하나의 `binary 로 구성된 실행 파일 (executable file) 을 생성`한다.

- `로더 (Loader)` : binary excutable file 을 메모리에 로딩해서 실행시킨다.

  - 새로운 프로세스를 생성한다는 의미 : `fork()` 를 통해 새로운 프로세스를 생성한다. (Linux)

  - 프로세스가 생성되면 로더를 `exec()` 로 invokes 한다. 그러면 로더는 실행 파일을 메모리에 로딩해서 실행시킨다.

- `재배치 (Relocation)` : linking and loading 과정에서 일어난다.

  - 최종적으로 프로그램이 올라갈 physical 한 메모리 주소를 assign 하는 과정

  - 최종적인 주소와 매칭이 될 수 있도록 프로그램에서 사용된 코드와 데이터의 실직적 주소를 조절

- `DLLs (Dynamically linked libraries)` : **특정한 프로그램 뿐 아니고 자주 사용되는 object 파일들**을 DLL 에 `별도로 로딩`해놓은 것.

  - DLL 에 해당하는 부분만큼 프로그램 사이즈가 작아진다. (자주 사용하거나, 공통되는 부분을 DLL 에 로드해놔서 메모리를 효과적으로 사용할 수 있다.)

### Object files and executable files

- 일반적으로 standart formats 을 가진다 : 유닉스 / 리눅스에서 `ELF (Executable and Linkable Format)`, 윈도우에서 `PE (Portable Executable)`, MacOS 에서 `Mach - O`

- 함수들과 변수들과 관련된 메타 데이터를 포함하고 있는 `symbol table` 과 `컴파일된 machine code` 를 가지고 있다.

- ELF 는 relocatable, executable file 이다. entry point (프로그램이 실행될 때, 실행할 첫번째 instruction 의 주소를 가리킴) 를 가진다.

## 2-6. Why Applications are OS specific 한가..

- 어플리케이션이 OS 에 따라 동작을 하기도하고 안하기도 하는데, 왜 어플리케이션은 OS specific 할까..

- 많은 `시스템 콜을 통해 동작이 이뤄지는데`, 이런 **시스템 콜은 OS 마다 전부 다르다**. 그렇기 때문에 OS 마다 고유의 시스템 콜의 집합을 가지고 있다.

  - **OS 마다 서비스의 기본적인 기능은 같지만, 서비스를 제공하는 방식이 전부 다르기 때문**에 어플리케이션은 OS specific 할 수 밖에 없다.

- Multiple operating systems 에서 실행할 수 있는 어플리케이션

  - interpreted language 를 사용하는 경우 (Python)

    - reads each line

    - 통역을 거치기 때문에 performance problem

  - 실행하는 어플리케이션을 포함하는 virtual machine 을 사용하는 방법

    - virtual machine is part of the language's full RTE : ex. JAVA

    - RTE 에 다양한 OS 를 porting

    - 중간에 VM 을 거치기 때문에 performance problem

  - standard language or API 사용

    - 컴파일러가 OS specific 한 언어를 이용해서 binary code 를 생성하는 방식

    - 각 OS 별로 각각의 compiler 를 이용해서 서로 다른 실행코드를 만들어서 실행하는 것.

    - API 가 같은 경우 소스 코드를 바꾸고 컴파일만 각 OS 맞게 하면 되기 때문에 비교적 쉽게 OS 에 porting 가능

## 2-8. OS 구조

### Traditional 유닉스 시스템 구조

<img width="415" alt="스크린샷 2023-03-18 오후 7 09 28" src="https://user-images.githubusercontent.com/87372606/226099211-9e5232d3-9442-4885-b682-b3dd3c1996a8.png">

- `kernel` : 운영체제에 가장 핵심적인 서비스를 제공하는 부분

- 유저가 `Shell` 을 통하여 커널과 인터페이스

- `OS specific 한 라이브러리를 가진 컴파일러나 인터프리터`를 통해 유저가 작성한 소스코드를 커널이 실행할 수 있는 executable file 로 만든다.

- 장치를 직접적으로 제어하기 위해 드라이버 가 필요하고 일반적인 I/O 장치와 관련된 동작을 표준화된 명령어로 내리면 드라이버에서는 각 장치에 specific 한 명령으로 바꾸는 역할을 한다.

  - 그런 명령어들이 각 장치를 제어하는 하드웨어 컨트롤러를 통해서 장치로 전달이 된다.

### Monolithic 구조

<img width="169" alt="스크린샷 2023-03-18 오후 7 17 08" src="https://user-images.githubusercontent.com/87372606/226099526-1b1410cb-53b8-4573-8a93-1da765a406f1.png">

- `Monolithic 구조` : 특정한 구조가 아니고, **모든 커널이 하나의 static binary file 로 되어있는 OS**

  - 모든 기능이 하나의 커널에 통합이 되어있다.

  - 내부적으로는 layered 구조를 가지고 있지만 `single address space 이기 때문에 monolithic` 이다.

  - 구현이 어렵고 확장이 어렵다.

  - 하나로 묶여있기 때문에 overhead 가 적어서 성능면에서 advantage, 유지보수면에서 disadvantage

### Layered Approach

<img width="200" alt="스크린샷 2023-03-18 오후 7 24 04" src="https://user-images.githubusercontent.com/87372606/226099849-37df3c65-de37-4e48-998b-dd12f28ca43d.png">

- `여러 단의 layer 로 구성`

  - 각 layer 는 `loosely coupled` : 각 layer 들이 독립적, 각 레이어의 내부적 변경이 다른 레이어에 영향을 미치지 않음

  - 특정한, 제한된 기능을 가진 smaller components 로 구성되어 있다.

- layer 1 ~ layer N-1 까지가 kernel 부분

  - 아래 쪽 layer 들은 위의 layer 가 invoking 했을 때 실행이 가능한 구조

- 서비스는 맨 위 레이어로 시작해서 차례차례 아래로 진행되는 것.

  - 동작 예측이 쉽다.

  - 항상 인접한 레이어에서만 동작이 이뤄지니까 구성이 간단하다.

  - ex. TCP/IP 같은 컴퓨터 네트워크

  - 각 레이어의 functionality 를 적절하게 정의하는 것이 어렵 = 해당 레이어의 적절한 기능을 정의해야한다.

- 여러 레이어를 통과해서 가야하는 경우 overhead 가 커져서 성능이 크게 저하된다.

### Microkernels

<img width="439" alt="스크린샷 2023-03-18 오후 7 28 24" src="https://user-images.githubusercontent.com/87372606/226100010-6b14655e-fde5-40ea-8ab6-ce0c027f7baf.png">
 
- Monolithic 구조는 커널안에 모든 기능이 포함되어 있었는데, `필요하지 않은 component 는 제거`해서 `필수적인 기능만 남긴 small kernel` 을 만들고 **별도의 address space 에 user level 프로그램 형태로 원래 OS 가 제공하던 서비스 기능을 별도로 만들어 두는 것**

    - 커널은 최소한의 기능만 제공하고 그 밖의 OS 가 제공해야하는 서비스를 마치 **유저 프로그램 (별도의 어플리케이션 프로그램) 처럼 실행해서 제공**하는 것

- 커널은 최소한의 기능만 제공하기 때문에 커널의 기능을 개선하거나 디버깅 하는 것이 간단하다.

  - `확장이 쉽다.`

  - 새로운 서비스는 유저 프로그램 처럼 실행되면 되기 때문에 마치 유저 프로그램 만들듯이 만들어서 커널에 의해 실행될 수 있도록 만들면 된다.

  - Secure and reliable

### Modules

- Loadable kernel modules (LKMs) : 모듈을 이용한 가장 진보적인 방식 (유닉스, 리눅스, 맥OS, 솔라리스, 윈도우)

- 커널은 core components 의 집합

  - 모듈을 통해 추가적인 서비스와 연결할 수 있다. (부팅, 런 타임 시에)

- `커널은 core services 만 제공하고 다른 서비스들은 dynamically 하게 구현된다.`

- `Layered 시스템과의 유사점` : **각 커널 섹션은 정확하게 정의**되어있고 **interface 가 보호**되어 있어서 변경하지 않고 사용한다.

  - `차이점` : layered 시스템에 비해 **훨씬 flexible 하고 하나의 모듈이 다른 어떤 모듈도 볼 수 있다.**

- `Microkernel 시스템과 유사점` : primary 모듈이 **core 기능**만 가지고 있고, **다른 모듈과 어떻게 load 하고 communicate** 하는지에 대한 정보만 갖고 있다.

  - `차이점` : 모듈이 **message passing 을 통해 정보를 교환할 필요가 없기 때문에 훨씬 효율적**이다.

- EX. Linux

  - LKMs 는 아무 때나 load 하거나 remove 될 수 있다. 모듈을 마치 유저 프로그램처럼 사용할 수 있는 것.

  - `dynamic and modular kernel` : monolithic system 의 성능을 유지하면서 각각 모듈화 되어있어서 인터페이스끼리만 연결되므로 유지보수가 쉽고 필요없는 경우 내보낼 수 있기 때문에 메모리를 효율적으로 사용 가능하다.

### Hybrid Systems

- 각각 다른 구조를 섞어서 사용.

- EX. Linux : monolithic + modular

  - 모듈을 불러서 사용하는데 single address space 에서 이뤄지기 때문에 마치 monolithic 처럼 보여지지만 모듈들은 항상 상주하는 것이 아닌 필요할 때 load 하고 필요없으면 remove 하는 방식으로 동작

- EX. Windows : largely monolithic + some typical behavior of microkernel + dynamically loadable kernel modules

## 2-9. OS 빌드, 부팅 (Building and Booting an OS)

### System Boot

1. BIOS / UEFI : 이 프로그램이 시스템을 초기화시키고 OS 를 로딩하는 동작까지 수행

2. Bootstrap program or boot loader 로 커널 위치를 찾음

3. 커널을 메모리에 로딩하고 시작 : 이때부터 OS 가 동작

4. 커널이 하드웨어를 초기화

5. The root file system is mounted : secondary storage 를 사용할 수 있게 준비 동작 까지 완료.

## <정리>

- `OS` : 프로그램을 실행하기 위한 다양한 서비스를 제공하는 역할

- `OS 와의 interacting` : CLI, GUL, 터치스크린

- `시스템 콜` : OS 가 제공하는 서비스를 사용하기 위한 인터페이스, API 를 통해 이뤄진다.

- 시스템 콜 기능 6가지

- standary C 라이브러리가 유닉스 / 리눅스에서 시스템 콜 인터페이스 제공하는 역할

- OS 에 있는 시스템 프로그램은 사용자에게 유틸리티를 제공

- `링커`는 컴파일을 통해 만든 relocatable object modules 를 결합해서 하나의 실행가능한 binary 파일을 만드는 역할

- `로더`는 이런 실행가능한 파일을 메모리에 올려 실행할 수 있게 만들어주는데 이 과정에서 실제 physical address 를 부여하는 relocation 이 이뤄진다.

- OS 에 따라 실행할 수 있는 어플리케이션이 달라지는데 (`specific OS`) OS 마다 binary formats, instruction sets (명령어 set), system calls 서로 다르기 때문.

- `OS 의 구조` : Monolithic, Layered, Microkernels, Modules
