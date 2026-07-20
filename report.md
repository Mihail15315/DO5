# Simple Docker. DO5_SimpleDocker.

## Part 0. Установка докера

  * *Скриншот выполнения команд:*
    ![Установка докера](/screens/download_docker.png)
    *(На скриншоте показан процесс скачивания докера).*

## Part 1. Готовый докер

* **Проверка готовности докера и выкачивание официального образа Nginx из Docker Hub**
  * Использована команда `docker pull nginx` для загрузки последней стабильной версии образа.
* **Запуск и проверка первого изолированного контейнера**
  * Контейнер запущен в фоновом режиме с помощью команды `docker run -d nginx`.
  * Статус работы контейнера проверен через команду `docker ps`.
  * *Скриншот выполнения команд:*
  * ![Скриншот выполнения команд](/screens/screen_pull_images.png)

* **Инспекция контейнера и сбор параметров для отчёта:**
  * Детальная информация получена с помощью команды `docker inspect -s <container_id>`. Флаг `-s` применен для принудительного вычисления занимаемого дискового пространства.
  * ![команда docker inspect](/screens/docker_inspect.png)
  * Из вывода команды определены следующие ключевые метрики контейнера:
    * **Размер контейнера:** `SizeRw` (собственный слой записи) — 0 B (изменений нет), `SizeRootFs` (виртуальный размер) — 143 MB.
  * ![Размер контейнера](/screens/screen_inspect_size.png)
    * **Список замапленных портов:** В блоке `"Ports"` указано значение `null` (порты не пробрасывались на хост-машину).
  * ![Список замапленных портов](/screens/screen_inspect_ports.png)
    * **IP-адрес контейнера:** В подсистеме `"NetworkSettings"` -> `"Networks"` -> `"bridge"` зафиксирован IP-адрес `172.17.0.2`.
  * ![Вывод команды docker inspect с выделенными параметрами](/screens/screen_inspect_network.png)
    *(На скриншотах были показаны ключевые секции JSON-файла инспекции: блок размера SizeRootFs, пустая конфигурация Ports и внутренний IP-адрес контейнера).*

* **Остановка базового контейнера и Публикация портов и проверка веб-сервера:**
  * Контейнер остановлен командой `docker stop <container_id>`.
  * Факт остановки подтвержден повторным вызовом `docker ps`, который вернул пустую таблицу процессов.
  * Запущен новый контейнер с маппингом портов 80 и 443 один к одному на локальную машину командой: `docker run -d -p 80:80 -p 443:443 --name my_nginx nginx`.
  * Выполнена проверка доступности веб-сервера. Поскольку окружение развернуто на Ubuntu Server, отправлен локальный запрос через консольную утилиту `curl http://localhost:80`.
  * *Скриншот отсановки, а после запуска с портами и приветственной страницы:*
  * ![Запуск контейнера с маппингом портов и проверка доступности страницы](/screens/screen_localhost_80.png)
    *(На скриншоте показаны вышеописанные команды).*
  * *Скриншот доступности через браузер посредством проброса порта:*
  * ![Скриншот с браузера](/screens/screen_localhost_browser.png)
    *(Выполнена проверка доступности веб-сервера через браузер).*
    
* **Перезапуск контейнера и финальный аудит:**
  * Выполнен перезапуск веб-сервера с помощью команды `docker restart my_nginx`.
  * Работоспособность контейнера после рестарта успешно проверена через `docker ps` (статус сменился на "Up X seconds").
  * *Скриншот перезапуска контейнера:*
    ![Перезапуск контейнера my_nginx и проверка его аптайма](/screens/screen_restart.png)
    *(На скриншоте виден вызов команды docker restart и финальная проверка через docker ps, отображающая активный статус контейнера).*

## Part 2. Операции с контейнером

*   **Чтение оригинальной конфигурации Nginx**
    *   Была выполнена команда `docker exec` для просмотра дефолтного конфигурационного файла `nginx.conf` внутри запущенного контейнера `my-nginx2`:
    ```bash
    docker exec my-nginx2 cat /etc/nginx/nginx.conf
    ```
    ![Вызов и вывод команды exec для чтения оригинального nginx.conf внутри контейнера](/screens/01_cat_original_config.png)

*   **Создание и настройка файла конфигурации на локальной машине**
    *   На хост-машине был создан локальный файл `nginx.conf`, в блок `server` которого добавлен путь `/status` с директивой `stub_status on;` для сбора метрик состояния веб-сервера.
    *   Полное содержимое созданного конфигурационного файла, открытого в текстовом редакторе, представлено на скриншоте ниже:
    *   ![Содержимое созданного локального файла конфигурации nginx.conf с настроенным блоком stub_status](/screens/02_local_nginx_config.png)

*   **Применение конфигурации и перезапуск веб-сервера**
    *   Созданный файл конфигурации был скопирован внутрь контейнера поверх старого файла с помощью команды `docker cp`.
    *   После этого была выполнена команда `docker exec` с сигналом `nginx -s reload` для безопасного перезапуска Nginx внутри контейнера без его остановки:
    ```bash
    docker cp nginx.conf my-nginx2:/etc/nginx/nginx.conf
    docker exec my-nginx2 nginx -s reload
    ```
    *   ![Процесс копирования файла конфигурации внутрь контейнера и последующий reload службы Nginx](/screens/03_copy_and_reload.png)

*   **Проверка работоспособности страницы статуса**
    *   С помощью утилиты `curl` выполнен проверочный запрос к порту веб-сервера. По пути `/status` успешно отдаётся страница с актуальными техническими метриками (количество активных подключений, принятых и обработанных запросов):
    ```bash
    curl http://localhost:8000/status
    ```
    *   ![Успешный вывод страницы статуса сервера Nginx по адресу localhost:8000/status через утилиту curl](/screens/04_check_status_first.png)


## Перенос и восстановление контейнера через архив

*   **Экспорт контейнера и остановка служб**
    *   Файловая система настроенного контейнера со всеми изменениями была экспортирована в один плоский архивный файл `container.tar`.
    *   После завершения экспорта исходный работающий контейнер был остановлен:
    ```bash
    docker export my-nginx2 > container.tar
    docker stop my-nginx2
    ```
    *   ![Вызов команд экспорта файловой системы контейнера в архив container.tar и последующей остановки контейнера](/screens/05_export_and_stop.png)

*   **Принудительное удаление оригинального образа и контейнера**
    *   Был удален оригинальный докер-образ `nginx` с использованием флага принудительного удаления `-f`, при этом сам остановленный контейнер на данном шаге намеренно оставался в системе.
    *   Вслед за образом был удален и сам остановленный контейнер `my-nginx2`:
    ```bash
    docker rmi -f nginx
    docker rm my-nginx2
    ```
    *   ![Принудительное удаление докер-образа nginx и окончательное удаление старого контейнера](/screens/06_rmi_and_rm.png)

*   **Импорт образа из архива и запуск нового контейнера**
    *   Из созданного ранее слепка `container.tar` был импортирован новый докер-образ под именем `imported_nginx`.
    *   На базе импортированного образа был создан и запущен новый контейнер `my_new_nginx`. На старте была вручную передана команда запуска Nginx в фоновом режиме для восстановления утерянных метаданных:
    ```bash
    docker import container.tar imported_nginx
    docker run -d -p 80:80 --name my_new_nginx imported_nginx nginx -g 'daemon off;'
    ```
    ![Импорт нового образа из файла container.tar и успешный запуск контейнера с явным указанием команды старта](/screens/07_import_and_run.png)

*   **Итоговая проверка работоспособности восстановленной системы**
    *   Выполнен повторный запрос к адресу `http://localhost:80/status`. Страница состояния Nginx успешно отдаёт данные, что подтверждает сохранность конфигурационного файла после процедуры экспорта и импорта контейнера через `.tar` архив:
    ```bash
    curl http://localhost:80/status
    ```
    *   ![Финальная проверка доступности страницы статуса Nginx на заново развернутом из архива контейнере](/screens/08_final_status_check.png)

## Part 3. Мини веб-сервер

* **Написание и компиляция FastCGI-сервера на C (`server.c`)**
  * Создан файл `server.c` с выводом страницы «Hello, World!».
  * Сборка исходного кода в исполняемый бинарный файл:
    ```bash
    gcc server.c -o server -lfcgi
    ```

* **Запуск сервера через spawn-fcgi**
  * Выдача прав на исполнение бинарника и его запуск на порту 8080:
    ```bash
    chmod +x ./server
    spawn-fcgi -p 8080 ./server
    ```

* **Настройка конфигурации Nginx**
  * Создан файл по пути `./nginx/nginx.conf` со следующим блоком проксирования:
    ```nginx
    server {
        listen 81;
        location / {
            fastcgi_pass 127.0.0.1:8080;
            include /etc/nginx/fastcgi_params;
        }
    }
    ```

* **Запуск Nginx и проверка работоспособности**
  * Остановка системной службы и запуск кастомного конфига по абсолютному пути:
    ```bash
    sudo systemctl stop nginx
    sudo nginx -c \$(pwd)/nginx/nginx.conf
    ```
  * Проверка ответа веб-страницы на 81 порту:
    ```bash
    curl http://localhost:81
    ```

## Part 4. Свой докер

* **Создание стартового скрипта (`run.sh`)**
  * Написан скрипт для одновременного запуска FastCGI-приложения и Nginx на переднем плане:
    ```bash
    #!/bin/bash
    spawn-fcgi -p 8080 ./server
    nginx -g "daemon off;"
    ```

* **Написание инструкции сборки (`Dockerfile`)**
  * Создан `Dockerfile` (базовый образ `nginx:latest`) с объединением команд в один вызов `RUN` для оптимизации слоев:
    ```dockerfile
    FROM nginx:latest
    WORKDIR /home/
    COPY server.c .
    COPY run.sh .
    RUN apt-get update && \
        apt-get install -y gcc libfcgi-dev spawn-fcgi && \
        gcc server.c -o server -lfcgi && \
        chmod +x run.sh
    EXPOSE 80
    CMD ["./run.sh"]
    ```

* **Сборка и проверка докер-образа**
  * Сборка кастомного образа с тегом и проверка его наличия в локальной системе:
    ```bash
    docker build -t my_server:v1 .
    docker images
    ```

* **Настройка конфигурации Nginx на хост-машине**
  * Создан файл по пути `./nginx/nginx.conf` для проксирования на внутренний FastCGI:
    ```nginx
    server {
        listen 80;
        location / {
            fastcgi_pass 127.0.0.1:8080;
            include /etc/nginx/fastcgi_params;
        }
    }
    ```

* **Запуск контейнера с маппингом портов и папки**
  * Запуск контейнера с пробросом 81-го порта на 80-й и точечным монтированием файла конфигурации:
    ```bash
    docker run -d -p 81:80 -v \$(pwd)/nginx/nginx.conf:/etc/nginx/nginx.conf --name fcgi_container my_server:v1
    ```
  * Проверка работы страницы в терминале хоста:
    ```bash
    curl http://localhost:81
    ```

* **Добавление страницы статуса на лету**
  * В локальный файл `./nginx/nginx.conf` внутрь блока `server` добавлен путь `/status`:
    ```nginx
    location /status {
        stub_status on;
    }
    ```
  * Применение изменений путём перезапуска контейнера (без пересборки образа) и итоговая проверка:
    ```bash
    docker restart fcgi_container
    curl http://localhost:81/status
    ```
## Part 5. Dockle

* **Первичное сканирование образа на безопасность**
  * Запуск утилиты `dockle` через технический контейнер для поиска уязвимостей CIS Benchmarks:
    ```bash
    sudo docker run --rm -v /var/run/docker.sock:/var/run/docker.sock goodwithtech/dockle --accept-key NGINX_GPGKEY my_server:v1
    ```
  * Обнаружены критические ошибки (`FATAL`): использование утилиты `sudo`, запуск процессов от пользователя `root` и ложные срабатывания на GPG-ключи Nginx.

* **Обновление конфигурации Nginx (`./nginx/nginx.conf`)**
  * Перевод Nginx на безопасный порт `8080` (для работы без прав root) и изменение временных путей к кэшу:
    ```nginx
    server {
        listen 8080;
        client_body_temp_path /tmp/client_temp;
        proxy_temp_path       /tmp/proxy_temp;
        fastcgi_temp_path     /tmp/fastcgi_temp;

        location /status { stub_status on; }
        location / {
            fastcgi_pass 127.0.0.1:8081;
            include /etc/nginx/fastcgi_params;
        }
    }
    ```

* **Обновление стартового скрипта (`run.sh`)**
  * Удаление команды `sudo` и перевод FastCGI-сервера на непривилегированный порт `8081`:
    ```bash
    #!/bin/bash
    spawn-fcgi -p 8081 ./server
    nginx -g "daemon off;"
    ```

* **Написание безопасного `Dockerfile`**
  * Исключение `sudo`, выдача прав пользователю `nginx` на системные каталоги логов и сброс битов `setuid`/`setgid`:
    ```dockerfile
    FROM nginx:latest
    WORKDIR /home/
    COPY server.c .
    COPY run.sh .
    RUN apt-get update && \
        apt-get install -y gcc libfcgi-dev spawn-fcgi && \
        gcc server.c -o server -lfcgi && \
        chmod +x run.sh && \
        chown -R nginx:nginx /var/cache/nginx /var/log/nginx /etc/nginx /home/ && \
        touch /var/run/nginx.pid && \
        chown nginx:nginx /var/run/nginx.pid && \
        find / -perm /6000 -type f -exec chmod a-s {} \; 2>/dev/null || true && \
        apt-get clean && \
        rm -rf /var/lib/apt/lists/*
    USER nginx
    EXPOSE 8080
    CMD ["./run.sh"]
    ```

* **Пересборка образа и финальная проверка безопасности**
  * Сборка защищённой версии образа `v3`:
    ```bash
    docker build -t my_server:v3 .
    ```
  * Итоговый запуск сканера с игнорированием системных переменных GPG-ключей веб-сервера Nginx:
    ```bash
    sudo docker run --rm -v /var/run/docker.sock:/var/run/docker.sock goodwithtech/dockle --accept-key NGINX_GPGKEYS --accept-key NGINX_GPGKEY_PATH --accept-key NGINX_GPGKEY my_server:v3
    ```
  * Результат проверки: **`PASS`** (0 ошибок `FATAL`, 0 ошибок `WARN`).

## Part 6. Базовый Docker Compose

* **Обновление стартового скрипта приложения (`run.sh`)**
  * Изменён режим запуска FastCGI-сервера на передний план (флаг `-n`) для удержания контейнера:
    ```bash
    #!/bin/bash
    exec spawn-fcgi -p 81 -n ./server
    ```

* **Написание изолированного `Dockerfile`**
  * Создан `Dockerfile` на базе Ubuntu без установки Nginx и без использования инструкции `EXPOSE`:
    ```dockerfile
    FROM ubuntu:24.04
    WORKDIR /home/
    COPY server.c .
    COPY run.sh .
    RUN apt-get update && \
        apt-get install -y gcc libfcgi-dev spawn-fcgi && \
        gcc server.c -o server -lfcgi && \
        chmod +x run.sh && \
        useradd -m -s /bin/bash app_user && \
        chown -R app_user:app_user /home/ && \
        find / -perm /6000 -type f -exec chmod a-s {} \; 2>/dev/null || true && \
        apt-get clean && \
        rm -rf /var/lib/apt/lists/*
    USER app_user
    CMD ["./run.sh"]
    ```

* **Создание конфигурации проксирующего Nginx (`./nginx/nginx.conf`)**
  * Настроен проброс трафика с порта `8080` на порт `81` контейнера приложения `web_app`:
    ```nginx
    server {
        listen 8080;
        location /status { stub_status on; }
        location / {
            fastcgi_pass web_app:81;
            include /etc/nginx/fastcgi_params;
        }
    }
    ```

* **Написание оркестрового сценария (`docker-compose.yml`)**
  * Создан файл конфигурации для связывания многоконтейнерной архитектуры в общую сеть:
    ```yaml
    version: '3.8'
    services:
      web_app:
        build: .
        image: my_fcgi_app:v1
        container_name: fcgi_backend
        restart: always

      nginx_proxy:
        image: nginx:latest
        container_name: nginx_balancer
        ports:
          - "80:8080"
        volumes:
          - ./nginx/nginx.conf:/etc/nginx/nginx.conf
        depends_on:
          - web_app
        restart: always
    ```

* **Очистка системы, сборка и запуск проекта**
  * Остановка всех старых процессов на 80-м порту хоста, сборка образов и запуск инфраструктуры в фоне:
    ```bash
    sudo fuser -k 80/tcp 2>/dev/null
    sudo docker stop \$(sudo docker ps -aq) 2>/dev/null
    sudo docker rm \$(sudo docker ps -aq) 2>/dev/null
    
    docker compose build
    docker compose up -d
    ```

* **Итоговая проверка доступности сервисов**
  * Тестирование ответа FastCGI-сервера и страницы статуса Nginx на внешнем 80-м порту:
    ```bash
    curl http://localhost:80
    curl http://localhost:80/status
    ```
