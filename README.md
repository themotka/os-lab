# Интеграция компонентов калькулятора в EcoLab1

> Добавление вычислительных интерфейсов IEcoCalculatorX и IEcoCalculatorY в модуль **EcoLab1** с использованием механизмов включения и агрегирования.

---

## Цель проекта

Расширить функциональность компонента **EcoLab1**, внедрив поддержку калькулятора и обеспечив безопасное получение интерфейсов через резервные механизмы.

### Основные задачи

* Реализовать доступ к `IEcoCalculatorX` и `IEcoCalculatorY`
* Использовать **include** и **aggregation** стратегии
* Добавить fallback-логику для отказоустойчивости
* Проверить корректность запроса интерфейсов и выполнения вычислений

---

## Архитектурная схема
<img width="837" height="346" alt="image" src="https://github.com/user-attachments/assets/6fa725a4-db13-4afc-8d84-98341e3913c6" />

### Логика получения интерфейсов

#### IEcoCalculatorX

| Шаг | Источник    | Механизм                |
| --- | ----------- | ----------------------- |
| 1   | Компонент C | Include |
| 2   | Компонент B | Aggregation  |

#### IEcoCalculatorY

| Шаг | Источник    | Механизм                           |
| --- | ----------- | ---------------------------------- |
| 1   | Компонент E | Include                            |
| 2   | Компонент D | Include                            |
| 3   | Компонент C | QueryInterface (если создан ранее) |

---

## Реализация
Фрагмент реализации стратегии получения компонентов и их интерфейсов:
```startLine:endLine:SourceFiles/CEcoLab1.c
printf("\n--- Interface Access via IEcoLab1 ---\n");
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoUnknown, (void**)&pUnk);
    if (result == 0 && pUnk != 0) {
        print_test_result("IEcoUnknown interface", TEST_PASS, "Successfully obtained");
    } else {
        print_test_result("IEcoUnknown interface", TEST_FAIL, "Acquisition failed");
    }
    
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pIX);
    if (result == 0 && pIX != 0) {
        print_test_result("IEcoCalculatorX interface", TEST_PASS, "Successfully obtained");
    } else {
        print_test_result("IEcoCalculatorX interface", TEST_FAIL, "Acquisition failed");
    }
    
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**)&pIY);
    if (result == 0 && pIY != 0) {
        print_test_result("IEcoCalculatorY interface", TEST_PASS, "Successfully obtained");
    } else {
        print_test_result("IEcoCalculatorY interface", TEST_FAIL, "Acquisition failed");
    }
```
Стратегия интеграции обеспечивает:

* Иерархическое разрешение интерфейсов
* Резервные цепочки получения объектов
* Унифицированные запросы через `QueryInterface`

---

## Тестирование

<img width="832" height="675" alt="image" src="https://github.com/user-attachments/assets/8be52e99-afbd-4fe3-aa66-6c27743034ed" />


