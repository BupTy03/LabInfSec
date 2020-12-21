#include <stdio.h>
#include <assert.h>


#define MAX_DIGITS_COUNT 19


// Умножение по модулю
int modulo_multiply(int a, int b, int m) { return (a * b) % m; }

// Быстрое возведение в степень по модулю (возвести x в степень n по модулю m)
int modulo_power(int x, int n, int m)
{
    assert(n >= 0);
    if(n == 0)
        return 1;

    while(n % 2 == 0)
    {
        x = modulo_multiply(x, x, m);
        n /= 2;
    }

    int result = x;
    n /= 2;
    while (n != 0)
    {
        x = modulo_multiply(x, x, m);
        if(n % 2 != 0)
            result = modulo_multiply(result, x, m);

        n /= 2;
    }

    return result;
}

// rsa - это возведение message в степень key по модулю n
int rsa(int message, int key, int n) { return modulo_power(message, key, n); }


typedef struct extended_gcd_result_s
{
    int x;   // коэффициент x из теоремы Безу: xa + yb = НОД(a, b)
    int gcd; // НОД(a, b)
} extended_gcd_result;

// Расширенный алгоритм Евклида
extended_gcd_result extended_gcd(int a, int b)
{
    int x0 = 1;
    int x1 = 0;
    while (b != 0)
    {
        const int quotient = a / b;
        const int remainder = a % b;

        const int x2 = x0 - quotient  * x1;
        x0 = x1;
        x1 = x2;
        a = b;
        b = remainder;
    }

    extended_gcd_result r = {x0, a};
    return r;
}

/*
 * Находит число обратное к a по модулю b.
 *
 * Из теоремы Безу, мы знаем, что xa + yb = gcd(a, b)
 * поэтому xa = gcd(a, b) - yb
 *
 * Если gcd(a, b) = 1, то xa = 1 - yb
 *
 * Поэтому произведение x и a равно 1 плюс некоторое кратное b,
 * или, иными словами: xa = 1 mod b
 *
 * Два числа, произведение которых равно 1,
 * являются взаимно обратными относительно умножения.
 *
 * Поскольку a и b - взаимно простые числа и их gcd(a, b) = 1
 * значит x является обратным к a по модулю b.
*/
int multiplicative_inverse(int a, int b)
{
    const extended_gcd_result p = extended_gcd(a, b);

    // если gcd(a, b) != 1 значит числа a и b не взаимно простые
    // в таком случае вернём 0
    if(p.gcd != 1)
        return 0;

    if(p.x < 0)
        return p.x + b;

    return p.x;
}

// Функция Эйлера
int euler(int p, int q) { return (p - 1) * (q - 1); }

/*
 *   Простой и тупой алгоритм хеширования:
 *   1. Берём два числа и перемножаем их (если вторая цифра 0 - берём первую)
 *   2. Берём последнюю цифру от результата
 *   3. Перемножаем на следующую цифру числв (если следующая цифра - не ноль)
 *   4. Повторяем шаги 2 и 3 до последней цивры числа включительно
 */
int stupid_hash(int m)
{
    // m должен содержать минимум 2 цифры
    if(m < 10) return m;

    // заполняем массив в виде:
    // digits = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3] (для m = 123)
    int digits[MAX_DIGITS_COUNT] = {0};
    int d = MAX_DIGITS_COUNT - 1;
    while (m > 0)
    {
        digits[d] = (m % 10);

        if(digits[d] == 0) digits[d] = 1;

        m /= 10;
        --d;
    }

    // перемещаем индекс d к первой цифре числа
    ++d;

    int result = 1;
    for (; d < MAX_DIGITS_COUNT; ++d)
    {
        result *= digits[d];

        const int firstDigit = result / 10;
        const int secondDigit = result % 10;

        result = (secondDigit == 0) ? firstDigit : secondDigit;
    }

    return result;
}

// Схема шифрования 1
void scheme1(int e, int d, int n, int M)
{
    const int C1 = rsa(M, e, n);
    const int M1 = rsa(C1, d, n);

    printf("C1=%d, M1=%d\n", C1, M1);
}

// Схема шифрования 2
void scheme2(int e, int d, int n, int M)
{
    const int C2 = rsa(M, d, n);
    const int M1 = rsa(C2, e, n);

    printf("C2=%d, M1=%d\n", C2, M1);
}

void task_1_and_2()
{
    puts("------------- tasks #1 and #2 --------------");

    const int p = 3;
    const int q = 11;
    const int d = 7;
    const int M = 14;

    const int n = p * q;
    const int z = euler(p, q);
    const int e = multiplicative_inverse(d, z);

    if(e == 0)
    {
        printf("Error: Unable to find e for d=%d\n", d);
        return;
    }

    printf("p=%d, q=%d, d=%d, M=%d, n=%d, z=%d, e=%d\n", p, q, d, M, n, z, e);

    printf("task1: ");
    scheme1(e, d, n, M);

    printf("task2: ");
    scheme2(e, d, n, M);
}

void task3()
{
    puts("----------------- task #3 -----------------");

    const int M = 947829;
    const int p = 7;
    const int q = 11;
    const int d = 17;

    const int n = p * q;
    const int z = euler(p, q);
    const int e = multiplicative_inverse(d, z);
    if(e == 0)
    {
        printf("Error: Unable to find e for d=%d\n", d);
        return;
    }

    printf("M=%d, p=%d, q=%d, e=%d, d=%d, n=%d, z=%d\n", M, p, q, e, d, n, z);

    const int h = stupid_hash(M);
    printf("hash=%d\n", h);

    const int D = rsa(h, d, n);
    printf("D=%d\n", D);

    const int decD = rsa(D, e, n);
    printf("Decrypted D=%d\n", decD);
    printf("Decrypted D = hash, %d = %d\n", decD, stupid_hash(M));
}

void task4(int alpha, int Xa, int Xb, int q)
{
    puts("---------------- task #4 -----------------");
    printf("alpha=%d, q=%d, Xa=%d, Xb=%d\n", alpha, q, Xa, Xb);
    const int Ya = modulo_power(alpha, Xa, q);
    const int Yb = modulo_power(alpha, Xb, q);
    printf("Ya=%d, Yb=%d\n", Ya, Yb);

    const int Ka = modulo_power(Yb, Xa, q);
    const int Kb = modulo_power(Ya, Xb, q);
    printf("Ka = Kb, %d = %d\n", Ka, Kb);
}

void task5(int alpha, int Xa, int Xb, int Xz, int q)
{
    puts("---------------- task #5 -----------------");
    printf("alpha=%d, q=%d, Xa=%d, Xb=%d, Xz=%d\n", alpha, q, Xa, Xb, Xz);
    const int Ya = modulo_power(alpha, Xa, q);
    const int Yb = modulo_power(alpha, Xb, q);
    const int Yz = modulo_power(alpha, Xz, q);
    printf("Ya=%d, Yb=%d, Yz=%d\n", Ya, Yb, Yz);

    const int Ka = modulo_power(Yz, Xa, q);
    const int Kza = modulo_power(Ya, Xz, q);
    printf("Ka = Kza, %d = %d\n", Ka, Kza);

    const int Kb = modulo_power(Yz, Xb, q);
    const int Kzb = modulo_power(Yb, Xz, q);
    printf("Kb = Kzb, %d = %d\n", Kb, Kzb);
}

// Лабораторная работа 1
void lab_rab1()
{
    task_1_and_2();
    puts("\n");
    task3();
}

// Лабораторная работа 2
void lab_rab2()
{
    const int alpha = 7;
    const int q = 71;
    const int Xa = 5;
    const int Xb = 12;
    const int Xz = 3;

    task4(alpha, Xa, Xb, q);
    puts("\n");
    task5(alpha, Xa, Xb, Xz, q);
}


int main()
{
    lab_rab1();
    puts("\n");
    lab_rab2();
    return 0;
}
