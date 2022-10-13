
typedef int HttpHandler(char *buffer);

int RunWebServer(HttpHandler *handler);