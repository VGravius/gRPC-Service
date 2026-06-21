# 🛠️ Реализация gRPC сервиса для системы управления параметрами 

Сервис для управления параметрами системы, реализованный на C++ с использованием gRPC.

---

## 📋 Описание проекта

Данный проект представляет собой gRPC-сервис, который позволяет:
- **Получать** значение параметра по его имени (`GetParameter`).
- **Устанавливать** значение параметра с проверкой уникальности запроса (`SetParameter`).

Сервис написан на C++17 с использованием библиотек gRPC и Protocol Buffers.  
Реализована полная валидация входных данных, логирование операций, обработка ошибок и кэширование параметров в памяти.

---

## 🏗️ Архитектура

Проект построен по принципам **SOLID** и разделён на следующие компоненты:

|          Компонент         |                                    Описание                                           |
|----------------------------|---------------------------------------------------------------------------------------|
| **`TelemetryServiceImpl`** | Основной gRPC-сервис. Реализует методы `GetParameter` и `SetParameter`.               |
|        **`Cache`**         | Потокобезопасное хранилище параметров в памяти (`std::unordered_map` + `std::mutex`). |
|       **`Validator`**      | Проверяет входные данные: имя, значение, уникальность `request_id`.                   |
|        **`Logger`**        | Записывает все операции в консоль с временной меткой.                                 |

### Взаимодействие компонентов

```
┌─────────────────┐
│    Клиент       │
│   (grpcurl)     │
└────────┬────────┘
         │ gRPC-запрос
         ▼
┌────────────────────────────────────────────────┐
│          TelemetryServiceImpl                  │
│        (обработка gRPC-запросов)               │
└─────┬──────────────┬──────────────┬────────────┘
      │              │              │
      ▼              ▼              ▼
┌─────────────┐ ┌─────────────┐ ┌─────────────┐
│  Validator  │ │    Cache    │ │   Logger    │
│  проверка   │ │  хранение   │ │ логирование │
│   данных    │ │ параметров  │ │  операций   │
└─────────────┘ └─────────────┘ └─────────────┘
```

## 📄 Контракт API (telemetry.proto)

Сервис предоставляет два метода:
- `GetParameter` — получение параметра по имени.
- `SetParameter` — установка параметра с проверкой `request_id`.

Сообщения используют стандартные типы:
- `google.rpc.Status` — для ошибок.
- `google.protobuf.Timestamp` — для времени.

---

## 📦 Используемые технологии

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![gRPC](https://img.shields.io/badge/gRPC-1.76.0-brightgreen.svg)
![CMake](https://img.shields.io/badge/CMake-3.15+-blue.svg)
![Google Test](https://img.shields.io/badge/GoogleTest-1.17.0-red.svg)
![vcpkg](https://img.shields.io/badge/vcpkg-2026-blueviolet.svg)
![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)

- **C++17** — стандарт языка.
- **gRPC** — фреймворк для удалённого вызова процедур.
- **Protocol Buffers** — сериализация данных.
- **CMake** — система сборки.
- **vcpkg** — менеджер пакетов.
- **Google Test** — unit-тестирование.

---

## ⚙️ Особенности реализации

- **Кэширование:** параметры хранятся в `std::unordered_map` с блокировками `std::mutex` для потокобезопасности. Это обеспечивает O(1) доступ к данным.

- **Валидация:** выделен отдельный класс `Validator`, который проверяет:
  - имя параметра — не пустое;
  - значение параметра — не пустое;
  - `request_id` — уникальный (защита от повторных запросов).

- **Логирование:** все операции записываются в консоль с временной меткой в формате `YYYY-MM-DD HH:MM:SS.mmm`. Это позволяет отслеживать работу сервиса в реальном времени.

- **Обработка ошибок:** используются стандартные gRPC-статусы:
  - `OK` (0) — успешное выполнение;
  - `NOT_FOUND` (5) — параметр не найден;
  - `INVALID_ARGUMENT` (3) — некорректные данные;
  - `ALREADY_EXISTS` (6) — повторный `request_id`;
  - `DEADLINE_EXCEEDED` (4) — таймаут запроса.

- **RAII:** применяется `std::lock_guard` для автоматической блокировки мьютексов и `std::unique_ptr` для управления памятью сервера. Это гарантирует освобождение ресурсов даже при возникновении исключений.

- **Синхронный сервер:** выбран для простоты и читаемости кода. При необходимости может быть расширен до асинхронного с использованием `grpc::CompletionQueue`.

- **Unit-тесты:** реализованы с использованием Google Test. Покрывают основные компоненты: `Cache`, `Validator`, `Logger`, `TelemetryServiceImpl`. Все 15 тестов успешно проходят.

- **Документация кода:** оформлена в формате Doxygen. Каждый класс и метод снабжены комментариями, описывающими их назначение и логику работы.

---

## 📁 Структура проекта

	gRPC-Service/
	├── proto/
	│ └── telemetry.proto # Контракт gRPC-сервиса
	├── src/
	│ ├── main.cpp # Точка входа
	│ ├── telemetry_service.h/cpp # Реализация gRPC-сервиса
	│ ├── cache.h/cpp # Кэш параметров
	│ ├── validator.h/cpp # Валидация данных
	│ └── logger.h/cpp # Логирование
	├── unit_tests/ # Unit-тесты
	│ ├── test_cache.cpp
	│ ├── test_validator.cpp
	│ ├── test_logger.cpp
	│ └── test_telemetry_service.cpp
	├── CMakeLists.txt # Сборка
	└── README.md # Документация

---

## 🚀 Сборка и запуск

### Требования

- **Windows 10/11** или **Linux**
- **Visual Studio 2022** (или GCC на Linux)
- **CMake 3.15+**
- **vcpkg**

### Установка зависимостей

**Windows (cmd / PowerShell):**

	git clone https://github.com/Microsoft/vcpkg.git
	cd vcpkg
	.\bootstrap-vcpkg.bat
	.\vcpkg install grpc:x64-windows
	.\vcpkg install gtest:x64-windows
	.\vcpkg integrate install

**Linux (bash):**

	git clone https://github.com/Microsoft/vcpkg.git
	cd vcpkg
	./bootstrap-vcpkg.sh
	./vcpkg install grpc:x64-linux
	./vcpkg install gtest:x64-linux
	./vcpkg integrate install

Примечание: Если vcpkg установлен в другую папку, путь в командах сборки нужно будет указать соответствующий.

### Сборка проекта

**Windows (cmd / PowerShell):**

	cd gRPC-Service
	mkdir build
	cd build
	cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
	cmake --build . --config Release

**Linux (bash):**

	cd gRPC-Service
	mkdir build
	cd build
	cmake .. -DCMAKE_TOOLCHAIN_FILE=/путь/к/vcpkg/scripts/buildsystems/vcpkg.cmake
	cmake --build . --config Release

### Запуск сервера

**Windows (cmd / PowerShell):**

	cd Release
	gRPC-Service.exe

**Linux (bash):**

	cd Release
	./gRPC-Service
	
После запуска вы увидите:
	
	Telemetry gRPC Service v1.0
	Сервер запущен на 0.0.0.0:50051

## 🔧 Установка grpcurl

Для отправки запросов к серверу используется утилита `grpcurl`.

### Windows

1. Скачайте файл `grpcurl_1.9.3_windows_x86_64.zip`  
   с [официальной страницы релизов](https://github.com/fullstorydev/grpcurl/releases).
2. Распакуйте архив в удобную папку (например, `C:\dev\grpcurl\`).
3. Добавьте путь к папке в переменную окружения `PATH` или используйте полный путь при вызове.

### Linux / macOS

1. Скачайте соответствующий файл для вашей архитектуры:  
   - `grpcurl_1.9.3_linux_amd64.tar.gz` — для 64-битной Linux  
   - `grpcurl_1.9.3_linux_arm64.tar.gz` — для ARM-систем  
   - `grpcurl_1.9.3_macos_x86_64.tar.gz` — для macOS (если доступен)
2. Распакуйте архив:
	
		tar -xzf grpcurl_1.9.3_linux_amd64.tar.gz
		sudo mv grpcurl /usr/local/bin/

---

## 🧪 Примеры запросов

Ниже приведены все основные сценарии работы с сервисом: успешные операции, обработка ошибок, защита от дублей и таймауты.

---

### ✅ 1. Успешное сохранение параметра

**Цель теста:** Проверить, что сервер корректно сохраняет параметр с уникальным request_id и возвращает статус OK.

**Запрос:**

	grpcurl -plaintext -d "{\"name\":\"temperature\",\"value\":\"23.5\",\"request_id\":\"id1\"}" localhost:50051 telemetry.v1.TelemetryService/SetParameter

**Ответ сервера (JSON):**

	{
	  "status": {
	    "message": "OK"
	  }
	}
	
**Ответ сервера (Log):**

	[2026-06-20 14:47:01.244] SetParameter: temperature - OK (value: 23.5, request_id: id1)

**Результат:** ✅ Параметр успешно сохранён.

### ✅ 2. Успешное получение параметра

**Цель теста:** Проверить, что сервер корректно возвращает ранее сохранённый параметр вместе с его значением и временной меткой.

**Запрос:**

	grpcurl -plaintext -d "{\"name\":\"temperature\"}" localhost:50051 telemetry.v1.TelemetryService/GetParameter

**Ответ сервера (JSON):**

	{
	  "name": "temperature",
	  "value": "23.5",
	  "timestamp": "2026-06-20T11:47:54.244371400Z",
	  "status": {
	    "message": "OK"
	  }
	}
	
**Ответ сервера (Log):**

	[2026-06-20 14:47:54.246] GetParameter: temperature - OK (value: 23.5)

**Результат:** ✅ Параметр успешно получен.

### ❌ 3. Ошибка: параметр не найден

**Цель теста:** Проверить, что сервер возвращает ошибку NOT_FOUND при запросе несуществующего параметра.

**Запрос:**

	grpcurl -plaintext -d "{\"name\":\"pressure\"}" localhost:50051 telemetry.v1.TelemetryService/GetParameter

**Ответ сервера (JSON):**

	ERROR:
	  Code: NotFound
	  Message: Parameter not found
	
**Ответ сервера (Log):**

	[2026-06-20 14:49:06.086] GetParameter: pressure - NOT_FOUND

**Результат:** ❌ Сервер вернул ожидаемую ошибку `NOT_FOUND`.
	
### ❌ 4. Ошибка: повторный request_id (защита от дублей)

**Цель теста:** Проверить, что сервер отклоняет запросы с уже использованным request_id и возвращает ошибку ALREADY_EXISTS.

**Первый запрос (успешный):**

	grpcurl -plaintext -d "{\"name\":\"humidity\",\"value\":\"45\",\"request_id\":\"id1\"}" localhost:50051 telemetry.v1.TelemetryService/SetParameter

**Первый ответ сервера (JSON - успешный):**

	{
	  "status": {
	    "message": "OK"
	  }
	}
	
**Первый ответ сервера (Log - успешный):**

	[2026-06-20 16:01:30.086] SetParameter: humidity - OK (value: 45, request_id: id1)

**Второй запрос (ошибка):**

	grpcurl -plaintext -d "{\"name\":\"humidity\",\"value\":\"50\",\"request_id\":\"id1\"}" localhost:50051 telemetry.v1.TelemetryService/SetParameter

**Второй ответ сервера (JSON - ошибка):**

	ERROR:
	  Code: AlreadyExists
	  Message: Request ID already used
	
**Второй ответ сервера (Log - ошибка):**

	[2026-06-20 16:01:32.576] SetParameter: humidity - ALREADY_EXISTS (request_id: id1)

**Результат:** ❌ Сервер вернул ожидаемую ошибку `ALREADY_EXISTS`.

### ❌ 5. Ошибка: пустое имя параметра

**Цель теста:** Проверить, что сервер отклоняет запросы с пустым именем параметра и возвращает ошибку INVALID_ARGUMENT.

**Запрос:**

	grpcurl -plaintext -d "{\"name\":\"\",\"value\":\"23.5\",\"request_id\":\"id2\"}" localhost:50051 telemetry.v1.TelemetryService/SetParameter

**Ответ сервера (JSON):**

	ERROR:
	  Code: InvalidArgument
	  Message: Invalid name
	
**Ответ сервера (Log):**

	[2026-06-20 14:51:13.833] ОШИБКА: SetParameter: invalid name (empty)

**Результат:** ❌ Сервер вернул ожидаемую ошибку `INVALID_ARGUMENT`.

### ❌ 6. Ошибка: пустое значение параметра

**Цель теста:** Проверить, что сервер отклоняет запросы с пустым значением параметра и возвращает ошибку INVALID_ARGUMENT.

**Запрос:**

	grpcurl -plaintext -d "{\"name\":\"humidity\",\"value\":\"\",\"request_id\":\"id3\"}" localhost:50051 telemetry.v1.TelemetryService/SetParameter

**Ответ сервера (JSON):**

	ERROR:
	  Code: InvalidArgument
	  Message: Invalid value
	
**Ответ сервера (Log):**

	[2026-06-20 14:51:46.694] ОШИБКА: SetParameter: invalid value (empty)

**Результат:** ❌ Сервер вернул ожидаемую ошибку `INVALID_ARGUMENT`.
	
### ❌ 7. Ошибка: пустой request_id

**Цель теста:** Проверить, что сервер отклоняет запросы с пустым request_id и возвращает ошибку INVALID_ARGUMENT.

**Запрос:**

	grpcurl -plaintext -d "{\"name\":\"humidity\",\"value\":\"45\",\"request_id\":\"\"}" localhost:50051 telemetry.v1.TelemetryService/SetParameter

**Ответ сервера (JSON):**

	ERROR:
	  Code: InvalidArgument
	  Message: Invalid request_id
	
**Ответ сервера (Log):**

	[2026-06-20 14:52:26.568] ОШИБКА: SetParameter: invalid request_id (empty)

**Результат:** ❌ Сервер вернул ожидаемую ошибку `INVALID_ARGUMENT`.

### ⏱️ 8. Таймаут запроса (клиент отменил запрос)

**Цель теста:** Проверить, как сервер обрабатывает ситуацию, когда клиент не дожидается ответа (отмена запроса по таймауту).

**Запрос:**

	grpcurl -plaintext -connect-timeout 3 -d "{\"name\":\"temperature\"}" localhost:50051 telemetry.v1.TelemetryService/GetParameter

**Ожидаемый ответ (при возникновении таймаута):**

	ERROR:
	  Code: DeadlineExceeded
	  Message: Request timeout

**Фактический ответ в текущем тестовом окружении:**

	{
	  "name": "temperature",
	  "value": "23.5",
	  "timestamp": "2026-06-20T12:46:15.766015500Z",
	  "status": {
	    "message": "OK"
	  }
	}
	
**Лог сервера (при попытке воспроизвести таймаут):**

	[2026-06-20 15:43:17.431] GetParameter: temperature - OK (value: 23.5)

**Результат:** ⚠️ Таймаут не воспроизведён (сервер отвечает мгновенно). Логика обработки реализована в коде и будет работать при долгих операциях (БД, вычисления, внешние вызовы).

**Пояснение:**

Таймаут не был воспроизведён в стандартных условиях, поскольку сервер отвечает мгновенно. Однако:

- ✅ Логика обработки таймаута реализована в коде (`context->IsCancelled()`).
- ✅ При возникновении долгих операций (например:
  - выполнение сложных запросов к базам данных (SQL, NoSQL);
  - тяжёлые вычисления (обработка больших объёмов данных);
  - ожидание ответа от внешних API и микросервисов;
  - работа с медленными дисками или сетевыми ресурсами)
  сервер в цикле проверяет `context->IsCancelled()`.
  Если клиент отменил запрос (по таймауту), сервер немедленно прерывает обработку и возвращает статус `DEADLINE_EXCEEDED`.
- ✅ Ошибка таймаута логируется и будет видна в консоли сервера.

Таким образом, функциональность обработки таймаутов присутствует в реализации и может быть задействована при появлении длительных операций, что соответствует требованиям к надёжности сервиса в условиях высокой нагрузки.

**Логирование ошибки (при реальном возникновении таймаута):**

	[2026-06-20 15:43:17.431] ОШИБКА: GetParameter: request cancelled by client (timeout)

## ▶️ Запуск unit-тестов

Проект покрыт unit-тестами с использованием Google Test. Тесты проверяют основные компоненты: Cache, Validator, Logger и TelemetryServiceImpl.

### Команда для запуска:

**Windows (cmd / PowerShell):**

	cd build\Release
	unit_tests.exe

**Linux (bash):**

	cd build/Release
	./unit_tests

### Пример вывода

После запуска вы увидите:

	[==========] Running 15 tests from 4 test suites.
	[----------] Global test environment set-up.
	[----------] 4 tests from CacheTest
	[ RUN      ] CacheTest.SetAndGet
	[       OK ] CacheTest.SetAndGet (0 ms)
	[ RUN      ] CacheTest.GetNotFound
	[       OK ] CacheTest.GetNotFound (0 ms)
	[ RUN      ] CacheTest.Exists
	[       OK ] CacheTest.Exists (0 ms)
	[ RUN      ] CacheTest.Overwrite
	[       OK ] CacheTest.Overwrite (0 ms)
	[----------] 4 tests from CacheTest (0 ms total)
	
	[----------] 5 tests from ValidatorTest
	[ RUN      ] ValidatorTest.ValidateName
	[       OK ] ValidatorTest.ValidateName (0 ms)
	[ RUN      ] ValidatorTest.ValidateValue
	[       OK ] ValidatorTest.ValidateValue (0 ms)
	[ RUN      ] ValidatorTest.ValidateRequestId
	[       OK ] ValidatorTest.ValidateRequestId (0 ms)
	[ RUN      ] ValidatorTest.RequestIdUniqueness
	[       OK ] ValidatorTest.RequestIdUniqueness (0 ms)
	[ RUN      ] ValidatorTest.RequestIdMultiple
	[       OK ] ValidatorTest.RequestIdMultiple (0 ms)
	[----------] 5 tests from ValidatorTest (2 ms total)
	
	[----------] 3 tests from LoggerTest
	[ RUN      ] LoggerTest.Log
	[2026-06-20 20:03:24.846] GetParameter: temperature - OK (value: 23.5)
	[       OK ] LoggerTest.Log (3 ms)
	[ RUN      ] LoggerTest.LogError
	[2026-06-20 20:03:24.851] ╬╪╚┴╩└: Test error message
	[       OK ] LoggerTest.LogError (0 ms)
	[ RUN      ] LoggerTest.LogWithoutDetails
	[2026-06-20 20:03:24.852] SetParameter: temperature - OK
	[       OK ] LoggerTest.LogWithoutDetails (0 ms)
	[----------] 3 tests from LoggerTest (7 ms total)
	
	[----------] 3 tests from TelemetryServiceTest
	[ RUN      ] TelemetryServiceTest.CompilationTest
	[       OK ] TelemetryServiceTest.CompilationTest (0 ms)
	[ RUN      ] TelemetryServiceTest.ValidationTest
	[       OK ] TelemetryServiceTest.ValidationTest (0 ms)
	[ RUN      ] TelemetryServiceTest.CacheTest
	[       OK ] TelemetryServiceTest.CacheTest (0 ms)
	[----------] 3 tests from TelemetryServiceTest (7 ms total)
	
	[----------] Global test environment tear-down
	[==========] 15 tests from 4 test suites ran. (23 ms total)
	[  PASSED  ] 15 tests.

**Где лежат тесты:**

Исходный код тестов находится в папке unit_tests/ в корне проекта. Каждый файл покрывает отдельный компонент сервиса.

	unit_tests/
	├── test_cache.cpp # Тесты для класса Cache
	├── test_validator.cpp # Тесты для класса Validator
	├── test_logger.cpp # Тесты для класса Logger
	└── test_telemetry_service.cpp # Тесты для класса TelemetryServiceImpl

**Результат работы**

Все 15 тестов успешно пройдены. Это подтверждает корректность работы ключевых компонентов сервиса:

	1. Cache — сохранение, получение, проверка существования, перезапись.
	2. Validator — валидация имени, значения, request_id, уникальность.
	3. Logger — логирование операций и ошибок.
	4. TelemetryServiceImpl — интеграция компонентов в сервисе.

## 📊 Итоговый вывод

### Почему выбран синхронный сервер

В рамках данного тестового задания был осознанно выбран **синхронный** подход к обработке запросов. Это решение обусловлено следующими факторами:

- **Читаемость и простота кода:** Синхронный сервер позволяет сохранить код линейным, прозрачным и легко сопровождаемым. Это важно для демонстрации архитектурных решений и понимания логики работы сервиса.
- **Полное соответствие ТЗ:** Все обязательные функциональные требования (валидация, логирование, обработка ошибок, потокобезопасность) реализованы в полном объёме. Асинхронность является рекомендацией по производительности, а не обязательным условием.
- **Адекватность нагрузке:** Для целей тестового задания, где не требуется обработка тысяч запросов в секунду, синхронная модель является достаточной и эффективной.
- **Возможность расширения:** В случае необходимости, архитектура проекта позволяет легко перейти на асинхронную обработку с использованием `grpc::CompletionQueue` без изменения бизнес-логики.

Таким образом, выбор синхронного сервера является **осознанным архитектурным решением**, направленным на достижение баланса между производительностью, читаемостью и соответствием требованиям тестового задания.

### Выполнение требований

Все обязательные и дополнительные требования тестового задания выполнены.

| Требование                      | Статус     |
|---------------------------------|------------|
| C++17                           | Выполнено  |
| gRPC сервис                     | Выполнено  |
| Обработка ошибок (gRPC статусы) | Выполнено  |
| SOLID                           | Выполнено  |
| RAII                            | Выполнено  |
| Валидация данных                | Выполнено  |
| Логирование                     | Выполнено  |
| Кэширование                     | Выполнено  |
| Защита от повторных запросов    | Выполнено  |
| Unit-тесты (Google Test)        | Выполнено  |
| Документация кода (Doxygen)     | Выполнено  |
| README с архитектурой           | Выполнено  |

Сервис готов к использованию.

## 📄 Лицензия

Проект создан в рамках тестового задания на реализацию gRPC сервиса

## 👤 Автор проекта

**Владислав (vladcrim/VGravius)**



