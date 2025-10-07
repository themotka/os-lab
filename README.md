### 1. ЗАДАЧА ПРОЕКТА

Задача проекта — реализация вычисления гамма‑функции Γ(x) для различных числовых типов в рамках компонентной архитектуры.

Цели проекта:
- Реализация алгоритмов вычисления Γ(x) без использования стандартной библиотеки C (libc) и сторонних библиотек.
- Поддержка типов: float, double, long double.
- Интеграция вычислений в компонентную систему EcoSystem через интерфейс `IEcoLab1`.
- Комплексное тестирование на характерных значениях (целые, полуцелые, большие целые).

### 2. ОПИСАНИЕ ГАММА-ФУНКЦИИ И ЕЕ РЕАЛИЗАЦИЯ

Гамма‑функция Γ(x) обобщает факториал: для целых n>0, Γ(n)= (n−1)!. Для полуцелых значений: Γ(1/2)=√π, Γ(3/2)= 1/2·√π, и т.д.

В проекте применены:
- Нормализация аргумента для устойчивости: поднятие x до диапазона [1, ∞) по тождеству Γ(v) = Γ(v+1)/v.
- Аппроксимация Ланцоша (g=7, 9 коэф.) для точного вычисления Γ(x) на [1, ∞).
- Собственные реализации элементарных функций без libc: exp, ln, pow, sqrt (ряды и итерации Ньютона), в варианте, совместимом с C89.

Принцип работы алгоритма:
1. Если x < 1, поднимаем x до v ≥ 1, накапливая произведение acc *= v и увеличивая v на 1; далее Γ(x) = Γ(v) / acc.
2. Вычисляем Γ(v) по аппроксимации Ланцоша:
   - z = v − 1
   - t = z + g + 1/2
   - Γ(v) ≈ √(2π) · t^(z+1/2) · e^(−t) · Σ c_i/(z+i), i=0..N
3. Итого Γ(x) = Γ(v) / acc.

Реализация в проекте:

```startLine:endLine:SourceFiles/CEcoLab1.c
static long double CEcoLab1_gamma_lanczos_ld(long double x) {
    static const long double COEF[9] = {
        0.99999999999980993L, 676.5203681218851L, -1259.1392167224028L,
        771.32342877765313L, -176.61502916214059L, 12.507343278686905L,
        -0.13857109526572012L, 0.0000099843695780195716L,
        0.00000015056327351493116L
    };
    static const long double G = 7.0L;
    static const long double SQRT_TWO_PI = 2.5066282746310005024157652848110L;
    long double z;
    long double sum;
    long double t;
    int i;
    z = x - 1.0L;
    sum = COEF[0];
    for (i = 1; i < 9; ++i) {
        sum += COEF[i] / (z + (long double)i);
    }
    t = z + G + 0.5L;
    return SQRT_TWO_PI * CEcoLab1_pow_ld(t, z + 0.5L) * CEcoLab1_exp_ld(-t) * sum;
}
```

```startLine:endLine:SourceFiles/CEcoLab1.c
static long double CEcoLab1_gamma_ld(long double x) {
    long double acc;
    long double v;
    if (x <= 0.0L) {
        return 0.0L;
    }
    acc = 1.0L;
    v = x;
    while (v < 1.0L) {
        acc *= v;
        v += 1.0L;
    }
    return CEcoLab1_gamma_lanczos_ld(v) / acc;
}
```

Публичный интерфейс для типов:

```startLine:endLine:SharedFiles/IEcoLab1.h
/* Вычисление гамма-функции для разных типов */
int16_t (ECOCALLMETHOD *gamma_float)(IEcoLab1Ptr_t me, float x, float* result);
int16_t (ECOCALLMETHOD *gamma_double)(IEcoLab1Ptr_t me, double x, double* result);
int16_t (ECOCALLMETHOD *gamma_longdouble)(IEcoLab1Ptr_t me, long double x, long double* result);
```

```startLine:endLine:SourceFiles/CEcoLab1.c
static int16_t ECOCALLMETHOD CEcoLab1_gamma_double(IEcoLab1Ptr_t me, double x, double* out) {
    if (me == 0 || out == 0) {
        return ERR_ECO_POINTER;
    }
    if (x <= 0.0) {
        return -2;
    }
    *out = (double)CEcoLab1_gamma_ld((long double)x);
    return ERR_ECO_SUCCESES;
}
```

### 3. АСИМПТОТИКА ВЫЧИСЛЕНИЯ

- Ланцош (фиксированное число коэффициентов): O(1) по времени, O(1) по памяти на один вызов, т.к. выполняется постоянное количество операций.
- Нормализация (поднятие до v≥1): O(k), где k — число шагов подъёма (для практических x обычно 0 или 1).
- Элементарные функции exp/ln/pow/sqrt реализованы по рядам/Ньютону с ограниченным числом итераций: амортизированно O(1).

Итог: один вызов Γ(x) имеет константную трудоёмкость при фиксированных параметрах аппроксимации.

### 4. РЕАЛИЗАЦИЯ И ДЕТАЛИ КОДА

Интерфейс компонента:

```startLine:endLine:SharedFiles/IEcoLab1.h
typedef struct IEcoLab1VTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoLab1Ptr_t me, const UGUID* riid, voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoLab1Ptr_t me);
    uint32_t (ECOCALLMETHOD *Release)(IEcoLab1Ptr_t me);
    int16_t (ECOCALLMETHOD *MyFunction)(IEcoLab1Ptr_t me, char_t* Name, char_t** CopyName);

    /* Вычисление гамма-функции */
    int16_t (ECOCALLMETHOD *gamma_float)(IEcoLab1Ptr_t me, float x, float* result);
    int16_t (ECOCALLMETHOD *gamma_double)(IEcoLab1Ptr_t me, double x, double* result);
    int16_t (ECOCALLMETHOD *gamma_longdouble)(IEcoLab1Ptr_t me, long double x, long double* result);
} IEcoLab1VTbl, *IEcoLab1VTblPtr;
```

Таблица виртуальных функций:

```startLine:endLine:SourceFiles/CEcoLab1.c
IEcoLab1VTbl g_x277FC00C35624096AFCFC125B94EEC90VTbl = {
    CEcoLab1_QueryInterface,
    CEcoLab1_AddRef,
    CEcoLab1_Release,
    CEcoLab1_MyFunction,
    CEcoLab1_gamma_float,
    CEcoLab1_gamma_double,
    CEcoLab1_gamma_longdouble
};
```

Фрагмент публичного API для float:

```startLine:endLine:SourceFiles/CEcoLab1.c
static int16_t ECOCALLMETHOD CEcoLab1_gamma_float(IEcoLab1Ptr_t me, float x, float* out) {
    if (me == 0 || out == 0) {
        return ERR_ECO_POINTER;
    }
    if (x <= 0.0f) {
        return -2;
    }
    *out = (float)CEcoLab1_gamma_ld((long double)x);
    return ERR_ECO_SUCCESES;
}
```

Система тестирования (пример сравнения с допуском, double):

```startLine:endLine:UnitTestFiles/SourceFiles/EcoLab1.c
static int approx_equal_double(double a, double b, double eps) {
    double d = a - b;
    if (d < 0.0) d = -d;
    return d <= eps * (b >= 0.0 ? (b + 1.0) : (-b + 1.0));
}

/* Пример тестирования */
{
    double out = 0.0;
    double expected = 362880.0; /* 9! */
    pIEcoLab1->pVTbl->gamma_double(pIEcoLab1, 10.0, &out);
    printf("Gamma(10.0) = %.2f | expected 362880.0 -> %s\n",
        out, approx_equal_double(out, expected, 1e-12) ? "OK" : "FAIL");
}
```

Проверка характерных значений в тестах:
- Целые: Γ(1)=1, Γ(2)=1, Γ(3)=2, Γ(6)=120, Γ(12)=39916800, Γ(10)=362880.
- Полуцелые: Γ(0.5)=√π≈1.77245…, Γ(1.5)=0.5√π≈0.8862269…
- Типы: float/double/long double.

Дополнительно:
- Реализации `CEcoLab1_exp_ld`, `CEcoLab1_ln_ld`, `CEcoLab1_pow_ld`, `CEcoLab1_sqrt_ld` написаны без libc.
- Для отрицательных/некорректных аргументов возвращается код ошибки, а значение не вычисляется.

### 3. ВЫВОД
<img width="780" height="504" alt="image" src="https://github.com/user-attachments/assets/4724e385-2e1b-4965-a294-96bbfd1906c0" />

