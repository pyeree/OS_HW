# 베이스 이미지 설정 (Ubuntu 22.04 LTS)
FROM ubuntu:22.04

RUN sed -i 's/archive.ubuntu.com/mirror.kakao.com/g' /etc/apt/sources.list

# 필요한 패키지 설치 (gcc, make 등)
RUN apt-get update
RUN apt-get install -y \
    build-essential \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*

# 작업 디렉토리 생성 및 이동
WORKDIR /app

# 호스트의 소스 코드 및 Makefile을 컨테이너로 복사
COPY . /app

# 프로그램 빌드
RUN make

# 실행 명령어 정의 (빌드된 실행 파일 이름에 맞춰 변경)
CMD ["./program"]