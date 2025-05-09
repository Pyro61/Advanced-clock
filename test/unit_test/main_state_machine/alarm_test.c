#include "unity/fixture/unity_fixture.h"
#include "assert.h"
#include "../Src/main_state_machine/state.h"
#include "../Src/main_state_machine/state_alarm/state_alarm.h"
#include "display_mock.h"
#include "time_mock.h"
#include <string.h>


/* Messages showed by display */
#define CLEARED_MESSAGE             "\0"
#define ENTRY_MESSAGE               "\n        ALARM\n      00:00:00"

typedef const struct main_state_interface * (*main_state_get_t)(void);
static const main_state_get_t alarm_funs = main_state_alarm_get;
static enum state_status return_state;


static char test_buf[80];

/* Helper function definitions */
static void reset_test_buf(void);
static void reset_time(void);
static void init_state(void);
static void entry_state(void);
static void exit_state(void);
static enum state_status press_ok_mode_button(void);
static void refresh(void);


TEST_GROUP(alarm);

TEST_SETUP(alarm)
{
    /* Init before every test */
    /* Call state init and reset buffers */
    init_state();
    display_mock_reset_buf();
    reset_test_buf();
    reset_time();
}


TEST_TEAR_DOWN(alarm)
{
    /* Cleanup after every test */
}


TEST(alarm, OnEntryDisplayOutput)
{
    entry_state();
    TEST_ASSERT_EQUAL_STRING(ENTRY_MESSAGE, test_buf);
}


TEST(alarm, OnExitDisplayOutput)
{
    entry_state();
    exit_state();
    TEST_ASSERT_EQUAL_STRING(CLEARED_MESSAGE, test_buf);
}


TEST(alarm, PressedOkButtonReturnedStateIsTheSameAsGivenToStateEntry)
{
    return_state = WEATHER_OUT;
    entry_state();
    enum state_status state = press_ok_mode_button();
    TEST_ASSERT_EQUAL(return_state, state);
}


TEST(alarm, WriteTimeThenEntryAlarmStateTimeShownOnDisplayIsTheSameAsWritten)
{
    struct time example_time = {10, 57, 4};
    time_set(example_time);
    entry_state();
    TEST_ASSERT_EQUAL_STRING("\n        ALARM\n      10:57:04", test_buf);
}


TEST(alarm, AfterRefreshAlarmStringIsGone)
{
    entry_state();
    refresh();
    TEST_ASSERT_EQUAL_STRING("\n             \n      00:00:00", test_buf);
}


TEST(alarm, AfterDoubleRefreshAlarmStringIsBackAndTimeDoesNotChange)
{
    struct time example_time = {19, 50, 0};
    time_set(example_time);
    entry_state();
    refresh();
    refresh();
    TEST_ASSERT_EQUAL_STRING("\n        ALARM\n      19:50:00", test_buf);
}


/* Helper functions */
static void reset_test_buf(void)
{
    memset(test_buf, 0, 80);
}


static void reset_time(void)
{
    struct time t = {0, 0, 0};
    time_set(t);
}


static void init_state(void)
{
    alarm_funs() -> init(get_display_mock_funs());
}


static void entry_state(void)
{
    alarm_funs() -> on_entry(return_state);
    display_mock_read_buf(test_buf);
}


static void exit_state(void)
{
    alarm_funs() -> on_exit();
    display_mock_read_buf(test_buf);
}


static enum state_status press_ok_mode_button(void)
{
    return alarm_funs() -> on_ok_mode_button_pressed();
}


static void refresh(void)
{
    alarm_funs() -> on_refresh();
    display_mock_read_buf(test_buf);
}