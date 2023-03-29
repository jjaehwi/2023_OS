# Linux

- [Linux 구조](#linux-의-구조)

- [Linux 명령어](#linux-명령의-구조)

  - [기초 명령어 사용](#기초-명령의-사용)

  - [Script 방식](#script-방식)

---

## Linux 의 구조

<img width="292" alt="스크린샷 2023-03-29 오후 9 40 35" src="https://user-images.githubusercontent.com/87372606/228538122-1b0f7fc6-53c3-4a4b-b6c7-b2538318d130.png">

- 커널 (Kernel)

  - 컴퓨터 자원 관리

    - 메모리, 파일 시스템 장치 관리

- 쉘 (Shell)

  - 사용자 인터페이스 제공

  - 사용자의 명령을 입력받아 처리한 후 결과 출력

- 유틸리티 (Utility)

  - 각종 프로그래밍 개발 도구, 문서 편집도구 등

---

## Linux 명령의 구조

**_`명령 [옵션] [인자]`_**

- `명령 (Command)`

  - Linux 에 작업을 지시하는 프로그램 이름

  - Shell 내장 명령과 유틸리티 (독립된 프로그램) 로 구분

  - (ex) ls, date, pwd ... 등

- `옵션 (Options)`

  - 명령의 세부 기능을 선택

  - '-' 로 시작하고 숫자, 대문자, 소문자를 사용

- `인자 (Arguments)`

  - 명령으로 전달되는 값

  - 보통 파일명이나 디렉토리 명

  - (ex) ls /tmp, ls -l /tmp

- Linux command cheat sheets 참조

---

### 기초 명령의 사용

- `date` : 날짜와 시간을 출력

- `clear` : 화면을 지움

- `man` : 명령에 대한 설명 출력 (ex. man clear), manual

- `passwd` : 패스워드를 변경 (기존 password 로 바꾸는 것은 불가능)

- `md` : make directory, 디렉토리를 만드는 명령어

- `cd` : 현재 위치를 바꾸는 명령어

- 리눅스에서 작업을 하다보면 Root 권한으로 작업을 해야할 때가 있다. `root 는 강력한 권한`을 가진다. 이런 root의 비밀번호를 변경할 수 있다.

- `sudo passwd root` : root 비밀번호 변경

  - 현재 사용자 비밀번호 입력, 새 root passwd 입력, root passwd 입력 확인

---

### Script 방식

- Ubuntu 에서는 `BASH` 를 사용한다.

  - Shell 종류 중 하나이며, end - user 가 사용하기 가장 편한 shell 로 손꼽힌다.

- `Shell Script`

  - Shell 명령은 앞서 배운 ls, cd, passwd 와 같이 하나의 명령어로 직접 prompt 창에 입력할 수 있지만, `script file 로 저장하여 한번에 여러 명령어를 순차적으로 수행`할 수 있다.

    - 사용자 개인의 Linux machine 설정을 shell script 로 설정

    - 복잡한 명령을 간단하게 alias (가명) 으로 설정하여 사용

    - 여러 개의 명령어 집합을 하나의 shell script 로 만들어 사용하는 방법

<img width="590" alt="스크린샷 2023-03-29 오후 10 01 20" src="https://user-images.githubusercontent.com/87372606/228543505-8955df69-5018-46fa-a423-0f8fdf061ff5.png">

- 대표적인 사용 예시가 **사용자 개인 설정을 Shell Script 에 저장하고 사용**하는 것 이다.

  - (ex) `bashrc`

  - `~/.bashrc` 는 terminal 을 실행할 때 같이 실행되는 default file 이다.

  - `$ vi ~/.bashrc`

  - 매번 설정하기 귀찮을 때 사용

  - `source` 는 해당 .bashrc 를 terminal 이 인식하게 하는 명령어 이다.

```
실습
: ~/os_prac/week2 디렉토리를 만들고, 터미널 시작과 동시에 해당 directory 로 이동하게 하는 명령을 .bashrc 에 입력하여 test 한다.

// 디렉토리 생성
md os_prac
cd os_prac
md week2
cd week2

gedit ~/.bashrc 로 편집기를 열어서 수정할 수 있다.
```

<img width="590" alt="스크린샷 2023-03-29 오후 10 14 09" src="https://user-images.githubusercontent.com/87372606/228549406-1cea75da-989a-4c87-8878-289300329bca.png">

- 복잡한 명령을 `alias` (별명) 를 통해 축약

  - alias 명령어는, 여러 불편한 명령어들을 하나의 명령어로 '별명' 을 붙일 수 있다.

  - ~/.bashrc 에 위와 같은 예제로 작성하고, source ~/.bashrc 를 하면 명령어를 변경하여 사용할 수 있다.

```
실습
: Hello 를 입력하면 'Hello Linux!' 라는 문자열을 출력하는 alias 를 만들어보자. (hint : echo 명령어를 사용한다.)

alias hello = 'echo Hello Linux!'
hello
```

- 순차적 명령어를 하나의 shell script 로 실행할 수 있다.

  - example.sh

  - 어떤 프로젝트 폴더에서, 항상 수행할 작업 목록들을 하나의 file 로 만들어둔다.

  - Shell Script / TCL / Pearl / Python 등 여러 scripting 언어를 이용하여 사용할 수 있다.

  - Shell 명령어는 C 언어처럼 코딩 또한 가능하다.

```
실습
: 사용자를 등록하는 기능을 구현, 사용자가 원하는 ID 를 입력받아 현재 시스템에 등록된 ID 인지 확인하는 shell script
```

<img width="615" alt="스크린샷 2023-03-29 오후 10 19 37" src="https://user-images.githubusercontent.com/87372606/228550917-5e584449-bb40-4f7e-9265-cd93b0fb63d4.png">
