#include <stdio.h>

enum { wine = 2, bear = 4, vodka = 7 };
const char* error_message = "Invalid input\n";
const char* death_message = "YOU DIED\n";

int main()
{
    int health;

    health = 50;

    while (health > 0) {
        int user_input;
        printf("Welcome to our bar!\n");
        printf("Your current health: %d\n", health);
        printf("1 - Wine: %d\n2 - Bear: %d\n3 - Vodka: %d\n", wine, bear, vodka);
        scanf("%d", &user_input);

        if (user_input == 1) {
            health -= wine;
        } else if (user_input == 2) {
            health -= bear;
        } else if (user_input == 3) {
            health -= vodka;
        } else {
            perror(error_message);
        }
    }
    printf("%s",death_message);
    return 0;
}
