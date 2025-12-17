/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoVFB1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */


/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoMemoryManager1Lab.h"
#include "IEcoVirtualMemory1.h"
#include "IEcoTaskScheduler1.h"
#include "IdEcoTaskScheduler1Lab.h"
#include "IdEcoTimer1.h"
#include "IEcoSystemTimer1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "IdEcoIPCCMailbox1.h"
#include "IdEcoVFB1.h"
#include "IEcoVBIOS1Video.h"
#include "IdEcoMutex1Lab.h"
#include "IdEcoSemaphore1Lab.h"

/* Начало свободного участка памяти */
extern char_t __heap_start__;

/* Указатель на интерфейс для работы c мьютекс */
IEcoMutex1* g_pIMutex = 0;
/* Указатель на интерфейс для работы c семафор */
IEcoSemaphore1* g_pISemaphore = 0;

/* Указатель на интерфейсы */
IEcoVBIOS1Video* g_pIVideo = 0;
IEcoSystemTimer1* g_pISysTimer = 0;

char_t g_strTask[2] = {0};


void TimerHandler(void) {
    g_pIMutex->pVTbl->Lock(g_pIMutex);
    //g_pISemaphore->pVTbl->Wait(g_pISemaphore, 0);
    if (g_strTask[0] == '\\') {
        g_strTask[0] = '|';
    }
    else if (g_strTask[0] == '|') {
        g_strTask[0] = '/';
    }
    else if (g_strTask[0] == '/') {
        g_strTask[0] = '-';
    }
    else  {
        g_strTask[0] = '\\';
    }
    g_pIMutex->pVTbl->UnLock(g_pIMutex);
    //g_pISemaphore->pVTbl->Post(g_pISemaphore);
}

void printProgress() {
    if (g_strTask[0] == '\\') {
        g_strTask[0] = '|';
    }
    else if (g_strTask[0] == '|') {
        g_strTask[0] = '/';
    }
    else if (g_strTask[0] == '/') {
        g_strTask[0] = '-';
    }
    else  {
        g_strTask[0] = '\\';
    }
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 1, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, g_strTask, 1);
}

void Task1() {
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime +  5000000ul;
    uint64_t changeTime = currentTime;
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 0, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "1", 1);
    while ( endTime >= currentTime) {
        if (changeTime >= currentTime) {
            printProgress();
            changeTime += 50000ul;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
}

void Task2() {
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime +  5000000ul;
    uint64_t changeTime = currentTime;
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 0, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "2", 1);
    while ( endTime >= currentTime) {
        if (changeTime >= currentTime) {
            printProgress();
            changeTime += 50000ul;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
}

void Task3() {
    uint64_t currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    uint64_t endTime = currentTime +  5000000ul;
    uint64_t changeTime = currentTime;
    g_pIVideo->pVTbl->WriteString(g_pIVideo, 0, 0, 0, 0, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, "3", 1);
    while ( endTime >= currentTime) {
        if (changeTime >= currentTime) {
            printProgress();
            changeTime += 50000ul;
        }
        currentTime = g_pISysTimer->pVTbl->get_SingleTimerCounter(g_pISysTimer);
    }
}

/* Функция для вывода результата теста */
static void printTestResult(IEcoVBIOS1Video* pIVideo, uint16_t line, char_t* testName, int16_t passed) {
    char_t* result;
    byte_t color;
    
    if (passed != 0) {
        result = "PASS";
        color = CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN;
    } else {
        result = "FAIL";
        color = CHARACTER_ATTRIBUTE_FORE_COLOR_RED;
    }
    
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, line, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, testName, 20);
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 20, line, color, result, 4);
}

/* Тест 1: Базовое выделение памяти */
static int16_t testBasicAllocation(IEcoMemoryAllocator1* pIMem, IEcoVBIOS1Video* pIVideo, uint16_t line) {
    char_t* ptr1;
    char_t* ptr2;
    int16_t result;
    
    result = 0;
    ptr1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 100);
    if (ptr1 == 0) {
        return 0;
    }
    
    ptr2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 200);
    if (ptr2 == 0) {
        pIMem->pVTbl->Free(pIMem, ptr1);
        return 0;
    }
    
    /* Проверка, что указатели не равны */
    if (ptr1 != ptr2) {
        result = 1;
    }
    
    pIMem->pVTbl->Free(pIMem, ptr1);
    pIMem->pVTbl->Free(pIMem, ptr2);
    
    printTestResult(pIVideo, line, "Test1: Basic Alloc", result);
    return result;
}

/* Тест 2: Освобождение и повторное использование */
static int16_t testFreeAndReuse(IEcoMemoryAllocator1* pIMem, IEcoVBIOS1Video* pIVideo, uint16_t line) {
    char_t* ptr1;
    char_t* ptr2;
    char_t* ptr3;
    int16_t result;
    
    result = 0;
    
    /* Выделяем два блока */
    ptr1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 100);
    ptr2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 100);
    
    if (ptr1 == 0 || ptr2 == 0) {
        if (ptr1 != 0) {
            pIMem->pVTbl->Free(pIMem, ptr1);
        }
        if (ptr2 != 0) {
            pIMem->pVTbl->Free(pIMem, ptr2);
        }
        return 0;
    }
    
    /* Освобождаем первый блок */
    pIMem->pVTbl->Free(pIMem, ptr1);
    
    /* Выделяем новый блок - Next Fit должен использовать освобожденное место */
    ptr3 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 100);
    
    if (ptr3 != 0) {
        /* Проверяем, что новый блок находится в освобожденном месте */
        if (ptr3 == ptr1) {
            result = 1;
        }
        pIMem->pVTbl->Free(pIMem, ptr3);
    }
    
    if (ptr2 != 0) {
        pIMem->pVTbl->Free(pIMem, ptr2);
    }
    
    printTestResult(pIVideo, line, "Test2: Free & Reuse", result);
    return result;
}

/* Тест 3: Next Fit - поиск с последнего выделенного блока */
static int16_t testNextFitBehavior(IEcoMemoryAllocator1* pIMem, IEcoVBIOS1Video* pIVideo, uint16_t line) {
    char_t* ptr1;
    char_t* ptr2;
    char_t* ptr3;
    char_t* ptr4;
    int16_t result;
    
    result = 0;
    
    /* Выделяем три блока */
    ptr1 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 100);
    ptr2 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 100);
    ptr3 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 100);
    
    if (ptr1 == 0 || ptr2 == 0 || ptr3 == 0) {
        if (ptr1 != 0) {
            pIMem->pVTbl->Free(pIMem, ptr1);
        }
        if (ptr2 != 0) {
            pIMem->pVTbl->Free(pIMem, ptr2);
        }
        if (ptr3 != 0) {
            pIMem->pVTbl->Free(pIMem, ptr3);
        }
        return 0;
    }
    
    /* Освобождаем средний блок */
    pIMem->pVTbl->Free(pIMem, ptr2);
    
    /* Выделяем новый блок - Next Fit должен начать поиск с ptr3 (последний выделенный) */
    /* и найти место после ptr3 или перед ptr1 */
    ptr4 = (char_t*)pIMem->pVTbl->Alloc(pIMem, 100);
    
    if (ptr4 != 0) {
        /* Новый блок должен быть либо в освобожденном месте (ptr2), либо после ptr3 */
        if (ptr4 == ptr2 || ptr4 > ptr3) {
            result = 1;
        }
        pIMem->pVTbl->Free(pIMem, ptr4);
    }
    
    pIMem->pVTbl->Free(pIMem, ptr1);
    pIMem->pVTbl->Free(pIMem, ptr3);
    
    printTestResult(pIVideo, line, "Test3: Next Fit", result);
    return result;
}

/* Тест 4: Множественные выделения и освобождения */
static int16_t testMultipleAllocations(IEcoMemoryAllocator1* pIMem, IEcoVBIOS1Video* pIVideo, uint16_t line) {
    char_t* ptrs[10];
    int16_t i;
    int16_t result;
    int16_t allAllocated;
    
    result = 0;
    allAllocated = 1;
    
    /* Выделяем 10 блоков */
    for (i = 0; i < 10; i++) {
        ptrs[i] = (char_t*)pIMem->pVTbl->Alloc(pIMem, 50);
        if (ptrs[i] == 0) {
            allAllocated = 0;
            break;
        }
    }
    
    if (allAllocated != 0) {
        /* Освобождаем каждый второй блок */
        for (i = 0; i < 10; i += 2) {
            if (ptrs[i] != 0) {
                pIMem->pVTbl->Free(pIMem, ptrs[i]);
                ptrs[i] = 0;
            }
        }
        
        /* Выделяем новые блоки в освобожденные места */
        allAllocated = 1;
        for (i = 0; i < 10; i += 2) {
            ptrs[i] = (char_t*)pIMem->pVTbl->Alloc(pIMem, 50);
            if (ptrs[i] == 0) {
                allAllocated = 0;
                break;
            }
        }
        
        if (allAllocated != 0) {
            result = 1;
        }
    }
    
    /* Освобождаем все блоки */
    for (i = 0; i < 10; i++) {
        if (ptrs[i] != 0) {
            pIMem->pVTbl->Free(pIMem, ptrs[i]);
        }
    }
    
    printTestResult(pIVideo, line, "Test4: Multiple", result);
    return result;
}

/* Тест 5: Проверка статуса памяти */
static int16_t testMemoryStatus(IEcoMemoryManager1* pIMemMgr, IEcoVBIOS1Video* pIVideo, uint16_t line) {
    IEcoMemoryAllocator1* pIMem;
    ECOMEMORYMANAGER1STATUS status1;
    ECOMEMORYMANAGER1STATUS status2;
    char_t* ptr;
    int16_t result;
    int16_t queryResult;
    
    result = 0;
    
    /* Получаем интерфейс аллокатора */
    queryResult = pIMemMgr->pVTbl->QueryInterface(pIMemMgr, &IID_IEcoMemoryAllocator1, (void**)&pIMem);
    if (queryResult != 0 || pIMem == 0) {
        return 0;
    }
    
    /* Получаем начальный статус */
    if (pIMemMgr->pVTbl->get_Status(pIMemMgr, &status1) == 0) {
        pIMem->pVTbl->Release(pIMem);
        return 0;
    }
    
    /* Выделяем память */
    ptr = (char_t*)pIMem->pVTbl->Alloc(pIMem, 1000);
    if (ptr == 0) {
        pIMem->pVTbl->Release(pIMem);
        return 0;
    }
    
    /* Получаем статус после выделения */
    if (pIMemMgr->pVTbl->get_Status(pIMemMgr, &status2) == 0) {
        pIMem->pVTbl->Free(pIMem, ptr);
        pIMem->pVTbl->Release(pIMem);
        return 0;
    }
    
    /* Проверяем, что использованная память увеличилась */
    if (status2.usedBlocks > status1.usedBlocks && status2.freeSize < status1.freeSize) {
        result = 1;
    }
    
    pIMem->pVTbl->Free(pIMem, ptr);
    pIMem->pVTbl->Release(pIMem);
    
    printTestResult(pIVideo, line, "Test5: Status", result);
    return result;
}

/* Тест 6: Граничный случай - попытка выделить больше доступного */
static int16_t testOversizedAllocation(IEcoMemoryAllocator1* pIMem, IEcoVBIOS1Video* pIVideo, uint16_t line) {
    char_t* ptr;
    int16_t result;
    
    result = 0;
    
    /* Пытаемся выделить очень большой блок (1 МБ) */
    ptr = (char_t*)pIMem->pVTbl->Alloc(pIMem, 0x100000);
    
    /* Должен вернуть NULL, так как доступно только 512 КБ */
    if (ptr == 0) {
        result = 1;
    } else {
        pIMem->pVTbl->Free(pIMem, ptr);
    }
    
    printTestResult(pIVideo, line, "Test6: Oversized", result);
    return result;
}

/* Запуск всех тестов */
static void runMemoryTests(IEcoMemoryManager1* pIMemMgr, IEcoVBIOS1Video* pIVideo) {
    IEcoMemoryAllocator1* pIMem;
    int16_t queryResult;
    int16_t testResults;
    uint16_t line;
    
    line = 10;
    testResults = 0;
    
    /* Получаем интерфейс аллокатора */
    queryResult = pIMemMgr->pVTbl->QueryInterface(pIMemMgr, &IID_IEcoMemoryAllocator1, (void**)&pIMem);
    if (queryResult != 0 || pIMem == 0) {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, line, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "Failed to get allocator", 23);
        return;
    }
    
    /* Выводим заголовок */
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, line, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "Memory Manager Tests:", 20);
    line++;
    
    /* Запускаем тесты */
    testResults += testBasicAllocation(pIMem, pIVideo, line);
    line++;
    
    testResults += testFreeAndReuse(pIMem, pIVideo, line);
    line++;
    
    testResults += testNextFitBehavior(pIMem, pIVideo, line);
    line++;
    
    testResults += testMultipleAllocations(pIMem, pIVideo, line);
    line++;
    
    testResults += testMemoryStatus(pIMemMgr, pIVideo, line);
    line++;
    
    testResults += testOversizedAllocation(pIMem, pIVideo, line);
    line++;
    
    /* Выводим итоговый результат */
    if (testResults == 6) {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, line, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "All tests PASSED!", 17);
    } else {
        pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, line, CHARACTER_ATTRIBUTE_FORE_COLOR_RED, "Some tests FAILED", 17);
    }
    
    pIMem->pVTbl->Release(pIMem);
}

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoMemoryManager1* pIMemMgr = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoVirtualMemory1* pIVrtMem = 0;
    /* Указатель на интерфейс для работы с планировщиком */
    IEcoTaskScheduler1* pIScheduler = 0;
    IEcoTask1* pITask1 = 0;
    IEcoTask1* pITask2 = 0;
    IEcoTask1* pITask3 = 0;
    /* Указатель на интерфейс для работы c буфером кадров видеоустройства */
    IEcoVFB1* pIVFB = 0;
    ECO_VFB_1_SCREEN_MODE xScreenMode = {0};
    IEcoVBIOS1Video* pIVideo = 0;
    /* Указатель на интерфейс для работы c системным таймером */
    IEcoSystemTimer1* pISysTimer = 0;
    /* Указатель на интерфейс для работы c таймером */
    IEcoTimer1* pITimer = 0;

    char_t* strHello = "Hello, World!";
    uint16_t offset = 0;
    uint16_t x1 = 0;
    uint16_t y1 = 32;
    uint16_t x2 = 70;
    byte_t color = 170; /* 3-3-2 bit RGB */

    /* Создание экземпляра интерфейсной шины */
    result = GetIEcoComponentFactoryPtr_00000000000000000000000042757331->pVTbl->Alloc(GetIEcoComponentFactoryPtr_00000000000000000000000042757331, 0, 0, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    /* Проверка */
    if (result != 0 && pIBus == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с памятью */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_0000000000000000000000004D656D31);
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMemoryManager1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_81589BFED0B84B1194524BEE623E1838);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с ящиком прошивки */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoIPCCMailbox1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_F10BC39A4F2143CF8A1E104650A2C302);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Запрос расширения интерфейсной шины */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        /* Установка расширения менаджера памяти */
        //pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1);
        pIMemExt->pVTbl->set_Manager(pIMemExt, &CID_EcoMemoryManager1Lab);
        /* Установка разрешения расширения пула */
        pIMemExt->pVTbl->set_ExpandPool(pIMemExt, 1);
    }

    /* Получение интерфейса управления памятью */
    //pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1Lab, 0, &IID_IEcoMemoryManager1, (void**) &pIMemMgr);
    if (result != 0 || pIMemMgr == 0) {
        /* Возврат в случае ошибки */
        return result;
    }

    /* Выделение области памяти 512 КБ */
    pIMemMgr->pVTbl->Init(pIMemMgr, &__heap_start__, 0x080000);

    /* Получение интерфейса для работы с виртуальной памятью */
    result = pIMemMgr->pVTbl->QueryInterface(pIMemMgr, &IID_IEcoVirtualMemory1, (void**)&pIVrtMem);
    if (result == 0 && pIVrtMem != 0) {
        /* Инициализация виртуальной памяти */
        result = pIVrtMem->pVTbl->Init(pIVrtMem);
        /* TO DO */
    }
    /* Регистрация статического компонента для работы с планировщиком */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoTaskScheduler1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_902ABA722D34417BB714322CC761620F);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с таймером */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoTimer1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_8DB93F3DF5B947D4A67F7C100B569723);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с VBF */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoVFB1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_939B1DCDB6404F7D9C072291AF252188);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с мьютекс */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoMutex1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_2F48BBCBE4884CC08ECFC45990017215);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Регистрация статического компонента для работы с семафор */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoSemaphore1Lab, (IEcoUnknown*)GetIEcoComponentFactoryPtr_0741985B8FD0476C867CAE177CD26E7C);
    /* Проверка */
    if (result != 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение интерфейса для работы с планировщиком */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoTaskScheduler1Lab, 0, &IID_IEcoTaskScheduler1, (void**) &pIScheduler);
    /* Проверка */
    if (result != 0 || pIScheduler == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Инициализация */
    pIScheduler->pVTbl->InitWith(pIScheduler, pIBus, &__heap_start__+0x090000, 0x080000);

    /* Создание статических задач */
    pIScheduler->pVTbl->NewTask(pIScheduler, Task1, 0, 0x100, &pITask1);
    pIScheduler->pVTbl->NewTask(pIScheduler, Task2, 0, 0x100, &pITask2);
    pIScheduler->pVTbl->NewTask(pIScheduler, Task3, 0, 0x100, &pITask3);

    /* Получение интерфейса для работы с мьютекс */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMutex1Lab, 0, &IID_IEcoMutex1, (void**) &g_pIMutex);
    /* Проверка */
    if (result != 0 || g_pIMutex == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение интерфейса для работы с семафор */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoSemaphore1Lab, 0, &IID_IEcoSemaphore1, (void**) &g_pISemaphore);
    /* Проверка */
    if (result != 0 || g_pISemaphore == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение интерфейса для работы с системным таймером */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoTimer1, 0, &IID_IEcoSystemTimer1, (void**) &pISysTimer);
    /* Проверка */
    if (result != 0 || pISysTimer == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }
    g_pISysTimer = pISysTimer;

    /* Установка обработчика прерывания программируемого таймера */
    result = pISysTimer->pVTbl->QueryInterface(pISysTimer, &IID_IEcoTimer1, (void**)&pITimer);
    /* Проверка */
    if (result != 0 || pITimer == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    pITimer->pVTbl->set_Interval(pITimer, 100000);
    pITimer->pVTbl->set_IrqHandler(pITimer, TimerHandler);
    pITimer->pVTbl->Start(pITimer);

    /* Получение интерфейса для работы с видео сервисами VBF */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoVFB1, 0, &IID_IEcoVFB1, (void**) &pIVFB);
    /* Проверка */
    if (result != 0 || pIVFB == 0) {
        /* Освобождение в случае ошибки */
        goto Release;
    }

    /* Получение информации о текущем режиме экрана */
    result = pIVFB->pVTbl->get_Mode(pIVFB, &xScreenMode);
    pIVFB->pVTbl->Create(pIVFB, 0, 0, xScreenMode.Width, xScreenMode.Height);
    result = pIVFB->pVTbl->QueryInterface(pIVFB, &IID_IEcoVBIOS1Video, (void**) &pIVideo);

    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 4, 5, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, strHello, 13);

    /* Вывод 1 строки "Эко ОС!!!" - кодовая страница 1251 */
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 0, 1, CHARACTER_ATTRIBUTE_FORE_COLOR_YELLOW, "\xdd\xea\xee\x20\xce\xd1\x21\x21\x21", 9);

    /* Рисуем линию - подчеркивание */
    for (offset = x1; offset <= x2; offset++) {
        pIVideo->pVTbl->WriteDot(pIVideo, color, 0, offset, y1);
    }

    /* Вывод 4 строки "Привет Мир!" */
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 4, 4, CHARACTER_ATTRIBUTE_FORE_COLOR_GREEN, "\xcf\xf0\xe8\xe2\xe5\xf2\x20\xcc\xe8\xf0\x21", 11);
    pIVideo->pVTbl->WriteString(pIVideo, 0, 0, 4, 5, CHARACTER_ATTRIBUTE_FORE_COLOR_WHITTE, strHello, 13);
    g_pIVideo = pIVideo;
    
    /* Запуск unit-тестов для проверки алгоритма Next Fit */
    if (pIMemMgr != 0 && pIVideo != 0) {
        runMemoryTests(pIMemMgr, pIVideo);
    }
    
    pIScheduler->pVTbl->Start(pIScheduler);

    while(1) {
        asm volatile ("NOP\n\t" ::: "memory");
    }

Release:

    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение интерфейса VFB */
    if (pIVFB != 0) {
        pIVFB->pVTbl->Release(pIVFB);
    }

    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}

