#include <fcgi_stdio.h>
#include <stdlib.h>

int main(void) {
    // Бесконечный цикл ожидания запросов от веб-сервера
    while (FCGI_Accept() >= 0) {
        // Обязательный заголовок ответа для браузера
        printf("Content-type: text/html\r\n\r\n");
        
        // Тело простейшей HTML-страницы по ТЗ
        printf("<html>\n");
        printf("<head><title>FastCGI Hello World</title></head>\n");
        printf("<body>\n");
        printf("<h1>Hello, World!</h1>\n");
        printf("</body>\n");
        printf("</html>\n");
    }
    return 0;
}
