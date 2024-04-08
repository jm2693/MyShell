// consumer.c
#include <stdio.h>

int main() {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stdin)) {
        printf("Consumer received: %s", buffer);
    }
    return 0;
}
