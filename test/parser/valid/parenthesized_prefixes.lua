local result = (factory)();
local value = (makeTable()).field;
local indexed = (data)[key];

(makeTable()).field = value;
(makeTable())[key] = value;
(object):method()
