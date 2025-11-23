# EcoLab03

## 1. Постановка задачи

Цель: в ранее созданный компонент добавить точки подключения с интерфейсом событий, демонстрирующими пошаговую работу алгоритма вычисления гамма-функции.

## 2. Реализация

### События гамма-функции

Компонент `CEcoLab1` генерирует четыре типа событий, позволяющих отслеживать процесс вычисления гамма-функции:

1. **OnGammaStart** — событие начала вычисления. Передаёт исходное значение аргумента `x`, для которого будет вычисляться гамма-функция. Срабатывает в начале каждого вызова функции `gamma`.

2. **OnGammaNormalize** — событие нормализации аргумента. Срабатывает, когда исходное значение `x` меньше 1.0 и требуется нормализация до диапазона [1.0, 2.0]. Передаёт исходное значение `x` и нормализованное значение `normalized`. Это событие отражает важный этап алгоритма, необходимый для корректной работы приближения Ланцоша.

3. **OnGammaLanczos** — событие вычисления приближения Ланцоша. Срабатывает после применения формулы Ланцоша к нормализованному значению. Передаёт нормализованное значение `x` и результат приближения `result`. Это событие демонстрирует применение основной вычислительной части алгоритма.

4. **OnGammaComplete** — событие завершения вычисления. Содержит исходное значение `x` и финальный результат вычисления гамма-функции. Срабатывает после завершения всех этапов вычисления.

### Интерфейс событий

```c
typedef struct IEcoLab1VTblEvents {
    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ struct IEcoLab1Events* me, 
                                            /* in */ const UGUID* riid, 
                                            /* out */ void **ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ struct IEcoLab1Events* me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ struct IEcoLab1Events* me);

    /* IEcoLab1Events */
    int16_t (ECOCALLMETHOD *OnGammaStart)(/* in */ struct IEcoLab1Events* me, 
                                          /* in */ long double x);
    int16_t (ECOCALLMETHOD *OnGammaNormalize)(/* in */ struct IEcoLab1Events* me, 
                                               /* in */ long double x, 
                                               /* in */ long double normalized);
    int16_t (ECOCALLMETHOD *OnGammaLanczos)(/* in */ struct IEcoLab1Events* me, 
                                             /* in */ long double x, 
                                             /* in */ long double result);
    int16_t (ECOCALLMETHOD *OnGammaComplete)(/* in */ struct IEcoLab1Events* me, 
                                               /* in */ long double x, 
                                               /* in */ long double result);
} IEcoLab1VTblEvents;
```

### Пример реализации обработчика событий

```c
int16_t ECOCALLMETHOD CEcoLab1Sink_OnGammaStart(/* in */ struct IEcoLab1Events* me, 
                                                  /* in */ long double x) {
    CEcoLab1Sink* pCMe = (CEcoLab1Sink*)me;
    
    if (me == 0) {
        return -1;
    }
    
    pCMe->m_iStep++;
    printf("Step %d : Gamma calculation started for x = %.6Lf\n", 
           pCMe->m_iStep, x);
    
    return 0;
}
```

## 3. Интеграция Connection Point

### Настройка точки подключения

Компонент `CEcoLab1` реализует интерфейс `IEcoConnectionPointContainer`, что позволяет внешним объектам подписываться на события. Инициализация точки подключения происходит в функции `initCEcoLab1`:

```c
/* Создание точки подключения */
result = createCEcoLab1ConnectionPoint((IEcoUnknown*)pCMe->m_pISys,
                                       (IEcoConnectionPointContainer*)pCMe,
                                       &IID_IEcoLab1Events,
                                       (IEcoConnectionPoint**)&pCMe->m_pISinkCP);
```

Это обеспечивает возможность подключения нескольких подписчиков одновременно, что расширяет возможности системы мониторинга и отладки.

### Особенности реализации

Для обеспечения работы connection points без зависимости от внешнего компонента списка (`CID_EcoList1`), реализована поддержка двух режимов хранения подключений:

1. **Режим со списком** — если компонент `CID_EcoList1` доступен, используется стандартный список подключений через интерфейс `IEcoList1`.

2. **Режим с массивом** — если компонент списка недоступен, используется простой динамический массив подключений (`EcoConnectionData**`), который автоматически расширяется при необходимости.

Такая реализация обеспечивает работоспособность connection points даже в окружениях, где компонент списка недоступен.

### Генерация событий в алгоритме

Алгоритм вычисления гамма-функции реализован для трёх типов данных: `float`, `double` и `long double`. Основная логика вычисления:

1. **Нормализация**: если `x < 1.0`, значение нормализуется до диапазона [1.0, 2.0] путём умножения на накапливающий множитель `acc`.
2. **Приближение Ланцоша**: к нормализованному значению применяется формула приближения Ланцоша с коэффициентами.
3. **Финальный результат**: результат приближения делится на накапливающий множитель.

**Фрагмент реализации:**

```c
static long double CEcoLab1_gamma_ld_internal(IEcoLab1Ptr_t me, 
                                              long double x, 
                                              long double* p_normalized, 
                                              long double* p_lanczos_result) {
    long double acc = 1.0L;
    long double v = x;
    long double normalized = v;
    
    /* Нормализация для x < 1.0 */
    while (v < 1.0L) {
        acc *= v;
        v += 1.0L;
        normalized = v;
    }
    
    /* Генерация события нормализации */
    if (p_normalized != 0 && normalized != x && me != 0) {
        *p_normalized = normalized;
        CEcoLab1_Fire_OnGammaNormalize(me, x, normalized);
    }
    
    /* Применение приближения Ланцоша */
    long double lanczos_result = CEcoLab1_gamma_lanczos_ld(v);
    if (p_lanczos_result != 0 && me != 0) {
        *p_lanczos_result = lanczos_result;
        CEcoLab1_Fire_OnGammaLanczos(me, normalized, lanczos_result);
    }
    
    /* Финальный результат */
    return lanczos_result / acc;
}

static int16_t ECOCALLMETHOD CEcoLab1_gamma_float(IEcoLab1Ptr_t me, 
                                                   float x, 
                                                   float* out) {
    long double x_ld = (long double)x;
    
    /* Генерация события начала вычисления */
    CEcoLab1_Fire_OnGammaStart(me, x_ld);
    
    long double result_ld = CEcoLab1_gamma_ld_internal(me, x_ld, 
                                                        &normalized, 
                                                        &lanczos_result);
    
    /* Генерация события завершения */
    CEcoLab1_Fire_OnGammaComplete(me, x_ld, result_ld);
    
    *out = (float)result_ld;
    return ERR_ECO_SUCCESES;
}
```

### Функции генерации событий

Функции генерации событий (`CEcoLab1_Fire_OnGammaStart`, `CEcoLab1_Fire_OnGammaNormalize`, `CEcoLab1_Fire_OnGammaLanczos`, `CEcoLab1_Fire_OnGammaComplete`) используют механизм перечисления подключений для доставки событий всем зарегистрированным подписчикам:

```c
static int16_t ECOCALLMETHOD CEcoLab1_Fire_OnGammaStart(/* in */ struct IEcoLab1* me, 
                                                         /* in */ long double x) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    IEcoEnumConnections* pEnum = 0;
    IEcoLab1Events* pIEvents = 0;
    EcoConnectionData cd;

    if (me == 0 || pCMe->m_pISinkCP == 0) {
        return -1;
    }

    /* Получение перечислителя подключений */
    result = ((IEcoConnectionPoint*)pCMe->m_pISinkCP)->pVTbl->EnumConnections(
        (IEcoConnectionPoint*)pCMe->m_pISinkCP, &pEnum);
    
    if (result == 0 && pEnum != 0) {
        /* Перебор всех подключений и вызов обработчиков */
        while (pEnum->pVTbl->Next(pEnum, 1, &cd, 0) == 0) {
            result = cd.pUnk->pVTbl->QueryInterface(cd.pUnk, 
                                                     &IID_IEcoLab1Events, 
                                                     (void**)&pIEvents);
            if (result == 0 && pIEvents != 0) {
                pIEvents->pVTbl->OnGammaStart(pIEvents, x);
                pIEvents->pVTbl->Release(pIEvents);
            }
            cd.pUnk->pVTbl->Release(cd.pUnk);
        }
        pEnum->pVTbl->Release(pEnum);
    }
    
    return result;
}
```

## 4. Использование

### Подключение к событиям

Для получения событий необходимо создать объект, реализующий интерфейс `IEcoLab1Events`, и подключить его к компоненту через connection point:

```c
/* Создание sink для обработки событий */
CEcoLab1Sink* pSink = 0;
IEcoLab1Events* pIEcoLab1Events = 0;
result = createCEcoLab1Sink(pIMem, &pIEcoLab1Events);

/* Подключение к событиям */
pSink = (CEcoLab1Sink*)pIEcoLab1Events;
result = pSink->Advise(pSink, pIEcoLab1);

/* Выполнение вычислений - события будут генерироваться автоматически */
float result_float = 0.0f;
pIEcoLab1->pVTbl->gamma_float(pIEcoLab1, 5.0f, &result_float);

/* Отключение от событий */
pSink->Unadvise(pSink, pIEcoLab1);
```

### Пример вывода событий

При выполнении вычисления гамма-функции для `x = 0.5` будет сгенерирована следующая последовательность событий:

```
Step 1 : Gamma calculation started for x = 0.500000
Step 2 : Normalized x from 0.500000 to 1.500000
Step 3 : Lanczos approximation result for x = 1.500000 is 0.886227
Step 4 : Gamma calculation completed for x = 0.500000, result = 1.772454
```

## 5. Заключение

Реализованная система connection points позволяет отслеживать пошаговое выполнение алгоритма вычисления гамма-функции.

## 6. Результат работы программы
<img width="716" height="704" alt="image" src="https://github.com/user-attachments/assets/efa5e6a5-bf9f-4c70-81ad-c1f3514c24b9" />
<img width="717" height="329" alt="image" src="https://github.com/user-attachments/assets/c7754d01-cb67-4d51-8579-a1056ea841ca" />
