0x....................

-- Lua 5.5: Пример с новыми фичами

-- 1. Новый синтаксис для объявления глобальных переменных (защита от опечаток)
global AppState

AppState = {
    version = "5.5",
    features = {}
}

-- 2. "Именованные" vararg таблицы (Named Vararg Tables)
-- Раньше нужно было использовать {...} или select, теперь можно так:
function log_event(... details)
    -- details.n содержит общее количество аргументов, включая nil
    -- details[1], details[2]... содержат сами значения
    local msg = string.format("Event received. Args count: %d", details.n)
    print(msg)

    -- Выводим первый аргумент, если он есть
    if details.n > 0 then
        print("First arg:", details[1])
    end
end

log_event("user_login", 42)
log_event() -- n = 0

-- 3. Оптимизация памяти: table.create
-- Предварительное выделение памяти под массив (narr элементов)
-- и хеш-часть (nrec элементов). Это быстрее, чем заполнять таблицу "на лету".
local big_array = table.create(1000, 2)

for i = 1, 1000 do
    big_array[i] = i * 2
end
big_array.meta = "created efficiently"

print("\nTable created with capacity 1000. Size:", #big_array)

-- 4. Переменные цикла for теперь доступны только для чтения (Read-Only)
-- Любая попытка изменить i вызовет ошибку компиляции.
print("\nCounting (i is read-only):")
for i = 1, 3 do
    print("Iteration:", i)
end

-- 5. Улучшенная точность вывода float (читаются обратно корректно)
print("\nFloat precision example:")
print(0.1 + 0.2) -- Выведет 0.3 (а не 0.30000000000000004)