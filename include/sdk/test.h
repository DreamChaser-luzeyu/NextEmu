#pragma once

/**
 * NOTE: Please only include this header file from your Main_Test case document!
 *       Your test case will not run if you define your Main_Test case, include
 *       this header file from other document, and call RunUnitTest_Test(),
 *       because all symbols defined here are marked static.
 */

#include <iostream>
#include <cassert>

#include "sdk/console.h"

typedef struct TestCase_Desc {
    const char* name;
    void (*func)(void);
    const char* file;
    int line;
    struct TestCase_Desc* next;
} TestCase_Desc_t;

static TestCase_Desc_t* TestCaseList_Head = nullptr;

static void register_test(TestCase_Desc* desc) {
    TestCase_Desc_t* old_list_head = TestCaseList_Head;
    TestCaseList_Head = desc;
    desc->next = old_list_head;
}

/**
 * NOTE: Has to be called in the same document!
 *       If you want to call this function at some other place, you could define & export
 *       another function calling this function, and call the function you defined at some
 *       other document.
 */
static void RunUnitTest_Test() {
    if(!TestCaseList_Head) {
        LOG_WARN("No test case to run!");
        return;
    }

    for(TestCase_Desc_t* ptr = TestCaseList_Head; ptr; ptr = ptr->next) {
        std::cout << STYLE_BKG_YELLOW << "[TEST ] Case " << ptr->name << " running..." << STYLE_RST << std::endl;
        ptr->func();
        std::cout << STYLE_BKG_GREEN << "[TEST ] Case " << ptr->name << " passed!" << STYLE_RST << std::endl;
    }
    std::cout << STYLE_BKG_GREEN << "[TEST ] All case passed!" << STYLE_RST << std::endl;
    LOG_CLEAR();
}

/**
 * NOTE: You should not define TEST_CASE in header files
 */
#ifdef __GNUC__
#define TEST_CASE(case_name, case_desc)                                  \
static void case_name(void);                                             \
static TestCase_Desc_t case_name##_desc = {                              \
    .name = case_desc,                                                   \
    .func = case_name,                                                   \
    .next = nullptr                                                      \
};                                                                       \
                                                                         \
static void __attribute__((constructor(101))) case_name##_helper() {     \
    register_test(&case_name##_desc);                                    \
}                                                                        \
static void case_name(void)
#endif

/**
 * Every `TEST_CASE` in the same document compose a test unit.
 * `RUN_TEST_UNIT` means to run all TEST_CASEs in the test unit.
 * @NOTE `unit_name` should not conflict with any global symbol name
 */
#define REGISTER_TEST_UNIT(unit_name) void unit_name() { RunUnitTest_Test(); }
#define RUN_TEST_UNIT(unit_name) do { extern void unit_name(); unit_name(); } while(0)
