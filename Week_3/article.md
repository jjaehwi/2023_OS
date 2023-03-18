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
