-- https://metin2.dev/topic/15905-syreldars-quest-functions/
---@diagnostic disable: lowercase-global, undefined-field, deprecated, undefined-global
--[[
    Checks if a `num` number is a valid number.
    The function also includes a sanity check to avoid passing "nan", "inf" and "-inf" as values.

    Time complexity: O(1).
    Space complexity: O(1).
]]
is_valid_number = function(num, allow_negative, allow_decimals)
    if (type(num) ~= "number") then
        return false;

    elseif (not allow_negative and num < 0) then
        return false;

    elseif (not allow_decimals and num ~= math.floor(num)) then
        return false;

    elseif (({["nan"] = true, ["inf"] = true, ["-inf"] = true})[tostring(num)]) then
        return false;
    end -- if/elseif

    return true;
end -- function

--[[
    Checks if a `num` number is an integer number.

    Time complexity: O(1).
    Space complexity: O(1).
]]
is_integer_number = function(num)
    return is_valid_number(num, true, false);
end -- function

--[[
    Checks if a `num` number is even.

    Time complexity: O(1).
    Space complexity: O(1).
]]
is_number_even = function(num)
    return is_integer_number(num) and math.mod(num, 2) == 0;
end -- function

--[[
    Checks if a `num` number is odd.

    Time complexity: O(1).
    Space complexity: O(1).
]]
is_number_odd = function(num)
    return is_integer_number(num) and math.mod(num, 2) ~= 0;
end -- function

--[[
    Outputs the factorial of a `num` number.

    Time complexity: O(n).
    Space complexity: O(1).
]]
factorial = function(num)
    local result = 1;

    for i = 2, num do
        result = result * i;
    end -- for

    return result;
end -- function

--[[
    Checks if an `element` item is contained inside of a `table_ex` table.

    Time complexity: O(n).
    Space complexity: O(1).
]]
table_is_in = function(table_ex, element)
    for _, item in ipairs(table_ex) do
        if (item == element) then
            return true;
        end -- if
    end -- for

    return false;
end -- function

--[[
    Checks if the 'table_ex' array contains an argument which key is 'keyword',

    Example:
        local table_ex = {
            ["cat"] = 1,
            ["dog"] = 2,
            ["bird"] = 3
        };
        table_contains_keyword(table_ex, "dog"); -> returns true.
        table_contains_keyword(table_ex, "monkey"); -> returns false. -- not found

    Time complexity: O(1).
    Space complexity: O(1).
]]
table_contains_keyword = function(table_ex, keyword)
    return table_ex[keyword] ~= nil;
end -- function

--[[
    Outputs a random element from within a `table_ex` table.

    Example:
        local table_ex = {1, 2, 3, 4, 5};
        table_get_random_item(table_ex); -> returns a random element from table_ex, like 2.

    Time complexity: O(1).
    Space complexity: O(1).
]]
table_get_random_item = function(table_ex)
    local table_ex_len = table.getn(table_ex)
    local random_index = math.random(table_ex_len);
    return table_ex[random_index];
end -- function

--[[
    Outputs a random element from inside the 'table_ex' table that is not included in 'except_table'.

    Example:
        local table_ex = {1, 2, 3, 4, 5};
        table_get_random_item_except(table_ex, {4, 1}); -> returns a random element from table_ex that is not 1 or 4, like 3.

    Time complexity: O(k + n), so O(n).
    -> k = number of excluded values.
    Space complexity: O(1).
]]
table_get_random_item_except = function(table_ex, except_table)
    local except_set = {};
    for _, value in pairs(except_table) do
        except_set[value] = true;
    end -- for

    local accepted_items = {};
    for _, value in pairs(table_ex) do
        if (not except_set[value]) then
            table.insert(accepted_items, value);
        end -- if
    end -- for

    return table_get_random_item(accepted_items);
end -- function

--[[
    Outputs a random element from inside the 'table_ex' table that is not included in 'except_table'.

    Example:
        local table_ex = {1, 2, 3, 4, 5};
        table_get_random_item_except(table_ex, {4, 1});
        -> returns a random element from table_ex that is not 1 or 4, for example 3.

    Time complexity: O(k + n), so O(n).
    -> k = number of excluded values.
    Space complexity: O(k).
    -> k = number of excluded values.
]]
table_get_random_item_except = function(table_ex, except_table)
    local excluded_values = {};
    for _, value in pairs(except_table) do
        excluded_values[value] = true;
    end -- for

    local accepted_items = {};
    for _, value in pairs(table_ex) do
        if (not excluded_values[value]) then
            table.insert(accepted_items, value);
        end -- if
    end -- for

    return table_get_random_item(accepted_items);
end -- function

--[[
    Outputs a random number within range_min =>--<= range_max that is not included in 'except_table'.

    Example:
        local random_num = get_random_number_within_except(1, 6, {5, 2});
        -> returns a random number within the range of 1 and 6 (both included) that is not 2 or 5, for example 3.

    Time complexity: O(k + n), so O(n)
    -> k = number of excluded values.
    Space complexity: O(k).
    -> k = number of excluded values.
]]
get_random_number_within_except = function(range_min, range_max, except_table)
    local excluded_values = {};
    for _, value in pairs(except_table) do
        excluded_values[value] = true;
    end -- for

    local random_num = math.random(range_min, range_max);
    while excluded_values[random_num] do
        random_num = math.random(range_min, range_max);
    end -- while

    return random_num;
end -- function

--[[
    Returns:
        The index of the table containing the specified element.

    Example:
        local table_ex = {5, 7, 9};

        table_get_element_index(table_ex, 5); -> returns 1.
        table_get_element_index(table_ex, 9); -> returns 3.
        table_get_element_index(table_ex, 11); -> returns 0. -- not found

    Time complexity: O(n).
    Space complexity: O(1).
]]
table_get_element_index = function(table_ex, element)
    for index, value in ipairs(table_ex) do
        if (value == element) then
            return index;
        end -- if
    end -- for

    return nil;
end -- function

--[[
    Returns:
        true, if any subarray of the specified table_ex contains a specified keyword inside it,
        else false.

    Example:
        local table_ex = {
            {["dog"] = 1},
            {["cat"] = 2},
            {["bird"] = 3},
        };

        table_is_any_subarray_containing_keyword(table_ex, "cat"); -> returns true.
        table_is_any_subarray_containing_keyword(table_ex, "cow"); -> returns false. -- not found

    Time complexity: O(n).
    Space complexity: O(1).
]]
table_is_any_subarray_containing_keyword = function(table_ex, keyword)
    for _, subarray in ipairs(table_ex) do
        if (subarray[keyword]) then
            return true;
        end -- if
    end -- for

    return false;
end -- function

--[[
    Returns:
        The index of the subarray containing a specified keyword inside it.

    Example:
        local table_ex = {
            {["bird"] = "chirp"},
            {["cat"] = "meow"},
            {["dog"] = "whoof"},
        };

        table_get_subarray_keyword_index(table_ex, "dog"); -> returns 3.
        table_get_subarray_keyword_index(table_ex, "bird"); -> returns 1.
        table_get_subarray_keyword_index(table_ex, "worm"); -> returns 0. -- not found

    Time complexity: O(n).
    Space complexity: O(1).
]]
table_get_subarray_keyword_index = function(table_ex, keyword)
    for index, subarray in ipairs(table_ex) do
        if (subarray[keyword]) then
            return index;
        end -- if
    end -- for

    return nil;
end -- function

--[[
    Returns:
        The shuffled version of a table.

    Example:
        local table_ex = {
            [1] = "cow",
            [2] = "bird",
            [3] = "dog",
            [4] = "cat",
            [5] = "monkey"
        };

        table_shuffle(table_ex) -> returns {
            [1] = "monkey",
            [2] = "cow",
            [3] = "bird",
            [4] = "cat",
            [5] = "dog"
        };

        (Since it's random, some values may be the same as before.)

    Time complexity: O(n).
    Space complexity: O(n).
]]
table_shuffle = function(table_ex)
    local shuffled_table = {};
    for i = 1, table.getn(table_ex) do
        shuffled_table[i] = table_ex[i];
    end -- for

    local rand = 0;
    for i = table.getn(shuffled_table), 2, -1 do
        rand = math.random(i);
        shuffled_table[i], shuffled_table[rand] = shuffled_table[rand], shuffled_table[i];
    end -- for

    return shuffled_table;
end -- function

--[[
    Returns:
        The number of seconds contained in a number of minutes given by 'value'.
]]
time_min_to_sec = function(value)
    return 60*value;
end -- function

--[[
    Returns:
        The number of seconds contained in a number of hours given by 'value'.
]]
time_hour_to_sec = function(value)
    return time_min_to_sec(60)*value;
end -- function

--[[
    Returns:
        The number of seconds contained in a number of days given by 'value'.
]]
time_day_to_sec = function(value)
    return time_hour_to_sec(24)*value;
end -- function

--[[
    Returns:
        The number of weeks contained in a number of hours given by 'value'.
]]
time_week_to_sec = function(value)
    return time_day_to_sec(7)*value;
end -- function

--[[
    Returns:
        The number of seconds contained in a number of 28-day months given by 'value'.
]]
time_month_to_sec_28_days = function(value)
    return time_day_to_sec(28)*value;
end -- function

--[[
    Returns:
        The number of seconds contained in a number of 30-day months given by 'value'.
]]
time_month_to_sec_30_days = function(value)
    return time_day_to_sec(30)*value;
end -- function

--[[
    Returns:
        The number of seconds contained in a number of 30-day months given by 'value'.
]]
time_month_to_sec_31_days = function(value)
    return time_day_to_sec(31)*value;
end -- function

--[[
    A the formatted version of an 'amount', which separates sets of 3 digits with a dot.
    It supports both floating-point and negative numbers.

    Time complexity: O(n).
    Space complexity: O(1).
]]
get_gold_format = function(amount)
    if (not is_valid_number(amount, true, true)) then
        return amount;
    end -- if

    local _, _, minus, int, fraction = string.find(tostring(amount), '([-]?)(%d+)([.]?%d*)');
    int = string.gsub(string.reverse(int), "(%d%d%d)", "%1.");
    return minus .. string.reverse(int) .. fraction;
end -- function

--[[
    Returns:
        A string representing the time in a "Days, Hours, Minutes and Seconds" format of a 'sec' amount of seconds.

    Example:
        get_time_format(52165786) => will return "603 days, 18 hours, 29 minutes and 46 seconds".
        get_time_format(52386) => will return "14 hours, 33 minutes and 6 seconds".

    Time complexity: O(1).
    Space complexity: O(1).
]]
get_time_format = function(sec)
    local days = math.floor(sec / time_day_to_sec(1));
    local remainder = math.mod(sec, time_day_to_sec(1));
    local hours = math.floor(remainder / time_hour_to_sec(1));
    local minutes = math.floor(math.mod(remainder, time_hour_to_sec(1)) / time_min_to_sec(1));
    local seconds = math.mod(sec, time_min_to_sec(1));

    local time_parts = {};
    if (days > 0) then
        table.insert(time_parts, string.format("%s %s", days, (days > 1 and "days" or "day")));
    end -- if

    if (hours > 0) then
        table.insert(time_parts, string.format("%s %s", hours, (hours > 1 and "hours" or "hour")));
    end -- if

    if (minutes > 0) then
        table.insert(time_parts, string.format("%s %s", minutes, (minutes > 1 and "minutes" or "minute")));
    end -- if

    if (seconds > 0) then
        table.insert(time_parts, string.format("%s %s", seconds, (seconds > 1 and "seconds" or "second")));
    end -- if

    local last_part = table.remove(time_parts);
    if (table.getn(time_parts) > 0) then
        return string.format("%s and %s", table.concat(time_parts, ", "), last_part);
    end -- if

    return last_part;
end -- function

--[[
    Outputs a string representing an 'int' number converted into its Roman equivalent.

    Example:
        int_to_roman_str(10) = "X",
        int_to_roman_str(8) = "VIII",

    Time complexity: O(1).
    Space complexity: O(1).
]]
int_to_roman_str = function(int)
    if (not is_valid_number(int, false, false)) then
        return "0";
    end -- if

    local roman_numerals = {
        {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"},
        {100, "C"}, {90, "XC"}, {50, "L"}, {40, "XL"},
        {10, "X"}, {9, "IX"}, {5, "V"}, {4, "IV"}, {1, "I"}
    };

    local ret = "";
    local temp_int = int;
    for i = 1, table.getn(roman_numerals) do
        while (temp_int >= roman_numerals[i][1]) do
            ret = ret .. roman_numerals[i][2];
            temp_int = temp_int - roman_numerals[i][1];
        end -- while
    end -- for

    return ret;
end -- function

--[[
    Checks if a 'str' string is a valid Roman number.

    Example:
        is_valid_roman_str("MCVIII") -> true,
        is_valid_roman_str("ZXCII") = false,

    Time complexity: O(n).
    Space complexity: O(1).
]]
is_valid_roman_str = function(str)
    if (not str or str == "") then
        return false;
    end -- if

    local valid_romans = {["I"] = 1, ["V"] = 5, ["X"] = 10, ["L"] = 50, ["C"] = 100, ["D"] = 500, ["M"] = 1000};
    local cur_char = "";

    for i = 1, string.len(str) do
        cur_char = string.sub(str, i, i);
        if (not valid_romans[cur_char]) then
            return false;
        end -- if
    end -- for

    return true;
end -- function

--[[
    Outputs an integer which is the value of a 'str' Roman number.

    Example:
        roman_str_to_int("M") = 1000,
        roman_str_to_int("DCCCLVI") = 856,

    Time complexity: O(n).
    Space complexity: O(1).
]]
roman_str_to_int = function(str)
    if (not is_valid_roman_str(str)) then
        return 0;
    end -- if

    local valid_romans = {["I"] = 1, ["V"] = 5, ["X"] = 10, ["L"] = 50, ["C"] = 100, ["D"] = 500, ["M"] = 1000};
    local ret = 0;
    local prev_val, cur_val = 0, 0;

    for i = string.len(str), 1, -1 do
        cur_val = valid_romans[string.sub(str, i, i)];
        if (cur_val < prev_val) then
            ret = ret - cur_val;
        else
            ret = ret + cur_val;
        end -- if/else

        prev_val = cur_val;
    end -- for

    return ret;
end -- function

--[[
    Outputs the decimal version of a 'bin' binary number.

    Time complexity: O(n).
    Space complexity: O(1).
]]
bin_to_num = function(bin)
    local decimal = 0;
    local bin_len = table.getn(bin);

    for i = 1, bin_len do
        decimal = decimal + tonumber(string.sub(bin, i, i)) * math.pow(bin_len - i, 2);
    end -- for

    return decimal;
end -- function

--[[
    Outputs the binary version of a 'num' decimal number.

    Time complexity: O(log n).
    Space complexity: O(log n).
]]
num_to_bin = function(num)
    local binary = "";
    local temp_num = num;

    while (temp_num > 0) do
        binary = math.mod(temp_num, 2) .. binary;
        temp_num = math.floor(temp_num / 2);
    end -- while

    return binary;
end -- function

--[[
    Executes:
        A clear of the currently active set regen, if present.
        Kills all currently alive dungeon monsters.
        Some monsters revive once, thus a second d.kill_all() call is needed.
]]
clear_dungeon = function()
    d.clear_regen();
    d.kill_all();
    d.kill_all();
end -- function

--[[
    Returns:
        The pids of all the online party members in the same instance (online and in the same channel, core and map).
]]
party_get_member_pids = function()
    return {party.get_member_pids()};
end -- function

--[[
    Returns:
        The number of members in the party that are in the same instance (online and in the same channel, core and map).
]]
party_get_member_count = function()
    return table.getn(party_get_member_pids());
end -- function

--[[
    Returns:
        The index of the first dungeon instance of the map which index is 'map_index'.
]]
ToDungeonIndex = function(map_index)
    local dungeon_index_start = 10000;
    return map_index * dungeon_index_start;
end -- function

--[[
    Returns:
        The normalized index of the map which index is 'map_index'.
]]
ToNormalIndex = function(map_index)
    local dungeon_index_start = 10000;
    return map_index >= dungeon_index_start and math.floor(map_index / dungeon_index_start) or map_index;
end -- function

--[[
    Returns:
        true, if the player is in the dungeon instance of the map which index is 'map_index',
        else, false.
]]
InDungeon = function(map_index)
    local pc_index = pc.get_map_index();
    return
        pc.in_dungeon() and
        pc_index >= ToDungeonIndex(map_index) and
        pc_index < ToDungeonIndex(map_index+1);
end -- function

--[[
    Returns:
        An array containing 2 subarrays:
        - The first subarray, vnum_list, contains the vnums of the skills;
        - The second subarray, name_list, contains the names of the skills;

    Needs: https://metin2.dev/topic/29230-syreldars-useful-globals
]]
BuildSkillList = function(job, group, min_level, max_level)
    local vnum_list = {};
    local name_list = {};

    if (not job) then
        job = pc.get_job();
    end -- if

    if (not group) then
        local pc_group = pc.get_skill_group();
        group = pc_group > 0 and pc_group or DOCTRINE_1;
    end -- if

    if (not min_level) then
        min_level = BASE_SKILL_LEVEL;
    end -- if

    if (not max_level) then
        max_level = PERFECT_MASTER_SKILL_LEVEL;
    end -- if

    local skill_level = 0;
    local skill_list = ACTIVE_SKILL_LIST[job][group];

    for _, skill_vnum in ipairs(skill_list) do
        skill_level = pc.get_skill_level(skill_vnum);
        if (skill_level >= min_level and skill_level <= max_level) then
            table.insert(vnum_list, skill_vnum);
            table.insert(name_list, SKILL_NAME_TABLE[skill_vnum]);
        end -- if
    end -- for

    return {vnum_list, name_list};
end -- function

--[[
    A normal "say" with altered color, given by the 'color' arg.
    Warning: The color must be present in the COLORS array.

    Example:
        color_say("red", "The sun is burning my skin!").

    Needs: https://metin2.dev/topic/29230-syreldars-useful-globals
]]
color_say = function(color, text)
    local rgb = COLORS[color];
    local r, g, b = rgb[1], rgb[2], rgb[3];

    say(color256(r, g, b)..text..color256(200, 200, 200))
end -- function
