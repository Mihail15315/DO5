## Инструкция по проверке и запуску скриптов (Part 1 - Part 3)

### Part 1. Генератор файлов

* **Запуск скрипта создания папок и файлов**
  * Передача 6 обязательных параметров (путь, папки, маска папок, файлы, маска файлов, размер):
    ```bash
    ./main.sh /home/m/test_folder 4 abc 5 def 10kb
    ```

* **Проверка результатов для проверяющего**
  * Просмотр созданной структуры каталогов:
    ```bash
    ls -la /home/m/test_folder
    ```
  * Просмотр автоматически созданного текстового журнала генерации:
    ```bash
    cat *.log
    ```

### Part 2. Засорение файловой системы

* **Запуск скрипта штормовой атаки диска**
  * Фиксация объема свободного места до начала теста:
    ```bash
    df -h /
    ```
  * Запуск генератора тяжелых файлов (маски имен, маска файлов, размер строго в Mb):
    ```bash
    ./main.sh az az.txt 100Mb
    ```

* **Проверка лимитов для проверяющего**
  * Повторная проверка дискового пространства после завершения/остановки скрипта:
    ```bash
    df -h /
    ```
  * *Ожидаемый результат:* Доступное место (`Avail`) на диске уменьшилось до защитного лимита в **1.0G** (или упёрлось в квоту пользователя).
  * Вывод сформированного лога сессии засорения:
    ```bash
    cat garbage_collector.log
    ```

### Part 3. Очистка файловой системы

* **Способ 1. Удаление по лог-файлу**
  * Запуск утилиты и ввод абсолютного пути к журналу из Part 2 (скрипт удалит мусор точечно):
    ```bash
    ./main.sh 1
    # При запросе пути ввести: /home/m/04/garbage_collector.log
    ```

* **Способ 2. Удаление по дате и времени создания**
  * Запуск утилиты и ввод временного интервала, в который проводился тест засорения:
    ```bash
    ./main.sh 2
    # При запросе ввести промежуток, например:
    # Время начала: 2026-07-20 14:00
    # Время конца: 2026-07-20 14:15
    ```

* **Способ 3. Удаление по маске (имени и дате)**
  * Запуск утилиты и ввод уникального сквозного ключа букв и текущей даты (`ДДММГГ`):
    ```bash
    ./main.sh 3
    # При запросе маски ввести: az_200726
    ```

* **Итоговая проверка восстановления системы**
  * Подтверждение проверяющему, что всё мусорное пространство полностью очищено, а свободный объем вернулся к исходным показателям:
    ```bash
    df -h /
    ```
## Инструкция по проверке и запуску скриптов (Part 4 - Part 6)

### Part 4. Генератор логов

* **Запуск скрипта генерации логов Nginx**
  * Скрипт создаёт 5 файлов логов в формате Combined с фейковыми данными (коды ответов, IP, методы, даты):
    ```bash
    ./main.sh
    ```

* **Проверка результатов для проверяющего**
  * Проверка наличия сгенерированных файлов в текущей директории:
    ```bash
    ls -la nginx_day_*.log
    ```
  * Просмотр структуры записей (каждая строка должна содержать валидный IP, дату, HTTP-метод, URL, код ответа и User-Agent):
    ```bash
    head -n 5 nginx_day_1.log
    ```

### Part 5. Мониторинг

* **Запуск и проверка сбора метрик через Prometheus**
  * Скрипт-парсер из этой части анализирует логи из Part 4 по заданному параметру (`1` — все записи с кодом 2xx, `2` — уникальные IP, `3` — запросы с ошибками и т.д.):
    ```bash
    ./main.sh 3
    ```
  * *Ожидаемый результат:* Вывод в консоль отфильтрованных данных согласно выбранному режиму.

* **Проверка доступности веб-интерфейсов для защиты**
  * Продемонстрировать проверяющему, что службы мониторинга запущены и собирают системные метрики:
    ```bash
    # Проверка работы Prometheus (метрики, таргеты)
    curl -s http://localhost:9090/api/v1/targets | jq '.data.activeTargets[].job'

    # Проверка работы Node Exporter (сырые метрики железа)
    curl -s http://localhost:9100/metrics | head -n 5
    ```
  * *Демонстрация Grafana:* Открыть в браузере `http://localhost:3000` и показать живой дашборд (например, ID 795), где графики CPU, RAM и диска меняются в реальном времени.

### Part 6. GoAccess

* **Запуск анализатора логов GoAccess в терминале**
  * Запуск утилиты для интерактивного анализа одного из сгенерированных в Part 4 лог-файлов прямо в консоли:
    ```bash
    goaccess nginx_day_1.log --log-format=COMBINED
    ```
  * *Ожидаемый результат:* Откроется псевдографический интерфейс со сводной статистикой по кодам ответов, трафику и посетителям. Выход из режима — клавиша `q`.

* **Генерация красивого HTML-отчёта**
  * Сборка сквозной аналитики по всем 5 лог-файлам разом в одну веб-страницу:
    ```bash
    cat nginx_day_*.log | goaccess - --log-format=COMBINED -o report.html
    ```

* **Проверка результата для проверяющего**
  * Открыть созданный файл `report.html` в браузере хост-машины (через проброс портов Nginx или просто кликнув по файлу).
  * *Ожидаемый результат:* Интерактивная веб-панель с графиками, диаграммами и детальным распределением HTTP-ошибок и уникальных хостов.

## Part 7. Prometheus и Grafana

* **Настройка сбора метрик в Prometheus (`/etc/prometheus/prometheus.yml`)**
  * В конфигурационный файл добавлены задания (`jobs`) для мониторинга самого Prometheus и хост-машины через Node Exporter:
    ```yaml
    scrape_configs:
      - job_name: 'prometheus'
        static_configs:
          - targets: ['localhost:9090']

      - job_name: 'node-exporter'
        static_configs:
          - targets: ['localhost:9100']
    ```
  * Перезапуск службы для применения настроек:
    ```bash
    sudo systemctl restart prometheus
    ```

* **Проверка активности целей (Targets) через API**
  * Запрос к Prometheus для подтверждения статуса `up` у всех настроенных джобов:
    ```bash
    curl -s http://localhost:9090/api/v1/targets | jq '.data.activeTargets[] | {job, health}'
    ```

* **Интеграция Grafana и запуск дашбордов**
  * Проверка связи Grafana с базой данных Prometheus (Data Source настроен по адресу `http://localhost:9090`).
  * Импортирован официальный дашборд (Node Exporter Full, ID: `1860` или `11074`) для визуализации системных метрик.

* **Анализ базовых метрик в Grafana (PromQL)**
  * **ЦПУ (CPU):** Построение графика текущей утилизации процессора:
    ```text
    avg_over_time(node_cpu_seconds_total{mode!="idle"}[5m])
    ```
  * **Оперативная память (RAM):** Расчёт занятого объёма памяти в байтах:
    ```text
    node_memory_MemTotal_bytes - node_memory_MemFree_bytes - node_memory_Buffers_bytes - node_memory_Cached_bytes
    ```
  * **Жесткий диск (Disk Space):** Расчёт свободного места на корневом разделе `/`:
    ```text
    node_filesystem_free_bytes{mountpoint="/"}
    ```

* **Проведение стресс-теста для демонстрации живых графиков**
  * Запуск утилиты `stress` для искусственной загрузки процессора и оперативной памяти на 20 секунд:
    ```bash
    stress -c 2 -m 1 --vm-bytes 128M -t 20s
    ```
  * *Ожидаемый результат для проверяющего:* В интерфейсе Grafana на графиках CPU наблюдается резкий пик утилизации до 100%, а график свободной памяти RAM пропорционально падает вниз.

  ## Part 9. Свой node_exporter

* **Создание Bash-скрипта экспортера (`my_exporter.sh`)**
  * Разработан скрипт для циклического сбора реальных системных метрик (каждые 3 секунды) и их вывода в официальном текстовом формате Prometheus:
    ```bash
    #!/bin/bash
    OUTPUT_FILE="/var/www/html/metrics.txt"
    while true; do
        CPU_USAGE=\$(awk '/cpu / {print 100 - (\$5*100/(\$2+\$3+\$4+\$5+\$6+\$7+\$8))}' /proc/stat)
        RAM_FREE=\$(free -b | awk '/Mem:/ {print \$4}')
        DISK_FREE=\$(df -B1 / | awk 'NR==2 {print \$4}')

        sudo tee "\$OUTPUT_FILE" << EOF > /dev/null
    # HELP my_cpu_usage_percent Current CPU usage in percent.
    # TYPE my_cpu_usage_percent gauge
    my_cpu_usage_percent \$CPU_USAGE
    # HELP my_ram_free_bytes Free RAM size in bytes.
    # TYPE my_ram_free_bytes gauge
    my_ram_free_bytes \$RAM_FREE
    # HELP my_disk_free_bytes Free root filesystem space in bytes.
    # TYPE my_disk_free_bytes gauge
    my_disk_free_bytes \$DISK_FREE
    EOF
        sleep 3
    done
    ```
  * Выдача прав и запуск скрипта в фоновом режиме:
    ```bash
    chmod +x my_exporter.sh
    sudo ./my_exporter.sh &
    ```

* **Настройка Nginx для отдачи кастомных метрик (`/etc/nginx/sites-available/custom_exporter`)**
  * Создан файл конфигурации виртуального хоста на порту `9101` для трансляции plain-text файла:
    ```nginx
    server {
        listen 9101;
        server_name localhost;
        location /metrics {
            root /var/www/html;
            default_type text/plain;
            try_files /metrics.txt =404;
        }
    }
    ```
  * Активация конфигурации и перезапуск веб-сервера:
    ```bash
    sudo ln -sf /etc/nginx/sites-available/custom_exporter /etc/nginx/sites-enabled/
    sudo systemctl restart nginx
    ```

* **Добавление кастомного джоба в Prometheus (`/etc/prometheus/prometheus.yml`)**
  * В секцию `scrape_configs` добавлен блок опроса нашего собственного экспортера с интервалом в 3 секунды:
    ```yaml
      - job_name: 'my_own_node_exporter'
        scrape_interval: 3s
        static_configs:
          - targets: ['localhost:9101']
    ```
  * Применение изменений: `sudo systemctl restart prometheus`

* **Контрольная проверка сбора данных**
  * Проверка доступности сырого текста метрик через Nginx:
    ```bash
    curl http://localhost:9101/metrics
    ```
  * Проверка успешного подключения новой цели со статусом `up` в Prometheus:
    ```bash
    curl -s http://localhost:9090/api/v1/targets | jq '.data.activeTargets[] | select(.job=="my_own_node_exporter") | {job, health}'
    ```

* **Отображение кастомных метрик в Grafana**
  * Создан новый дашборд, где для вывода графиков используются прямые PromQL-запросы к метрикам нашего скрипта:
    * Панель CPU: `my_cpu_usage_percent` (Unit: `Percent`)
    * Панель RAM: `my_ram_free_bytes` (Unit: `Data -> Bytes (IEC)`)
    * Панель Disk: `my_disk_free_bytes` (Unit: `Data -> Bytes (IEC)`)
