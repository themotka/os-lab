# Реализация алгоритма «Next Fit» (следующего подходящего) выделения участков памяти по запросу

## 1. Описание алгоритма

### Общая концепция

**Next Fit**  — это алгоритм выделения памяти, который является модификацией алгоритма First Fit. Основное отличие заключается в том, что поиск свободного блока начинается не с начала кучи, а с места последнего успешного выделения памяти.

### Принцип работы

1. **Инициализация**: При первом выделении памяти поиск начинается с начала кучи.
2. **Последующее выделение**: Каждое следующее выделение начинается с места последнего успешного выделения.
3. **Циклический поиск**: Если с текущей позиции не найдено подходящего места, поиск продолжается по кругу до возврата к начальной позиции.
4. **Освобождение**: При освобождении блока он помечается как свободный, но указатель последнего выделения не изменяется.

---

## 2. Реализация алгоритма

### Архитектура решения

Алгоритм реализован с использованием следующих компонентов:

1. **Структура MemChunk** — описывает блок памяти
2. **Связанный список блоков** — для отслеживания выделенных блоков
3. **Пул чанков** — предвыделенный массив структур для метаданных
4. **Указатель последнего выделения** — для реализации Next Fit

---

## 3. Структуры данных

### MemChunk

```c
typedef struct MemChunk {
    char_t* pBegin;      /* Начало блока памяти */
    char_t* pEnd;        /* Конец блока памяти */
    bool_t bInUse;       /* Флаг использования (1 = занят, 0 = свободен) */
    struct MemChunk* pNext;  /* Указатель на следующий блок в списке */
    struct MemChunk* pPrev;  /* Указатель на предыдущий блок в списке */
} MemChunk;
```

### Структура менеджера памяти

```c
typedef struct CEcoMemoryManager1Lab_623E1838 {
    /* другие поля */
    
    /* Данные для Next Fit */
    char_t* m_pMemBase;           /* Начало доступной памяти */
    char_t* m_pMemLimit;          /* Конец доступной памяти */
    MemChunk* m_pChunkList;       /* Голова связанного списка блоков */
    MemChunk* m_pLastFit;         /* Указатель на последний выделенный блок */
    MemChunk* m_pChunkPool;       /* Пул предвыделенных структур MemChunk */
    uint32_t m_uChunkPoolSize;    /* Размер пула чанков */
} CEcoMemoryManager1Lab_623E1838;
```

---

## 4. Примеры кода

### Инициализация менеджера памяти

```c
static int16_t ECOCALLMETHOD CEcoMemoryManager1Lab_623E1838_Init(
    /* in */ IEcoMemoryManager1Ptr_t me, 
    /* in */ voidptr_t startAddress, 
    /* in */ uint32_t size) {
    
    CEcoMemoryManager1Lab_623E1838* pCMe = (CEcoMemoryManager1Lab_623E1838*)me;
    uint32_t uChunkPoolSize;
    uint32_t uChunkPoolMemSize;
    MemChunk* pChunk;
    uint32_t i;

    /* Инициализация данных для Next Fit */
    uChunkPoolSize = 1000;
    uChunkPoolMemSize = uChunkPoolSize * sizeof(MemChunk);
    
    /* Пул чанков размещается в начале выделенной области */
    pCMe->m_pChunkPool = (MemChunk*)startAddress;
    
    /* Память для данных начинается после пула чанков */
    pCMe->m_pMemBase = (char_t*)startAddress + uChunkPoolMemSize;
    pCMe->m_pMemLimit = (char_t*)startAddress + size;
    pCMe->m_uChunkPoolSize = uChunkPoolSize;
    pCMe->m_pChunkList = 0;
    pCMe->m_pLastFit = 0;
    
    /* Инициализация пула чанков */
    pChunk = pCMe->m_pChunkPool;
    for (i = 0; i < uChunkPoolSize; i++) {
        pChunk->bInUse = 0;
        pChunk->pBegin = 0;
        pChunk->pEnd = 0;
        pChunk->pNext = 0;
        pChunk->pPrev = 0;
        pChunk++;
    }

    return 0;
}
```

### Основная функция выделения памяти (Next Fit)

```c
static void* nextFitAllocate(
    uint32_t uSize, 
    MemChunk** ppLastFit,      /* Указатель на последний выделенный блок */
    MemChunk** ppListHead,     /* Голова списка блоков */
    MemChunk* pPool,           /* Пул свободных чанков */
    uint32_t uPoolSize,        /* Размер пула */
    char_t* pBase,             /* Начало доступной памяти */
    char_t* pLimit) {          /* Конец доступной памяти */
    
    MemChunk* pCurrent;
    MemChunk* pNewChunk;
    MemChunk* pListHead;
    char_t bWrapped;
    uint32_t uGap;
    
    pListHead = *ppListHead;
    bWrapped = 0;
    
    /* Если список пуст - первое выделение */
    if (pListHead == 0) {
        if ((uint32_t)(pLimit - pBase) >= uSize) {
            pNewChunk = findFreeChunk(pPool, uPoolSize);
            if (pNewChunk == 0) {
                return 0;
            }
            initChunk(pNewChunk, pBase, uSize, 0, 0);
            *ppListHead = pNewChunk;
            *ppLastFit = pNewChunk;
            return pNewChunk->pBegin;
        }
        return 0;
    }
    
    /* Инициализация текущей позиции для поиска */
    if (*ppLastFit != 0) {
        pCurrent = *ppLastFit;  /* Начинаем с последнего выделенного */
    } else {
        pCurrent = pListHead;   /* Если нет последнего - с начала */
    }
    
    /* Циклический поиск подходящего места */
    while (bWrapped == 0 || pCurrent != *ppLastFit) {
        /* Проверка промежутка между текущим и следующим блоком */
        if (pCurrent->pNext != 0) {
            uGap = (uint32_t)(pCurrent->pNext->pBegin - pCurrent->pEnd);
            if (uGap >= uSize) {
                /* Нашли подходящее место - выделяем блок */
                pNewChunk = findFreeChunk(pPool, uPoolSize);
                if (pNewChunk == 0) {
                    return 0;
                }
                initChunk(pNewChunk, pCurrent->pEnd, uSize, pCurrent, pCurrent->pNext);
                pCurrent->pNext->pPrev = pNewChunk;
                pCurrent->pNext = pNewChunk;
                *ppLastFit = pNewChunk;  /* Обновляем указатель последнего выделения */
                return pNewChunk->pBegin;
            }
            pCurrent = pCurrent->pNext;
            continue;
        }
        
        /* Проверка места после последнего блока */
        uGap = (uint32_t)(pLimit - pCurrent->pEnd);
        if (uGap >= uSize) {
            pNewChunk = findFreeChunk(pPool, uPoolSize);
            if (pNewChunk == 0) {
                return 0;
            }
            initChunk(pNewChunk, pCurrent->pEnd, uSize, pCurrent, 0);
            pCurrent->pNext = pNewChunk;
            *ppLastFit = pNewChunk;
            return pNewChunk->pBegin;
        }
        
        /* Переход к началу списка для циклического поиска */
        bWrapped = 1;
        pCurrent = pListHead;
        
        /* Проверка места перед первым блоком */
        uGap = (uint32_t)(pCurrent->pBegin - pBase);
        if (uGap >= uSize) {
            pNewChunk = findFreeChunk(pPool, uPoolSize);
            if (pNewChunk == 0) {
                return 0;
            }
            initChunk(pNewChunk, pBase, uSize, 0, pCurrent);
            pCurrent->pPrev = pNewChunk;
            *ppListHead = pNewChunk;
            *ppLastFit = pNewChunk;
            return pNewChunk->pBegin;
        }
    }
    
    return 0;  /* Не найдено подходящего места */
}
```

### Функция освобождения памяти

```c
static void nextFitFree(char_t* pPtr, MemChunk** ppListHead) {
    MemChunk* pChunk;
    
    if (ppListHead == 0 || *ppListHead == 0) {
        return;
    }
    
    pChunk = *ppListHead;
    
    /* Если освобождается первый блок */
    if (pChunk->pBegin == pPtr) {
        pChunk->bInUse = 0;
        if (pChunk->pNext != 0) {
            pChunk->pNext->pPrev = 0;
            *ppListHead = pChunk->pNext;
        } else {
            *ppListHead = 0;
        }
        return;
    }
    
    /* Поиск блока для освобождения */
    while (pChunk != 0) {
        if (pChunk->pBegin == pPtr) {
            pChunk->bInUse = 0;
            /* Обновление связей в списке */
            if (pChunk->pNext != 0) {
                pChunk->pNext->pPrev = pChunk->pPrev;
            }
            if (pChunk->pPrev != 0) {
                pChunk->pPrev->pNext = pChunk->pNext;
            }
            break;
        }
        pChunk = pChunk->pNext;
    }
}
```

### Вспомогательные функции

```c
/* Поиск свободного чанка в пуле */
static MemChunk* findFreeChunk(MemChunk* pPool, uint32_t uSize) {
    MemChunk* pChunk;
    uint32_t i;
    
    if (pPool == 0) {
        return 0;
    }
    
    pChunk = pPool;
    for (i = 0; i < uSize; i++) {
        if (pChunk->bInUse == 0) {
            return pChunk;
        }
        pChunk++;
    }
    return 0;
}

/* Инициализация чанка памяти */
static void initChunk(
    MemChunk* pChunk, 
    char_t* pStart, 
    uint32_t uSize, 
    MemChunk* pPrev, 
    MemChunk* pNext) {
    
    if (pChunk == 0) {
        return;
    }
    pChunk->bInUse = 1;
    pChunk->pBegin = pStart;
    pChunk->pEnd = pStart + uSize;
    pChunk->pPrev = pPrev;
    pChunk->pNext = pNext;
}
```

---

## 5. Описание unit-тестов

### Test1: Basic Alloc (Базовое выделение памяти)

**Назначение**: Проверяет корректность базового выделения памяти.

**Алгоритм теста**:
1. Выделяет первый блок размером 100 байт
2. Выделяет второй блок размером 200 байт
3. Проверяет, что оба указателя не равны NULL
4. Проверяет, что указатели не равны друг другу
5. Освобождает оба блока

---

### Test2: Free & Reuse (Освобождение и повторное использование)

**Назначение**: Проверяет, что освобожденная память может быть повторно использована.

**Алгоритм теста**:
1. Выделяет два блока (ptr1, ptr2)
2. Освобождает первый блок (ptr1)
3. Выделяет новый блок (ptr3) того же размера
4. Проверяет, что ptr3 == ptr1 (повторное использование)
5. Освобождает все блоки

---

### Test3: Next Fit (Поведение алгоритма Next Fit)

**Назначение**: Проверяет специфическое поведение алгоритма Next Fit — поиск начинается с последнего выделенного блока.

**Алгоритм теста**:
1. Выделяет три блока подряд (ptr1, ptr2, ptr3)
2. Освобождает средний блок (ptr2)
3. Выделяет новый блок (ptr4)
4. Проверяет, что ptr4 либо равен ptr2 (использовано освобожденное место), либо находится после ptr3 (Next Fit продолжает с последнего)

---

### Test4: Multiple (Множественные выделения и освобождения)

**Назначение**: Проверяет работу алгоритма при множественных операциях выделения и освобождения.

**Алгоритм теста**:
1. Выделяет 10 блоков по 50 байт каждый
2. Освобождает каждый второй блок (индексы 0, 2, 4, 6, 8)
3. Выделяет новые блоки в освобожденные места
4. Проверяет, что все выделения успешны
5. Освобождает все блоки

---

### Test5: Status (Проверка статуса памяти)

**Назначение**: Проверяет корректность функции получения статуса памяти.

**Алгоритм теста**:
1. Получает начальный статус памяти
2. Выделяет блок памяти (1000 байт)
3. Получает статус после выделения
4. Проверяет, что количество использованных блоков увеличилось
5. Проверяет, что свободная память уменьшилась
6. Освобождает блок

---

### Test6: Oversized (Граничный случай - слишком большой блок)

**Назначение**: Проверяет обработку запроса на выделение блока, превышающего доступную память.

**Алгоритм теста**:
1. Пытается выделить очень большой блок (1 МБ = 0x100000 байт)
2. Проверяет, что возвращается NULL (так как доступно только 512 КБ)
3. Если блок был выделен (не должно быть), освобождает его

---

## 6. Заключение

Реализованный алгоритм Next Fit обеспечивает:

1. **Эффективное использование памяти** - начинается поиск с последнего выделения
2. **Корректную работу с освобожденными блоками** - они могут быть повторно использованы
3. **Циклический поиск** - если с текущей позиции не найдено места, поиск продолжается по кругу
4. **Надежность** - все граничные случаи обрабатываются корректно

Unit-тесты покрывают все основные сценарии использования и позволяют убедиться в корректности работы алгоритма.

