FROM nginx:latest

WORKDIR /home/

COPY server.c .
COPY run.sh .

# Ставим пакеты, собираем сервер и настраиваем права
RUN apt-get update && \
    apt-get install -y gcc libfcgi-dev spawn-fcgi && \
    gcc server.c -o server -lfcgi && \
    chmod +x run.sh && \
    # Даем пользователю nginx права на управление логами и кэшем Nginx
    chown -R nginx:nginx /var/cache/nginx /var/log/nginx /etc/nginx /home/ && \
    touch /var/run/nginx.pid && \
    chown nginx:nginx /var/run/nginx.pid && \
    # Безопасность CIS: убираем s-биты
    find / -perm /6000 -type f -exec chmod a-s {} \; 2>/dev/null || true && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Работаем от официального встроенного пользователя nginx (без root прав)
USER nginx

EXPOSE 8080

CMD ["./run.sh"]

