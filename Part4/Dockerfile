# Используем официальный готовый образ Nginx как базовый
FROM nginx:latest

# Указываем рабочую директорию внутри контейнера
WORKDIR /home/

# Копируем исходный код сервера и стартовый скрипт внутрь контейнера
COPY server.c .
COPY run.sh .

# Один оптимизированный вызов RUN для сборки системы
RUN apt-get update && \
    apt-get install -y gcc libfcgi-dev spawn-fcgi && \
    gcc server.c -o server -lfcgi && \
    chmod +x run.sh && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Открываем порт 80 контейнера наружу
EXPOSE 80

# Главная команда при старте контейнера — запуск нашего скрипта
CMD ["./run.sh"]

