// MIT License

// Copyright (c) 2020 Rob Loach @RobLoach

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "core/core.h"

#if defined(TIC_BUILD_WITH_GRAVITY)

#include <string.h>
#include "gravity_compiler.h"
#include "gravity_macros.h"
#include "gravity_vmmacros.h"
#include "gravity_core.h"
#include "gravity_vm.h"
#include "gravity_delegate.h"
#include "gravity_memory.h"

#define GRAVITY_VERIFY_ARGS(min_arguments, max_arguments) \
    gravity_delegate_t* delegate = gravity_vm_delegate(vm); \
    if (!delegate) { \
        RETURN_ERROR("Gravity VM delegate not found."); \
    } \
    tic_mem* tic = (tic_mem*)delegate->xdata; \
    if (!tic) { \
        RETURN_ERROR("TIC instance not found."); \
    } \
    if (nargs < (min_arguments)) { \
        RETURN_ERROR("Requires a minimum of " #min_arguments " arguments."); \
    } \
    if (nargs > (max_arguments)) { \
        RETURN_ERROR("Requires a maximum of " #max_arguments " arguments."); \
    }

static void gravityReportError(gravity_vm *vm, error_type_t error_type, const char *message, error_desc_t error_desc, void *xdata) {
    tic_core* core = (tic_core*)xdata;
    char buffer[1024];
    const char *type = "N/A";

    if (!core) {
        return;
    }

    switch (error_type) {
        case GRAVITY_ERROR_NONE: type = "NONE"; break;
        case GRAVITY_ERROR_SYNTAX: type = "SYNTAX"; break;
        case GRAVITY_ERROR_SEMANTIC: type = "SEMANTIC"; break;
        case GRAVITY_ERROR_RUNTIME: type = "RUNTIME"; break;
        case GRAVITY_WARNING: type = "WARNING"; break;
        case GRAVITY_ERROR_IO: type = "I/O"; break;
    }

    snprintf(buffer, sizeof buffer, "%s: line %d, column %d: %s", type, error_desc.lineno, error_desc.colno, message);
    core->data->error(core->data->data, buffer);
}

static void gravityLogCallback(gravity_vm *vm, const char *message, void *xdata) {
    tic_core* core = (tic_core*)xdata;
    if (core) {
        core->data->trace(core->data->data, message, 0);
    }
}

static void closeGravity(tic_mem* tic)
{
    tic_core* core = (tic_core*)tic;

    if (core) {
        gravity_vm* vm = core->gravity;
        if (vm) {
            gravity_vm_free(vm);
            gravity_core_free();
            core->gravity = NULL;
        }
    }
}

/**
 * Functions
 */
static bool gravityPrint(gravity_vm *vm, gravity_value_t *args, uint16_t nargs, uint32_t rindex) {
    GRAVITY_VERIFY_ARGS(1, 7);

    char* text = VALUE_AS_CSTRING(args[1]);
    int x = VALUE_AS_INT(args[2]);
    int y = VALUE_AS_INT(args[3]);
    u8 color = 3;
    bool fixed = true;
    s32 scale = 1;
    bool alt = false;

    s32 returnValue = 0;

    printf("gravityPrint 1 %s\n", text);

    returnValue = tic_api_print(tic, text, x, y, color, fixed, scale, alt);
    printf("gravityPrint 2\n");

    gravity_value_t rt = VALUE_FROM_UNDEFINED;

    rt = VALUE_FROM_INT(returnValue);
    RETURN_VALUE(rt, rindex);
}

static bool gravityBtn(gravity_vm *vm, gravity_value_t *args, uint16_t nargs, uint32_t rindex) {
    GRAVITY_VERIFY_ARGS(1, 1);

    int id = VALUE_AS_INT(args[1]);
    bool output = tic_api_btn(tic, id) > 0;

    gravity_value_t outputValue = VALUE_FROM_BOOL(output);
    RETURN_VALUE(outputValue, rindex);
}

static bool gravityCls(gravity_vm *vm, gravity_value_t *args, uint16_t nargs, uint32_t rindex) {
    GRAVITY_VERIFY_ARGS(1, 1);

    u8 color = VALUE_AS_INT(args[1]);
    tic_api_cls(tic, color);
    RETURN_NOVALUE();
}

static void setupGravity(tic_mem* tic, gravity_vm *vm) {
    gravity_vm_setvalue(vm, "print", NEW_CLOSURE_VALUE(gravityPrint));
    gravity_vm_setvalue(vm, "btn", NEW_CLOSURE_VALUE(gravityBtn));
    gravity_vm_setvalue(vm, "cls", NEW_CLOSURE_VALUE(gravityCls));
}

static const char* gravityPrecode(void *xdata) {
  return "extern var btn;"
         "extern var cls;"
         "extern var print;";
}

/**
 * Initialization
 */
static bool initGravity(tic_mem* tic, const char* code)
{
    tic_core* core = (tic_core*)tic;
    closeGravity(tic);

    // Put together the Gravity options.
    gravity_delegate_t delegate = {
        .error_callback = gravityReportError,
        .log_callback = gravityLogCallback,
        .report_null_errors = true,
        .xdata = tic,
        .precode_callback = gravityPrecode
    };

    // Construct the compiler.
    gravity_compiler_t *compiler = gravity_compiler_create(&delegate);
    if (!compiler) {
        core->data->error(core->data->data, "Error creating compiler.");
        return false;
    }

    // Interpret the code into the compiler.
    gravity_closure_t *closure = gravity_compiler_run(compiler, code, strlen(code), 0, true, true);
    if (!closure) {
        printf("gravity_compiler_run\n");
        gravity_compiler_free(compiler);
        return false;
    }


    // Set up the virtual machine.
    gravity_vm *vm = gravity_vm_new(&delegate);
    if (!vm) {
        printf("gravity_vm_new\n");
        gravity_compiler_free(compiler);
        return false;
    }

    // Transfer the memory to the virtual machine, and clean up the compiler.
    gravity_compiler_transfer(compiler, vm);
    gravity_compiler_free(compiler);

    // Setup the TIC-80 API.
    setupGravity(tic, vm);

    // Load the closure into the virtual machine context.
    gravity_vm_loadclosure(vm, closure);
    gravity_closure_free(vm, closure);

    // Set up the virtual machine as the active TIC-80 Gravity context.
    core->gravity = vm;

    return true;
}

static void callGravityTick(tic_mem* tic)
{
    tic_core* core = (tic_core*)tic;
    gravity_vm *vm = core->gravity;
    if(!vm)
    {
        return;
    }

    // TODO: Move this to initGravity() to save time.
    gravity_value_t tic_function = gravity_vm_getvalue(vm, "TIC", 3);
    if(!VALUE_ISA_CLOSURE(tic_function))
    {
        core->data->error(core->data->data, "func TIC() not found");
        return;
    }

    gravity_closure_t *tic_closure = VALUE_AS_CLOSURE(tic_function);
    gravity_vm_runclosure(vm, tic_closure, VALUE_FROM_NULL, NULL, 0);
}

static void callGravityScanline(tic_mem* tic, s32 row, void* data)
{
    tic_core* core = (tic_core*)tic;
    gravity_vm *vm = core->gravity;
    if(!vm)
    {
        return;
    }

    // TODO: Move this to initGravity() to save time.
    gravity_value_t scn_function = gravity_vm_getvalue(vm, SCN_FN, strlen(SCN_FN));
    if(!VALUE_ISA_CLOSURE(scn_function))
    {
        return;
    }

    // convert function to closure
    gravity_closure_t *scn_closure = VALUE_AS_CLOSURE(scn_function);
    gravity_value_t p1 = VALUE_FROM_INT(row);
    gravity_value_t params[] = {p1};
    gravity_vm_runclosure(vm, scn_closure, VALUE_FROM_NULL, params, 1);
}

static void callGravityOverline(tic_mem* tic, void* data)
{
    tic_core* core = (tic_core*)tic;
    gravity_vm *vm = core->gravity;
    if(!vm)
    {
        return;
    }

    // TODO: Move this to initGravity() to save time.
    gravity_value_t ovr_function = gravity_vm_getvalue(vm, OVR_FN, strlen(OVR_FN));
    if(!VALUE_ISA_CLOSURE(ovr_function))
    {
        return;
    }

    gravity_closure_t *ovr_closure = VALUE_AS_CLOSURE(ovr_function);
    gravity_vm_runclosure(vm, ovr_closure, VALUE_FROM_NULL, NULL, 0);
}

static const char* const GravityKeywords [] =
{
    "class", "var", "func", "return" "if", "is", "String", "Int",
    "Object", "Float", "Bool", "Null", "Class", "Function", "Fiber",
    "List", "Map", "Range", "enum", "count", "System", "push",
    "contains", "join", "map", "filter", "reduce", "sort", "hasKey",
    "keys", "disassemble", "static", "print", "bind", "unbind",
    "public", "private", "in", "loop", "call", "isDone", "put", "Math",
    "random", "true", "false", "init"
};

static const tic_script_config GravitySyntaxConfig =
{
    .init               = initGravity,
    .close              = closeGravity,
    .tick               = callGravityTick,
    .scanline           = callGravityScanline,
    .overline           = callGravityOverline,

    .getOutline         = NULL, // TODO: Implement callGravityOutline()
    .eval               = NULL, // TODO: Implement callGravityEval()

    .blockCommentStart  = "/*",
    .blockCommentEnd    = "*/",
    .blockCommentStart2 = NULL,
    .blockCommentEnd2   = NULL,
    .blockStringStart   = NULL,
    .blockStringEnd     = NULL,
    .singleComment      = "//",

    .keywords           = GravityKeywords,
    .keywordsCount      = COUNT_OF(GravityKeywords),
};

const tic_script_config* getGravityScriptConfig()
{
    return &GravitySyntaxConfig;
}


#endif /* defined(TIC_BUILD_WITH_GRAVITY) */