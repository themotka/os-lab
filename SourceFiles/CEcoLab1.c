/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов CEcoLab1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoLab1.h"

/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoLab1
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoLab1_QueryInterface(/* in */ IEcoLab1Ptr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    /* Проверка указателей */
    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoLab1) ) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoLab1
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_AddRef(/* in */ IEcoLab1Ptr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoLab1
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_Release(/* in */ IEcoLab1Ptr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    /* Уменьшение счетчика ссылок на компонент */
    --pCMe->m_cRef;

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoLab1((IEcoLab1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция MyFunction
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoLab1_MyFunction(/* in */ IEcoLab1Ptr_t me, /* in */ char_t* Name, /* out */ char_t** copyName) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    int16_t index = 0;

    /* Проверка указателей */
    if (me == 0 || Name == 0 || copyName == 0) {
        return ERR_ECO_POINTER;
    }

    /* Копирование строки */
    while(Name[index] != 0) {
        index++;
    }
    pCMe->m_Name = (char_t*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, index + 1);
    index = 0;
    while(Name[index] != 0) {
        pCMe->m_Name[index] = Name[index];
        index++;
    }
    *copyName = pCMe->m_Name;

    return ERR_ECO_SUCCESES;
}

/* Математические приближения без libc */
static long double CEcoLab1_abs_ld(long double x) {
    return x < 0.0L ? -x : x;
}

static long double CEcoLab1_exp_ld(long double y) {
    /* Разложение в ряд Тейлора с простым масштабированием */
    int negate = 0;
    long double x = y;
    int k = 0;
    long double term = 1.0L;
    long double sum = 1.0L;
    int n;
    if (x < 0.0L) { negate = 1; x = -x; }
    while (x > 1.0L) { x *= 0.5L; k++; }
    for (n = 1; n < 60; ++n) {
        term = term * x / (long double)n;
        sum += term;
        if (CEcoLab1_abs_ld(term) < 1e-20L) break;
    }
    while (k-- > 0) {
        sum = sum * sum;
    }
    if (negate) {
        return 1.0L / sum;
    }
    return sum;
}

static long double CEcoLab1_ln_ld(long double a) {
    static const long double LN2 = 0.69314718055994530941723212145818L;
    int k = 0;
    long double x = a;
    long double z;
    long double z2;
    long double term;
    long double sum;
    int n;
    if (a <= 0.0L) {
        return -1.0e300L;
    }
    while (x > 2.0L) { x *= 0.5L; k++; }
    while (x < 0.5L) { x *= 2.0L; k--; }
    z = (x - 1.0L) / (x + 1.0L);
    z2 = z * z;
    term = z;
    sum = 0.0L;
    for (n = 0; n < 200; ++n) {
        long double denom = (long double)(2*n + 1);
        sum += term / denom;
        term *= z2;
        if (CEcoLab1_abs_ld(term/denom) < 1e-20L) break;
    }
    return 2.0L * sum + (long double)k * LN2;
}

static long double CEcoLab1_pow_ld(long double a, long double b) {
    long double ln;
    if (a <= 0.0L) {
        /* ограниченная поддержка: положительное основание */
        return 0.0L;
    }
    ln = CEcoLab1_ln_ld(a);
    return CEcoLab1_exp_ld(b * ln);
}

static long double CEcoLab1_sqrt_ld(long double x) {
    long double r;
    int i;
    if (x <= 0.0L) return 0.0L;
    r = x;
    for (i = 0; i < 40; ++i) {
        r = 0.5L * (r + x / r);
    }
    return r;
}

static long double CEcoLab1_gamma_stirling_ld(long double z) {
    /* Модифицированная формула Стирлинга с поправками */
    static const long double SQRT_TWO_PI = 2.5066282746310005024157652848110L; /* sqrt(2*pi) */
    long double series;
    long double t1;
    long double t2;
    series = 1.0L + 1.0L/(12.0L*z) + 1.0L/(288.0L*z*z)
        - 139.0L/(51840.0L*z*z*z) - 571.0L/(2488320.0L*z*z*z*z);
    t1 = CEcoLab1_pow_ld(z, z - 0.5L);
    t2 = CEcoLab1_exp_ld(-z);
    return SQRT_TWO_PI * t1 * t2 * series;
}

static long double CEcoLab1_gamma_lanczos_ld(long double x) {
    static const long double COEF[9] = {
        0.99999999999980993L,
        676.5203681218851L,
        -1259.1392167224028L,
        771.32342877765313L,
        -176.61502916214059L,
        12.507343278686905L,
        -0.13857109526572012L,
        0.0000099843695780195716L,
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

/* Публичные функции вычисления гамма-функции */
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

static int16_t ECOCALLMETHOD CEcoLab1_gamma_longdouble(IEcoLab1Ptr_t me, long double x, long double* out) {
    if (me == 0 || out == 0) {
        return ERR_ECO_POINTER;
    }
    if (x <= 0.0L) {
        return -2;
    }
    *out = CEcoLab1_gamma_ld(x);
    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция Init
 * </сводка>
 *
 * <описание>
 *   Функция инициализации экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD initCEcoLab1(/*in*/ IEcoLab1Ptr_t me, /* in */ struct IEcoUnknown *pIUnkSystem) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    IEcoInterfaceBus1* pIBus = 0;
    int16_t result = -1;

    /* Проверка указателей */
    if (me == 0 ) {
        return result;
    }

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pCMe->m_pISys->pVTbl->QueryInterface(pCMe->m_pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

    /* Проверка указателей */
    if (me == 0 ) {
        return result;
    }

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;

    /* Освобождение */
    pIBus->pVTbl->Release(pIBus);

    return result;
}

/* Create Virtual Table IEcoLab1 */
IEcoLab1VTbl g_x277FC00C35624096AFCFC125B94EEC90VTbl = {
    CEcoLab1_QueryInterface,
    CEcoLab1_AddRef,
    CEcoLab1_Release,
    CEcoLab1_MyFunction,
    CEcoLab1_gamma_float,
    CEcoLab1_gamma_double,
    CEcoLab1_gamma_longdouble
};

/*
 *
 * <сводка>
 *   Функция Create
 * </сводка>
 *
 * <описание>
 *   Функция создания экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD createCEcoLab1(/* in */ IEcoUnknown* pIUnkSystem, /* in */ IEcoUnknown* pIUnkOuter, /* out */ IEcoLab1** ppIEcoLab1) {
    int16_t result = -1;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    CEcoLab1* pCMe = 0;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    /* Проверка указателей */
    if (ppIEcoLab1 == 0 || pIUnkSystem == 0) {
        return result;
    }

    /* Получение системного интерфейса приложения */
    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void **)&pISys);

    /* Проверка */
    if (result != 0 && pISys == 0) {
        return result;
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

    /* Получение идентификатора компонента для работы с памятью */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Получение интерфейса распределителя памяти */
    pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 && pIMem == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
        pISys->pVTbl->Release(pISys);
        return result;
    }

    /* Выделение памяти для данных экземпляра */
    pCMe = (CEcoLab1*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoLab1));

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = pISys;

    /* Сохранение указателя на интерфейс для работы с памятью */
    pCMe->m_pIMem = pIMem;

    /* Установка счетчика ссылок на компонент */
    pCMe->m_cRef = 1;

    /* Создание таблицы функций интерфейса IEcoLab1 */
    pCMe->m_pVTblIEcoLab1 = &g_x277FC00C35624096AFCFC125B94EEC90VTbl;

    /* Инициализация данных */
    pCMe->m_Name = 0;

    /* Возврат указателя на интерфейс */
    *ppIEcoLab1 = (IEcoLab1*)pCMe;

    /* Освобождение */
    pIBus->pVTbl->Release(pIBus);

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Delete
 * </сводка>
 *
 * <описание>
 *   Функция освобождения экземпляра
 * </описание>
 *
 */
void ECOCALLMETHOD deleteCEcoLab1(/* in */ IEcoLab1* pIEcoLab1) {
    CEcoLab1* pCMe = (CEcoLab1*)pIEcoLab1;
    IEcoMemoryAllocator1* pIMem = 0;

    if (pIEcoLab1 != 0 ) {
        pIMem = pCMe->m_pIMem;
        /* Освобождение */
        if ( pCMe->m_Name != 0 ) {
            pIMem->pVTbl->Free(pIMem, pCMe->m_Name);
        }
        if ( pCMe->m_pISys != 0 ) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}