# 5강 Process : IPC and concurrent process, Linux Practice: installation

- [3-4. IPC](#3-4-ipc-interprocess-communication)

- [3-5. IPC in Shared - Memory Systems](#3-5-ipc-in-shared---memory-systems)

- [3-6. IPC in Message - Passing Systems](#3-6-ipc-in-message---passing-systems)

  - [Direct (Naming)](#direct-naming-방식)

  - [Indirect](#indirect-방식)

  - [Synchronization](#synchronization)

  - [Buffering](#buffering)

- [3-7. Exapmles of IPC Systems](#3-7-exapmles-of-ipc-systems)

  - [POSIX shared memory](#posix-shared-memory)

  - [Pipes](#pipes)

  - [Named pipes](#named-pipes)

- [3-8. Communication in Client - Server Systems](#3-8-communication-in-client---server-systems)

  - [RPC](#rpc-remote-procedure-calls)

- [Summary](#summary)

## 3-4. IPC (Interprocess Communication)

<img width="407" alt="스크린샷 2023-03-29 오후 10 41 50" src="https://user-images.githubusercontent.com/87372606/228557135-4e76eb58-a4bd-4188-8fc6-1bd3bf710514.png">

- 멀티프로그래밍 환경에서는 여러 개의 프로세스가 존재하는데 서로 데이터를 `sharing 하는지 여부`에 따라 **_independent_**, **_cooperation 프로세스_**로 나눈다.

---

## 3-5. IPC in Shared - Memory Systems

<img width="190" alt="스크린샷 2023-03-29 오후 10 43 51" src="https://user-images.githubusercontent.com/87372606/228557708-8c7c9f34-3ff5-43ef-9fde-fe0265bbb49c.png">

- Shared - memory : 공유 메모리를 구성하기 위해 `communication process` 가 필요하다.

  - `Remove protection` for shared memory

    - 프로세스 A 와 프로세스 B 가 shared memory 를 갖고 IPC 동작을 할 때, 원칙적으로는 서로 영역에 접근을 못하는데 shared memory 부분은 공유되어야하기 때문에 `protection 을 remove` 한다.

  - remove protection 이후엔 OS 의 제어 없이 공유된 영역에 대해 read / write 를 할 수 있다.

- `cooperation processes` 의 대표적인 예 : **_producer - consumer problem_**

  - producer : 데이터를 생산하는 프로세스

  - consumer : 생산된 데이터를 읽어와서 사용하는 프로세스

  - 둘은 별도의 프로세스이지만 producer 에서 consumer 로 데이터의 이동이 있어야하기 때문에 서로 cooperation 해야한다.

  - shared memory 영역에 아이템을 채울 수 있는 버퍼를 만든다.

  - producer 는 해당하는 버퍼를 채우고 consumer 는 버퍼에 데이터를 가져다가 사용한다. (`동기화 (synchronized) 가 필요`하다.)

  - 이 때, unbounded buffer (크기 제한 없는) / bounded buffer (크기 제한 있는) 를 사용할 수 있다.

```
예시 : Producer - consumer with bounded buffer

#define BUFFER_SIZE 10
typedef struct {
    . . .
} item;
item buffer [BUFFER_SIZE];  // circular array with two logical pointers: in, out
int in = 0;                 // empty when in == out
int out = 0;                // full when ((in +1) % BUFFER_SIZE) == out

---

producer

item next_produced;
while (true){
    // produce an item in next_produced
    while (((in +1) % BUFFER_SIZE) == out) // full 인 경우
    ; // do nothing
    buffer[in] = next_produced; // write 동작
    in = (in + 1) % BUFFER_SIZE; // 썼으니까 하나 올리는 것
} // BUFFER_SIZE - 1 만큼의 아이템이 들어갈 수 있는 이유 : 배열이기 때문

---

consumer

item next_consumed;
while(true){
    while (in == out) // empty
    ; // do nothing
    next_consumed = buffer[out] // read 동작
    out = (out + 1) % BUFFER_SIZE
    // consume the item in next_consumed
}
```

---

## 3-6. IPC in Message - Passing Systems

- shared memory 를 이용한 IPC 에서는 shared memory 의 주소를 서로 공유하면서 데이터를 쓰거나 읽었다.

- `Message - Passing 방식`에서는 **_주소를 공유할 필요없이 서로 communicate 하고 synchronize 할 수 있다._**

  - `send` 와 `receive` 를 통해 메세지 교환을 할 수 있다.

  - 메세지는 fixed 한 크기 일 수도, variable 한 크기 일 수도 있다.

- `Logical implementation` 에 따라 메세지 패싱 방식이 달라진다.

  - Direct or indirect communication

  - Synchronous or asynchronous communication

  - Automatic or explicit (명시적) buffering

---

### Direct (Naming) 방식

- `symmetry` 한 구성

  - send 와 receive 에서 프로세스를 지정하고 해당 프로세스와 주고 받는 동작을 한다.

    - **send (P, message)** : 프로세스 P 로 메세지를 보냄

    - **receive (Q, message)** : 프로세스 Q 로부터 메세지를 받음

  - `link` 는 `identity` 를 통해 자동적으로 설정된다.

  - Link is associated with exactly two processes

  - 프로세스의 각 pair 에 대해 link 는 하나만 존재한다.

- `asymmetry` 한 구성

  - **send (P, message)** : 프로세스 P 로 메세지를 보낸다.

  - **receive (id, message)** : any 프로세스로부터 메세지를 받는다.

- **direct 방식은 modularity 구현에 제한적이다.** : 프로세스가 변경되면 함수들을 일일이 찾아서 바꿔줘야 하기 때문에 (identifier 가 바뀌면 다 바뀌는 것)

---

### Indirect 방식

- messages are sent to and received from `mailboxes`, or `ports`

  - `mailboxes` 나 `ports` 를 통해 데이터를 주고 받는다.

- `Mailbox` : 메세지를 가지고 있는 유니크한 아이디를 가진 object

  - 메세지를 넣어두고 (placed) 가져갈 수도 (removed) 있다. 일종의 버퍼

  - **send (A, message)** : 메일 박스 A 로 메세지를 보냄 (리눅스의 경우 integer 값을 가짐)

  - **receive (A, message)** : 메일 박스 A 로부터 메시지를 받음

- 이 방식을 사용하는 프로세스들은 메일 박스를 공유하고 공유된 메일 박스에는 link 가 형성된다.

- link 는 direct 방식과 달리 하나의 link 가 단 두개의 프로세스 사이에서만 존재하지 않는다.

- 프로세스의 pair 사이엔 관련된 메일박스가 있는 경우 `여러 개의 다른 link 가 존재할 수 있다.`

- 메일 박스는 프로세스가 소유할 수도 있고, OS 가 소유할 수도 있다.

---

### Synchronization

- 데이터가 의도치 않게 덮히거나 값이 변하는 경우를 막는다.

- `Blocking send 방식`

  - sending 을 다른 프로세스나 메일박스가 받을 때까지 막는다. (전송이 완료된 후 다음 전송을 하는 것)

- `Nonblocking send 방식`

  - 메세지를 보내고 나서 상대가 받았는지 확인할 필요없이 다음 전송을 시작할 수 있다.

- `Blocking receive 방식`

  - 메세지가 available 할 때까지 receiver 를 block

- `Nonblocking receive 방식`

  - valid 한 메세지가 있건 없건 상관없이 받는 동작을 계속 수행

- 프로세스에 따라 위의 4가지 방식을 조합해서 선택해서 사용할 수 있다.

---

### Buffering

- `Buffering` : temporary queue implementation for IPC

  - **메세지 패싱 시스템에서 버퍼링이라는 것은 IPC 를 위해 `임시적인 큐`를 구현하는 방식**

  - 3 가지 방식으로 구현할 수 있다.

1. Zero capacity

   - maximum length of zero (버퍼링을 하지 않는 것)

   - 메세지를 중간에 저장하지 않는다. (no waiting messages)

   - blocking 동작만 허용

2. Bounded capacity

   - 유한한 길이의 메세지

   - 길이가 다 차게 되면 (if the link is full) 센더는 block 된다. (sender is blocked)

3. Unbounded capacity

   - 무한한 길이를 갖는 버퍼를 가진다.

   - sender is never blocked

---

## 3-7. Exapmles of IPC Systems

### POSIX Shared Memory

- POSIX Shared Memory 는 memory - mapped files 를 통해 구현한다.

<img width="764" alt="스크린샷 2023-03-30 오전 1 30 52" src="https://user-images.githubusercontent.com/87372606/228606437-b456c1ee-8377-4b33-b934-c9e535e011da.png">

- ftruncate(fd, 4096) 을 통해 shared - memory 오브젝트를 byte 단위로 configure 한다.

- mmap() 함수를 사용해서 shared - memory object 를 가지고 있는 memory - mapped file 을 설정할 수 있다.

<img width="796" alt="스크린샷 2023-03-30 오전 1 33 53" src="https://user-images.githubusercontent.com/87372606/228607124-48caff4d-e393-41ab-81a9-60e10538ca2e.png">

<img width="796" alt="스크린샷 2023-03-30 오전 1 34 05" src="https://user-images.githubusercontent.com/87372606/228607171-0e0214d4-0b84-44e2-b01d-9d6cd00c5772.png">

- gcc 컴파일 할 때 -lrt 파라미터를 추가해야한다.

- 실제로 메세지 패싱을 구현하려면 이 코드를 변형해서 프로듀서와 컨슈머가 동시에 프로세스로 active 하게 남아있도록 해야한다. (ex. 프로듀서에서 컨슈머에 대한 child 프로세스를 실행시키는 방식)

---

### Pipes

<img width="406" alt="스크린샷 2023-03-30 오전 1 41 33" src="https://user-images.githubusercontent.com/87372606/228608926-a4802ff4-50cd-40fe-99b5-acee5f99ded5.png">

- Parent - Child 가 아니어도 서로 다른 프로세스 끼리도 가능

- `파이프는 IPC 를 구현하는 간단한 방법`

- 구현하는 방식

  - Bidirectional (양방향) / unidirectional (단방향)

  - half duplex (길이 하나인 것) / full duplex (P -> Q, Q -> P 가 동시에 가능한 것)

  - Relationship (ex. parent - child) 이 필요한가?

  - network 를 통한 communicate 가 이뤄지는지 / 내부에서 이뤄지는지

- Ordinary Pipes 의 경우 Unidirectional 하고 외부에서 access 할 수 없다.

  - parent - child 관계의 IPC 에 사용하는데, parent 가 child 를 생성하면서 child 는 pipe 의 대한 특성을 상속받는다.

  - two - way communication 이 필요한 경우 unidirectional 이기 때문에 두 개의 pipes 가 필요하다.

    - pipe(int fd[]) : fd[0] : read end, and fd[1] : write end

    - 이런 파이프는 특별한 형태의 파일로 취급하기 때문에 read 와 write 시스템 콜을 통해 메세지 전송을 할 수 있다.

  - 프로세스가 다른 프로세스와 communication 하는 동안에만 존재한다.

<img width="831" alt="스크린샷 2023-03-30 오전 1 53 03" src="https://user-images.githubusercontent.com/87372606/228611696-8d8000a2-f65e-4420-a49d-87cd1036488d.png">

### Named Pipes

- `half - duplex` 가 가능한 `Bidirectional` 한 파이프로 생성하고 **parent - child 관계가 필요하지 않다.**

- 여러 개의 프로세스가 communication 을 위해 named pipes 를 사용할 수 있다.

- 프로세스간 communication 이 끝난 뒤에도 계속해서 존재한다.

- referred to as `FIFOs` in UNIX systems (UNIX 시스템에서 FIFO 라고 불린다.)

- UNIX / Linux 에서..

  - mkfifo() 시스템 콜을 통해 생성

  - open(), read(), write(), close() 시스템 콜을 통해 manipulation

  - byte - oriented data 가 transmit 된다.

```
파이프 실행 예제

ls | less

: result of list directory -> input of less (listing a screen at a time)
: 현재 디렉토리의 파일을 보여주는 ls 의 결과를 less 명령어의 입력으로 넣어준 것
: ls 명령어의 출력값을 파이프로 보내서 less 가 파이프에서 결과물을 입력으로 읽어들여서 출력을 한다.
```

---

## 3-8. Communication in Client - Server Systems

<img width="254" alt="스크린샷 2023-03-30 오전 2 03 57" src="https://user-images.githubusercontent.com/87372606/228614392-d9acbc4e-03aa-4d06-a1d4-fe2951551c86.png">

- 서로 다른 컴퓨터 시스템에 있는 프로세스 사이의 통신을 위해서는 Client - Server 시스템을 이용한다.

- Shared memory 방식, message passing 방식, `Sockets 방식`

- `Socket` : 통신을 위한 endpoint

  - `IP 주소`와 `port number` 를 통해 **_identified_** (ex. 146.86.5.20:1625)

  - client - server 구조

- **네트워크를 통해 pair 를 이루는 프로세스가 통신을 하려면 소켓도 pair** 를 이뤄야한다.

  - `서버는 클라이언트의 요청이 오기를 기다린다. (listening)`

    - 둘 사이 link 가 되면 주시해야할 port 가 정해지기 때문에 그 특정 포트만 listening 한다.

  - 서버는 `well - known 포트` (<1024) 에 특정한 서비스를 구현한다.

    - SSH : 22, FTP : 21, HTTP : 80

  - 클라이언트 프로세스는 특정 서비스가 아닌 경우 >1024 포트에 할당한다.

  - 모든 연결은 유일하다. (unique connection) (포트 번호를 다르게 한다.)

---

### RPC (Remote Procedure Calls)

- `remote service` 를 제공하는 형태

- 소켓을 이용한 client - server 시스템의 통신에서는 단순한 데이터들이 전송되는 반면, **`RPC` 에서는 A 라는 머신에서 B 라는 머신에 어떤 프로세스를 실행시켜서 서비스가 진행되도록 하는 동작**이다.

  - 단순한 메세지가 아닌 `well structured messages` (no longer just packets of data)

    - remote service 를 실행하기 위해 어떤 함수가 실행되어야하는지에 대한 identifier 와 함수의 parameters 를 포함한다.

    - 실행하고 결과를 돌려받아야한다.

  - 하나의 IP 주소에 multiple ports 를 갖는 경우 각 port 별로 서로 다른 서비스를 지정해서 multiple services 를 할 수 있다.

    - distributed file system 을 구현할 때 유용한 구현 방식이다.

---

## Summary

- Process : 실행중인 프로그램, current activity 의 상태

- 메모리에 올라온 프로세스의 Layout

  - text

  - data

  - heap

  - stack

- 프로세스 상태

  - ready

  - running

  - waiting

  - terminated

- PCB : kernel data structure, 프로세스를 실행하기 위한 모든 정보를 저장하고 있다.

  - context switching 이 이뤄질 때 필요한 정보를 저장하고 읽어와서 프로세스를 재개

- Process Scheduler : CPU 에서 실행할 프로세스를 선택

- Context Switching : 프로세스를 다른 프로세스로 변경할 때 필요한 동작

  - 현재 실행되고 있는 프로세스의 정보를 PCB 에 저장하고, 실행할 프로세스의 정보를 PCB 에서 읽어온다.

- The fork() : child 프로세스를 생성하는 시스템 콜

- IPC : shared memory / exchaging messages / pipe 를 통해 구현

- Client - server communication

  - 소켓 : 데이터 패킷

  - RPC : function 의 concept 을 abstracting 해서 원격으로 네트워크로 연결된 다른 머신에 있는 서비스를 call 해서 실행시키는 동작
