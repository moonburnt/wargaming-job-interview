Тестовое задание для Wargaming.

## Требования:

В 'Tools_Test Task.pdf'


## Сборка:

Сборка проекта осуществляется через CMake.

0. Клонировать репозиторий
1. `cmake . -B ./build`
2. `cmake --build ./build --config Release`

Проект будет собран в ./build/editor/


## Тестирование:

0. Скопировать содержимое `test_data` в директорию с исполняемым файлом и запустить editor.
1. В меню Load ввести `./components.json` и нажать Load


## Примеры использования:

[Usage Video](https://files.catbox.moe/czzekr.mp4)

![Screenshot 0](https://files.catbox.moe/3d2red.jpg)

![Screenshot 1](https://files.catbox.moe/3m3erw.jpg)

![Screenshot 2](https://files.catbox.moe/cqkzh3.jpg)


## Детали реализации:

### Стэк:

- C++17
- Raylib для вывода графики
- ImGui + rlImGui для интерфейса
- nlohmann-json для работы с json
- spdlog + fmt для логгирования

### Формат данных:

Информация о пропах хранится в специально отформатированном json.
Пример - в ./test_data/components.json

### Ограничения:

- Выбор пути загрузки/выгрузки файла - текстовый интерфейс ввода. Сделано для
того чтобы не зависеть от системных библиотек.
- В текстовых полях тестировался только рендеринг латиницы. Поддержка кириллицы
и иных алфавитов может отсутствовать.
