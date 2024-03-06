#include <Globals.h>
#include <Functions.h>
#include <Constants.h>

int utc_offset = 19800; // default to Sri Lanka's offset.
struct tm timeinfo;     // contains time data. pre defined struct type.

void set_time_zone()
{
    int temp_offset_hours = utc_offset / 3600;
    int temp_offset_minutes = utc_offset / 60 - temp_offset_hours * 60;

    while (true)
    {
        display.clearDisplay();
        display.fillRoundRect(30, 12, 39, 34, 4, WHITE);
        if (temp_offset_hours < 0) // Sign is handled separately since it should be formatted inversed in display.
        {
            println("-", 9, 18, 3, false, WHITE);
        }
        else if (temp_offset_hours > 0)
        {
            println("+", 9, 18, 3, false, WHITE);
        }
        println(formatNumber(abs(temp_offset_hours)), 33, 18, 3, false, BLACK); // absoluting the hours to display because sign is handled above separately.
        println(formatNumber(abs(temp_offset_minutes)), 83, 18, 3, true, WHITE);
        println(":", 67, 18, 3, true, WHITE);

        int pressed = wait_for_button_press();

        if (pressed == PB_UP)
        {
            delay(200);
            temp_offset_hours++;
            if (temp_offset_hours > 14)
            {                            // 14 hours multiplies by 60.
                temp_offset_hours = -12; // 12 hours multiplies by 60.
            }
        }
        else if (pressed == PB_DOWN)
        {
            delay(200);
            temp_offset_hours--;
            if (temp_offset_hours < -12)
            {
                temp_offset_hours = 14;
            }
        }
        else if (pressed == PB_OK)
        {
            delay(200); // since the offset is finally a single variable counted in seconds, setting it here globally is unnecessary. It will be set after taking the minutes as well.
            break;
        }
        else if (pressed == PB_CANCEL)
        {
            delay(200);
            return;
        }
    }

    bool is_edge_case = (temp_offset_hours == 14 || temp_offset_hours == -12);

    while (true)
    {
        display.fillRoundRect(30, 12, 39, 34, 4, BLACK);
        println(formatNumber(abs(temp_offset_hours)), 33, 18, 3, false, WHITE); // absoluting the hours to display because sign is handled above separately.
        // Above two lines removes the white background around the hour setting state.(i.e. inverts the hour part of the display back.)
        display.fillRoundRect(80, 12, 39, 34, 4, WHITE);
        println(formatNumber(abs(is_edge_case ? temp_offset_hours = 0 : temp_offset_minutes) ), 83, 18, 3, true, BLACK); // This will always make the user see "00" as minutes if the hours are set to 14 or -12. Actual change of variable will be done later in thecode. And the abs is used because no need to print the sign of the minutes.

        int pressed = wait_for_button_press();

        if (pressed == PB_OK || is_edge_case && pressed != PB_CANCEL) //when press ok or edge case is true and not press cancel, have to update the time accordingly.
        {
            delay(200);
            utc_offset = temp_offset_hours * 3600 + temp_offset_minutes * 60;

            configTime(utc_offset, UTC_OFFSET_DST, NTP_SERVER);
            display.clearDisplay();
            update_time();
            println("Time zone is set", 0, 0, 2, true);
            delay(1000);
            break;
        }
        else if (pressed == PB_UP)
        {
            delay(200);
            temp_offset_minutes++;
            temp_offset_minutes = temp_offset_minutes % 60;
        }
        else if (pressed == PB_DOWN)
        {
            delay(200);
            temp_offset_minutes--;
            temp_offset_minutes = temp_offset_minutes % 60;
            if (temp_offset_minutes < 0)
            {
                temp_offset_minutes = 59;
            }
        }
        else if (pressed == PB_CANCEL)
        {
            delay(200);
            break;
        }
    }
}

void print_time_now()
{
    display.clearDisplay();
    println(timeinfo, "%H:%M:%S", 18, 0, 2);
    println(timeinfo, "%d %B %Y", 25, 22, 1);
}

void update_time()
{
    if (!getLocalTime(&timeinfo))
    {
        display.clearDisplay();
        println("Failed to fetch time from server!", 0, 0, 2, true);
    }
}